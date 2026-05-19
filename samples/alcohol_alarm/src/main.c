// Copyright (c) 2026 ID8 Engineering AB
// SPDX-License-Identifier: Apache-2.0

#include <errno.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/drivers/led_strip.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/device.h>
#include <zephyr/display/cfb.h>
#include <zephyr/drivers/display.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

#define ADC_NODE            DT_PATH(zephyr_user)
#define PPM_THRESHOLD       100U
#define MQ3_MV_TO_PPM_SCALE 0.03846f
#define LED_ON              50U
#define LED_OFF             0U
#define BUZZER_FREQ_HZ      2000U
#define NS_PER_SEC          1000000000U
#define BUZZER_PERIOD_NS    (NS_PER_SEC / BUZZER_FREQ_HZ)
#define BUZZER_PULSE_NS     (BUZZER_PERIOD_NS / 2U)

static const struct adc_dt_spec mikrobus_2_an = ADC_DT_SPEC_GET_BY_NAME(ADC_NODE, mikrobus_2_an);
static const struct device *const oled = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
static const struct device *led_strip = DEVICE_DT_GET(DT_CHOSEN(zephyr_led_strip));
static const struct pwm_dt_spec buzzer = PWM_DT_SPEC_GET(DT_NODELABEL(buzzer));

static int init_adc(struct adc_sequence *sequence, uint16_t *raw)
{
	if (!adc_is_ready_dt(&mikrobus_2_an)) {
		LOG_ERR("ADC device is not ready");
		return -ENODEV;
	}

	sequence->buffer = raw;
	sequence->buffer_size = sizeof(*raw);

	if (adc_channel_setup_dt(&mikrobus_2_an) != 0) {
		LOG_ERR("Could not setup ADC channel");
		return -EIO;
	}

	if (adc_sequence_init_dt(&mikrobus_2_an, sequence) != 0) {
		LOG_ERR("Could not init ADC sequence");
		return -EIO;
	}

	return 0;
}

static int init_oled(void)
{
	if (!device_is_ready(oled)) {
		LOG_ERR("OLED device is not ready");
		return -ENODEV;
	}

	if (display_set_pixel_format(oled, PIXEL_FORMAT_MONO10) != 0 &&
	    display_set_pixel_format(oled, PIXEL_FORMAT_MONO01) != 0) {
		LOG_ERR("Could not set OLED pixel format");
		return -ENOTSUP;
	}

	if (cfb_framebuffer_init(oled) != 0) {
		LOG_ERR("Could not init OLED framebuffer");
		return -EIO;
	}

	if (cfb_framebuffer_invert(oled) != 0) {
		LOG_ERR("Could not invert OLED framebuffer");
		return -EIO;
	}

	if (display_blanking_off(oled) != 0) {
		LOG_ERR("Could not unblank OLED");
		return -EIO;
	}

	return 0;
}

static int read_adc(const struct adc_sequence *sequence, uint16_t *raw, int32_t *mv)
{
	*raw = 0;

	if (adc_read_dt(&mikrobus_2_an, sequence) != 0) {
		LOG_ERR("Could not read ADC");
		return -EIO;
	}

	*mv = *raw;
	if (adc_raw_to_millivolts_dt(&mikrobus_2_an, mv) != 0) {
		LOG_ERR("Could not convert ADC to mV");
		return -EIO;
	}

	return 0;
}

static int display_value(uint32_t ppm)
{
	char text[16];
	uint8_t font_w;
	uint8_t font_h;
	int width;
	int height;
	int len;

	width = cfb_get_display_parameter(oled, CFB_DISPLAY_WIDTH);
	height = cfb_get_display_parameter(oled, CFB_DISPLAY_HEIGHT);
	if (width <= 0 || height <= 0) {
		LOG_ERR("Could not get OLED size");
		return -EIO;
	}

	if (cfb_get_font_size(oled, 0, &font_w, &font_h) != 0) {
		LOG_ERR("Could not get OLED font size");
		return -EIO;
	}

	len = snprintk(text, sizeof(text), "PPM: %u", ppm);
	if (len < 0) {
		LOG_ERR("Could not format OLED text");
		return -EIO;
	}

	if (cfb_framebuffer_clear(oled, false) != 0) {
		LOG_ERR("Could not clear OLED framebuffer");
		return -EIO;
	}

	if (cfb_print(oled, text, (uint16_t)((width - (len * font_w)) / 2),
		      (uint16_t)((height - font_h) / 2)) != 0) {
		LOG_ERR("Could not print to OLED");
		return -EIO;
	}

	return cfb_framebuffer_finalize(oled);
}

int main(void)
{
	int ret;
	uint16_t raw = 0;
	int32_t mv;
	uint32_t ppm;
	struct adc_sequence sequence = {0};
	uint32_t pulse_ns = 0U;

	struct led_rgb pixel = {
		.r = LED_OFF,
		.g = LED_ON,
		.b = LED_OFF,
	};

	if (!device_is_ready(led_strip)) {
		LOG_ERR("LED strip device is not ready");
		return -ENODEV;
	}

	if (!pwm_is_ready_dt(&buzzer)) {
		LOG_ERR("Buzzer device is not ready");
		return -ENODEV;
	}

	ret = init_adc(&sequence, &raw);
	if (ret < 0) {
		LOG_ERR("init_adc failed: %d", ret);
		return 0;
	}

	ret = init_oled();
	if (ret < 0) {
		LOG_ERR("init_oled failed: %d", ret);
		return 0;
	}

	ret = led_strip_update_rgb(led_strip, &pixel, 1);
	if (ret < 0) {
		LOG_ERR("Initial LED update failed: %d", ret);
	}

	while (1) {
		ret = read_adc(&sequence, &raw, &mv);
		if (ret < 0) {
			LOG_ERR("read_adc failed: %d", ret);
			k_sleep(K_SECONDS(1));
			continue;
		}

		ppm = (uint32_t)((float)mv * MQ3_MV_TO_PPM_SCALE + 0.5f);
		LOG_INF("ADC raw=%u mv=%d ppm=%u", raw, mv, ppm);

		if (PPM_THRESHOLD < ppm) {
			pulse_ns = (pulse_ns == 0U) ? BUZZER_PULSE_NS : 0U;
			ret = pwm_set_dt(&buzzer, BUZZER_PERIOD_NS, pulse_ns);
			if (ret < 0) {
				LOG_ERR("Buzzer update failed: %d", ret);
			}

			pixel.r = (pixel.r == LED_OFF) ? LED_ON : LED_OFF;
			pixel.g = LED_OFF;
			ret = led_strip_update_rgb(led_strip, &pixel, 1);
			if (ret < 0) {
				LOG_ERR("LED update failed: %d", ret);
			}
		} else {
			pulse_ns = 0U;
			ret = pwm_set_dt(&buzzer, BUZZER_PERIOD_NS, 0U);
			if (ret < 0) {
				LOG_ERR("Buzzer update failed: %d", ret);
			}
			pixel.r = LED_OFF;
			pixel.g = LED_ON;
			ret = led_strip_update_rgb(led_strip, &pixel, 1);
			if (ret < 0) {
				LOG_ERR("LED update failed: %d", ret);
			}
		}

		ret = display_value(ppm);
		if (ret < 0) {
			LOG_ERR("OLED update failed: %d", ret);
		}
		k_sleep(K_SECONDS(1));
	}

	return 0;
}
