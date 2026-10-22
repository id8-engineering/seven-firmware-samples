# Temperature & Humidity

This sample reads temperature and relative humidity from an HTS221 sensor on a
Temp&Hum Click board.

## Hardware

- A [Seven board](https://github.com/id8-engineering/seven-hardware)
- A [Temp&Hum Click board](https://www.mikroe.com/temp-hum-click) installed in
  mikroBUS socket 1

## Prerequisites

Complete the toolchain, workspace, and hardware setup in the root
[Getting started guide](../../README.md#requirements). Run the commands below
from the `seven-firmware-samples` repository root.

## Build

Build Zephyr's HTS221 sample with the Seven-specific devicetree overlay:

```bash
west build -p always -b seven/nrf9151/ns ../zephyr/samples/sensor/hts221 -- \
  -DEXTRA_DTC_OVERLAY_FILE=$(pwd)/samples/temp_and_hum/boards/seven_nrf9151_ns.overlay
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
>Observation:1
>Temperature:25.0 C
>Relative Humidity:54.0%
>Observation:2
>Temperature:24.1 C
>Relative Humidity:46.5%
>```
