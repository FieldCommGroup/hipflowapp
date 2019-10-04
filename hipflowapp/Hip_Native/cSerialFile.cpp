/*************************************************************************************************
 *
 * Workfile: cSerialFile.cpp
 * 05dec2018 - stevev
 *
 *************************************************************************************************
 * The content of this file is the 
 *     Proprietary and Confidential property of the HART Communication Foundation
 * Copyright (c) 2018, Fieldcomm Group Inc., All Rights Reserved 
 *************************************************************************************************
 *
 * Description:
 *		This handles the configuration file interactions (ie NonVolatile stuff).
 *
 */

#include "cSerialFile.h"
#include "cDataRaw.h" // the only thing we file
#include <errno.h>
#include <string.h> // to get error string
#include "debug.h"  /* use tim's logit */

extern const float hartLENaN;
extern  bool       writeNeeded; // cDatabase.h
 
serializationFile::serializationFile()
{
	pFD = NULL;
}

serializationFile::~serializationFile()
{
	if ( pFD )
	{
		fclose(pFD);
		pFD = NULL;
	}
}

/* returns true on file exists and is usable */
bool serializationFile::CheckFile(char* filespec)
{
	FILE* pSF = NULL;
	if ( filespec == NULL || filespec[0] == '\0' )
	{
		return false;
	}
	if ( (pSF = fopen(filespec, "r+"))  ) // read and write
	{
		fclose(pSF);
		sFilespec = filespec;
		return true;
	}
	else
	{
		return false;
	}
}
//  Open_File - Open existing or make the file and open it
//  returns 0 on existing file ready				   is open
//          1 on didn't exist, we made it, it's empty but open
//         -1 on doesn't exist and we failed to make it:closed
//
int serializationFile::Open_File(char* filespec)
{
	struct stat sb;
	int ret = -1;

	char* pFspec = (filespec)?filespec:const_cast<char*>(sFilespec.c_str());// if null spec, use recorded

	if ( pFD )// if there is a file open;
	{
		Close_File();
	}

	while ( pFD == NULL ) // allow for retry
	{
		if (  (pFD = fopen(pFspec, "r+"))  ) // read and write
		{// not null - it exists
			if (stat(pFspec, &sb) == -1) 
			{/* check the value of errno */
				fprintf(stderr, "File '%s' failed to open 4 stat. Error: %s\n", pFspec, strerror(errno));
				Close_File();
				ret = -1;
			}
			else
			{//got the file stats
				//printf("%9jd", (intmax_t) statbuf.st_size);
				if (((unsigned)sb.st_size) >= sizeof(NONvolatileData) )// like checking the version
				{
					int want, is;
					want = sizeof(NONvolatileData);
					is   = sb.st_size;
					dbgp_logdbg("File size is usable:(size = %d, need %d)\n", is,want);
					// this is done at exit...sFilespec = filespec;
					ret = 0;// all is well, return with file open
				}
				else
				if (sb.st_size == 0)
				{// file exists but is empty
					fprintf(stderr, "File is found but empty.\n");
					ret = 1;// return with file open
				}
				else
				if (((unsigned)sb.st_size) < sizeof(NONvolatileData))
				{// we're going to have a hard time reading it
					fprintf(stderr, "File '%s' is not usable.(size = %9jd, we need at least %9jd)\n", 
							filespec, sb.st_size, sizeof(NONvolatileData));
					fprintf(stderr, "    You should probably delete it and start over with default values.\n");
					Close_File();
					ret = -1;
				}
				else
				{ // we're going to have a hard time reading it
					fprintf(stderr, "File stats are not usable.(size = %9jd)\n", sb.st_size);
					Close_File();
					ret = -1;
				}
			}
			break; // outa while...we had our chance
		}
		else
		{ //make it;
			if ( make_file(pFspec) )// couldn't make it
			{
				ret = -1;
				break; // outa retry loop
			}
			// else, it's made and closed;  loop to try and open it again
		}  
	}
	if ( ret >= 0 && pFspec)
	{
		sFilespec = pFspec;
	}
	return ret;
}


