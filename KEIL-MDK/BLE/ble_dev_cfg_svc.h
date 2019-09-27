#ifndef __BLE_DEV_CFG_SVC_H__
#define __BLE_DEV_CFG_SVC_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"
#include "nrf_sdh_ble.h"

#ifdef __cplusplus
extern "C" {
#endif

/**@brief   Macro for defining a ble_lbs instance.
 *
 * @param   _name   Name of the instance.
 * @hideinitializer
 */
#define BLE_DEV_CFG_BLE_OBSERVER_PRIO	2
#define BLE_DEV_CFG_DEF(_name)                                                                      \
static ble_dev_cfg_t _name;                                                                         \
NRF_SDH_BLE_OBSERVER(_name ## _obs,                                                                 \
                     BLE_DEV_CFG_BLE_OBSERVER_PRIO,                                                 \
                     ble_dev_cfg_on_ble_evt, &_name)

#define DEV_CFG_UUID_BASE        					{0x23, 0xD1, 0xBC, 0xEA, 0x5F, 0x78, 0x23, 0x15,\
													 0xDE, 0xEF, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00}

/* 倾角蓝牙设备配置服务与特征 */
#define DEV_I_CFG_UUID_SERVICE						0x1580
#define DEV_I_CFG_UUID_GATEWAY_ADDR_CHAR			(DEV_I_CFG_UUID_SERVICE+1)
#define DEV_I_CFG_UUID_LONG_ADDR_CHAR				(DEV_I_CFG_UUID_SERVICE+2)
#define DEV_I_CFG_UUID_SHORT_ADDR_CHAR				(DEV_I_CFG_UUID_SERVICE+3)
#define DEV_I_CFG_UUID_MODE_CHAR					(DEV_I_CFG_UUID_SERVICE+4)
#define DEV_I_CFG_UUID_INTERVAL_CHAR				(DEV_I_CFG_UUID_SERVICE+5)
#define DEV_I_CFG_UUID_TIME_STAMP_CHAR				(DEV_I_CFG_UUID_SERVICE+6)
#define DEV_I_CFG_UUID_BATTERY_CHAR					(DEV_I_CFG_UUID_SERVICE+7)
#define DEV_I_CFG_UUID_TEMPERATURE_CHAR				(DEV_I_CFG_UUID_SERVICE+8)
#define DEV_I_CFG_UUID_X_ANGLE_CHAR					(DEV_I_CFG_UUID_SERVICE+9)
#define DEV_I_CFG_UUID_Y_ANGLE_CHAR					(DEV_I_CFG_UUID_SERVICE+10)
#define DEV_I_CFG_UUID_Z_ANGLE_CHAR					(DEV_I_CFG_UUID_SERVICE+11)
#define DEV_I_CFG_UUID_X_ANGLE_THRESHOLD_CHAR		(DEV_I_CFG_UUID_SERVICE+12)
#define DEV_I_CFG_UUID_Y_ANGLE_THRESHOLD_CHAR		(DEV_I_CFG_UUID_SERVICE+13)
#define DEV_I_CFG_UUID_Z_ANGLE_THRESHOLD_CHAR		(DEV_I_CFG_UUID_SERVICE+14)
#define DEV_I_CFG_UUID_LORA_RSSI_CHAR				(DEV_I_CFG_UUID_SERVICE+15)
#define DEV_I_CFG_UUID_SW_VERSION_CHAR				(DEV_I_CFG_UUID_SERVICE+16)
#define DEV_I_CFG_UUID_HW_VERSION_CHAR				(DEV_I_CFG_UUID_SERVICE+17)
													 
