#include "ble_init.h"
#include "ble.h"
#include "ble_err.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "nrf_sdh_soc.h"
#include "nrf_ble_gatt.h"
#include "nrf_ble_qwr.h"
#include "app_timer.h"
#include "light.h"
#include "ble_dev_cfg_svc.h"
#include "ble_lora_cfg_svc.h"
#include "ble_param_cfg_svc.h"
#include "sys_param.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#define APP_BLE_OBSERVER_PRIO           3                                       /**< Application's BLE observer priority. You shouldn't need to modify this value. */
#define APP_BLE_CONN_CFG_TAG            1                                       /**< A tag identifying the SoftDevice BLE configuration. */

/* �㲥��� */
#define APP_ADV_INTERVAL(x)             MSEC_TO_UNITS(x, UNIT_0_625_MS)			/**< The advertising interval (in units of 0.625 ms; this value corresponds to 40 ms). */
#define APP_ADV_DURATION                BLE_GAP_ADV_TIMEOUT_GENERAL_UNLIMITED   /**< The advertising time-out (in units of seconds). When set to 0, we will never time out. */

/* ���Ӳ��� */
#define MIN_CONN_INTERVAL(x)            MSEC_TO_UNITS(x, UNIT_1_25_MS)        	/**< Minimum acceptable connection interval */
#define MAX_CONN_INTERVAL(x)            MSEC_TO_UNITS(x, UNIT_1_25_MS)        	/**< Maximum acceptable connection interval */
#define SLAVE_LATENCY                   0                                       /**< Slave latency. */
#define CONN_SUP_TIMEOUT(x)             MSEC_TO_UNITS(x, UNIT_10_MS)         	/**< Connection supervisory time-out (4 seconds). */

#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(20000)                  /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (15 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(5000)                   /**< Time between each call to sd_ble_gap_conn_param_update after the first call (5 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT    3                                       /**< Number of attempts before giving up the connection parameter negotiation. */

BLE_DEV_CFG_DEF(m_dev_cfg);                                                     /**< LED Button Service instance. */
BLE_LORA_CFG_DEF(m_lora_cfg);                                                     /**< LED Button Service instance. */
BLE_PARAM_CFG_DEF(m_ble_cfg);                                                     /**< LED Button Service instance. */
NRF_BLE_GATT_DEF(m_gatt);                                                       /**< GATT module instance. */
NRF_BLE_QWR_DEF(m_qwr);                                                         /**< Context for the Queued Write module.*/

typedef struct {
	uint16_t adv_interval;
	uint16_t min_conn_interval;
	uint16_t max_conn_interval;
	uint16_t conn_sup_timeout;
}ble_param_t;

static uint16_t m_conn_handle = BLE_CONN_HANDLE_INVALID;                        /**< Handle of the current connection. */
static uint8_t m_adv_handle = BLE_GAP_ADV_SET_HANDLE_NOT_SET;                   /**< Advertising handle used to identify an advertising set. */
static uint8_t m_enc_advdata[BLE_GAP_ADV_SET_DATA_SIZE_MAX];                    /**< Buffer for storing an encoded advertising set. */
static uint8_t m_enc_scan_response_data[BLE_GAP_ADV_SET_DATA_SIZE_MAX];         /**< Buffer for storing an encoded scan data. */
static int8_t ble_rssi = 0;
static ble_param_t ble_param;

__weak void ble_conn_handler(void){}
__weak void ble_disconn_handler(void){}

/**@brief Struct that contains pointers to the encoded advertising data. */
static ble_gap_adv_data_t m_adv_data =
{
    .adv_data =
    {
        .p_data = m_enc_advdata,
        .len    = BLE_GAP_ADV_SET_DATA_SIZE_MAX
    },
    .scan_rsp_data =
    {
        .p_data = m_enc_scan_response_data,
        .len    = BLE_GAP_ADV_SET_DATA_SIZE_MAX

    }
};

static void ble_param_characteristic_tx_power_update(uint8_t value)
{
	ret_code_t err_code;
	ble_gatts_value_t ble_gatts_value = {
		.len = 1,
		.offset = 0,
		.p_value = &value,
	};
	err_code = sd_ble_gatts_value_set(m_conn_handle, m_ble_cfg.param_tx_power_char_handles.value_handle, &ble_gatts_value);
	APP_ERROR_CHECK(err_code);
}

static void ble_param_characteristic_adv_interval_update(uint16_t value)
{
	ret_code_t err_code;
	ble_gatts_value_t ble_gatts_value = {
		.len = 2,
		.offset = 0,
		.p_value = (uint8_t*)&value,
	};
	err_code = sd_ble_gatts_value_set(m_conn_handle, m_ble_cfg.param_adv_interval_char_handles.value_handle, &ble_gatts_value);
	APP_ERROR_CHECK(err_code);
}

static void ble_param_characteristic_adv_time_update(uint16_t value)
{
	ret_code_t err_code;
	ble_gatts_value_t ble_gatts_value = {
		.len = 2,
		.offset = 0,
		.p_value = (uint8_t*)&value,
	};
	err_code = sd_ble_gatts_value_set(m_conn_handle, m_ble_cfg.param_adv_time_char_handles.value_handle, &ble_gatts_value);
	APP_ERROR_CHECK(err_code);
}

static void ble_param_characteristic_min_conn_interval_update(uint16_t value)
{
	ret_code_t err_code;
	ble_gatts_value_t ble_gatts_value = {
		.len = 2,
		.offset = 0,
		.p_value = (uint8_t*)&value,
	};
	err_code = sd_ble_gatts_value_set(m_conn_handle, m_ble_cfg.param_min_conn_interval_char_handles.value_handle, &ble_gatts_value);
	APP_ERROR_CHECK(err_code);
}

static void ble_param_characteristic_max_conn_interval_update(uint16_t value)
{
	ret_code_t err_code;
	ble_gatts_value_t ble_gatts_value = {
		.len = 2,
		.offset = 0,
		.p_value = (uint8_t*)&value,
	};
	err_code = sd_ble_gatts_value_set(m_conn_handle, m_ble_cfg.param_max_conn_interval_char_handles.value_handle, &ble_gatts_value);
	APP_ERROR_CHECK(err_code);
}

