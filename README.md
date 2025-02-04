# IOsonata
IOsonata multi-platform multi-architecture optimized software library for fast and easy iot products development

This is the new refactoring of the EHAL library (https://github.com/I-SYST/EHAL).

Although this refactoring includes supports for multiple IDE/Compilers.  The prefered IDE is still Eclipse/GCC.  GCC is the facto standard for embedded software development. Eclipse is 100% free and the most flexible IDE.  It could be little overwhelming for newbies at first (like any other IDE if you are new to it anyway).

For desktop pc version of the library, native compiler and IDE are used.  XCode for OSX, Visual Studio for Windows, Eclipse for Linux.

### IDE limiations :

* Eclipse & GCC : Full C++ supports, full file io supports
* IAR : Full C++ support, no system support for file io.  File io only available with semihosting. Bug in IAR : It cannot debug or flash Nordic nRF series using CMSIS-DAP. 
* uVision : Could not create library compilation properly. To use IOsonata with uVision, you need to add the library sources directly into your firmware project
* CrossWorks : GCC C++ is stripped down to bare bone, no file io supports, no atomic supports and many others. In order to use full GCC C++, CrossWorks must be configured to use with external compiler
* Segger Stusio : Strip down version of CrossWorks.  Even less functional. Only supports jlink, cannot be used with any other jtag. SES is not recommended for heavy firmware development. 


external vendors' SDK and library required :
--- 
 
[nRF5_SDK](https://developer.nordicsemi.com)  : Nordic nRF5x Bluetooth Low Energy

[nrf5_SDK_Mesh](https://www.nordicsemi.com/Software-and-Tools/Software/nRF5-SDK-for-Mesh/Download#infotabs) : Nordic nRF5 SDK for Bluetoth Mesh

[ICM-20948 Motion_Driver](https://www.invensense.com/developers) : Create a user at https://www.invensense.com/developers. Under "Downloads" download "DK-20948 eMD-SmartMotion ...". Unzip the downloaded file and navigate to EMD-Core/sources. Copy the folder Invn to external/Invn as indected in the folder tree bellow.

[BSEC](https://www.bosch-sensortec.com/bst/products/all_products/bsec) : Bosch Sensortec Environmental Cluster (BSEC) Software for #BME680 environmental sensor.  BSEC is needed for calculating Air Quality Index.  Go to https://www.bosch-sensortec.com/bst/products/all_products/bsec at the end of the page.  Select checkbox to accept license terms to download.  Unzip the the downloaded file. Rename the extracted folder BSEC and copy the whole folder to external as indicated in the folder tree bellow.  

![BLUEIO-TAG-EVIM](https://www.i-syst.com/images/BLUEIO-TAG-EVIM_page.png) 
 
<p align="center"> 
  
[Buy : BLUEIO-TAG-EVIM (BLYST Nano sensor board)](https://www.crowdsupply.com/i-syst/blyst-nano).  
[Nordic Thingy App compatible firmware project](https://github.com/IOsonata/IOsonata/tree/master/ARM/Nordic/nRF52/nRF52832/exemples/BlueIOThingy) 
 
</p> 

IOsonata folder structure
---
 
The way the IOsonata folder is structure is simple.  The deeper you go inside the more it is specific the the architecture or platform.  The parent folder contains all that is commonly available to the child folder.  Which means, source file from child folder can access any source in the upper parent folder but not the other way around.  This is the way to keep the abstraction separated from implementation and easier to keep track of things.


```
/your_root     - Development root directory
 |-- external        - Contains downloaded SDKs from silicon vendors
 |   |-- nRF5_SDK        - Latest Nordic SDK (https://developer.nordicsemi.com)
 |   |-- nrf5_SDK_Mesh   - Latest Nordic SDK for Mesh (https://www.nordicsemi.com/eng/nordic/Products/nRF5-SDK-for-Mesh/nRF5-SDK-for-Mesh/62377)
 |   |---nRF5_SDK_12     - Last version of Nordick SDK12 for nRF51 series
 |   |-- BSEC            - Bosch Sensortec Environmental Cluster (BSEC) Software (https://www.bosch-sensortec.com/bst/products/all_products/bsec) for #BME680
 |   |-- Invn            - Invensense SmartMotion Driver (download https://www.invensense.com/developers) 
 |   |   |-- Devices
 |   |   |...
 |   |-- Others as require
 |   |...
 |   |
 |-- IOsonata      - Put the IOsonata here
 |   |-- include     - Generic include common to all platforms
 |   |   |-- bluetooth   - Generic definition for Bluetooth
 |   |   |-- converters  - Generic definition for ADV, DAC, etc...
 |   |   |-- coredev     - Generic definition MCU builtin devices such as i2c, uart, spi, timer, etc...
 |   |   |-- miscdev     - Generic definition for other non categorized devices
 |   |   |-- sensors     - Generic definition for al sort of sensors (environmental, motion, etc...)
 |   |   |-- usb         - Generic definition for USB
 |   |   |...
 |   |-- src         - Generic implementation source common to all platforms
 |   |   |-- bluetooth   - Generic source for Bluetooth
 |   |   |-- converters  - Generic source for ADV, DAC, etc...
 |   |   |-- coredev     - Generic source for MCU builtin devices such as i2c, uart, spi, timer, etc...
 |   |   |-- miscdev     - Generic source for other non categorized devices
 |   |   |-- sensors     - Generic source for al sort of sensors (environmental, motion, etc...)
 |   |   |-- usb         - Generic source for USB
 |   |   |...
 |   |    
 |   |-- ARM         - ARM series based MCU
 |   |   |-- include     - Common include for all ARM platform
 |   |   |-- src         - Common source for all ARM platform
 |   |   |-- DbgConfig   - Debugger configuration files.
 |   |   |-- ldscript    - Linker script files
 |   |   |
 |   |   |-- NXP         - NXP based MCU
 |   |   |   |-- LPC11xx      - LPC11xx series MCU
 |   |   |   |   |-- include     - Common include for this target series
 |   |   |   |   |-- src         - Common source for this target series
 |   |   |   |   |-- LPC11U35    - LPC11U35 target
 |   |   |   |   |   |-- lib        - IOsonata library for this target
 |   |   |   |   |   |   |-- Eclipse   - Eclipse project for this lib
 |   |   |   |   |   |   |-- IAR       - IAR project for this lib
 |   |   |   |   |   |   |-- CrossWorks- CrossWorks project for this lib
 |   |   |   |   |   |   |...
 |   |   |   |   |   |   
 |   |   |   |   |   |-- exemples   - Example projects for this target
 |   |   |   |   |   |   |-- Blink     - Blink example
 |   |   |   |   |   |   |   |-- src      - Source code for this exaple
 |   |   |   |   |   |   |   |-- Eclipse  - Eclipse project for this example
 |   |   |   |   |   |   |   |-- IAR      - IAR project for this example
 |   |   |   |   |   |   |   |-- CrossWorks- CrossWorks project for this example
 |   |   |   |   |   |   |   |...
 |   |   |   |   |   |   |-- Many other examples same
 |   |   |   |   |   |   |
 |   |   |   |-- LPC17xx      - LPC17xx series MCU
 |   |   |   |   |-- include     - Common include for this target series
 |   |   |   |   |-- src         - Common source for this target series
 |   |   |   |   |-- LPC176x     - LPC176x target
 |   |   |   |   |   |-- lib        - IOsonata library for this target
 |   |   |   |   |   |   |-- Eclipse   - Eclipse project for this lib
 |   |   |   |   |   |   |-- IAR       - IAR project for this lib
 |   |   |   |   |   |   |-- CrossWorks- CrossWorks project for this lib
 |   |   |   |   |   |   |...
 |   |   |   |   |   |   
 |   |   |   |   |   |-- exemples   - Example projects for this target
 |   |   |   |   |   |   |-- Blink     - Blink example
 |   |   |   |   |   |   |   |-- src      - Source code for this exaple
 |   |   |   |   |   |   |   |-- Eclipse  - Eclipse project for this example
 |   |   |   |   |   |   |   |-- IAR      - IAR project for this example
 |   |   |   |   |   |   |   |-- CrossWorks- CrossWorks project for this example
 |   |   |   |   |   |   |   |...
 |   |   |   |   |   |   |-- Many other examples same
 |   |   |   |   |   |   |
 |   |   |
 |   |   |-- Nordic      - Nordic Semiconductor based  MCU
 |   |   |   |-- nRF51        - nRF51 series MCU
 |   |   |   |   |-- include     - Common include for this target series
 |   |   |   |   |-- src         - Common source for this target series
 |   |   |   |   |-- lib        - IOsonata library for this target
 |   |   |   |   |   |-- Eclipse   - Eclipse project for this lib
 |   |   |   |   |   |-- IAR       - IAR project for this lib
 |   |   |   |   |   |-- CrossWorks- CrossWorks project for this lib
 |   |   |   |   |   |...
 |   |   |   |   |   
 |   |   |   |   |-- exemples   - Example projects for this target
 |   |   |   |   |   |-- Blink     - Blink example
 |   |   |   |   |   |   |-- src      - Source code for this exaple
 |   |   |   |   |   |   |-- Eclipse  - Eclipse project for this example
 |   |   |   |   |   |   |-- IAR      - IAR project for this example
 |   |   |   |   |   |   |-- CrossWorks- CrossWorks project for this example
 |   |   |   |   |   |   |   |...
 |   |   |   |   |   |-- Many other examples same
 |   |   |   |   |
 |   |   |   |-- nRF52        - nRF52 serie MCU
 |   |   |   |   |-- include     - Common include for this target series
 |   |   |   |   |-- src         - Common source for this target series
 |   |   |   |   |-- nRF52832    - Target MCU
 |   |   |   |   |   |-- lib        - IOsonata library for this target
 |   |   |   |   |   |   |-- Eclipse   - Eclipse project for this lib
 |   |   |   |   |   |   |-- IAR       - IAR project for this lib
 |   |   |   |   |   |   |-- CrossWorks- CrossWorks project for this lib
 |   |   |   |   |   |   |...
 |   |   |   |   |   |   
 |   |   |   |   |   |-- exemples   - Example projects for this target
 |   |   |   |   |   |   |-- Blink     - Blink example
 |   |   |   |   |   |   |   |-- src      - Source code for this exaple
 |   |   |   |   |   |   |   |-- Eclipse  - Eclipse project for this example
 |   |   |   |   |   |   |   |-- IAR      - IAR project for this example
 |   |   |   |   |   |   |   |-- CrossWorks- CrossWorks project for this example
 |   |   |   |   |   |   |   |...
 |   |   |   |   |   |   |-- Many other examples same
 |   |   |   |   |   |   |
 |   |   |   |   |-- nRF52840    - Target MCU
 |   |   |   |   |   |-- lib        - IOsonata library for this target
 |   |   |   |   |   |   |-- Eclipse   - Eclipse project for this lib
 |   |   |   |   |   |   |-- IAR       - IAR project for this lib
 |   |   |   |   |   |   |-- CrossWorks- CrossWorks project for this lib
 |   |   |   |   |   |   |...
 |   |   |   |   |   |   
 |   |   |   |   |   |-- exemples   - Example projects for this target
 |   |   |   |   |   |   |-- Blink     - Blink example
 |   |   |   |   |   |   |   |-- src      - Source code for this exaple
 |   |   |   |   |   |   |   |-- Eclipse  - Eclipse project for this example
 |   |   |   |   |   |   |   |-- IAR      - IAR project for this example
 |   |   |   |   |   |   |   |-- CrossWorks- CrossWorks project for this example
 |   |   |   |   |   |   |   |...
 |   |   |   |   |   |   |-- Many other examples same
 |   |   |   |
 |   |   |   |-- nRF91        - Target series MCU
 |   |   |   |   |-- include     - Common include for this target series
 |   |   |   |   |-- src         - Common source for this target series
 |   |   |   |   |-- nRF9160     - Target MCU
 |   |   |   |   |   |-- lib        - IOsonata library for this target
 |   |   |   |   |   |   |-- Eclipse   - Eclipse project for this lib
 |   |   |   |   |   |   |-- IAR       - IAR project for this lib
 |   |   |   |   |   |   |-- CrossWorks- CrossWorks project for this lib
 |   |   |   |   |   |   |...
 |   |   |   |   |   |   
 |   |   |   |   |   |-- exemples   - Example projects for this target
 |   |   |   |   |   |   |-- Blink     - Blink example
 |   |   |   |   |   |   |   |-- src      - Source code for this exaple
 |   |   |   |   |   |   |   |-- Eclipse  - Eclipse project for this example
 |   |   |   |   |   |   |   |-- IAR      - IAR project for this example
 |   |   |   |   |   |   |   |-- CrossWorks- CrossWorks project for this example
 |   |   |   |   |   |   |   |...
 |   |   |   |   |   |   |-- Many other examples same
 |   |   |   |   |   |   |
 |   |   |
 |   |   |-- ST          - ST based MCU
 |   |   |   |-- STM32F0xx
 |   |   |   |-- STM32F4xx
 |   |   |   |-- STM32L0xx
 |   |   |   |-- STM32L1xx
 |   |   |   |-- STM32L4xx
 |   |   |   |   |-- include     - Common include for this target series
 |   |   |   |   |-- src         - Common source for this target series
 |   |   |   |   |-- STM32L476      - Target MCU
 |   |   |   |   |   |-- lib        - IOsonata library for this target
 |   |   |   |   |   |   |-- Eclipse   - Eclipse project for this lib
 |   |   |   |   |   |   |-- IAR       - IAR project for this lib
 |   |   |   |   |   |   |-- CrossWorks- CrossWorks project for this lib
 |   |   |   |   |   |   |...
 |   |   |   |   |   |   
 |   |   |   |   |   |-- exemples   - Example projects for this target
 |   |   |   |   |   |   |-- Blink     - Blink example
 |   |   |   |   |   |   |   |-- src      - Source code for this exaple
 |   |   |   |   |   |   |   |-- Eclipse  - Eclipse project for this example
 |   |   |   |   |   |   |   |-- IAR      - IAR project for this example
 |   |   |   |   |   |   |   |-- CrossWorks- CrossWorks project for this example
 |   |   |   |   |   |   |   |...
 |   |   |   |   |   |   |-- Many other examples same
 |   |   |   |   |   |   |
 |   |   |
 |   |   |-- Other silicon vendors
 |   |...
 |   |-- Linux
 |   |   |...
 |   |-- OSX
 |   |   |...
 |   |-- Win
 |   |   |...
 | ...
```
 