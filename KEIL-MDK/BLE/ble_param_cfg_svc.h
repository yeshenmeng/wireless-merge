#ifndef __BLE_PARAM_CFG_SVC_H__
#define __BLE_PARAM_CFG_SVC_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"
#include "nrf_sdh_ble.h"

#ifdef __cplusplus	
extern "C" {
#endif

#define BLE_PARAM_CFG_BLE_OBSERVER_PRIO	2
#define BLE_PARAM_CFG_DEF(_name)                                                                    \
static ble_param_cfg_t _name;                                                                       \
NRF_SDH_BLE_OBSERVER(_name ## _obs,                                                                 \
                     BLE_PARAM_CFG_BLE_OBSERVER_PRIO,                                               \
                     ble_param_cfg_on_ble_evt, &_name)

#define PARAM_CFG_UUID_BASE        				{0x23, 0xD1, 0xBC, 0xEA, 0x5F, 0x78, 0x23, 0x15, \
												 0xDE, 0xEF, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00}
#define PARAM_CFG_UUID_SERVICE					0x1540
#define PARAM_CFG_UUID_TX_POWER_CHAR			(PARAM_CFG_UUID_SERVICE+1)
#define PARAM_CFG_UUID_ADV_INTERVAL_CHAR		(PARAM_CFG_UUID_SERVICE+2)
#define PARAM_CFG_UUID_ADV_TIME_CHAR			(PARAM_CFG_UUID_SERVICE+3)
#define PARAM_CFG_UUID_MIN_CONN_INTERVAL_CHAR	(PARAM_CFG_UUID_SERVICE+4)
#define PARAM_CFG_UUID_MAX_CONN_INTERVAL_CHAR	(PARAM_CFG_UUID_SERVICE+5)
#define PARAM_CFG_UUID_SLAVE_LATENCY_CHAR		(PARAM_CFG_UUID_SERVICE+6)
#define PARAM_CFG_UUID_CONN_TIMEOUT_CHAR		(PARAM_CFG_UUID_SERVICE+7)

// Forward declaration of the ble_dev_cfg_t type.
typedef struct ble_param_cfg_s ble_param_cfg_t;

typedef uint8_t (*ble_param_cfg_tx_power_write_handler_t) (uint8_t* p_data, uint16_t len);
typedef uint8_t (*ble_param_cfg_adv_interval_write_handler_t) (uint8_t* p_data, uint16_t len);
typedef uint8_t (*ble_param_cfg_adv_time_write_handler_t) (uint8_t* p_data, uint16_t len);
typedef uint8_t (*ble_param_cfg_min_conn_interval_write_handler_t) (uint8_t* p_data, uint16_t len);
typedef uint8_t (*ble_param_cfg_max_conn_interval_write_handler_t) (uint8_t* p_data, uint16_t len);
typedef uint8_t (*ble_param_cfg_slave_latency_write_handler_t) (uint8_t* p_data, uint16_t len);
typedef uint8_t (*ble_param_cfg_conn_timeout_write_handler_t) (uint8_t* p_data, uint16_t len);

/** @brief LED Button Service init structure. This structure contains all options and data needed for
 *        initialization of the service.*/
typedef struct
{
    ble_param_cfg_tx_power_write_handler_t param_tx_power_write_handler; /**< Event handler to be called when the Characteristic is written. */
	ble_param_cfg_adv_interval_write_handler_t param_adv_interval_write_handler;
	ble_param_cfg_adv_time_write_handler_t param_adv_time_write_handler;
	ble_param_cfg_min_conn_interval_write_handler_t param_min_conn_interval_write_handler;
	ble_param_cfg_max_conn_interval_write_handler_t param_max_conn_interval_write_handler;
	ble_param_cfg_slave_latency_write_handler_t param_slave_latency_write_handler;
	ble_param_cfg_conn_timeout_write_handler_t param_conn_timeout_write_handler;
} ble_param_cfg_init_t;

/**@brief LED Button Service structure. This structure contains various status information for the service. */
struct ble_param_cfg_s
{
    uint16_t                    service_handle;      /**< Handle of Service (as provided by the BLE stack). */
    ble_gatts_char_handles_t    param_tx_power_char_handles;    /**< Handles related to the Characteristic. */
    ble_gatts_char_handles_t    param_adv_interval_char_handles;
    ble_gatts_char_handles_t    param_adv_time_char_handles;  
    ble_gatts_char_handles_t    param_min_conn_interval_char_handles;
    ble_gatts_char_handles_t    param_max_conn_interval_char_handles;  
    ble_gatts_char_handles_t    param_slave_latency_char_handles; 
    ble_gatts_char_handles_t    param_conn_timeout_char_handles; 
    uint8_t                     uuid_type;           /**< UUID type for the Service. */
    ble_param_cfg_tx_power_write_handler_t param_tx_power_write_handler; /**< Event handler to be called when the Characteristic is written. */
	ble_param_cfg_adv_interval_write_handler_t param_adv_interval_write_handler;
	ble_param_cfg_adv_time_write_handler_t param_adv_time_write_handler;
	ble_param_cfg_min_conn_interval_write_handler_t param_min_conn_interval_write_handler;
	ble_param_cfg_max_conn_interval_write_handler_t param_max_conn_interval_write_handler;
	ble_param_cfg_slave_latency_write_handler_t param_slave_latency_write_handler;
	ble_param_cfg_conn_timeout_write_handler_t param_conn_timeout_write_handler;
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
uint32_t ble_param_cfg_init(ble_param_cfg_t * p_param_cfg, const ble_param_cfg_init_t * p_param_cfg_init);


/**@brief Function for handling the application's BLE stack events.
 *
 * @details This function handles all events from the BLE stack that are of interest to the LED Button Service.
 *
 * @param[in] p_ble_evt  Event received from the BLE stack.
 * @param[in] p_context  LED Button Service structure.
 */
void ble_param_cfg_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context);

#ifdef __cplusplus
}
#endif

#endif

/** @} */
