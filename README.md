See below for new VGM mode for Video Game Module as Logic analyzer

Please start with the Getting Started page : https://github.com/pico-coder/sigrok-pico/blob/main/GettingStarted.md
/////////////////////////////////////////////////////////////////
Building this repo:
Bbuilding is not recommended, but some people insist on trying.....)

I once did a cross compile of PulseView for windows.  The instructions and installer are left in place for historical reference, they are likely out of date.
Instructions to download my build are here https://github.com/pico-coder/sigrok-pico/blob/main/pulseview/Readme.md
Again, please use the main sigrok installer.

For linux, many people have managed to combine my pull request into a libsigrok build. If you are on linux it's probably a practical experiment to try.  4GB of RAM is recommended for pulseview builds to avoid disk swap issues.  See SigrokBuildNotes.md

#
# sigrok-pico
Use a raspberry pi pico (rp2040) as a logic analyzer and oscilloscope with sigrok.
This implementation uses the pico SDK CDC serial library to communicate with sigrok-cli/pulseview through a sigrok driver.

## Directories:

pico_pgen is a simple digital function generator useful for creating patterns to test.

pico_sdk_sigrok is the pico sdk C code for the PICO RP2040 device.

The latest libsigrok code exists as a fork at https://github.com/pico-coder/libsigrok

## Files
PICOBuildNotes.md - build notes for building the PICO device assuming you have gone through the PICO C SDK "getting started with PICO".

SigrokBuildNotes.md - rough libsigrok build notes which will be depracated once raspberrypi_pico is mainline

GettingStarted.md - quick run down on setting things up.

AnalyzerDetails.md - details on supported modes of the analyzer and various limitations.

SerialProtocol.md - details of the "over the wire" protocol used between the sigrok driver and the device.

## Recent Changes (VGM Adaptation Branch)

The following enhancements have been made to support Video Game Module (VGM) hardware:

### New VGM Mode (Mode 3)
- Added a new VGM mode alongside the existing baseline, digital-26, and digital-32 modes
- VGM mode provides 18 digital channels and 3 analog channels
- Specialized pin mapping for VGM hardware including HDMI connectivity

### Pin Configuration Enhancements
- **GP0,1**: Debug UART TX/RX
- **GP2-GP5**: IMU SPI interface
- **GP6-GP7**: IMU sidebands
- **GP8-GP15**: LVDS output on HDMI port
- **GP16,17,22,23**: Digital inputs via connector
- **GP18,19,20**: I2C and CEC to HDMI port
- **GP21,24,25**: RGB LED control
- **GP26-29**: ADC analog inputs via connector

### ADC and Channel Management
- Implemented proper ADC channel configuration with reduced channel count to prevent startup hangs
- Fixed ADC enabling and channel mapping for accurate analog signal capture
- Added support for 4 analog channels in VGM mode (GP26-29)

### Bug Fixes
- Fixed pin mapping issues affecting digital and analog channel readings
- Resolved issues where some pins were always reading as 1
- Stabilized all GPIO pins to properly acquire signals

### Implementation Details
The changes primarily affect the `pico_sdk_sigrok/` directory:
- Device configuration in `sr_device.h` with mode definitions and pin assignments
- Driver implementation in `pico_sdk_sigrok.c` with hardware interface logic
- Device initialization in `sr_device.c` with proper ADC and GPIO setup
