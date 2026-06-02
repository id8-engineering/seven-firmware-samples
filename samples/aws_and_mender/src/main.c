// Copyright (c) 2026 ID8 Engineering AB
// SPDX-License-Identifier: Apache-2.0
// This sample is largly based on
// https://github.com/nrfconnect/sdk-nrf/tree/main/samples/net/aws_iot

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/reboot.h>

#include <net/aws_iot.h>

#include <modem/lte_lc.h>
#include <modem/nrf_modem_lib.h>

#include <mender/client.h>
#include <mender/zephyr-image-update-module.h>
#include <cJSON.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

static void aws_publish_work_fn(struct k_work *work);
static void aws_connect_work_fn(struct k_work *work);
static void aws_iot_event_handler(const struct aws_iot_evt *const evt);

static K_WORK_DELAYABLE_DEFINE(aws_publish_work, aws_publish_work_fn);
static K_WORK_DELAYABLE_DEFINE(aws_connect_work, aws_connect_work_fn);

#define AWS_TOPIC_INPUT              CONFIG_AWS_IOT_CLIENT_ID_STATIC "/input"
#define AWS_TOPIC_OUTPUT             CONFIG_AWS_IOT_CLIENT_ID_STATIC "/output"
#define AWS_PUBLISH_INTERVAL_SECONDS 10
#define AWS_CONNECTION_RETRY_SECONDS 5
#define MENDER_POLL_INTERVAL_SECONDS 60

static mender_identity_t mender_identity = {
	.name = "device_id",
	.value = CONFIG_MENDER_DEVICE_TYPE,
};

static bool aws_connected;

static int aws_app_topics_subscribe(void)
{
	static const struct mqtt_topic topic_list[] = {
		{
			.topic =
				{
					.utf8 = AWS_TOPIC_INPUT,
					.size = sizeof(AWS_TOPIC_INPUT) - 1,
				},
			.qos = MQTT_QOS_1_AT_LEAST_ONCE,
		},
	};

	return aws_iot_application_topics_set(topic_list, ARRAY_SIZE(topic_list));
}

static int aws_client_init(void)
{
	int err;

	err = aws_iot_init(aws_iot_event_handler);
	if (err) {
		return err;
	}

	return aws_app_topics_subscribe();
}

static void aws_publish_message(void)
{
	cJSON *json;
	char message[sizeof("Hello from !") + sizeof(CONFIG_AWS_IOT_CLIENT_ID_STATIC)];
	char payload[256];
	struct aws_iot_data tx_data = {
		.qos = MQTT_QOS_0_AT_MOST_ONCE,
		.topic =
			{
				.str = AWS_TOPIC_OUTPUT,
				.len = sizeof(AWS_TOPIC_OUTPUT) - 1,
			},
	};
	int err;

	json = cJSON_CreateObject();
	if (json == NULL) {
		LOG_ERR("Unable to create AWS JSON payload");
		return;
	}

	snprintk(message, sizeof(message), "Hello from %s!", CONFIG_AWS_IOT_CLIENT_ID_STATIC);

	if (cJSON_AddStringToObject(json, "message", message) == NULL) {
		LOG_ERR("Unable to add AWS message to payload");
		cJSON_Delete(json);
		return;
	}

	if (!cJSON_PrintPreallocated(json, payload, sizeof(payload), false)) {
		LOG_ERR("Unable to format AWS JSON payload");
		cJSON_Delete(json);
		return;
	}

	tx_data.ptr = payload;
	tx_data.len = strlen(payload);

	err = aws_iot_send(&tx_data);
	cJSON_Delete(json);

	if (err) {
		LOG_ERR("AWS publish failed: %d", err);
		return;
	}

	LOG_INF("Published \"%s\" to \"%s\"", payload, AWS_TOPIC_OUTPUT);
}

static void aws_publish_work_fn(struct k_work *work)
{
	ARG_UNUSED(work);

	if (!aws_connected) {
		return;
	}

	aws_publish_message();

	(void)k_work_reschedule(&aws_publish_work, K_SECONDS(AWS_PUBLISH_INTERVAL_SECONDS));
}

static void aws_connect_work_fn(struct k_work *work)
{
	int err;

	ARG_UNUSED(work);

	LOG_INF("Connecting to AWS IoT");

	err = aws_iot_connect(NULL);
	if (err == -EAGAIN) {
		(void)k_work_reschedule(&aws_connect_work, K_SECONDS(AWS_CONNECTION_RETRY_SECONDS));
		return;
	}

	if (err) {
		LOG_ERR("AWS connect failed: %d", err);
		(void)k_work_reschedule(&aws_connect_work, K_SECONDS(AWS_CONNECTION_RETRY_SECONDS));
	}
}

