# Getting started with Seven

[Seven](https://seven.id8-engineering.io) is an open-source platform for
prototyping low-power cellular IoT devices with the Nordic Semiconductor
nRF9151 and Zephyr RTOS.

This guide takes you from a new development environment to a running Zephyr
Hello World application. You will install the nRF Connect SDK, create a west
workspace, build and flash the application, and verify its output over UART.

## Requirements

### Hardware

- A Seven board
- A [Raspberry Pi Debug Probe](https://www.raspberrypi.com/products/debug-probe/)
- A USB cable for the debug probe
- A 6–36 V DC power supply for Seven

The debug probe provides the SWD programming connection and the UART console
connection. The board design and other hardware resources are available in the
[Seven hardware repository](https://github.com/id8-engineering/seven-hardware).

### Development machine

The tested development platforms are:

- Ubuntu 26.04
- macOS 26.5

Install the following software before continuing:

- [nRF Util](https://docs.nordicsemi.com/bundle/nrfutil/page/guides/installing.html)
- A serial terminal such as `minicom`

The nRF Util installation page lists any platform-specific prerequisites.

## 1. Connect the hardware

1. Connect the debug probe's SWD cable to the `DEBUG` connector on Seven.
2. Connect the debug probe's UART cable to the `UART` connector on Seven.
3. Connect the debug probe to your development machine over USB.
4. Connect a 6–36 V DC power supply to Seven.

## 2. Install the nRF Connect SDK

Install the SDK Manager command for nRF Util:

```bash {"name":"install-sdk-manager"}
nrfutil install sdk-manager
```

If SDK Manager is already installed, update it:

```bash {"name":"upgrade-sdk-manager"}
nrfutil upgrade sdk-manager
```

Install the nRF Connect SDK version used by this repository:

```bash {"name":"install-ncs"}
nrfutil sdk-manager install v3.4.0
```

Start a shell with the matching toolchain:

```bash {"excludeFromRunAll":true,"name":"launch-toolchain-shell"}
nrfutil sdk-manager toolchain launch --ncs-version v3.4.0 --shell
```

Run all remaining commands in this guide from that shell.

## 3. Create the workspace

Create and enter a new workspace directory:

```bash {"name":"create-workspace","tag":"setup-workspace"}
mkdir -p ~/src/seven-firmware-samples-workspace && cd ~/src/seven-firmware-samples-workspace
```

Initialize the west workspace from this repository:

```bash {"name":"initialize-workspace","tag":"setup-workspace"}
west init -m https://github.com/id8-engineering/seven-firmware-samples --mr main .
```

Enter the repository and fetch the nRF Connect SDK and Seven board support:

```bash {"name":"update-workspace","tag":"setup-workspace"}
cd seven-firmware-samples
west update
```

The repository directory is the workspace's application root. Run the build
commands below from this directory.

## 4. Build Hello World

Build Zephyr's Hello World sample for Seven's non-secure nRF9151 target:

```bash {"name":"build-hello-world"}
west build -p always -b seven/nrf9151/ns ../zephyr/samples/hello_world
```

## 5. Flash the board

Flash the application through the debug probe:

```bash
west flash
```

## 6. View the output

Open the UART port at 115200 baud. The port name differs by operating system.

On Ubuntu:

```bash
minicom -D /dev/ttyACM0 -b 115200
```

On macOS:

```bash
minicom -D /dev/cu.usbmodem1101 -b 115200
```

The exact port suffix can vary. When the application starts, the output
includes:

>```text
>Hello World! seven/nrf9151/ns
>```

You have now built, flashed, and run your first application on Seven.

## Next: explore cellular connectivity

Seven's defining feature is its integrated LTE-M/NB-IoT modem. A useful next
step is the nRF Connect SDK
[AT Client sample](https://docs.nordicsemi.com/bundle/ncs-latest/page/nrf/samples/cellular/at_client/README.html),
which exposes the modem's AT command interface over UART.

Build it from the repository root:

```bash {"name":"build-at-client"}
west build -p always -b seven/nrf9151/ns ../nrf/samples/cellular/at_client
```

Then flash it with `west flash` and follow the sample documentation to interact
with the modem. More examples are available in the
[nRF Connect SDK cellular samples](https://docs.nordicsemi.com/bundle/ncs-latest/page/nrf/samples/cellular.html).

## Seven samples

These samples demonstrate Seven's onboard peripherals and mikroBUS expansion
sockets:

- [Temperature & Humidity](samples/temp_and_hum/README.md) reads an HTS221
  sensor using a Temp&Hum Click board.
- [Alcohol Alarm](samples/alcohol_alarm/README.md) combines an analog alcohol
  sensor, OLED display, RGB LED, and buzzer.

You can also use the wider collections of
[Zephyr samples](https://docs.zephyrproject.org/latest/samples/index.html) and
[nRF Connect SDK samples](https://docs.nordicsemi.com/bundle/ncs-latest/page/nrf/samples.html)
with Seven. Use `seven/nrf9151/ns` as the board target and check each sample's
hardware and configuration requirements.

## Contributing and license

Contributions are welcome. See [CONTRIBUTING.md](CONTRIBUTING.md) for the pull
request and commit requirements.

This repository is licensed under the [Apache License 2.0](LICENSE).
