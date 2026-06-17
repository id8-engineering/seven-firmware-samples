# Prerequisites

This document collects the tools and workspace setup needed before building the
Seven firmware samples. Follow it once on a new development machine, then use
the individual sample README files for sample-specific setup and commands.

## Required hardware and software

Prior to development you will need to acquire the J-Link PLUS Compact to be able
to flash and debug over RTT. You will also need to install nRF Util to use NCS
(nRF Connect SDK).

* [J-Link PLUS Compact](https://shop.segger.com/debug-trace-probes/debug-probes/j-link/j-link-plus-compact)
* Install [nRF Util](https://docs.nordicsemi.com/bundle/nrfutil/page/guides/installing.html)

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

```bash
(v3.2.1) [user@host ~]$
```

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
