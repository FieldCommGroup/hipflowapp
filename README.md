# README

## ![FCG](https://avatars0.githubusercontent.com/u/26013747?s=50&v=4) FieldComm Group

**hipflowapp** is one component of the [HART-IP Developer Kit](https://github.com/FieldCommGroup/HART-IP-Developer-Kit/blob/master/doc/HART-IP%20FlowDevice%20Spec.md). It implements a [HART flow device](https://github.com/FieldCommGroup/HART-IP-Developer-Kit/blob/master/doc/HART-IP%20FlowDevice%20Spec.md) on the Raspberry Pi computer.   Together with the [**hipserver** ](https://github.com/FieldCommGroup/hipserver)component, it forms a fully functional HART-IP device.

more...

## Known Issues

more ...

## Developer Guide

Read below to learn how to build and modify this component.

### Architecture

**hipflowapp**  more...

The following diagram shows how the **hipflowapp** is related to the other components.

![](.gitbook/assets/flowcomponent.png)

### Repository Contents

The repository contains four folders:

* Hip\_Native
* Commands
* Data
* hipserver

To pull from the repository, use

```text
git clone --recurse-submodules https://github.com/FieldCommGroup/hipflowapp
```

#### **Hip\_Native folder**

```text
hipflowapp/hipflowapp/Hip_Native
```

This folder contains a Visual Studio 2017 project for the **hipflowapp**.  It uses the remote build and debugging facility of Visual Studio to construct and run the component under a debugger.

It also includes a _make_ based build system in files Makefile\*. To build **hipflowapp** using make, cd to this folder and type 'make'.  This process will leave a binary executable file named hipflowapp in the folder.

Some of the interesting files are described below.

| File | Contents |
| :--- | :--- |
| Hip\_Native.vcxprox | Visual Studio project file |
|  |  |

#### **Commands Folder**

```text
hipflowapp/hipflowapp/Hip_Native/Commands
```

more...

| File | Contents |
| :--- | :--- |
|  |  |

#### Data Folder

```text
hipflowapp/hipflowapp/Hip_Native/Data
```

more...

\*\*\*\*

| File | Contents |
| :--- | :--- |
|  |  |

#### hipserver

```text
hipflowapp/hipflowapp/Hip_Native/hipserver
```

This folder is a git "submodule".  It contains library code used for communication with the server component.  the "--recurse-submodules" flag on the git clone statement above, causes a named version of the [hipserver ](https://github.com/FieldCommGroup/hipserver)repository to be placed under Hip\_Native folder.  The Visual Studio build and make build facilities both expect the correct version of this folder to be at this location.