static void ble_param_characteristic_slave_latency_update(uint16_t value)
{
	ret_code_t err_code;
	ble_gatts_value_t ble_gatts_value = {
		.len = 2,
		.offset = 0,
		.p_value = (uint8_t*)&value,
	};
	err_code = sd_ble_gatts_value_set(m_conn_handle, m_ble_cfg.param_slave_latency_char_handles.value_handle, &ble_gatts_value);
	APP_ERROR_CHECK(err_code);
}

static void ble_param_characteristic_conn_timeout_update(uint16_t value)
{
	ret_code_t err_code;
	ble_gatts_value_t ble_gatts_value = {
		.len = 2,
		.offset = 0,
		.p_value = (uint8_t*)&value,
	};
	err_code = sd_ble_gatts_value_set(m_conn_handle, m_ble_cfg.param_conn_timeout_char_handles.value_handle, &ble_gatts_value);
	APP_ERROR_CHECK(err_code);
}

static void ble_lora_characteristic_freq_update(uint16_t value)
{
	ret_code_t err_code;
	ble_gatts_value_t ble_gatts_value = {
		.len = 2,
		.offset = 0,
		.p_value = (uint8_t*)&value,
	};
	err_code = sd_ble_gatts_value_set(m_conn_handle, m_lora_cfg.lora_freq_char_handles.value_handle, &ble_gatts_value);
	APP_ERROR_CHECK(err_code);
}

static void ble_lora_characteristic_power_update(uint8_t value)
{
	ret_code_t err_code;
	ble_gatts_value_t ble_gatts_value = {
		.len = 1,
		.offset = 0,
		.p_value = &value,
	};
	err_code = sd_ble_gatts_value_set(m_conn_handle, m_lora_cfg.lora_power_char_handles.value_handle, &ble_gatts_value);
	APP_ERROR_CHECK(err_code);
}

static void ble_lora_characteristic_bw_update(uint8_t value)
{
	ret_code_t err_code;
	ble_gatts_value_t ble_gatts_value = {
		.len = 1,
		.offset = 0,
		.p_value = &value,
	};
	err_code = sd_ble_gatts_value_set(m_conn_handle, m_lora_cfg.lora_bw_char_handles.value_handle, &ble_gatts_value);
	APP_ERROR_CHECK(err_code);
}

static void ble_lora_characteristic_sf_update(uint8_t value)
{
	ret_code_t err_code;
	ble_gatts_value_t ble_gatts_value = {
		.len = 1,
		.offset = 0,
		.p_value = &value,
	};
	err_code = sd_ble_gatts_value_set(m_conn_handle, m_lora_cfg.lora_sf_char_handles.value_handle, &ble_gatts_value);
	APP_ERROR_CHECK(err_code);
}

static void ble_lora_characteristic_code_rate_update(uint8_t value)
{
	ret_code_t err_code;
	ble_gatts_value_t ble_gatts_value = {
		.len = 1,
		.offset = 0,
		.p_value = &value,
	};
	err_code = sd_ble_gatts_value_set(m_conn_handle, m_lora_cfg.lora_code_rate_char_handles.value_handle, &ble_gatts_value);
	APP_ERROR_CHECK(err_code);
}

static void ble_lora_characteristic_preamble_update(uint8_t value)
{
	ret_code_t err_code;
	ble_gatts_value_t ble_gatts_value = {
		.len = 1,
		.offset = 0,
		.p_value = &value,
	};
	err_code = sd_ble_gatts_value_set(m_conn_handle, m_lora_cfg.lora_preamble_char_handles.value_handle, &ble_gatts_value);
	APP_ERROR_CHECK(err_code);
}

static void ble_lora_characteristic_header_update(uint8_t value)
{
	ret_code_t err_code;
	ble_gatts_value_t ble_gatts_value = {
		.len = 1,
		.offset = 0,
		.p_value = &value,
	};
	err_code = sd_ble_gatts_value_set(m_conn_handle, m_lora_cfg.lora_header_char_handles.value_handle, &ble_gatts_value);
	APP_ERROR_CHECK(err_code);
}

static void ble_lora_characteristic_crc_update(uint8_t value)
{
	ret_code_t err_code;
	ble_gatts_value_t ble_gatts_value = {
		.len = 1,
		.offset = 0,
		.p_value = &value,
	};
	err_code = sd_ble_gatts_value_set(m_conn_handle, m_lora_cfg.lora_crc_char_handles.value_handle, &ble_gatts_value);
	APP_ERROR_CHECK(err_code);
}

static void ble_dev_characteristic_gateway_addr_update(uint8_t* value)
{
	ret_code_t err_code;
	ble_gatts_value_t ble_gatts_value = {
		.len = 8,
		.offset = 0,
		.p_value = value,
	};
	err_code = sd_ble_gatts_value_set(m_conn_handle, m_dev_cfg.dev_gateway_addr_char_handles.value_handle, &ble_gatts_value);
	APP_ERROR_CHECK(err_code);
}

static void ble_dev_characteristic_long_addr_update(uint8_t* value)
{
	ret_code_t err_code;
	ble_gatts_value_t ble_gatts_value = {
		.len = 8,
		.offset = 0,
		.p_value = value,
	};
	err_code = sd_ble_gatts_value_set(m_conn_handle, m_dev_cfg.dev_long_addr_char_handles.value_handle, &ble_gatts_value);
	APP_ERROR_CHECK(err_code);
}

static void ble_dev_characteristic_short_addr_update(uint8_t* value)
{
	ret_code_t err_code;
	ble_gatts_value_t ble_gatts_value = {
		.len = 2,
		.offset = 0,
		.p_value = value,
	};
	err_code = sd_ble_gatts_value_set(m_conn_handle, m_dev_cfg.dev_short_addr_char_handles.value_handle, &ble_gatts_value);
	APP_ERROR_CHECK(err_code);
}

static void ble_dev_characteristic_sample_mode_update(uint8_t value)
{
	ret_code_t err_code;
	ble_gatts_value_t ble_gatts_value = {
		.len = 1,
		.offset = 0,
		.p_value = (uint8_t*)&value,
	};
	err_code = sd_ble_gatts_value_set(m_conn_handle, m_dev_cfg.dev_mode_char_handles.value_handle, &ble_gatts_value);
	APP_ERROR_CHECK(err_code);
}

