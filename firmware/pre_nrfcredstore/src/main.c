// Copyright (c) 2026 ID8 Engineering AB
// SPDX-License-Identifier: Apache-2.0

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <modem/nrf_modem_lib.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

int main(void)
{
	int err;
	err = nrf_modem_lib_init();
	if (err) {
		LOG_ERR("Modem library initialization failed: %d", err);
		return 0;
	}
}
