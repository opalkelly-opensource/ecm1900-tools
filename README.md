# ECM1900 Tools

## Overview

The ECM1900 Tools package provides a suite of applications to assist users
of the [ECM1900](https://opalkelly.com/products/ecm1900) integration module.

These tools include:

- smartvio-ecm - An application implementing SmartVIO functionality for
the Brain-1.

- si5341-ecm - Application used to configure the clock outputs from the
Si5340 on the ECM1900.

- si5338-ecm - Application used to configure the clock outputs from the
Si5338 on the BRK1900.

### SmartVIO ECM1900 Application

The `smartvio-ecm` application is an implementation of the SmartVIO library
for the ECM1900 development board by Opal Kelly. This application
handles all communication with peripheral MCU's and the on-board power
supplies to set the appropriate VIO voltages. This application can also be
used to read/write binary DNA blob files from/to a peripheral MCU.

Usage information is available by running `smartvio -h`

### Si5341 Configuration Application

The `si5341-ecm` application along with the `ECM1900-Si5341-Regs.h` ClockBuilder Pro 
configuration register export header file applies the default clock settings for the Si5341
on the ECM1900. The default clock settings set by this header file can be found in the ECM1900
[Clock Generator](https://docs.opalkelly.com/display/ECM1900/Clock+Generator)
documentation.

New clock settings can be applied by re-generating the ClockBuilder Pro configuration 
register export header file with new output frequency configuration settings. The SiLabs [ClockBuilder Pro](https://www.silabs.com/developers/clockbuilder-pro-software) 
software can be downloaded at the provided link. Please use our provided 
ClockBuilder Pro project file found in the ECM1900 documentation, apply changes for the desired 
output frequencies, and re-generate the configuration register export header file. The 
application expects this header to be named `ECM1900-Si5341-Regs.h`.
Simply replace the current `ECM1900-Si5341-Regs.h` header file with
the new header file containing the new register settings.
 
Usage: si5341-ecm [i2c device]
Example: si5341-ecm /dev/i2c-0

Note: The provided Opal Kelly linux image for the ECM1900 has the `si5341-ecm` application along 
with the default `ECM1900-Si5341-Regs.h` baked in. If you require different settings 
then you will need to recompile the application with a new register export header file.
This can be accomplished by use of gcc provided in the Opal Kelly linux image. Then you can run 
this recompiled application from the command line with the usage above. 

### Si5338 Configuration Application

The `si5338-ecm` application applies a set of clock settings for the Si5338
on the BRK1900. This application works in a similar manner to the Si5341
application and can be modified with different clock settings using the same
approach.


## Building

A Makefile is provided to assist with building this design. Simply run the
"make" command on a system with the necessary development tools installed to
build all provided applications. Note that the smartvio-ecm application
requires C++ 11 support. The JSON library used by smartvio-ecm requires
GCC 4.9 or later.

Individual applications can be built by running `make <application>`.

This build has been tested on a machine running Ubuntu 16.04 LTS with
GCC 5.4.0.