static void ble_dev_characteristic_sample_interval_update(uint32_t value)
{
	ret_code_t err_code;
	ble_gatts_value_t ble_gatts_value = {
		.len = 4,
		.offset = 0,
		.p_value = (uint8_t*)&value,
	};
	err_code = sd_ble_gatts_value_set(m_conn_handle, m_dev_cfg.dev_interval_char_handles.value_handle, &ble_gatts_value);
	APP_ERROR_CHECK(err_code);
}

static void ble_dev_characteristic_time_stamp_update(uint32_t value)
{
	ret_code_t err_code;
	ble_gatts_value_t ble_gatts_value = {
		.len = 4,
		.offset = 0,
		.p_value = (uint8_t*)&value,
	};
	err_code = sd_ble_gatts_value_set(m_conn_handle, m_dev_cfg.dev_time_stamp_char_handles.value_handle, &ble_gatts_value);
	APP_ERROR_CHECK(err_code);
}

static void ble_dev_characteristic_battery_update(uint8_t value)
{
	ret_code_t err_code;
	ble_gatts_value_t ble_gatts_value = {
		.len = 1,
		.offset = 0,
		.p_value = &value,
	};
	err_code = sd_ble_gatts_value_set(m_conn_handle, m_dev_cfg.dev_battery_char_handles.value_handle, &ble_gatts_value);
	APP_ERROR_CHECK(err_code);
}

static void ble_dev_characteristic_temperature_update(float value)
{
	ret_code_t err_code;
	ble_gatts_value_t ble_gatts_value = {
		.len = 4,
		.offset = 0,
		.p_value = (uint8_t*)&value,
	};
	err_code = sd_ble_gatts_value_set(m_conn_handle, m_dev_cfg.dev_temperature_char_handles.value_handle, &ble_gatts_value);
	APP_ERROR_CHECK(err_code);
}

static void ble_dev_characteristic_x_accel_update(float value)
{
	ret_code_t err_code;
	ble_gatts_value_t ble_gatts_value = {
		.len = 4,
		.offset = 0,
		.p_value = (uint8_t*)&value,
	};
	err_code = sd_ble_gatts_value_set(m_conn_handle, m_dev_cfg.dev_x_accel_char_handles.value_handle, &ble_gatts_value);
	APP_ERROR_CHECK(err_code);
}

static void ble_dev_characteristic_y_accel_update(float value)
{
	ret_code_t err_code;
	ble_gatts_value_t ble_gatts_value = {
		.len = 4,
		.offset = 0,
		.p_value = (uint8_t*)&value,
	};
	err_code = sd_ble_gatts_value_set(m_conn_handle, m_dev_cfg.dev_y_accel_char_handles.value_handle, &ble_gatts_value);
	APP_ERROR_CHECK(err_code);
}

static void ble_dev_characteristic_z_accel_update(float value)
{
	ret_code_t err_code;
	ble_gatts_value_t ble_gatts_value = {
		.len = 4,
		.offset = 0,
		.p_value = (uint8_t*)&value,
	};
	err_code = sd_ble_gatts_value_set(m_conn_handle, m_dev_cfg.dev_z_accel_char_handles.value_handle, &ble_gatts_value);
	APP_ERROR_CHECK(err_code);
}

static void ble_dev_characteristic_x_angle_update(float value)
{
	ret_code_t err_code;
	ble_gatts_value_t ble_gatts_value = {
		.len = 4,
		.offset = 0,
		.p_value = (uint8_t*)&value,
	};
	err_code = sd_ble_gatts_value_set(m_conn_handle, m_dev_cfg.dev_x_angle_char_handles.value_handle, &ble_gatts_value);
	APP_ERROR_CHECK(err_code);
}

static void ble_dev_characteristic_y_angle_update(float value)
{
	ret_code_t err_code;
	ble_gatts_value_t ble_gatts_value = {
		.len = 4,
		.offset = 0,
		.p_value = (uint8_t*)&value,
	};
	err_code = sd_ble_gatts_value_set(m_conn_handle, m_dev_cfg.dev_y_angle_char_handles.value_handle, &ble_gatts_value);
	APP_ERROR_CHECK(err_code);
}

static void ble_dev_characteristic_z_angle_update(float value)
{
	ret_code_t err_code;
	ble_gatts_value_t ble_gatts_value = {
		.len = 4,
		.offset = 0,
		.p_value = (uint8_t*)&value,
	};
	err_code = sd_ble_gatts_value_set(m_conn_handle, m_dev_cfg.dev_z_angle_char_handles.value_handle, &ble_gatts_value);
	APP_ERROR_CHECK(err_code);
}

static void ble_dev_characteristic_accel_slope_threshold_update(uint16_t value)
{
	ret_code_t err_code;
	ble_gatts_value_t ble_gatts_value = {
		.len = 2,
		.offset = 0,
		.p_value = (uint8_t*)&value,
	};
	err_code = sd_ble_gatts_value_set(m_conn_handle, m_dev_cfg.dev_accel_slope_threshold_char_handles.value_handle, &ble_gatts_value);
	APP_ERROR_CHECK(err_code);
}

static void ble_dev_characteristic_consecutive_data_points_update(uint16_t value)
{
	ret_code_t err_code;
	ble_gatts_value_t ble_gatts_value = {
		.len = 2,
		.offset = 0,
		.p_value = (uint8_t*)&value,
	};
	err_code = sd_ble_gatts_value_set(m_conn_handle, m_dev_cfg.dev_consecutive_data_points_char_handles.value_handle, &ble_gatts_value);
	APP_ERROR_CHECK(err_code);
}

static void ble_dev_characteristic_x_angle_threshold_update(float value)
{
	ret_code_t err_code;
	ble_gatts_value_t ble_gatts_value = {
		.len = 4,
		.offset = 0,
		.p_value = (uint8_t*)&value,
	};
	err_code = sd_ble_gatts_value_set(m_conn_handle, m_dev_cfg.dev_x_angle_threshold_char_handles.value_handle, &ble_gatts_value);
	APP_ERROR_CHECK(err_code);
}

static void ble_dev_characteristic_y_angle_threshold_update(float value)
{
	ret_code_t err_code;
	ble_gatts_value_t ble_gatts_value = {
		.len = 4,
		.offset = 0,
		.p_value = (uint8_t*)&value,
	};
	err_code = sd_ble_gatts_value_set(m_conn_handle, m_dev_cfg.dev_y_angle_threshold_char_handles.value_handle, &ble_gatts_value);
	APP_ERROR_CHECK(err_code);
}