/* 崩塌计蓝牙设备配置服务与特征 */
#define DEV_C_CFG_UUID_SERVICE						0x1580
#define DEV_C_CFG_UUID_GATEWAY_ADDR_CHAR			(DEV_C_CFG_UUID_SERVICE+1)
#define DEV_C_CFG_UUID_LONG_ADDR_CHAR				(DEV_C_CFG_UUID_SERVICE+2)
#define DEV_C_CFG_UUID_SHORT_ADDR_CHAR				(DEV_C_CFG_UUID_SERVICE+3)
#define DEV_C_CFG_UUID_MODE_CHAR					(DEV_C_CFG_UUID_SERVICE+4)
#define DEV_C_CFG_UUID_INTERVAL_CHAR				(DEV_C_CFG_UUID_SERVICE+5)
#define DEV_C_CFG_UUID_TIME_STAMP_CHAR				(DEV_C_CFG_UUID_SERVICE+6)
#define DEV_C_CFG_UUID_BATTERY_CHAR					(DEV_C_CFG_UUID_SERVICE+7)
#define DEV_C_CFG_UUID_TEMPERATURE_CHAR				(DEV_C_CFG_UUID_SERVICE+8)
#define DEV_C_CFG_UUID_X_ACCEL_CHAR					(DEV_C_CFG_UUID_SERVICE+9)
#define DEV_C_CFG_UUID_Y_ACCEL_CHAR					(DEV_C_CFG_UUID_SERVICE+10)
#define DEV_C_CFG_UUID_Z_ACCEL_CHAR					(DEV_C_CFG_UUID_SERVICE+11)
#define DEV_C_CFG_UUID_X_ANGLE_CHAR					(DEV_C_CFG_UUID_SERVICE+12)
#define DEV_C_CFG_UUID_Y_ANGLE_CHAR					(DEV_C_CFG_UUID_SERVICE+13)
#define DEV_C_CFG_UUID_Z_ANGLE_CHAR					(DEV_C_CFG_UUID_SERVICE+14)
#define DEV_C_CFG_UUID_ACCEL_SLOPE_THRESHOLD_CHAR	(DEV_C_CFG_UUID_SERVICE+15)
#define DEV_C_CFG_UUID_CONSECUTIVE_DATA_POINTS_CHAR	(DEV_C_CFG_UUID_SERVICE+16)
#define DEV_C_CFG_UUID_LORA_RSSI_CHAR				(DEV_C_CFG_UUID_SERVICE+17)
#define DEV_C_CFG_UUID_SW_VERSION_CHAR				(DEV_C_CFG_UUID_SERVICE+18)
#define DEV_C_CFG_UUID_HW_VERSION_CHAR				(DEV_C_CFG_UUID_SERVICE+19)


// Forward declaration of the ble_dev_cfg_t type.
typedef struct ble_dev_cfg_s ble_dev_cfg_t;

typedef void (*ble_dev_cfg_gateway_write_handler_t) (uint8_t* p_data, uint16_t len);
typedef void (*ble_dev_cfg_long_addr_write_handler_t) (uint8_t* p_data, uint16_t len);
typedef void (*ble_dev_cfg_short_addr_write_handler_t) (uint8_t* p_data, uint16_t len);
typedef void (*ble_dev_cfg_time_stamp_read_handler_t) (uint8_t* p_data, uint16_t* len);
//typedef void (*ble_dev_cfg_interval_write_handler_t) (uint8_t* p_data, uint16_t len);
//typedef void (*ble_dev_cfg_time_stamp_write_handler_t) (uint8_t* p_data, uint16_t len);
//typedef void (*ble_dev_cfg_battery_write_handler_t) (uint8_t* p_data, uint16_t len);
//typedef void (*ble_dev_cfg_sw_version_write_handler_t) (uint8_t* p_data, uint16_t len);
//typedef void (*ble_dev_cfg_hw_version_write_handler_t) (uint8_t* p_data, uint16_t len);

/** @brief LED Button Service init structure. This structure contains all options and data needed for
 *        initialization of the service.*/
typedef struct
{
	ble_dev_cfg_gateway_write_handler_t dev_gateway_write_handler;
	ble_dev_cfg_long_addr_write_handler_t dev_long_addr_write_handler;
	ble_dev_cfg_short_addr_write_handler_t dev_short_addr_write_handler;
	ble_dev_cfg_time_stamp_read_handler_t dev_time_stamp_read_handler;
//	ble_dev_cfg_interval_write_handler_t dev_interval_write_handler;
//	ble_dev_cfg_time_stamp_write_handler_t dev_time_stamp_write_handler;
//	ble_dev_cfg_battery_write_handler_t dev_battery_write_handler;
//	ble_dev_cfg_sw_version_write_handler_t dev_sw_version_write_handler;
//	ble_dev_cfg_hw_version_write_handler_t dev_hw_version_write_handler;
} ble_dev_cfg_init_t;