static void aws_on_connected(const struct aws_iot_evt *const evt)
{
	aws_connected = true;
	(void)k_work_cancel_delayable(&aws_connect_work);

	if (evt->data.persistent_session) {
		LOG_WRN("AWS persistent session is enabled");
	}

	(void)k_work_reschedule(&aws_publish_work, K_NO_WAIT);
}

static void aws_on_disconnected(void)
{
	aws_connected = false;
	(void)k_work_cancel_delayable(&aws_publish_work);
	(void)k_work_reschedule(&aws_connect_work, K_SECONDS(AWS_CONNECTION_RETRY_SECONDS));
}

static void aws_iot_event_handler(const struct aws_iot_evt *const evt)
{
	switch (evt->type) {
	case AWS_IOT_EVT_CONNECTING:
		LOG_INF("AWS_IOT_EVT_CONNECTING");
		break;
	case AWS_IOT_EVT_CONNECTED:
		LOG_INF("AWS_IOT_EVT_CONNECTED");
		aws_on_connected(evt);
		break;
	case AWS_IOT_EVT_DISCONNECTED:
		LOG_INF("AWS_IOT_EVT_DISCONNECTED");
		aws_on_disconnected();
		break;
	case AWS_IOT_EVT_DATA_RECEIVED:
		LOG_INF("Received AWS message: \"%.*s\" on topic \"%.*s\"", evt->data.msg.len,
			evt->data.msg.ptr, evt->data.msg.topic.len, evt->data.msg.topic.str);
		break;
	case AWS_IOT_EVT_PUBACK:
		LOG_INF("AWS_IOT_EVT_PUBACK, message ID: %d", evt->data.message_id);
		break;
	case AWS_IOT_EVT_PINGRESP:
		break;
	case AWS_IOT_EVT_ERROR:
		LOG_ERR("AWS_IOT_EVT_ERROR: %d", evt->data.err);
		aws_on_disconnected();
		break;
	default:
		break;
	}
}

static mender_err_t mender_network_connect_cb(void)
{
	return MENDER_OK;
}

static mender_err_t mender_network_release_cb(void)
{
	return MENDER_OK;
}

static mender_err_t mender_deployment_status_cb(mender_deployment_status_t status, const char *desc)
{
	LOG_INF("Mender deployment status: %s", desc);
	ARG_UNUSED(status);
	return MENDER_OK;
}

static mender_err_t mender_restart_cb(void)
{
	LOG_WRN("Mender requested reboot; waiting 5s for logs");
	k_sleep(K_SECONDS(5));
	sys_reboot(SYS_REBOOT_COLD);
	return MENDER_OK;
}

static mender_err_t mender_get_identity_cb(const mender_identity_t **identity)
{
	*identity = &mender_identity;
	return MENDER_OK;
}

int main(void)
{
	int err;

	LOG_INF("Starting LTE");

	err = nrf_modem_lib_init();
	if (err < 0) {
		LOG_ERR("Modem init failed: %d", err);
		return err;
	}

	err = lte_lc_connect();
	if (err < 0) {
		LOG_ERR("LTE connect failed: %d", err);
		return err;
	}

	LOG_INF("LTE connected");

	mender_client_config_t mender_config = {
		.recommissioning = false,
		.update_poll_interval = MENDER_POLL_INTERVAL_SECONDS,
	};

	mender_client_callbacks_t mender_callbacks = {
		.network_connect = mender_network_connect_cb,
		.network_release = mender_network_release_cb,
		.deployment_status = mender_deployment_status_cb,
		.restart = mender_restart_cb,
		.get_identity = mender_get_identity_cb,
	};

	if (mender_client_init(&mender_config, &mender_callbacks) != MENDER_OK) {
		LOG_ERR("Mender init failed");
		return -EIO;
	}

#ifdef CONFIG_MENDER_ZEPHYR_IMAGE_UPDATE_MODULE
	if (mender_zephyr_image_register_update_module() != MENDER_OK) {
		LOG_ERR("Mender update module registration failed");
		return err;
	}
#endif

	if (mender_client_activate() != MENDER_OK) {
		LOG_ERR("Mender activate failed");
		return err;
	}

	LOG_INF("Mender activated");
	LOG_INF("Starting AWS");

	err = aws_client_init();
	if (err) {
		LOG_ERR("AWS init failed: %d", err);
		return err;
	}

	(void)k_work_reschedule(&aws_connect_work, K_NO_WAIT);

	while (1) {
		k_sleep(K_SECONDS(10));
	}

	return 0;
}