static void ble_dev_characteristic_z_angle_threshold_update(float value)
{
	ret_code_t err_code;
	ble_gatts_value_t ble_gatts_value = {
		.len = 4,
		.offset = 0,
		.p_value = (uint8_t*)&value,
	};
	err_code = sd_ble_gatts_value_set(m_conn_handle, m_dev_cfg.dev_z_angle_threshold_char_handles.value_handle, &ble_gatts_value);
	APP_ERROR_CHECK(err_code);
}


static void ble_dev_characteristic_lora_rssi_update(uint8_t value)
{
	ret_code_t err_code;
	ble_gatts_value_t ble_gatts_value = {
		.len = 1,
		.offset = 0,
		.p_value = (uint8_t*)&value,
	};
	err_code = sd_ble_gatts_value_set(m_conn_handle, m_dev_cfg.dev_lora_rssi_char_handles.value_handle, &ble_gatts_value);
	APP_ERROR_CHECK(err_code);
}

static void ble_dev_characteristic_sw_version_update(uint8_t* value)
{
	ret_code_t err_code;
	ble_gatts_value_t ble_gatts_value = {
		.len = 4,
		.offset = 0,
		.p_value = value,
	};
	err_code = sd_ble_gatts_value_set(m_conn_handle, m_dev_cfg.dev_sw_version_char_handles.value_handle, &ble_gatts_value);
	APP_ERROR_CHECK(err_code);
}

static void ble_dev_characteristic_hw_version_update(uint8_t* value)
{
	ret_code_t err_code;
	ble_gatts_value_t ble_gatts_value = {
		.len = 4,
		.offset = 0,
		.p_value = value,
	};
	err_code = sd_ble_gatts_value_set(m_conn_handle, m_dev_cfg.dev_hw_version_char_handles.value_handle, &ble_gatts_value);
	APP_ERROR_CHECK(err_code);
}

static ble_char_update_t ble_char_update = {
	.ble_tx_power_update = ble_param_characteristic_tx_power_update,
	.ble_adv_interval_update = ble_param_characteristic_adv_interval_update,
	.ble_adv_time_update = ble_param_characteristic_adv_time_update,
	.ble_min_conn_interval_update = ble_param_characteristic_min_conn_interval_update,
	.ble_max_conn_interval_update = ble_param_characteristic_max_conn_interval_update,
	.ble_slave_latency_update = ble_param_characteristic_slave_latency_update,
	.ble_conn_timeout_update = ble_param_characteristic_conn_timeout_update,
	
	.lora_freq_update = ble_lora_characteristic_freq_update,
	.lora_power_update = ble_lora_characteristic_power_update,
	.lora_bw_update = ble_lora_characteristic_bw_update,
	.lora_sf_update = ble_lora_characteristic_sf_update,
	.lora_code_rate_update = ble_lora_characteristic_code_rate_update,
	.lora_preamble_update = ble_lora_characteristic_preamble_update,
	.lora_header_update = ble_lora_characteristic_header_update,
	.lora_crc_update = ble_lora_characteristic_crc_update,
	
	.dev_gateway_addr_update = ble_dev_characteristic_gateway_addr_update,
	.dev_long_addr_update = ble_dev_characteristic_long_addr_update,
	.dev_short_addr_update = ble_dev_characteristic_short_addr_update,
	.dev_sample_mode_update = ble_dev_characteristic_sample_mode_update,
	.dev_sample_interval_update = ble_dev_characteristic_sample_interval_update,
	.dev_time_stamp_update = ble_dev_characteristic_time_stamp_update,
	.dev_battery_update = ble_dev_characteristic_battery_update,
	.dev_temperature_update = ble_dev_characteristic_temperature_update,
	.dev_x_accel_update = ble_dev_characteristic_x_accel_update,
	.dev_y_accel_update = ble_dev_characteristic_y_accel_update,
	.dev_z_accel_update = ble_dev_characteristic_z_accel_update,
	.dev_x_angle_update = ble_dev_characteristic_x_angle_update,
	.dev_y_angle_update = ble_dev_characteristic_y_angle_update,
	.dev_z_angle_update = ble_dev_characteristic_z_angle_update,
	.dev_accel_slope_threshold_update = ble_dev_characteristic_accel_slope_threshold_update,
	.dev_consecutive_data_points_update = ble_dev_characteristic_consecutive_data_points_update,
	.dev_x_angle_threshold_update = ble_dev_characteristic_x_angle_threshold_update,
	.dev_y_angle_threshold_update = ble_dev_characteristic_y_angle_threshold_update,
	.dev_z_angle_threshold_update = ble_dev_characteristic_z_angle_threshold_update,
	.dev_lora_rssi_update = ble_dev_characteristic_lora_rssi_update,
	.dev_sw_version_update = ble_dev_characteristic_sw_version_update,
	.dev_hw_version_update = ble_dev_characteristic_hw_version_update,
};

void ble_adv_start(void)
{
    ret_code_t err_code = sd_ble_gap_adv_start(m_adv_handle, APP_BLE_CONN_CFG_TAG);
    APP_ERROR_CHECK(err_code);
}

void ble_adv_stop(void)
{
    ret_code_t err_code = sd_ble_gap_adv_stop(m_adv_handle);
    APP_ERROR_CHECK(err_code);
}

void ble_disconnect(void)
{
	ret_code_t err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
	APP_ERROR_CHECK(err_code);
}

void ble_rssi_get(int8_t *p_rssi, uint8_t *p_ch_index)
{
	*p_rssi = ble_rssi;
	p_ch_index = p_ch_index;
}

void ble_tx_power_set(uint8_t level)
{
	//level:-40dBm, -20dBm, -16dBm, -12dBm, -8dBm, -4dBm, 0dBm, +3dBm and +4dBm.
	static const int8_t tx_power_level[] = {-40, -20, -16, -12, -8, -4, 0, 3, 4};
	ret_code_t err_code = sd_ble_gap_tx_power_set(BLE_GAP_TX_POWER_ROLE_ADV, m_adv_handle, tx_power_level[level]);
	APP_ERROR_CHECK(err_code);
}

void ble_get_address(uint8_t* addr)
{
	ret_code_t err_code;
	ble_gap_addr_t ble_addr;
	err_code = sd_ble_gap_addr_get(&ble_addr);
	APP_ERROR_CHECK(err_code);
	
	for(int i=0; i<6; i++)
	{
		addr[i] = ble_addr.addr[i];
	}
}

