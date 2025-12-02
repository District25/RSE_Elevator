# XF Ports

## Introduction
A XF (execution framework) consists of a view classes which some need to be adjusted
when used on another platform.

Following you will find a list of classes which may be adjusted/reimplemented when
changing to another platform/environment:
 - `XF`
 - `Event`
 - `Dispatcher`
 - `EventQueue`
 - `Thread`
 - `Mutex`

These classes are also named `port classes` because they can be ported to another
platform. All port classes can be found in the _port_ folder whereas every port is
located in a separate folder.

Every port may pick some default implementation classes from the _common_ folder
which might be also located in the _port_ folder.

## Port Folder Structure
In case you want to add support for another platform to the XF, you need to add a
subfolder to the _port_ folder and put the port class files in there. Best is, to
take an already present port which is most similar to what you need, rename it and
start adaption of these files.

You may also mix-up your port with classes from the _common_ folder and your custom
folder (or even reference classes from other ports).

## Available Ports
Here is a list of currently available ports:

| Port Name | OS | Platform | Folder Name | Description |
|--|--|--|--|--|
| XF_PORT_QT | Qt | Qt library based implementation | qt | XF support for macOS, Linux and Windows |
| XF_PORT_ZEPHYR | Zephyr | Zephyr OS based implementation | zephyr | XF for ARM Cortex based microcontrollers |
