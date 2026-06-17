# UART Firmware Update

This sample tests firmware updates on Seven over UART using Zephyr, MCUmgr, and
MCUboot.

The sample uses Zephyr's `smp_svr` application. UART is used for the shell and
MCUmgr shell transport. Logs are routed through the shell backend and can be
enabled or disabled from the UART shell before running MCUmgr.

## Hardware

- [Seven](https://github.com/id8-engineering/seven-hardware)
- USB-UART adapter connected to Seven UART
- [J-Link PLUS Compact](https://shop.segger.com/debug-trace-probes/debug-probes/j-link/j-link-plus-compact) for the initial flash

## Prerequisites

See prerequisites [here](https://github.com/id8-engineering/seven-firmware-samples/blob/main/docs/PREREQUISITES.md).

Install the MCUmgr CLI:

```bash
go install github.com/apache/mynewt-mcumgr-cli/mcumgr@latest
export PATH="$PATH:$HOME/go/bin"
```

Check that the tool is available:

```bash
mcumgr version
```

## Build & Run

Build sample:

```bash
west build --sysbuild -p always -b seven/nrf9151/ns \
  ../zephyr/samples/subsys/mgmt/mcumgr/smp_svr -- \
  -DEXTRA_CONF_FILE=$(pwd)/samples/uart_update/conf/mcumgr_shell.conf \
  -Dmcuboot_EXTRA_CONF_FILE=$(pwd)/samples/uart_update/sysbuild/mcuboot.conf
```

Flash the initial image:

```bash
west flash
```

Open UART shell:

```bash
minicom -D /dev/ttyUSB0 -b 115200
```

Expected output:

>```text
>*** Booting nRF Connect SDK v3.2.1-d8887f6f32df ***
>*** Using Zephyr OS v4.2.99-ec78104f1569 ***
>[00:00:00.253,997] <inf> smp_sample: build time: Jun 16 2026 16:38:36
>uart:~$ 
>```

Enable logs when debugging:

```text
log enable inf
```

Disable logs before using MCUmgr:

```text
log disable
```

Exit minicom before running MCUmgr:

```text
Ctrl-A X
```

## Firmware Update

Check the current images:

```bash
mcumgr --conntype serial --connstring '/dev/ttyUSB0,baud=115200' image list
```

Expected output:

>```text
>Images:
> image=0 slot=0
>    version: 0.0.0
>    bootable: true
>    flags: active confirmed
>    hash: 4572bdd6fdb39ae28371c5b8dcb7e915638e087179327f2907a9ccaad59c5bc0
>Split status: N/A (0)
>```

Build a new image:

```bash
west build --sysbuild -p always -b seven/nrf9151/ns \
  ../zephyr/samples/subsys/mgmt/mcumgr/smp_svr -- \
  -DEXTRA_CONF_FILE=$(pwd)/samples/uart_update/conf/mcumgr_shell.conf \
  -Dmcuboot_EXTRA_CONF_FILE=$(pwd)/samples/uart_update/sysbuild/mcuboot.conf
```

Upload the signed image:

```bash
mcumgr --conntype serial --connstring '/dev/ttyUSB0,baud=115200' image upload build/smp_svr/zephyr/zephyr.signed.bin
```

List images again:

```bash
mcumgr --conntype serial --connstring '/dev/ttyUSB0,baud=115200' image list
```

Expected output:

>```text
>Images:
> image=0 slot=0
>    version: 0.0.0
>    bootable: true
>    flags: active confirmed
>    hash: 4572bdd6fdb39ae28371c5b8dcb7e915638e087179327f2907a9ccaad59c5bc0
> image=0 slot=1
>    version: 0.0.0
>    bootable: true
>    flags:
>    hash: 12e64055e13a820fc6bc8df98bb2a0d33aaf53a5ebb2e5cb08298d396cc258b9
>Split status: N/A (0)
>```

Mark the uploaded slot 1 image as pending:

```bash
mcumgr --conntype serial --connstring '/dev/ttyUSB0,baud=115200' image test <slot-1-hash>
```

Example:

```bash
mcumgr --conntype serial --connstring '/dev/ttyUSB0,baud=115200' image test 12e64055e13a820fc6bc8df98bb2a0d33aaf53a5ebb2e5cb08298d396cc258b9
```

Reset into the new image:

```bash
mcumgr --conntype serial --connstring '/dev/ttyUSB0,baud=115200' reset
```

Check that the new image is active:

```bash
mcumgr --conntype serial --connstring '/dev/ttyUSB0,baud=115200' image list
```

Expected output:

>```text
>Images:
> image=0 slot=0
>    version: 0.0.0
>    bootable: true
>    flags: active
>    hash: 12e64055e13a820fc6bc8df98bb2a0d33aaf53a5ebb2e5cb08298d396cc258b9
> image=0 slot=1
>    version: 0.0.0
>    bootable: true
>    flags:
>    hash: 4572bdd6fdb39ae28371c5b8dcb7e915638e087179327f2907a9ccaad59c5bc0
>Split status: N/A (0)
>```

Confirm the active image:

```bash
mcumgr --conntype serial --connstring '/dev/ttyUSB0,baud=115200' image confirm <active-hash>
```

Example:

```bash
mcumgr --conntype serial --connstring '/dev/ttyUSB0,baud=115200' image confirm 12e64055e13a820fc6bc8df98bb2a0d33aaf53a5ebb2e5cb08298d396cc258b9
```

Verify that the active image is confirmed:

```bash
mcumgr --conntype serial --connstring '/dev/ttyUSB0,baud=115200' image list
```

Expected output:

>```text
>Images:
> image=0 slot=0
>    version: 0.0.0
>    bootable: true
>    flags: active confirmed
>    hash: 12e64055e13a820fc6bc8df98bb2a0d33aaf53a5ebb2e5cb08298d396cc258b9
> image=0 slot=1
>    version: 0.0.0
>    bootable: true
>    flags:
>    hash: 4572bdd6fdb39ae28371c5b8dcb7e915638e087179327f2907a9ccaad59c5bc0
>Split status: N/A (0)
>```

## Notes

The UART must be quiet while MCUmgr is running. Before using MCUmgr, disable
logs and close minicom:

```text
log disable
```

Every new firmware image must keep MCUmgr and MCUboot support enabled. If an
updated application removes the MCUmgr server, future UART updates will require
recovery through SWD/J-Link.