void ble_conn_params_change(uint16_t min_interva, 
							uint16_t max_interval, 
							uint16_t slave_latency, 
							uint16_t conn_timeout)
{
	ble_gap_conn_params_t params;
	params.min_conn_interval = MIN_CONN_INTERVAL(min_interva);
	params.max_conn_interval = MAX_CONN_INTERVAL(max_interval);
	params.slave_latency = slave_latency;
	params.conn_sup_timeout = CONN_SUP_TIMEOUT(conn_timeout);
	ret_code_t err_code = ble_conn_params_change_conn_params(m_conn_handle, &params);
	if(err_code != 0)
	{
		err_code = 0;
	}
//	APP_ERROR_CHECK(err_code);
}

/**@brief Function for the GAP initialization.
 *
 * @details This function sets up all the necessary GAP (Generic Access Profile) parameters of the
 *          device including the device name, appearance, and the preferred connection parameters.
 */
static void gap_params_init(void)
{
	ret_code_t              err_code;
	ble_gap_conn_params_t   gap_conn_params;
	ble_gap_conn_sec_mode_t sec_mode;

	/* GAP�����������豸�������� */
	/* ����GAP�İ�ȫģʽ�����豸����������дȨ�� */
	char dev_name[20];
	sys_param_t* param = sys_param_get_handle();
	uint16_t dev_name_suffix = ((uint16_t)param->dev_long_addr[6] << 8) | param->dev_long_addr[7];
	char dev_type[2];
	dev_type[0] = 'I';
	dev_type[1] = '_';
	if(param->object_version == COLLAPSE_VERSION)
	{
		dev_type[0] = 'C';
	}
	sprintf(dev_name, "%s%s%d", SYS_PARAM_BLE_DEV_NAME_PREFIX, dev_type, dev_name_suffix);
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);
	err_code = sd_ble_gap_device_name_set(&sec_mode, (const uint8_t *)dev_name, strlen(dev_name));
	APP_ERROR_CHECK(err_code);
	
	/* GAP�����������豸������� */
//	err_code = sd_ble_gap_appearance_set(0);
//	APP_ERROR_CHECK(err_code);

	memset(&gap_conn_params, 0, sizeof(gap_conn_params));
	gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL(ble_param.min_conn_interval); //��С���Ӽ��
	gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL(ble_param.max_conn_interval); //������Ӽ��
	gap_conn_params.slave_latency     = SLAVE_LATENCY; //�ӻ��ӳ�
	gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT(ble_param.conn_sup_timeout); //�ල��ʱ

	/* GAP������������Χ�豸��ѡ���Ӳ������� */
	err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
	APP_ERROR_CHECK(err_code);
																				
	//GAP���������������豸��ַ��������
	//GAP�������������ɽ���˽�е�ַ����
}

void gatt_evt_handler(nrf_ble_gatt_t* p_gatt, nrf_ble_gatt_evt_t const* p_evt)
{
	if (p_evt->evt_id == NRF_BLE_GATT_EVT_ATT_MTU_UPDATED)
	{
		
	}
}

/**@brief Function for initializing the GATT module.
 */
static void gatt_init(void)
{
	/* ��ʼ��GATTģ�飬Ĭ�ϲ������ص�m_gatt�� */
	ret_code_t err_code = nrf_ble_gatt_init(&m_gatt, gatt_evt_handler);
	APP_ERROR_CHECK(err_code);
	
	err_code = nrf_ble_gatt_att_mtu_periph_set(&m_gatt, NRF_SDH_BLE_GATT_MAX_MTU_SIZE);
	APP_ERROR_CHECK(err_code);
}

#if 0
BLE_ADVERTISING_DEF(m_advertising);     //��������Ϊm_advertising�Ĺ㲥ģ��ʵ��
//�㲥�¼�������
static void on_adv_evt(ble_adv_evt_t ble_adv_evt)
{
    //�жϹ㲥�¼�����
    switch (ble_adv_evt)
    {
		case BLE_ADV_EVT_FAST: //���ٹ㲥�����¼������ٹ㲥�������������¼�
			LIGHT_1_TOGGLE();
            break;
        
        case BLE_ADV_EVT_IDLE: //�㲥IDLE�¼����㲥��ʱ���������¼�
			LIGHT_1_OFF();
			LIGHT_2_ON();
            break;

        default:
            break;
    }
}
//�㲥��ʼ��
static void advertising_init(void)
{
    ret_code_t err_code;	
	
    /* ����һ�����飬����Զ�������ݣ���Ϊble_advdata_manuf_data_t��
	   ��ͨ��ָ�����������ݵģ����Ա���Ҫ�������Զ������ݵı�������
	   �����ĵ�ַ��ֵ��ble_advdata_manuf_data_t */
    uint8_t my_adv_manuf_data[] = {0x01};
	/* �����һ���������Զ������ݵĽṹ����������ù㲥����ʱ���ñ����ĵ�ַ��ֵ��
	   �㲥���ݽṹ���е�p_manuf_specific_data */
	ble_advdata_manuf_data_t manuf_specific_data;
	manuf_specific_data.company_identifier = 0x0059; //������ID��0x0059��Nordic��ID
	manuf_specific_data.data.p_data = my_adv_manuf_data; //ָ���������Զ��������
    manuf_specific_data.data.size = sizeof(my_adv_manuf_data); //�������Զ�������ݴ�С(�ֽ���)

    ble_advertising_init_t init;
    
    memset(&init, 0, sizeof(init)); //����֮ǰ������
    init.advdata.name_type = BLE_ADVDATA_FULL_NAME; //�豸����ȫ��
    init.advdata.include_appearance = true; //�Ƿ�������
    init.advdata.flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE; //һ��ɷ���ģʽ����֧��BR/EDR
	
	ble_uuid_t adv_uuids[] = {{DEV_CFG_UUID_SERVICE, m_dev_cfg.uuid_type}}; //ȫ������UUID
	init.advdata.uuids_complete.uuid_cnt = sizeof(adv_uuids) / sizeof(adv_uuids[0]);
	init.advdata.uuids_complete.p_uuids = adv_uuids;
	
//	init.advdata.p_manuf_specific_data = &manuf_specific_data; //�㲥�м����������Զ�������
	
    init.config.ble_adv_fast_enabled = true; //���ù㲥ģʽΪ���ٹ㲥
    init.config.ble_adv_fast_interval = APP_ADV_INTERVAL(ble_param.adv_interval); //���ù㲥���
    init.config.ble_adv_fast_timeout  = APP_ADV_DURATION; //���ò�����ʱ��
    
    init.evt_handler = on_adv_evt; //�㲥�¼��ص�����
    
    err_code = ble_advertising_init(&m_advertising, &init); //��ʼ���㲥
    APP_ERROR_CHECK(err_code);
    
    ble_advertising_conn_cfg_tag_set(&m_advertising, APP_BLE_CONN_CFG_TAG); //�����������ñ�־����ǰSoftDevice�汾��ֻ��д1
}
//�����㲥���ú������õ�ģʽ����͹㲥��ʼ�������õĹ㲥ģʽһ��
void ble_advertising_start(void)
{
	ret_code_t err_code = ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST); //ʹ�ù㲥��ʼ�������õĹ㲥ģʽ�����㲥
	APP_ERROR_CHECK(err_code);
}
#endif


