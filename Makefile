.DEFAULT_GOAL := help

.PHONY: sample-hello-world
## sample-hello-world  Build hello world sample
sample-hello-world:
	west build --sysbuild -p always -b seven/nrf9151/ns ../zephyr/samples/hello_world

.PHONY: sample-blinky
## sample-blinky       Build LED strip blinky sample
sample-blinky:
	west build --sysbuild -p always -b seven/nrf9151/ns ../zephyr/samples/drivers/led/led_strip

.PHONY: sample-at-client
## sample-at-client    Build modem AT client sample
sample-at-client:
	west build --sysbuild -p always -b seven/nrf9151/ns ../nrf/samples/cellular/at_client/ -- -DCONF_FILE=$$PWD/samples/conf/at_client.conf

.PHONY: firmware-pre-nrfcredstore
## firmware-pre-nrfcredstore   Build firmware to initialize modem for nrfcredstore usage
firmware-pre-nrfcredstore:
	west build --sysbuild -p always -b seven/nrf9151/ns firmware/pre_nrfcredstore

.PHONY: sample-all
## sample-all         Build all sample-* targets
sample-all: sample-hello-world sample-blinky sample-at-client

.PHONY : help
help : Makefile
	@sed -n 's/^##//p' $<
