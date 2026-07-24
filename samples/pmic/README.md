# PMIC

This application controls PMIC load switch 1 on Seven.
Load switch 1 is connected to `VDD_GNSS`.

The purpose is to turn off `VDD_GNSS` when GNSS is not used. This reduces power
consumption because the GNSS antenna rail can still draw power when an antenna
is connected, even if GNSS is not actively used.

By default, this application turns `VDD_GNSS` off.

## Description

To use the PMIC from firmware, `spi1` must be disabled because `i2c1` and
`spi1` share the same nRF9151 serial block.

The onboard RGB LED uses `spi1`, so the RGB LED is not available while this
application is using the PMIC.

## Build

```bash
west build --sysbuild -p always -b seven/nrf9151/ns samples/pmic
```

## Flash

```bash
west flash
```

## Usage

In `src/main.c`, change:

```c
regulator_disable(vdd_gnss);
```

to:

```c
regulator_enable(vdd_gnss);
```