/**@brief Function for initializing the Advertising functionality.
 *
 * @details Encodes the required advertising data and passes it to the stack.
 *          Also builds a structure to be passed to the stack when starting advertising.
 */
static void advertising_init(void)
{
	ret_code_t    err_code;
	ble_advdata_t advdata;
	
	/* ���ù㲥���� */
	memset(&advdata, 0, sizeof(advdata));
	advdata.name_type          = BLE_ADVDATA_FULL_NAME; //�豸����ȫ��
	advdata.include_appearance = true; //�Ƿ�������
	advdata.flags              = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE; //һ��ɷ���ģʽ����֧��BR/EDR

	ble_advdata_t srdata;
//	ble_uuid_t adv_uuids[] = {{LORA_CFG_UUID_SERVICE, m_lora_cfg.uuid_type}};
//	memset(&srdata, 0, sizeof(srdata)); //ȫ������UUID
//	srdata.uuids_complete.uuid_cnt = sizeof(adv_uuids) / sizeof(adv_uuids[0]);
//	srdata.uuids_complete.p_uuids  = adv_uuids;
	
	/* �����Զ������� */
	memset(&srdata, 0, sizeof(srdata));
	ble_advdata_manuf_data_t ble_advdata_manuf_data;
	ble_advdata_manuf_data.company_identifier = 0X1001;
	sys_param_t* param = sys_param_get_handle();
	ble_advdata_manuf_data.data.p_data = param->dev_long_addr;
	ble_advdata_manuf_data.data.size = sizeof(param->dev_long_addr);
	srdata.p_manuf_specific_data = &ble_advdata_manuf_data;

	/* ����㲥���� */
	err_code = ble_advdata_encode(&advdata, m_adv_data.adv_data.p_data, &m_adv_data.adv_data.len);
	APP_ERROR_CHECK(err_code);

	/* ����㲥���� */
	err_code = ble_advdata_encode(&srdata, m_adv_data.scan_rsp_data.p_data, &m_adv_data.scan_rsp_data.len);
	APP_ERROR_CHECK(err_code);
	
	/* ���ù㲥���� */
	ble_gap_adv_params_t adv_params;
	memset(&adv_params, 0, sizeof(adv_params));
	adv_params.primary_phy     = BLE_GAP_PHY_1MBPS;
	adv_params.duration        = APP_ADV_DURATION;
	adv_params.properties.type = BLE_GAP_ADV_TYPE_CONNECTABLE_SCANNABLE_UNDIRECTED;
	adv_params.p_peer_addr     = NULL;
	adv_params.filter_policy   = BLE_GAP_ADV_FP_ANY;
	adv_params.interval        = APP_ADV_INTERVAL(ble_param.adv_interval);

	err_code = sd_ble_gap_adv_set_configure(&m_adv_handle, &m_adv_data, &adv_params);
	APP_ERROR_CHECK(err_code);
}

//�Ŷ�д���¼������������ڴ����Ŷ�д��ģ��Ĵ���
static void nrf_qwr_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}

//���Ӳ���Э��ģ���¼�������
static void on_conn_params_evt(ble_conn_params_evt_t * p_evt)
{
	ret_code_t err_code;
	//�ж��¼����ͣ������¼�����ִ�ж���
	//���Ӳ���Э��ʧ��
	if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
	{
		err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
		APP_ERROR_CHECK(err_code);
	}
	//���Ӳ���Э�̳ɹ�
	if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_SUCCEEDED)
	{
		//���ܴ���;
	}
}

//���Ӳ���Э��ģ��������¼�������nrf_error�����˴�����룬ͨ��nrf_error���Է���������Ϣ
static void conn_params_error_handler(uint32_t nrf_error)
{
	//���������
	APP_ERROR_HANDLER(nrf_error);
}

/**@brief Function for initializing the Connection Parameters module.
 */
