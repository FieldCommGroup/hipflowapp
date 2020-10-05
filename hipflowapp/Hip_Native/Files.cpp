/*************************************************************************************************
 * Copyright 2019 FieldComm Group, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Description:
 *		This handles the super user issue with accessing the ranges file.
 *
 */


#include "Files.h"

#include <string>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h> // stat
#include <errno.h>    // errno, ENOENT, EEXIST


bool isDirExist(const std::string& path)
{
    struct stat info;
    if (stat(path.c_str(), &info) != 0)// EACCESS - search permission denied; ENOENT A component of pathname does not exist, or pathname is an empty string
    {
        return false;
    }
    return (info.st_mode & S_IFDIR) != 0;//S_IFDIR    0040000   directory...S_ISDIR(m)  directory?S_ISREG(m)  is it a regular file?
}

bool makePath(const std::string& path)
{
    mode_t mode = 0755;
    int ret = mkdir(path.c_str(), mode);// if (mkdir(path, mode) != 0 && errno != EEXIST)

    if (ret == 0)
        return true;

    switch (errno)
    {
    case ENOENT:// ENOENT A directory component in pathname does not exist or is a dangling symbolic link.
        // parent didn't exist, try to create it
        {
            unsigned pos = path.find_last_of('/');
            if (pos == std::string::npos)
                return false;
            if (!makePath( path.substr(0, pos) ))
                return false;
        }
        // now, try to create again
        return 0 == mkdir(path.c_str(), mode);

    case EEXIST://EEXIST pathname already exists (not necessarily as a directory).
        // done!
        return isDirExist(path);

    default:
        return false;
    }
}


#if 0
int main(int argc, char* ARGV[])
{
    for (int i=1; i<argc; i++)
    {
        std::cout << "creating " << ARGV[i] << " ... " << (makePath(ARGV[i]) ? "OK" : "failed") << std::endl;
    }
    return 0;
}
===================================================================================================
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

int cp(const char *to, const char *from)
{
    int fd_to, fd_from;
    char buf[4096];
    ssize_t nread;
    int saved_errno;

    fd_from = open(from, O_RDONLY);
    if (fd_from < 0)
        return -1;

    fd_to = open(to, O_WRONLY | O_CREAT | O_EXCL, 0666);
    if (fd_to < 0)
        goto out_error;

    while (nread = read(fd_from, buf, sizeof buf), nread > 0)
    {
        char *out_ptr = buf;
        ssize_t nwritten;

        do {
            nwritten = write(fd_to, out_ptr, nread);

            if (nwritten >= 0)
            {
                nread -= nwritten;
                out_ptr += nwritten;
            }
            else if (errno != EINTR)
            {
                goto out_error;
            }
        } while (nread > 0);
    }

    if (nread == 0)
    {
        if (close(fd_to) < 0)
        {
            fd_to = -1;
            goto out_error;
        }
        close(fd_from);

        /* Success! */
        return 0;
    }

  out_error:
    saved_errno = errno;

    close(fd_from);
    if (fd_to >= 0)
        close(fd_to);

    errno = saved_errno;
    return -1;
}
shareimprove this answer
edited 

#endif // 0

// returns zero on success, an error number otherwise
int cpfile(FILE *to, FILE *from)
{
	return -1;
}



/* openSystemFile  will try to open the dataFileSpec as read-only file.
 *     at success  returns the file pointer.
 *        at fail  will try to open the systemFileSpec as a read-only file.
 *     at success  will generate the dataFileSpec
 *                 will copy the systemFileSpec to the dataFileSpec
 *                 will close the systemFileSpec
 *                 will return the file pointer to the dataFileSpec (rewound)
 *        at fail  will return NULL pointer...caller should warn to open as su before running
 * This complexity is due to not being able to debug in super user mode.
 ***/
FILE* openSystemFile( string& dataFileSpec, string& systemFileSpec )
{
	unsigned pos;
	int r;
	FILE *fp = NULL, *sp;
	errno = 0;
	uid_t  myuid = getuid();

	//will try to open the dataFileSpec as read-only file.
	if((fp = fopen(dataFileSpec.c_str(), "rb")))	// no user access
	{ 
		return fp;//     at success  returns the file pointer.
	}// else failed, we apparently have never run as su
	
	if ( errno != ENOENT ) // No such file or directory..anything else, we can't help
	{
		bcmLOGIT("%s failed to open for unhandled reason #%d.\n",dataFileSpec.c_str(),errno);
		goto openExit;;
	}// else failed not there...  will try to open the systemFileSpec as a read-only file.
	
	sp = fopen(systemFileSpec.c_str() , "rb");	// no user access

	if ( sp == NULL ) // failed to open
	{ 
		bcmLOGIT("%s failed to open;  superuser required.\n",systemFileSpec.c_str());
		goto openExit;;
	}// else success, will generate the dataFileSpec & modify group for memory access

	// while we're here and we know we're superuser, change the mod on the memory
	if (  (r = chmod(PI_MEMORY_PATH, 0660))  )// allow kmem user to write memory too (zero on success)
	{// failed, errno holds the issue number
		bcmLOGIT("%s failed to change mode to %s errno=%d .\n",PI_MEMORY_PATH, "0660", errno);
	}

	// we are super user here - we opened the system file
	// makedir
	
    pos = dataFileSpec.find_last_of('/');
    if (pos == string::npos) // no directory 
	{
		bcmLOGIT("%s has no path.\n",systemFileSpec.c_str());
	}
	else
    if (! makePath( dataFileSpec.substr(0, pos) ))
	{
		bcmLOGIT("%s path failed to make.\n",dataFileSpec.substr(0, pos));
		fclose(sp);
		goto openExit;;
	}

	// open dataFileSpec for writing
	if((fp = fopen(dataFileSpec.c_str(), "wb+")))	// no user access
	{ // success,  copy the systemFileSpec to the dataFileSpec
		if ( cpfile(fp, sp) )// true on success
		{// copied,  
			fclose(sp); //close the systemFileSpec
			rewind(fp); // return the file pointer to the dataFileSpec (rewound)
		}
		else
		{// failed to copy
			bcmLOGIT("%s did not copy to %s failed to copy.\n",systemFileSpec.c_str(),dataFileSpec.c_str() );
			fclose(sp);
			fclose(fp);
			fp = NULL;// return NULL pointer.
		}
	}
	else // failed to create a writable file
	{
		bcmLOGIT("%s failed to create.\n",dataFileSpec.c_str() );
		fclose(sp);
		fp = NULL;// return NULL pointer.
	}
 openExit:
	return fp;
}

