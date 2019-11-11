# README

## ![FCG](https://avatars0.githubusercontent.com/u/26013747?s=50&v=4) FieldComm Group

FieldComm Group

**hipflowapp** is one component of the [HART-IP Developer
Kit](https://github.com/FieldCommGroup/HART-IP-Developer-Kit/blob/master/doc/HART-IP%20FlowDevice%20Spec.md).
It implements a [HART flow
device](https://github.com/FieldCommGroup/HART-IP-Developer-Kit/blob/master/doc/HART-IP%20FlowDevice%20Spec.md)
on the Raspberry Pi computer. Together with the
[hipserver](https://github.com/FieldCommGroup/hipserver)component, it forms a
fully functional HART-IP device.

more...

Known Issues
------------

more ...

User Guide
----------

To launch the **hipflowapp**, the command line is:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
sudo ./hipflowapp
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

There are no command line options. sudo is required because the app uses direct
memory access to manipulate the AD/DA card resources , thus requiring elevated
privileges.

To terminate the app, type Ctrl-C on the command line.

Developer Guide
---------------

Pull the repository from the bash terminal with this command line:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   git clone --recurse-submodules https://github.com/FieldCommGroup/hipflowapp
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Note the --recurse-modules flag that causes a copy of the **hipserver** library
code to be pulled as well.

You can build the program with make as follows:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
cd hipflowapp/hipflowapp/Hip_Native/
make
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Developers wanting to debug one or both applications will launch the programs
separately. In this case, you must launch **hipserver** first, as it owns the
message queues.

To debug **hipflowapp,** start it using the IDE of your choice. Project files
for Visual Studio 2017+ are included in the repository. Debugging remotely from
Windows using ssh will require a root login from Windows. The [procedure for
configuring
Ubuntu](https://github.com/FieldCommGroup/HART-IP-Developer-Kit/blob/master/doc/Install%20OS.md)
supplied in this kit does not include adding a root login on Ubuntu.

#### Enable root login on Ubuntu

Login as root feature is disabled by default on Ubuntu. First, reset root's
password using passwd command.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
$ sudo passwd
Enter user1 password:
Enter new UNIX password:
Retype new UNIX password:
passwd: password updated successfully
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Then use su command to login as root using your new root password:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
$ su
Password:
$ whoami
root
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#### Enable root login via ssh

Login as root remotely via ssh is also disabled by default on Ubuntu. Manually
open the sshd configuration file /etc/ssh/sshd_config and change line:

FROM: \#PermitRootLogin prohibit-password

TO: PermitRootLogin yes

Then restart the ssh daemon:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
$ sudo systemctl restart sshd
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

After you restart the sshd daemon you will be able to remotely login as a root
user:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
$ ssh root@10.0.0.55
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#### Configure Visual Studio for remote Debugging

In Visual Studio, go to Tools \> Options \> Cross Platform \> Connection manager
and add the root ssh login for your Pi.

### Architecture

**hipflowapp** more...

The following diagram shows how the **hipflowapp** is related to the other
components.

![](media/05b9fc6b8559a0536b9e8e1531e07c84.png)

### Repository Contents

The hipflowapp repository contains the folder Hip_Native and the Visual Studio
solution file. The Hip_Native source code file contains the folders:

-   Commands

-   Data

-   hipserver

To pull from the repository, use

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
git clone --recurse-submodules https://github.com/FieldCommGroup/hipflowapp
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#### Hip_Native folder

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
hipflowapp/hipflowapp/Hip_Native
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This folder contains a Visual Studio 2017 project for the **hipflowapp**. It
uses the remote build and debugging facility of Visual Studio to construct and
run the component under a debugger.

It also includes a *make* based build system in files Makefile\*. To build
**hipflowapp** using make, cd to this folder and type 'make'. This process will
leave a binary executable file named hipflowapp in the folder.

Some of the interesting files are described below.

| File                                          | Contents                                                                                                       |
|-----------------------------------------------|----------------------------------------------------------------------------------------------------------------|
| Hip_Native.vcxproj bcm2835.cpp commandSet.cpp | Visual Studio project file Low level driver code from Mike McCauley Holds the commands and selects the handler |
|                                               |                                                                                                                |

#### Commands Folder

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
hipflowapp/hipflowapp/Hip_Native/Commands
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This folder contains a dot-h file for each of the commands in the device (some
of which may not be implemented). The command implementation is contained in
these files (there are no .cpp files to match).

| File Cmd_xxxx.h | Contents Command implementation |
|-----------------|---------------------------------|
|                 |                                 |

#### Data Folder

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
hipflowapp/hipflowapp/Hip_Native/Data
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

| This folder holds the base data structures used to hold and record the device’s data set. |   |
|-------------------------------------------------------------------------------------------|---|


#### hipserver

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
hipflowapp/hipflowapp/Hip_Native/hipserver
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This folder is a git "submodule". It contains library code used for
communication with the server component. the "--recurse-submodules" flag on the
git clone statement above, causes a named version of the
[hipserver](https://github.com/FieldCommGroup/hipserver)repository to be placed
under Hip_Native folder. The Visual Studio build and make build facilities both
expect the correct version of this folder to be at this location.
