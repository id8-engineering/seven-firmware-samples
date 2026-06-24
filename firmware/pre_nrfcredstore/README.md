# Pre-nrfcredstore

Nrfcredstore is a CLI-tool to write credentials to the modem inside the nRF9151 
module.

Before you are able to run `nrfcredstore` commands you need to initialize the 
modem inside nRF9151 SiP. 

>[!NOTE]
> Flashing any firmware where the modem gets initialized and then goes online
> will not let you write to the modem with `nrfcredstore`

## Hardware

* [USB to Serial port cabel](https://www.electrokit.com/kabel-usb/ttl-4-pin-vcc/rx/tx/gnd3.3v-hona?gad_source=1&gad_campaignid=17338847491&gclid=CjwKCAjwgO7RBhBKEiwAZNP85jHslA3N3mMAQ7rdPzuHm0boKi6o4WtuyYjld4kXO83Urs0CLf9bxhoCQ34QAvD_BwE)

## Prerequisites

All commands should be run from repository root.

See [docs/PREREQUISITES.md](../../docs/PREREQUISITES.md), which is common prerequisites for all samples 
in this repository.

## Build & Run

This firmware is available to build with make:

```bash
make firmware-pre-nrfcredstore
```

Flash firmware:

```bash
west flash
```

Debug:

```bash
west rtt
```

## Nrfcredstore commands

Before you are able to talk to the modem, you will need to connect you USB cable
to Sevens onboard UART pinheader.

Install nrfcredstore:

```bash
pip3 install nrfcredstore
```

List already written credentials:

```bash
nrfcredstore --cmd-type at --rtscts-off /dev/ttyUSB0 list
```

Writing credentials for different applications will not be the same, here is a
example on how to write credentials for an AWS application:

```bash
nrfcredstore --cmd-type at --rtscts-off /dev/ttyUSB0 write 123 ROOT_CA_CERT ~/Downloads/AmazonRootCA1.pem
nrfcredstore --cmd-type at --rtscts-off /dev/ttyUSB0 write 123 CLIENT_CERT ~/Downloads/device-certificate.pem.crt
nrfcredstore --cmd-type at --rtscts-off /dev/ttyUSB0 write 123 CLIENT_KEY ~/Downloads/private-key.pem.key
```

Nordic uses a tag called **Security tag** and in Zephyr it´s called SEC_TAG, you
can have different tags for multiple applications. On the example above you will
see **123** as the Sec-tag.
