# seven-firmware-samples

Seven is a rapid‑prototyping platform for low‑cost, low‑power connected
IoT devices with integrated LTE cellular connectivity.

Designed for teams moving from concept to deployment, Seven bridges the gap
between traditional development boards and finalized products—making it ideal
for pilot projects, field trials, and early production runs.

Specification:

* Nordic Semiconductor nRF9151 SiP with integrated LTE-M/NB-IoT modem and GPS
  * 64 MHz Arm Cortex-M33
  * 256 KiB RAM
  * 1 MiB flash
* 6-36 V input voltage
* 2 x mikroBUS™ sockets for easy expansion with Click Boards
* RGB LED
* Passive buzzer
* Programming interface (SWD) via Raspberry Pi Debug Probe
* Debug interface (UART) via Raspberry Pi Debug Probe
* Industrial temperature range (-40 to 85 Celsius)

The software stack is built on the Zephyr Project, providing a robust, secure,
and scalable RTOS foundation. It includes ready‑to‑use modules for over‑the‑air
(OTA) updates and cloud connectivity, with primary support for AWS IoT.

By adopting the widely used mikroBUS™ add‑on standard, Seven offers access to
thousands of off‑the‑shelf Click Boards. This makes it easy to extend the
platform with sensors (motion, proximity, temperature, etc.) or interfaces such
as RS‑232, RS‑485, CAN, and more—without custom hardware design.

Seven is open source and open hardware, giving developers complete transparency
and control to customize, manufacture, and evolve their products without vendor
lock‑in.

---

This repository contains sample applications and tutorials for the Seven board.

## Prerequisites

See [docs/PREREQUISITES.md](docs/PREREQUISITES.md), which is common prerequisites for all samples in this repository.

### Build and flash sample

Build and flash Zephyr project and NCS samples:

* See all samples from Zephyr [here](https://docs.zephyrproject.org/latest/samples/index.html).
* See all samples from NCS [here](https://nrfconnectdocs.nordicsemi.com/ncs/latest/nrf/samples.html)

To build applications/samples on Seven use this commando and build the sample
you want to build:

Zephyr:

```bash
west build -p always -b seven/nrf9151/ns ../zephyr/samples/drivers/led/led_strip/
```

NCS:

```bash
west build -p always -b seven/nrf9151/ns ../nrf/samples/cellular/at_client/
```

Flash:

```bash
west flash
```

Debug:

To debug on Seven we use UART, connect a JST-SH cable including in the
Raspberry Pi Debug Probe package and connect it from Probes UART to Sevens UART
header.

To see the output use a serial monitor, for example `minicom` or `screen`. See
examples down below:

### Linux

```bash
minicom -D /dev/ttyACM0 -b 115200
```

### macOS example

```bash
minicom -D /dev/cu.usbmodem1101 -b 115200
```
