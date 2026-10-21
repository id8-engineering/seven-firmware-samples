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

See [docs/PREREQUISITES.md](docs/PREREQUISITES.md), which is common
prerequisites for all samples in this repository.

## Upstream samples

There are 100s of samples and demos available from Zephyr and nRF Connect SDK.

* [Zephyr Project - Samples and Demos](https://docs.zephyrproject.org/latest/samples/index.html).
* [nRF Connect SDK - Samples](https://nrfconnectdocs.nordicsemi.com/ncs/latest/nrf/samples.html)

### Upstream hello world sample

Example to build [Zephyr Project - Hello World](https://docs.zephyrproject.org/latest/samples/hello_world/README.html):

Build the sample:

```bash
west build -p always -b seven/nrf9151/ns ../zephyr/samples/hello_world
```

> [!NOTE]
> To build a nRF Connect SDK sample run:
>
> ```bash
> west build -p always -b seven/nrf9151/ns ../nrf/samples/<sample>
> ```

Flash the sample to the board:

```bash
west flash
```

To view the console output use a terminal emulator, e.g `minicom`.

Example on Linux:

>```bash
>minicom -D /dev/ttyACM0 -b 115200
>*** Booting Zephyr OS build v4.5.0-rc1-492-ge35b686be50a ***
>Hello World! seven/nrf9151/ns
>```

Example on MacOS:

>```bash
>minicom -D /dev/cu.usbmodem1101 -b 115200
>*** Booting Zephyr OS build v4.5.0-rc1-492-ge35b686be50a ***
>Hello World! seven/nrf9151/ns
>```

## Seven samples

These are samples tailored for the Seven board and developed by us.

* [Temperature & Humidity sample](samples/temp_and_hum/README.md)
* [Alcohol Alarm sample](samples/alcohol_alarm)

## License

This repository is licensed under the [Apache License 2.0](LICENSE).
