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
* Programming interface (SWD) via TAG connector
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

Build Zephyr Project samples, see samples in [Makefile](Makefile/):

```bash
make sample-hello-world
```

Flash Seven:

```bash
west flash
```

Use West RTT to access Seven serial console:

```bash
west rtt
```

Sample output should look like:

>```text
>*** Booting nRF Connect SDK v3.2.1-d8887f6f32df ***
>*** Using Zephyr OS v4.2.99-ec78104f1569 ***
>Hello World! seven/nrf9151
>```

To see all available samples run:

```bash
make help
```
