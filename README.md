# DEPTH CAMERA DEMO FROM PERCIPIO XYZ #

----------

This is a basic template for the users.

This directory implements a samll opencv2.4.9 based application. It gets depth, 3D point cloud, color or IR-grey data from the PERCIPIO's depth camera and shows it in windows.


## sample directory ##

The sample directory includes source code of the samples.

## lib directory ##

The lib directory includes the relevant libraries needed by linux and Windows.

x64、i686、armv7hf、armv7l_RaspberryPi_3B、aarch64 is supported in linux. 

x64 is supported in Windows.

## include directory ##

The include directory includes the relevant headers needed to call the dynamic library functions.

## Doc directory ##

Percipio Camport SDK Documents generated by Doxygen, which includes the classes, structs, unions and interfaces with detail descriptions.  

## Usage ##

## Compiling the Sample Code on ubuntu based platform ##

First install the Libusb and Opencv2.4.9 releated packages,

Second, copy the libtycam.so for target platform to /usr/lib directory, and excute ldconfig to creates the necessary links and cache to the shared library.

Then run the following commands to compile in sample directory

    1、mkdir build
    2、cd build
    3、cmake ..
    4、make

Finally, all executables generated in bin directory

run sudo ./bin/xxxx to show the xyz secret, enjoy it!


## Compiling the Sample Code on windows based platform ##

First install driver in host side which located in lib/win/driver,   

Second, copy the tycam.dll for target platform to the directory which in system environment variable.

Then generate .sln, .vcxproj files using cmake tools and copy the tycam.lib for target platform to the build directory.

Finally, open .sln file with visual studio to compile and debug.


Bear in mind that you may want to CROSS COMPILE the libusb and opencv source code first and then cross compile this demo code for your embedded platform.
=======
Bear in mind that you may want to CROSS COMPILE the libusb and opencv source code first and then cross compile this demo code for your EMBEDDED platform.

Please refer to http://bbs.percipio.xyz/viewforum.php?f=2 for more detais.