static void conn_params_init(void)
{
	ret_code_t             err_code;
	ble_conn_params_init_t cp_init;

	memset(&cp_init, 0, sizeof(cp_init));
	
	cp_init.p_conn_params                  = NULL; //����ΪNULL����������ȡ���Ӳ���
	cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY; //���ӻ�����֪ͨ���״η������Ӳ�����������֮���ʱ��(ticks)
	cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY; //ÿ�ε���sd_ble_gap_conn_param_update()�����������Ӳ�����������֮��ļ��ʱ��(ticks)
	cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT; //�������Ӳ���Э��ǰ�������Ӳ���Э�̵�������
	/* �������Ӳ������´�֪ͨʹ�ܻ��������¼���ʼ��ʱ
		 �����֪ͨʹ�ܿ�ʼ��ʱ������Ϊ��ӦCCCD���ͻ����������������������
		 ����������¼���ʼ��ʱ������ΪBLE_GATT_HANDLE_INVALID */
	cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
	cp_init.disconnect_on_fail             = false; //���Ӳ�������ʧ�ܲ��Ͽ�����
	cp_init.evt_handler                    = on_conn_params_evt; //ע�����Ӳ��������¼����
	cp_init.error_handler                  = conn_params_error_handler; //ע�����Ӳ������´����¼����

	/* ���ÿ⺯���������Ӳ������³�ʼ���ṹ��Ϊ�����������ʼ�����Ӳ���Э��ģ�� */
	err_code = ble_conn_params_init(&cp_init);
	APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling BLE events.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 * @param[in]   p_context   Unused.
 */
static void ble_evt_handler(ble_evt_t const * p_ble_evt, void * p_context)
{
	ret_code_t err_code;

	switch (p_ble_evt->header.evt_id)
	{
		/* �����¼� */
		case BLE_GAP_EVT_CONNECTED:
			NRF_LOG_INFO("Connected");
			m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
			err_code = sd_ble_gap_rssi_start(m_conn_handle, 2, 2);
			APP_ERROR_CHECK(err_code);
			err_code = nrf_ble_qwr_conn_handle_assign(&m_qwr, m_conn_handle);
			APP_ERROR_CHECK(err_code);
			ble_conn_handler();
			break;
		
		/* ���ӶϿ��¼� */
		case BLE_GAP_EVT_DISCONNECTED:
			NRF_LOG_INFO("Disconnected");
			m_conn_handle = BLE_CONN_HANDLE_INVALID;
			ble_disconn_handler();
			break;

		case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
			// Pairing not supported
			err_code = sd_ble_gap_sec_params_reply(m_conn_handle,
												   BLE_GAP_SEC_STATUS_PAIRING_NOT_SUPP,
												   NULL,
												   NULL);
			APP_ERROR_CHECK(err_code);
			break;

		/* PHY�����¼� */
		case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
		{
			NRF_LOG_DEBUG("PHY update request.");
			ble_gap_phys_t const phys =
			{
				.rx_phys = BLE_GAP_PHY_AUTO,
				.tx_phys = BLE_GAP_PHY_AUTO,
			};
			err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
			APP_ERROR_CHECK(err_code);
		} break;

		case BLE_GATTS_EVT_SYS_ATTR_MISSING:
			// No system attributes have been stored.
			err_code = sd_ble_gatts_sys_attr_set(m_conn_handle, NULL, 0, 0);
			APP_ERROR_CHECK(err_code);
			break;

		/* GATT�ͻ��˳�ʱ�¼� */
		case BLE_GATTC_EVT_TIMEOUT:
			// Disconnect on GATT Client timeout event.
			NRF_LOG_DEBUG("GATT Client Timeout.");
			err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
											 BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
			APP_ERROR_CHECK(err_code);
			ble_disconn_handler();
			break;

		/* GATT��������ʱ�¼� */
		case BLE_GATTS_EVT_TIMEOUT:
			// Disconnect on GATT Server timeout event.
			NRF_LOG_DEBUG("GATT Server Timeout.");
			err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
											 BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
			APP_ERROR_CHECK(err_code);
			ble_disconn_handler();
			break;

		case BLE_GAP_EVT_RSSI_CHANGED:
			ble_rssi = p_ble_evt->evt.gap_evt.params.rssi_changed.rssi;
			break;
		
		default:
			// No implementation needed.
			break;
	}
}

/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
static void ble_stack_init(void)
{
	ret_code_t err_code;
	
	/* ����ʹ��sofdevice������sdk_config.h�ļ��е�Ƶʱ�����������õ�Ƶʱ�� */
	err_code = nrf_sdh_enable_request();
	APP_ERROR_CHECK(err_code);

	/* ���屣��Ӧ�ó���RAM��ʼ��ַ�ı��� */
	uint32_t ram_start = 0;
	/* ʹ��sdk_config.h�ļ���Ĭ�����ò�������Э��ջ����ȡӦ�ó���RAM��ʼ��ַ
		 ���浽����ram_start */
	err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
	APP_ERROR_CHECK(err_code);

	/* ʹ��BLEЭ��ջ */
	err_code = nrf_sdh_ble_enable(&ram_start);
	APP_ERROR_CHECK(err_code);

	/* ע��BLE�¼��ص�����  */
	NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_evt_handler, NULL);
}

__weak uint8_t ble_param_tx_power_write_handler(uint8_t* p_data, uint16_t len){return 0;}
__weak uint8_t ble_param_adv_interval_write_handler(uint8_t* p_data, uint16_t len){return 0;}
__weak uint8_t ble_param_adv_time_write_handler(uint8_t* p_data, uint16_t len){return 0;}
__weak uint8_t ble_param_min_conn_interval_write_handler(uint8_t* p_data, uint16_t len){return 0;}
__weak uint8_t ble_param_max_conn_interval_write_handler(uint8_t* p_data, uint16_t len){return 0;}
__weak uint8_t ble_param_slave_latency_write_handler(uint8_t* p_data, uint16_t len){return 0;}
__weak uint8_t ble_param_conn_timeout_write_handler(uint8_t* p_data, uint16_t len){return 0;}
	
__weak uint8_t ble_lora_freq_write_handler(uint8_t* p_data, uint16_t len){return 0;}
__weak uint8_t ble_lora_power_write_handler(uint8_t* p_data, uint16_t len){return 0;}
__weak uint8_t ble_lora_bw_write_handler(uint8_t* p_data, uint16_t len){return 0;}
__weak uint8_t ble_lora_sf_write_handler(uint8_t* p_data, uint16_t len){return 0;}
__weak uint8_t ble_lora_code_rate_write_handler(uint8_t* p_data, uint16_t len){return 0;}
__weak uint8_t ble_lora_preamble_write_handler(uint8_t* p_data, uint16_t len){return 0;}
__weak uint8_t ble_lora_header_write_handler(uint8_t* p_data, uint16_t len){return 0;}
__weak uint8_t ble_lora_crc_write_handler(uint8_t* p_data, uint16_t len){return 0;}
	
__weak void ble_dev_gateway_write_handler(uint8_t* p_data, uint16_t len){}
__weak void ble_dev_long_addr_write_handler(uint8_t* p_data, uint16_t len){}
__weak void ble_dev_short_addr_write_handler(uint8_t* p_data, uint16_t len){}
__weak void ble_dev_time_stamp_read_handler(uint8_t* p_data, uint16_t* len){}
	
/**@brief Function for initializing services that will be used by the application.
 */
