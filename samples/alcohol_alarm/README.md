# Alcohol Alarm

This sample reads the analog output from an MQ-3 alcohol sensor, converts the
ADC value to a rough PPM estimate, and shows the result on an OLED display.

Below the configured threshold, the onboard RGB LED is green and the buzzer is
off. Above the threshold, the LED flashes red and the buzzer sounds at 2 kHz.
The application samples the sensor once per second and logs the raw ADC value,
millivolts, and estimated PPM.

> [!NOTE]
> The PPM calculation is a simple linear approximation. MQ-3 sensors require
> warm-up and calibration for the intended environment and measurement range.

## Hardware

- A Seven board
- An [OLED B Click](https://www.mikroe.com/oled-b-click) installed in mikroBUS
  socket 1
- An [Alcohol Click](https://www.mikroe.com/alcohol-click) installed in
  mikroBUS socket 2

## Prerequisites

Complete the toolchain, workspace, and hardware setup in the root
[Getting started guide](../../README.md#requirements). Run the commands below
from the `seven-firmware-samples` repository root.

## Build

```bash {"cwd":"../..","name":"build-alcohol-alarm"}
west build -p always -b seven/nrf9151/ns samples/alcohol_alarm
```

## Flash

```bash
west flash
```

## View the output

Open the UART port at 115200 baud. For example, on Ubuntu:

```bash
minicom -D /dev/ttyACM0 -b 115200
```

On macOS, use the corresponding `/dev/cu.usbmodem*` port as described in the
[getting started guide](../../README.md#6-view-the-output).

Expected application output:

>```text
>[00:00:00.396,636] <inf> main: ADC raw=3203 mv=2815 ppm=108
>[00:00:01.447,967] <inf> main: ADC raw=3366 mv=2958 ppm=114
>[00:00:02.499,298] <inf> main: ADC raw=3161 mv=2778 ppm=107
>```
