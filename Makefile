.DEFAULT_GOAL := help

.PHONY: sample-hello-world
## sample-hello-world  Build hello world sample
sample-hello-world:
	west build --sysbuild -p always -b seven/nrf9151/ns ../zephyr/samples/hello_world

.PHONY: sample-blinky
## sample-blinky       Build LED strip blinky sample
sample-blinky:
	west build --sysbuild -p always -b seven/nrf9151/ns ../zephyr/samples/drivers/led/led_strip

.PHONY : help
help : Makefile
	@sed -n 's/^##//p' $<
