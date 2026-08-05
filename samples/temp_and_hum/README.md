# Temperature and Humidity

This sample reads temperature and relative humidity from an HTS221 sensor on the
Temp&Hum Click board.

## Hardware

- [Seven](https://github.com/id8-engineering/seven-hardware)
- [Temp&Hum Click board connected to Seven mikroBUS socket 1](https://www.mikroe.com/temp-hum-click)

## Prerequisites

All commands should be run from repository root.

See [docs/PREREQUISITES.md](../../docs/PREREQUISITES.md), which is common prerequisites for all samples in this repository.

## Build & Run

Build sample:

```bash
west build -p always -b seven/nrf9151/ns ../zephyr/samples/sensor/hts221 -- \
  -DEXTRA_DTC_OVERLAY_FILE=$(pwd)/samples/temp_and_hum/boards/seven_nrf9151_ns.overlay
```

Flash sample:

```bash
west flash
```

Debug:

```bash
west rtt
```

Expected output:

>```text
>SEGGER J-Link V9.48 - Real time terminal output
>SEGGER J-Link V13.0, SN=853001439
>Process: JLinkGDBServerCLExe
>*** Booting nRF Connect SDK v3.2.1-d8887f6f32df ***
>*** Using Zephyr OS v4.2.99-ec78104f1569 ***
>Observation:1
>Temperature:25.0 C
>Relative Humidity:54.0%
>Observation:2
>Temperature:24.1 C
>Relative Humidity:46.5%
>Observation:3
>Temperature:24.0 C
>```