/**@brief LED Button Service structure. This structure contains various status information for the service. */
struct ble_dev_cfg_s
{
    uint16_t                    service_handle;      /**< Handle of Service (as provided by the BLE stack). */
    ble_gatts_char_handles_t    dev_gateway_addr_char_handles;    /**< Handles related to the Characteristic. */
    ble_gatts_char_handles_t    dev_long_addr_char_handles;
    ble_gatts_char_handles_t    dev_short_addr_char_handles;  
	ble_gatts_char_handles_t	dev_mode_char_handles;  
    ble_gatts_char_handles_t    dev_interval_char_handles;
    ble_gatts_char_handles_t    dev_time_stamp_char_handles;  
    ble_gatts_char_handles_t    dev_battery_char_handles; 
	ble_gatts_char_handles_t	dev_temperature_char_handles; 
	ble_gatts_char_handles_t	dev_x_accel_char_handles; 
	ble_gatts_char_handles_t	dev_y_accel_char_handles;
	ble_gatts_char_handles_t	dev_z_accel_char_handles;
	ble_gatts_char_handles_t	dev_x_angle_char_handles;
	ble_gatts_char_handles_t	dev_y_angle_char_handles;
	ble_gatts_char_handles_t	dev_z_angle_char_handles;
	ble_gatts_char_handles_t	dev_accel_slope_threshold_char_handles; 
	ble_gatts_char_handles_t	dev_consecutive_data_points_char_handles; 
	ble_gatts_char_handles_t	dev_x_angle_threshold_char_handles; 
	ble_gatts_char_handles_t	dev_y_angle_threshold_char_handles; 
	ble_gatts_char_handles_t	dev_z_angle_threshold_char_handles; 
	ble_gatts_char_handles_t	dev_lora_rssi_char_handles; 
    ble_gatts_char_handles_t    dev_sw_version_char_handles; 
    ble_gatts_char_handles_t    dev_hw_version_char_handles;
    uint8_t                     uuid_type;           /**< UUID type for the Service. */
	ble_dev_cfg_gateway_write_handler_t dev_gateway_write_handler; /**< Event handler to be called when the Characteristic is written. */
	ble_dev_cfg_long_addr_write_handler_t dev_long_addr_write_handler;
	ble_dev_cfg_short_addr_write_handler_t dev_short_addr_write_handler;
	ble_dev_cfg_time_stamp_read_handler_t dev_time_stamp_read_handler; /**< Event handler to be called when the Characteristic is read. */
//	ble_dev_cfg_interval_write_handler_t dev_interval_write_handler;
//	ble_dev_cfg_time_stamp_write_handler_t dev_time_stamp_write_handler;
//	ble_dev_cfg_battery_write_handler_t dev_battery_write_handler;
//	ble_dev_cfg_sw_version_write_handler_t dev_sw_version_write_handler;
//	ble_dev_cfg_hw_version_write_handler_t dev_hw_version_write_handler;
};


/**@brief Function for initializing the LED Button Service.
 *
 * @param[out] p_lbs      LED Button Service structure. This structure must be supplied by
 *                        the application. It is initialized by this function and will later
 *                        be used to identify this particular service instance.
 * @param[in] p_lbs_init  Information needed to initialize the service.
 *
 * @retval NRF_SUCCESS If the service was initialized successfully. Otherwise, an error code is returned.
 */
uint32_t ble_dev_cfg_init(ble_dev_cfg_t * p_dev_cfg, const ble_dev_cfg_init_t * p_dev_cfg_init);


/**@brief Function for handling the application's BLE stack events.
 *
 * @details This function handles all events from the BLE stack that are of interest to the LED Button Service.
 *
 * @param[in] p_ble_evt  Event received from the BLE stack.
 * @param[in] p_context  LED Button Service structure.
 */
void ble_dev_cfg_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context);


#ifdef __cplusplus
}
#endif

#endif // BLE_LBS_H__

/** @} */
