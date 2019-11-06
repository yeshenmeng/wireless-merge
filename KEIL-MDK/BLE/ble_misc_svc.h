#ifndef __BLE_MISC_SVC_H__
#define __BLE_MISC_SVC_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"
#include "nrf_sdh_ble.h"

#ifdef __cplusplus	
extern "C" {
#endif

#define BLE_MISC_BLE_OBSERVER_PRIO	2
#define BLE_MISC_DEF(_name)                                                                     \
static ble_misc_t _name;                                                                        \
NRF_SDH_BLE_OBSERVER(_name ## _obs,                                                             \
                     BLE_MISC_BLE_OBSERVER_PRIO,                                                \
                     ble_misc_on_ble_evt, &_name)

#define MISC_UUID_BASE        				{0x23, 0xD1, 0xBC, 0xEA, 0x5F, 0x78, 0x23, 0x15, \
											 0xDE, 0xEF, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00}
#define MISC_UUID_SERVICE					0x15A0
#define MISC_UUID_PAYLOAD_LENGTH_CHAR		(MISC_UUID_SERVICE+1)
#define MISC_UUID_COUNTING_MODE_CHAR		(MISC_UUID_SERVICE+2)
#define MISC_UUID_TIMER_MODE_CHAR			(MISC_UUID_SERVICE+3)
#define MISC_UUID_COMM_INTERVAL_CHAR		(MISC_UUID_SERVICE+4)
#define MISC_UUID_COMM_CTRL_CHAR			(MISC_UUID_SERVICE+5)
#define MISC_UUID_LOST_RATE_CHAR			(MISC_UUID_SERVICE+6)
#define MISC_UUID_TEST_PROGRESS_CHAR		(MISC_UUID_SERVICE+7)
#define MISC_UUID_OTA_CHAR					(MISC_UUID_SERVICE+8)
											 
// Forward declaration of the ble_dev_cfg_t type.
typedef struct ble_misc_s ble_misc_t;

typedef void (*ble_misc_payload_length_write_handler_t) (uint8_t* p_data, uint16_t len);
typedef void (*ble_misc_counting_mode_write_handler_t) (uint8_t* p_data, uint16_t len);
typedef void (*ble_misc_timer_mode_write_handler_t) (uint8_t* p_data, uint16_t len);
typedef void (*ble_misc_comm_interval_write_handler_t) (uint8_t* p_data, uint16_t len);
typedef void (*ble_misc_comm_ctrl_write_handler_t) (uint8_t* p_data, uint16_t len);
typedef void (*ble_misc_comm_ota_write_handler_t) (uint8_t* p_data, uint16_t len);
//typedef uint8_t (*ble_misc_lost_rate_write_handler_t) (uint8_t* p_data, uint16_t len);

/** @brief LED Button Service init structure. This structure contains all options and data needed for
 *        initialization of the service.*/
typedef struct
{
    ble_misc_payload_length_write_handler_t payload_length_write_handler; /**< Event handler to be called when the Characteristic is written. */
	ble_misc_counting_mode_write_handler_t counting_mode_write_handler;
	ble_misc_timer_mode_write_handler_t timer_mode_write_handler;
	ble_misc_comm_interval_write_handler_t comm_interval_write_handler;
	ble_misc_comm_ctrl_write_handler_t comm_ctrl_write_handler;
	ble_misc_comm_ota_write_handler_t comm_ota_write_handler;
} ble_misc_init_t;

/**@brief LED Button Service structure. This structure contains various status information for the service. */
struct ble_misc_s
{
    uint16_t                    service_handle;      /**< Handle of Service (as provided by the BLE stack). */
    ble_gatts_char_handles_t    payload_length_char_handles;    /**< Handles related to the Characteristic. */
    ble_gatts_char_handles_t    counting_mode_char_handles;
    ble_gatts_char_handles_t    timer_mode_char_handles;  
    ble_gatts_char_handles_t    comm_interval_char_handles;
    ble_gatts_char_handles_t    comm_ctrl_char_handles;  
    ble_gatts_char_handles_t    lost_rate_char_handles; 
	ble_gatts_char_handles_t	test_progress_char_handles;
	ble_gatts_char_handles_t	ota_char_handles;
    uint8_t                     uuid_type;           /**< UUID type for the Service. */
    ble_misc_payload_length_write_handler_t payload_length_write_handler; /**< Event handler to be called when the Characteristic is written. */
	ble_misc_counting_mode_write_handler_t counting_mode_write_handler;
	ble_misc_timer_mode_write_handler_t timer_mode_write_handler;
	ble_misc_comm_interval_write_handler_t comm_interval_write_handler;
	ble_misc_comm_ctrl_write_handler_t comm_ctrl_write_handler;
	ble_misc_comm_ota_write_handler_t comm_ota_write_handler;
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
uint32_t ble_misc_init(ble_misc_t * p_misc, const ble_misc_init_t * p_misc_init);


/**@brief Function for handling the application's BLE stack events.
 *
 * @details This function handles all events from the BLE stack that are of interest to the LED Button Service.
 *
 * @param[in] p_ble_evt  Event received from the BLE stack.
 * @param[in] p_context  LED Button Service structure.
 */
void ble_misc_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context);

#ifdef __cplusplus
}
#endif

#endif

/** @} */
