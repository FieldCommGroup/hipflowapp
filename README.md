# README

## ![FCG](https://avatars0.githubusercontent.com/u/26013747?s=50&v=4) FieldComm Group

## hipflowapp
**hipflowapp** is one component of the [HART-IP Developer
Kit](https://github.com/FieldCommGroup/HART-IP-Developer-Kit).
It implements a [HART flow
device](https://github.com/FieldCommGroup/HART-IP-Developer-Kit/blob/master/doc/hart-ip-flowdevice-spec.md)
on the Raspberry Pi computer. Together with the
[hipserver](https://github.com/FieldCommGroup/hipserver) component, it forms a
fully functional HART-IP device.

## Known Issues

Totalizer implementation for the tertiary variable has not been implemented.

On-Change burst trigger infrastructure and implementation do not exist.

Support for several data types have been left blank because they are not needed
in this implementation. They may be needed in some other implementations. (e.g.
cDatabase.cpp)

All of the trigger-mode + burst-command combinations have not been tested.

There is no provision for the device to recover automatically from a power cycle.

There is no provision for the device to be placed into write-protect mode.

## User Guide

Launch the flow device using the following command lines:
```
cd ~user1/flowdevice
sudo ./hipserver ./hipflowapp
```
assuming both executables are in the current working directory. See the [**hipserver**](https://github.com/FieldCommGroup/hipserver) documentation for details on that component.

There are no command line options. 

sudo is required because the app uses direct
memory access to manipulate the AD/DA card resources, which needs elevated privileges.

To terminate the app, type Ctrl-C on the command line.

To restore the device to factory settings, terminate the flow device and type:
```
sudo rm -rf /ect/native_dev
```
This removes the folder containing the device's configuration.  The folder will be recreated with the factory settings the next time the device is started.

## Developer Guide

```
 sudo apt-get install ninja-build
 sudo apt-get install -y gdb
 sudo apt-get install -y gdbserver
```

### Launching the programs for development

Developers wanting to debug one or both applications will launch the programs
separately. In this case, you must launch **hipserver** first, as it owns the
message queues that communicate with the app.
```
sudo ./hipserver
```

To launch the **hipflowapp**, run this command line from a separate terminal:

``` 
sudo ./hipflowapp
```

Note that the app will immediately exit if the hipserver is not running when the
hipflowapp is invoked.

### Pull from the repository

Pull the repository from the bash terminal with this command line:

```
git clone --recurse-submodules https://github.com/FieldCommGroup/hipflowapp
```

Note the --recurse-modules flag that causes a copy of the **hipserver** library
code to be pulled as well.
### Buiding with make

You can build the program with make as follows:

```
cd hipflowapp/hipflowapp/Hip_Native/
make
```
The Makefile is configured to build a HART7 device.
### Debugging
To debug **hipflowapp,** start it using the IDE of your choice. Project files
for Visual Studio 2017 are included in the repository. Debugging remotely from Windows using ssh will require a root login from Windows. The [procedure for
configuring Ubuntu](https://github.com/FieldCommGroup/HART-IP-Developer-Kit/blob/master/doc/Install%20OS.md)
supplied in this kit does not include adding a root login on Ubuntu.

#### Enable root login on Ubuntu

Login as root feature is disabled by default on Ubuntu. First, reset root's
password using passwd command.

```
$ sudo passwd
Enter user1 password:
Enter new UNIX password:
Retype new UNIX password:
passwd: password updated successfully
```

Then use su command to login as root using your new root password:

```
$ su
Password:
$ whoami
root
```

#### Enable root login via ssh

Login as root remotely via ssh is also disabled by default on Ubuntu. Manually
open the sshd configuration file /etc/ssh/sshd_config and change line:

FROM: \#PermitRootLogin prohibit-password

TO: PermitRootLogin yes

Then restart the ssh daemon:

```
$ sudo systemctl restart sshd
```

After you restart the sshd daemon you will be able to remotely login as a root
user:

```
$ ssh root@xxx.xxx.xxx.xxx
```

#### Configure Visual Studio for remote Debugging

In Visual Studio, go to Tools \> Options \> Cross Platform \> Connection manager
and add the root ssh login for your Pi.

The Visual Studio **DbgFlow** configuration makes
the Flow device report as a HART 5 device, allowing it to work with the HART OPC Server and therefore, the SDC625. The **RlsFlow** configuration defines _IS_HART7 on the compiler options, so the device will report itself as a HART 7 device with the requisite changes in command zero.

### Architecture

The following diagram shows how the **hipflowapp** is related to the other
components.

![Flow Device Components](.gitbook/assets/flowcomponent.png)

### Repository Contents

The hipflowapp repository contains the folder Hip_Native and the Visual Studio
solution file. The Hip_Native source code file contains the folders:

-   Commands

-   Data

-   hipserver

To pull from the repository, use

```
git clone --recurse-submodules https://github.com/FieldCommGroup/hipflowapp
```

#### Hip_Native folder

```
hipflowapp/hipflowapp/Hip_Native
```

This folder contains a Visual Studio 2017 project for the **hipflowapp**. It
uses the remote build and debugging facility of Visual Studio to construct and
run the component under a debugger.

It also includes a *make* based build system in files Makefile\*. To build
**hipflowapp** using make, cd to this folder and type 'make'. This process will
leave a binary executable file named hipflowapp in the folder.

Some of the interesting files are described below.

| File                                          | Contents                                                                                                       |
|----|----|
| Hip_Native.vcxproj   | Visual Studio project file   |
| bcm2835.cpp|Low level driver code from Mike McCauley |
| commandSet.cpp|Holds the commands and selects the handler |


#### Commands Folder

```
hipflowapp/hipflowapp/Hip_Native/Commands
```

This folder contains a .h file for each of the commands in the device (some
of which may not be implemented). The command implementation is contained in
these files (there are no .cpp files to match).

#### Data Folder

```
hipflowapp/hipflowapp/Hip_Native/Data
```

This folder contains the base data structures used to hold and record the device’s data set.


#### hipserver

```
hipflowapp/hipflowapp/Hip_Native/hipserver
```

This folder is a git submodule. It contains library code used for
communication with the server component. The "--recurse-submodules" flag on the
git clone statement above, causes a named version of the
[hipserver](https://github.com/FieldCommGroup/hipserver)repository to be placed
under Hip_Native folder. The Visual Studio build and make build facilities both
expect the correct version of this folder to be at this location.
