# BACnet Client Module for Eclipse 4diac

BACnet/IP client module for [Eclipse 4diac](https://www.eclipse.org/4diac/), which was developed throughout the course of writing bachelor's thesis at the TU Vienna.

This module is based on the functions provided by the [BACnet Stack library](http://bacnet.sourceforge.net/) and on the FORTE's dynamic IO configuration concept and emBrick module.

Features:
+ Allows to configure and issue ReadProperty, WriteProperty, and SubscribeCOV service requests.
+ Supports BACnet's Who-Is/I-Am services requests and provides automatic network address discovery.
+ Supports handling of simple and complex acknowledgments (in response to ReadProperty and WriteProperty) and UnconfirmedCOVNotification services.

Tested on:
+ Ubuntu 18.04.5 LTS
+ Raspberry Pi 2 running Raspbian Stretch
+ Raspberry Pi 4 running Raspbian Buster

## Building BACnet Client Module for 4diac FORTE
The build process consists of three steps and is performed using [cmake-gui](https://cmake.org/cmake/help/latest/manual/cmake-gui.1.html#):
1. Building the BACnet Stack Library.
2. Building the 4diac FORTE with the BACnet Client Module.
3. Importing BACnet Client function blocks into the 4diac IDE.

### Building BACnet Stack Library
1. Download the [BACnet Stack Library 1.0.0](https://sourceforge.net/projects/bacnet/files/bacnet-stack/bacnet-stack-1.0.0/) and unpack its content. Hereafter, the root folder of the library will be referred to as *{BACNET_STACK_ROOT}*.
2. Create a build directory inside *{BACNET_STACK_ROOT}* (e.g. *{BACNET_STACK_ROOT}*/build/). From now on, the build directory will be referred to as *{BACNET_STACK_BUILD}*
3. Run 'cmake-gui'
4. Set the source code folder to *{BACNET_STACK_ROOT}* and the build folder to *{BACNET_STACK_BUILD}*. 
5. Press 'Configure'.
6. CMake will ask you to specify the generator and the compiler. If you plan to use the BACnet Client module on a PC running Linux, leave it as it is, press 'finish', and skip the next step.
7. If you want to run the BACnet Client module on a Raspberry Pi running Raspbian, you have to specify options for cross-compiling.
	1. Get the GCC Linaro Toolchain for ARM. Go [here](https://releases.linaro.org/components/toolchain/binaries/latest-7/arm-linux-gnueabihf/), download 'gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabihf.tar.xz' and unpack it. Starting from now, the root folder of this toolchain will be referred to as *{ARM_TOOLCHAIN_ROOT}*
	2. Check the 'Specify options for cross-compiling' radio-button and click 'next'.
	3. Type 'Generic' in the Operating System field, for the C compiler select *{ARM_TOOLCHAIN_ROOT}/bin/arm-linux-gnueabihf-gcc*, for the C++ compiler select *{ARM_TOOLCHAIN_ROOT}/bin/arm-linux-gnueabihf-g++*.
	4. Press 'Finish'
8. Uncheck 'BACNET_STACK_BUILD_APPS', set CMAKE_BUILD_TYPE to 'Debug', press 'Configure' and then press 'Generate'.
9. Run 'make' command inside *{BACNET_STACK_BUILD}*. BACnet Stack Library library is all set. 

### Building 4diac FORTE with the BACnet Client module
1. Download [4diac FORTE 1.12.0 sources](https://www.eclipse.org/downloads/download.php?file=/4diac/releases/1.12/forte/forte-incubation_1.12.0.zip) and unpack them. The root directory of the 4diac FORTE will be referred to as *{FORTE_ROOT}*
2. Create a directory that is going to hold FORTE's external modules. This directory will be referred to as *{FORTE_EXTERNAL_MODULES}*.
3. Go into the *{FORTE_EXTERNAL_MODULES}* directory and clone this repo by executing 'git clone https://github.com/alexandertepaev/forte_bacnet_client_dev.git'.
4. Create a directory where you want to store your FORTE build. The directory will be referred to as *{FORTE_BUILD}*.
5. Run 'cmake-gui'.
6. Set the source code folder to *{FORTE_ROOT}* and the build folder to *{FORTE_BUILD}*.
7. Press 'Configure'
8. CMake will ask you to specify the generator and the compiler. If you plan to use the BACnet Client module on a PC running Linux, leave it as it is, press 'finish', and skip the next step.
9. If you want to run the BACnet Client module on a Raspberry Pi running Raspbian, you have to specify options for cross-compiling.
	1. Get the GCC Linaro Toolchain for ARM. Go [here](https://releases.linaro.org/components/toolchain/binaries/latest-7/arm-linux-gnueabihf/), download 'gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabihf.tar.xz' and unpack it. Starting from now, the root folder of this toolchain will be referred to as *{ARM_TOOLCHAIN_ROOT}*
	2. Check the 'Specify options for cross-compiling' radio-button and click 'next'.
	3. Type 'Generic' in the Operatin System field, for the C compiler select *{ARM_TOOLCHAIN_ROOT}/bin/arm-linux-gnueabihf-gcc*, for the C++ compiler select *{ARM_TOOLCHAIN_ROOT}/bin/arm-linux-gnueabihf-g++*.
	4. Press 'Finish'}
10. Set CMAKE_BUILD_TYPE to 'Debug', set FORT_ARCHITECTURE to 'Posix', check FORTE_IO and FORTE_MODULE_IEC61131, set FORTE_EXTERNAL_MODULES_DIRECTORY to *{FORTE_EXTERNAL_MODULES}* created in Step 3. Press 'Configure'.  
11. Check the FORTE_MODULE_BACnetClient checkbox. Press 'Configure'.
12. Set BACNET_STACK_DIR to *{BACNET_STACK_ROOT}*. Press 'Configure'.
13. Set BACNET_STACK_BUILD_DIR to *{BACNET_STACK_BUILD}*. Press 'Configure' and the press 'Generate'.
14. Run 'make' command inside the *{FORTE_BUILD}* directory. After the compilation process, 'forte' executable can be found in the *{FORTE_BUILD}*/src directory.

### Adding BACnet Client function blocks into the 4diac IDE
1. Download and start [4diac IDE](https://www.eclipse.org/4diac/en_dow.php), it will ask you to choose the workspace directory.
2. After choosing or creating a new workspace directory, copy the 'bacnet_client_fb_type_definitions' folder found in the root of this project into the workspace folder.
3. Start 4diac IDE, BACnet server function block can be now found in the FB Palette.

## Example BACnet Client IEC 61499 Application
![Example Client application for calculating temperature setpoint](./misc/client_app_readme.png)
This figure should provide an example of how a simple BACnet Client IEC 61499 application is built. It represents a simple device that calculates a temperature setpoint for a remote controller. It is configured to be able to issue two ReadProperty requests to two different devices (777 and 778). One device is responsible for providing the value of the outside temperature sensor, whereas the other provides the value of the inside temperature sensor. Based on those two values, it calculates a new setpoint for the temperature controller and sends the computed value to it using WriteProperty service request. This whole thing is performed every 5s.