static void services_init(void)
{
	ret_code_t err_code;
	nrf_ble_qwr_init_t qwr_init = {0};

	// Initialize Queued Write Module.
	qwr_init.error_handler = nrf_qwr_error_handler;
	err_code = nrf_ble_qwr_init(&m_qwr, &qwr_init);
	APP_ERROR_CHECK(err_code);

	ble_param_cfg_init_t ble_init = {
		.param_tx_power_write_handler 			= ble_param_tx_power_write_handler,
		.param_adv_interval_write_handler 		= ble_param_adv_interval_write_handler,
		.param_adv_time_write_handler 			= ble_param_adv_time_write_handler,
		.param_min_conn_interval_write_handler 	= ble_param_min_conn_interval_write_handler,
		.param_max_conn_interval_write_handler 	= ble_param_max_conn_interval_write_handler,
		.param_slave_latency_write_handler 		= ble_param_slave_latency_write_handler,
		.param_conn_timeout_write_handler 		= ble_param_conn_timeout_write_handler,
	};
	err_code =ble_param_cfg_init(&m_ble_cfg, &ble_init);
	APP_ERROR_CHECK(err_code);		
	
	ble_lora_cfg_init_t lora_init = {
		.lora_freq_write_handler 				= ble_lora_freq_write_handler,
		.lora_power_write_handler 				= ble_lora_power_write_handler,
		.lora_bw_write_handler 					= ble_lora_bw_write_handler,
		.lora_sf_write_handler 					= ble_lora_sf_write_handler,
		.lora_code_rate_write_handler 			= ble_lora_code_rate_write_handler,
		.lora_preamble_write_handler 			= ble_lora_preamble_write_handler,
		.lora_header_write_handler 				= ble_lora_header_write_handler,
		.lora_crc_write_handler 				= ble_lora_crc_write_handler,
	};
	err_code = ble_lora_cfg_init(&m_lora_cfg, &lora_init);
	APP_ERROR_CHECK(err_code);		

	ble_dev_cfg_init_t init = {
		.dev_gateway_write_handler 				= ble_dev_gateway_write_handler,
		.dev_long_addr_write_handler 			= ble_dev_long_addr_write_handler,
		.dev_short_addr_write_handler	 		= ble_dev_short_addr_write_handler,
		.dev_time_stamp_read_handler			= ble_dev_time_stamp_read_handler,
	};
	err_code = ble_dev_cfg_init(&m_dev_cfg, &init);
	APP_ERROR_CHECK(err_code);
}

ble_char_update_t* ble_char_update_handle_get(void)
{
	return &ble_char_update;
}

void ble_softdev_init(void)
{
	sys_param_t* param = sys_param_get_handle();
	ble_param.adv_interval = param->ble_adv_interval;
	ble_param.min_conn_interval = param->ble_min_conn_interval;
	ble_param.max_conn_interval = param->ble_max_conn_interval;
	ble_param.conn_sup_timeout = param->ble_conn_timeout;
	
	ble_stack_init(); //BLEЭ��ջ��ʼ��
	gap_params_init(); //GAP������ʼ��
	gatt_init(); //GATT��ʼ��
	services_init(); //�����ʼ��
	advertising_init(); //�㲥��ʼ��
	conn_params_init(); //���Ӳ�����ʼ��
	
	ble_char_update.ble_tx_power_update(param->ble_tx_power);
	ble_char_update.ble_adv_interval_update(param->ble_adv_interval);
	ble_char_update.ble_adv_time_update(param->ble_adv_time);
	ble_char_update.ble_min_conn_interval_update(param->ble_min_conn_interval);
	ble_char_update.ble_max_conn_interval_update(param->ble_max_conn_interval);
	ble_char_update.ble_slave_latency_update(param->ble_slave_latency);
	ble_char_update.ble_conn_timeout_update(param->ble_conn_timeout);
	
	ble_char_update.lora_freq_update(param->lora_freq);
	ble_char_update.lora_power_update(param->lora_power);
	ble_char_update.lora_bw_update(param->lora_bw);
	ble_char_update.lora_sf_update(param->lora_sf);
	ble_char_update.lora_code_rate_update(param->lora_code_rate);
	ble_char_update.lora_preamble_update(param->lora_preamble);
	ble_char_update.lora_header_update(param->lora_header);
	ble_char_update.lora_crc_update(param->lora_crc);
	
	ble_char_update.dev_gateway_addr_update(param->dev_gateway_addr);
	ble_char_update.dev_long_addr_update(param->dev_long_addr);
	ble_char_update.dev_short_addr_update(param->dev_short_addr);
	ble_char_update.dev_time_stamp_update(0);
	ble_char_update.dev_battery_update(100);
	ble_char_update.dev_temperature_update(0);
	ble_char_update.dev_x_angle_update(0);
	ble_char_update.dev_y_angle_update(0);
	ble_char_update.dev_z_angle_update(0);
	ble_char_update.dev_lora_rssi_update((uint8_t)-127);
	uint8_t sw_version[] = {(uint16_t)SYS_SW_MODIFY_VERSION,(uint16_t)SYS_SW_MODIFY_VERSION>>8,SYS_SW_SUB_VERSION,SYS_SW_MAIN_VERSION};
	ble_char_update.dev_sw_version_update(sw_version);
	uint8_t hw_version[] = {(uint16_t)SYS_HW_MODIFY_VERSION,(uint16_t)SYS_HW_MODIFY_VERSION>>8,SYS_HW_SUB_VERSION,SYS_HW_MAIN_VERSION};
	ble_char_update.dev_hw_version_update(hw_version);
	if(param->object_version == INCLINOMETER_VERSION)
	{
		ble_char_update.dev_sample_mode_update(param->iot_clinometer.iot_mode);
		ble_char_update.dev_sample_interval_update(param->iot_clinometer.iot_sample_interval);
		ble_char_update.dev_x_angle_threshold_update(param->iot_clinometer.iot_x_angle_threshold);
		ble_char_update.dev_y_angle_threshold_update(param->iot_clinometer.iot_y_angle_threshold);
		ble_char_update.dev_z_angle_threshold_update(param->iot_clinometer.iot_z_angle_threshold);
	}
	else if(param->object_version == COLLAPSE_VERSION)
	{
		ble_char_update.dev_sample_mode_update(param->iot_collapse.iot_mode);
		ble_char_update.dev_sample_interval_update(param->iot_collapse.iot_period);
		ble_char_update.dev_x_accel_update(0);
		ble_char_update.dev_y_accel_update(0);
		ble_char_update.dev_z_accel_update(0);
		ble_char_update.dev_accel_slope_threshold_update(param->iot_collapse.iot_accel_slope_threshold);
		ble_char_update.dev_consecutive_data_points_update(param->iot_collapse.iot_consecutive_data_points);
	}
}


