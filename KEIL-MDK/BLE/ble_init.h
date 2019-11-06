#ifndef __BLE_INIT_H__
#define __BLE_INIT_H__
#include "main.h"

typedef struct {
	void (*ble_tx_power_update)(uint8_t value);
	void (*ble_adv_interval_update)(uint16_t value);
	void (*ble_adv_time_update)(uint16_t value);
	void (*ble_min_conn_interval_update)(uint16_t value);
	void (*ble_max_conn_interval_update)(uint16_t value);
	void (*ble_slave_latency_update)(uint16_t value);
	void (*ble_conn_timeout_update)(uint16_t value);
	
	void (*lora_freq_update)(uint16_t value);
	void (*lora_power_update)(uint8_t value);
	void (*lora_bw_update)(uint8_t value);
	void (*lora_sf_update)(uint8_t value);
	void (*lora_code_rate_update)(uint8_t value);
	void (*lora_preamble_update)(uint8_t value);
	void (*lora_header_update)(uint8_t value);
	void (*lora_crc_update)(uint8_t value);
	
	void (*dev_gateway_addr_update)(uint8_t* value);
	void (*dev_long_addr_update)(uint8_t* value);
	void (*dev_short_addr_update)(uint8_t* value);
	void (*dev_sample_mode_update)(uint8_t value);
	void (*dev_sample_interval_update)(uint32_t value);
	void (*dev_time_stamp_update)(uint32_t value);
	void (*dev_battery_update)(uint8_t value);
	void (*dev_temperature_update)(float value);
	void (*dev_x_accel_update)(float value);
	void (*dev_y_accel_update)(float value);
	void (*dev_z_accel_update)(float value);
	void (*dev_x_angle_update)(float value);
	void (*dev_y_angle_update)(float value);
	void (*dev_z_angle_update)(float value);
	void (*dev_accel_slope_threshold_update)(uint16_t value);
	void (*dev_consecutive_data_points_update)(uint16_t value);
	void (*dev_x_angle_threshold_update)(float value);
	void (*dev_y_angle_threshold_update)(float value);
	void (*dev_z_angle_threshold_update)(float value);
	void (*dev_lora_rssi_update)(uint8_t value);
	void (*dev_sw_version_update)(uint8_t* value);
	void (*dev_hw_version_update)(uint8_t* value);
	void (*dev_time_offset_update)(uint16_t value);
	
	void (*misc_payload_length)(uint8_t value);
	void (*misc_counting_mode)(uint32_t value);
	void (*misc_timer_mode)(uint32_t value);
	void (*misc_comm_interval)(uint32_t value);
	void (*misc_comm_ctrl_notify)(uint8_t value);
	void (*misc_lost_rate_notify)(float value);
	void (*misc_test_progress_notify)(float value);
	void (*misc_ota)(uint8_t value);
}ble_char_update_t;

void ble_softdev_init(void);
void ble_adv_start(void);
void ble_adv_stop(void);
void ble_disconnect(void);
void ble_get_address(uint8_t* addr);
void ble_rssi_get(int8_t *p_rssi, uint8_t *p_ch_index);
void ble_tx_power_set(uint8_t level);
void ble_conn_params_change(uint16_t min_interva, 
							uint16_t max_interval, 
							uint16_t slave_latency, 
							uint16_t conn_timeout);
ble_char_update_t* ble_char_update_handle_get(void);
uint16_t ble_conn_handle_get(void);


#endif







