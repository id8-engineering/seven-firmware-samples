#include <errno.h>

#include <zephyr/device.h>
#include <zephyr/drivers/regulator.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(pmic, LOG_LEVEL_INF);

#define VDD_GNSS_NODE DT_NODELABEL(vdd_gnss)

int main(void)
{
	const struct device *const vdd_gnss = DEVICE_DT_GET(VDD_GNSS_NODE);
	int err;

	if (!device_is_ready(vdd_gnss)) {
		LOG_ERR("VDD_GNSS regulator device is not ready");
		return -ENODEV;
	}

	LOG_INF("Disabling/Enabling VDD_GNSS");

	err = regulator_disable(
		vdd_gnss); // Change function to regulator_enable() to enable VDD_GNSS again
	if (err != 0) {
		LOG_ERR("Failed to disable/enable VDD_GNSS: %d", err);
		return err;
	}

	LOG_INF("VDD_GNSS disabled/enabled");

	while (1) {
		k_sleep(K_SECONDS(1));
	}
}
