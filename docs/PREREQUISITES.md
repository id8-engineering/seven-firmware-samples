# Prerequisites

This document collects the tools and workspace setup needed before building the
Seven firmware samples. Follow it once on a new development machine, then use
the individual sample README files for sample-specific setup and commands.

## Required hardware and software

Prior to development you will need to acquire the J-Link PLUS Compact to be able
to flash and debug over RTT. You will also need to install nRF Util to use NCS
(nRF Connect SDK).

* [J-Link PLUS Compact](https://shop.segger.com/debug-trace-probes/debug-probes/j-link/j-link-plus-compact)
* [ARM20-CTX 20-Pin to TC2030-IDC Adapter for Cortex](https://www.tag-connect.com/product/arm20-ctx-20-pin-to-tc2030-idc-adapter-for-cortex)
* [TC2030 Cable](https://www.tag-connect.com/product/tc2030-idc-nl)
* [TC2030 Clip](https://www.tag-connect.com/product/tc2030-retaining-clip-board-3-pack)
* USB-C cable
* Install [nRF Util](https://docs.nordicsemi.com/bundle/nrfutil/page/guides/installing.html)
>[!NOTE]
>When you enter nRF Util install link please look at there own prerequisites.

Prior to flashing and using Seven you need to connect a few things to get the
board to work.

1. Assemble all the parts for the JLink PLUS compact, it should look like this:

>```text
>Computer
>  └─ USB-C cable
>      └─ J-Link PLUS Compact
>        └─ TC2030-IDC adapter
>            └─ TC2030-IDC cable
>              └─ Seven TC2030 footprint
>                  └─ TC2030 clip
>```


2. Connect external power supply to Seven:
>[!NOTE]
> Sevens supported power supply level is 6-36V but it is fully possible
> to connect 5V to the power supply contact.

### Supported platforms

We support a tested and verified development path on the following platforms:

* Ubuntu 26.04
* macOS 26.5

## Setup nRF Connect development environment

Install nRF Connect SDK manager:

```bash
nrfutil install sdk-manager
```

Install nRF Connect SDK:

```bash
nrfutil sdk-manager install v3.2.1
```

Start the nRF Connect SDK toolchain shell:

```bash
nrfutil sdk-manager toolchain launch --ncs-version v3.2.1 --shell
```

If the command succeeds, your shell prompt will change to something like:

<details>
<summary>Linux</summary>

>```bash
>(v3.2.1) [user@host ~]$
>```

</details>

<details>
<summary>macOS</summary>

>```bash
>(v3.2.1) hostname%
>```

</details>

All remaining commands in this guide should be run inside the `nrfutil` shell.

## Set up workspace

Create a new workspace and enter it:

```bash
mkdir -p ~/src/seven-firmware-samples-workspace
cd ~/src/seven-firmware-samples-workspace
```

Initialize the workspace:

```bash
west init -m https://github.com/id8-engineering/seven-firmware-samples --mr main .
```

Change into the project directory:

```bash
cd seven-firmware-samples
```

Fetch and check out sources:

```bash
west update
```

Now your workspace is ready. Go to one of the firmware sample directories and
follow the instructions there.
