# ECM1900 Tools

## Overview

The ECM1900 Tools package provides a suite of applications to assist users
of the [ECM1900](https://opalkelly.com/products/ecm1900) integration module.
These applications are baked into our provided Linux images and can be used
from the command line after boot. 

These tools include:

- `syzygy-ecm1900` - An application implementing the SYZYGY Standard for
the ECM1900 paired with the BRK1900. Also used to individually set the voltage 
of the VIO (VCCO) power rails on the ECM1900.

- `set-clock-ecm1900` - Application used to configure the clock frequencies on the
Si5341 programmable clock generator on the ECM1900.

- `set-clock-brk1900` - Application used to configure the clock frequencies on the
Si5338 programmable clock generator on the BRK1900.

### syzygy-ecm1900 Application

The `syzygy-ecm1900` application is an implementation of the [SYZYGY Standard](https://syzygyfpga.io/)
for the ECM1900 development board by Opal Kelly. This application
handles all communication with peripheral MCU's and the on-board power
supplies to set the appropriate VIO (VCCO) voltages. This application can also be
used to read/write binary DNA blob files from/to a peripheral MCU. 

This application also provides commands to set the VIO (VCCO) voltages manually.

Usage information is available by running `syzygy-ecm1900 -h`
```
Usage: syzygy-ecm1900 [option [argument]] <i2c device>
 <i2c device> is required for all commands. It must contain the
 path to the Linux i2c device. '/dev/i2c-0' should be used on the ECM1900
 
  Exactly one of the following options must be specified:
    -r - run smartVIO, queries attached MCU's and sets voltages accordingly
    -s - set VIO voltages to the values provided by -1, -2, -3, -4 options (Can only set one at a time)
    -j - print out a JSON object with DNA and SmartVIO information
    -h - print this help text
    -w <filename> - write a binary DNA to a peripheral, takes the DNA filename
                    as an argument
    -d <filename> - dump the DNA from a peripheral to a binary file, takes the
                    DNA filename as an argument
                    
  The following options may be used in conjunction with the above options:
    -1 <vio1> - Sets the voltage for VIO1(VCCO_87_88)
    -2 <vio2> - Sets the voltage for VIO2(VCCO_68)
    -3 <vio3> - Sets the voltage for VIO3(VCCO_67)
    -4 <vio4> - Sets the voltage for VIO4(VCCO_28)
                  *<vioX> must be specified as numbers in 10's of mV
                  *You may only set one voltage at a time
                  *The valid discrete voltage supplies provided by the power supply on the ECM1900 are:
                  VIO1: 120,  125,  150,  180,  250, 330 (Limited by HD bank range 1.2V to 3.3V)
                  VIO2: 120,  125,  150,  180 (Limited by HP bank range 1.0V to 1.8V)
                  VIO3: 120,  125,  150,  180 (Limited by HP bank range 1.0V to 1.8V)
                  VIO4: 120,  125,  150,  180 (Limited by HP bank range 1.0V to 1.8V)
    -p <number> - Specifies the peripheral number for the -w or -d options
    
  Examples:
    Run SmartVIO sequence:
      syzygy-ecm1900 -r /dev/i2c-0
    Dump DNA from the MCU on Port 1:
      syzygy-ecm1900 -d dna_file.bin -p 1 /dev/i2c-0
    Set VIO1 to 3.3V:
      syzygy-ecm1900 -s -1 330 /dev/i2c-0
```

### set-clock-ecm1900 Application

The `set-clock-ecm1900` application, along with the `ECM1900-Si5341-Regs.h` ClockBuilder Pro 
configuration register export header file, applies the default clock settings for the Si5341
on the ECM1900. The default clock frequencies set by this header file can be found in the ECM1900
[Clock Generator](https://docs.opalkelly.com/ecm1900/clock-generator/)
documentation.
```
Usage: set-clock-ecm1900 [i2c device]
Example: set-clock-ecm1900 /dev/i2c-0
```
New clock settings can be applied by re-generating the ClockBuilder Pro configuration 
register export header file with new output frequency configuration settings. The SiLabs [ClockBuilder Pro](https://www.silabs.com/developers/clockbuilder-pro-software) 
software can be downloaded at the provided link. Please use our provided 
ClockBuilder Pro project file found in the ECM1900 Clock Generator documentation page, apply changes for the desired 
output frequencies, and re-generate the configuration register export header file. The 
application expects this header to be named `ECM1900-Si5341-Regs.h`.
Simply replace the current `ECM1900-Si5341-Regs.h` header file with
the new header file containing the new register settings and run the provided makefile.
These build sources can be found in the `/home/root/tools` directory of the provided linux images.

Note: The provided Opal Kelly linux image for the ECM1900 has the `set-clock-ecm1900` application 
along with the default `ECM1900-Si5341-Regs.h` baked in. This application is run in the boot 
process to configure the clocks at startup. If you require new configuration settings to be run 
at startup, follow the instructions located at [Linux Image](https://docs.opalkelly.com/ecm1900/linux-image/)

### set-clock-brk1900 Application

The `set-clock-brk1900` application, along with the `BRK1900-Si5338-Regs.h` ClockBuilder Pro 
configuration register export header file, applies the default clock settings for the Si5338 
on the ECM1900. The default clock frequencies set by this header file can be found under 
‘Si5338 Programmable Clock’ at [BRK1900 Peripherals](https://docs.opalkelly.com/ecm1900/brk1900-breakout-board/brk1900-peripherals/)
```
Usage: set-clock-brk1900 [i2c device]
Example: set-clock-brk1900 /dev/i2c-0
```
New clock settings can be applied by re-generating the ClockBuilder Pro configuration 
register export header file with new output frequency configuration settings. The SiLabs [ClockBuilder Pro](https://www.silabs.com/developers/clockbuilder-pro-software) 
software can be downloaded at the provided link. Please use our provided 
ClockBuilder Pro project file found in the BRK1900 Peripherals documentation page, apply changes for the desired 
output frequencies, and re-generate the configuration register export header file. The 
application expects this header to be named `BRK1900-Si5338-Regs.h`.
Simply replace the current `BRK1900-Si5338-Regs.h` header file with
the new header file containing the new register settings and run the provided makefile.
These build sources can be found in the `/home/root/tools` directory of the provided linux images.

Note: The provided Opal Kelly linux image for the BRK1900 has the `set-clock-brk1900` application 
along with the default `BRK1900-Si5338-Regs.h` baked in. This application is run in the boot 
process to configure the clocks at startup. If you require new configuration settings to be run 
at startup, follow the instructions located at [BRK1900 Linux Image](https://docs.opalkelly.com/ecm1900/brk1900-breakout-board/brk1900-linux-image/)

## Building

Makefiles are provided to assist with building the applications in this repository. Simply run the 
"make" command on a system with the necessary development tools installed to build all provided 
applications. Note that the `syzygy-ecm1900` application requires C++ 11 support. The JSON library 
used by `syzygy-ecm1900` requires GCC 4.9 or later.

Our provided Linux images contain the necessary build tools to run the makefiles. The `set-clock-ecm1900` 
and `set-clock-brk1900` build sources located in this repository can be found in our Linux images at 
the `/home/root/tools` directory. You can easily apply the desired frequency configuration and run the 
makefile to rebuild the application. 

