# Alcohol_alarm

Reads the analog output from an MQ-3 alcohol sensor, converts the ADC value to
a rough PPM estimate, and shows the result on an OLED display.

When the estimated PPM rises above the configured threshold, the sample also
drives the on-board RGB LED and buzzer as visual and audible alarm outputs.

The sample reads the `AN` signal from the Alcohol Click through the ADC once
per second, logs the raw ADC value together with the converted millivolts and
estimated PPM, and shows the current value centered on the OLED as
`PPM: <value>`. Below the threshold, the RGB LED is set to green and the
buzzer is off. Above the threshold, the sample toggles the RGB LED red and
toggles the buzzer at 2 kHz to indicate an alarm condition.

The current PPM calculation is a simple linear conversion from the measured
millivolt value and should be treated as an approximation. MQ-3 sensors
typically need warm-up and calibration for the intended environment and range.

## Hardware:

- [OLED B Click](https://www.mikroe.com/oled-b-click) on mikroBUS 1
- [Alcohol Click](https://www.mikroe.com/alcohol-click) on mikroBUS 2
- Seven board with the `seven/nrf9151/ns` target

## Tutorial 

### Get started

Connect click boards like this:

* OLED click board -> MikroBUS socket *1*
* Alcohol click board -> MikroBUS socket *2*

### Build and flash sample

Build:

```bash
west build -p always  -b seven/nrf9151/ns samples/alcohol_alarm
```

Flash:

```bash
west flash
```

Debug:

```bash
west rtt
```

Example log:

```bash
SEGGER J-Link V9.48 - Real time terminal output
SEGGER J-Link V13.0, SN=853001439
Process: JLinkGDBServerCLExe
*** Booting nRF Connect SDK v3.2.1-d8887f6f32df ***
*** Using Zephyr OS v4.2.99-ec78104f1569 ***
[00:00:00.396,636] <inf> main: ADC raw=3203 mv=2815 ppm=108
[00:00:01.447,967] <inf> main: ADC raw=3366 mv=2958 ppm=114
[00:00:02.499,298] <inf> main: ADC raw=3161 mv=2778 ppm=107
[00:00:03.550,628] <inf> main: ADC raw=3377 mv=2968 ppm=114
[00:00:04.601,989] <inf> main: ADC raw=3177 mv=2792 ppm=107
[00:00:05.653,320] <inf> main: ADC raw=3431 mv=3015 ppm=116
[00:00:06.704,589] <inf> main: ADC raw=3271 mv=2874 ppm=111
[00:00:07.755,859] <inf> main: ADC raw=3450 mv=3032 ppm=117
[00:00:08.807,189] <inf> main: ADC raw=3394 mv=2983 ppm=115
[00:00:09.858,459] <inf> main: ADC raw=4095 mv=3599 ppm=138
```