// This this leaves the file closed (0) or non-existent on failure (-1)
int serializationFile::make_file(char * filespec)
{
	struct stat sb;
	int i, y = strlen(filespec) + 1;
	int ret = 0; // successfull made it, -1 on failed

	char filepath[y];filepath[0] = '\0';
	char filename[y];filename[0] = '\0';

	for ( i = y-1; i >= 0; i--)
	{
		if ( (filespec[i] == '\\') || (filespec[i] == '/') || i == 0 )
		{
			strcpy(filepath, filespec);
			filepath[i] = '\0';
			strcpy(filename,&(filespec[i+1]) );
			break; // outa for loop
		}
	}
	if (filename[0] == '\0' )
	{
		fprintf(stderr, "No filename supplied in '%s'\n", filespec);
		ret = -1;
	}
	else
	{// assume no file   see if directory exists
		if (stat(filepath, &sb) != 0 || ! S_ISDIR(sb.st_mode))
		{// does not exist, attempt dir construction...we can only make one dir depth
			if ( mkdir(filepath, 0777) ) // returns an error
			{
				fprintf(stderr, "Failed to generate directory '%s'.. Error: #%d, %s\n", filepath, errno, strerror(errno));
				ret = -1;
			}
			else
			{
				fprintf(stderr, "Created directory '%s'.\n", filepath);
				// leave ret 0
			}// fall thru to make the file
		}//else fall thru to make the file
	}//else fall thru to make the file

	if ( ret == 0 )
	{
		if ( ! (pFD = fopen(filespec, "w+")) ) // open for rd/wr, create if not there
		{// null:> return failed	
			fprintf(stderr, "Failed to create file '%s'.. Error: %s\n", filespec, strerror(errno));
			ret = -1;
		}
		else
		{	
			fprintf(stderr, "Created file '%s'.\n", filespec);
			Close_File();
			ret = 0; // all is well
		}
	}// else fall thru to return
	return ret;
}


void serializationFile::Close_File(void)
{
	if ( pFD == NULL )
	{		
		fprintf(stderr, "Close_File shows file was not open\n");
	}
	else
	{		
		fclose(pFD);
		pFD = NULL;
		fprintf(stderr, "Closed '%s'\n", sFilespec.c_str() );
	}
}


errVal_t serializationFile::getData(void)
{
	int s = sizeof(NONvolatileData);
	int L = 0;
	errVal_t R = NO_ERROR;

	if ( Open_File(NULL) >= 0 ) // success
	{
		L = fread(&NONvolatileData, 1, s, pFD);
		if ( L != s )
		{
			fprintf(stderr, "File read %d for requested %d.\n", L, s);
			R = WRITE_ERROR;
		}
	}
	else
	{
		R = FILE_ERROR;
	}
	Close_File();
	
	volatileData.expansion  = 254; 
	volatileData.loopCurrent = hartLENaN;//HART-IP devices don't have a loop to current

	return R;
}

errVal_t serializationFile::putData(void)
{
	int s = sizeof(NONvolatileData);
	int L = 0;
	errVal_t R = NO_ERROR;

	if ( sFilespec.empty() )
	{
		return PARAM_ERROR;// sFilespec should have been set before we get here
	}

	L =  Open_File(NULL);
	
	if ( L == -1 )// doesn't exist, we failed to open it
	{// assume the error message is already gone out
		R = FILE_ERROR;
	}
	else
	if ( L >= 0 )
	{// open, possibly empty
		L = fwrite(&NONvolatileData, 1, s, pFD);
		if ( L != s )
		{
			fprintf(stderr, "File wrote %d for requested %d.\n", L, s);
			R = WRITE_ERROR;
		}
		Close_File();
	}
	else
	{
		throw( "File system is lost");// should kill the app
	}
	writeNeeded = false;
	return R;
}