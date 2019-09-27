#ifndef __BLE_LORA_CFG_SVC_H__
#define __BLE_LORA_CFG_SVC_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"
#include "nrf_sdh_ble.h"

#ifdef __cplusplus	
extern "C" {
#endif

#define BLE_LORA_CFG_BLE_OBSERVER_PRIO	2
#define BLE_LORA_CFG_DEF(_name)                                                                     \
static ble_lora_cfg_t _name;                                                                        \
NRF_SDH_BLE_OBSERVER(_name ## _obs,                                                                 \
                     BLE_LORA_CFG_BLE_OBSERVER_PRIO,                                                \
                     ble_lora_cfg_on_ble_evt, &_name)

#define LORA_CFG_UUID_BASE        		{0x23, 0xD1, 0xBC, 0xEA, 0x5F, 0x78, 0x23, 0x15, \
										 0xDE, 0xEF, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00}
#define LORA_CFG_UUID_SERVICE			0x1560
#define LORA_CFG_UUID_FREQ_CHAR			(LORA_CFG_UUID_SERVICE+1)
#define LORA_CFG_UUID_POWER_CHAR		(LORA_CFG_UUID_SERVICE+2)
#define LORA_CFG_UUID_BW_CHAR			(LORA_CFG_UUID_SERVICE+3)
#define LORA_CFG_UUID_SF_CHAR			(LORA_CFG_UUID_SERVICE+4)
#define LORA_CFG_UUID_CODE_RATE_CHAR	(LORA_CFG_UUID_SERVICE+5)
#define LORA_CFG_UUID_PREAMBLE_CHAR		(LORA_CFG_UUID_SERVICE+6)
#define LORA_CFG_UUID_HEADER_CHAR		(LORA_CFG_UUID_SERVICE+7)
#define LORA_CFG_UUID_CRC_CHAR			(LORA_CFG_UUID_SERVICE+8)

// Forward declaration of the ble_dev_cfg_t type.
typedef struct ble_lora_cfg_s ble_lora_cfg_t;

typedef uint8_t (*ble_lora_cfg_freq_write_handler_t) (uint8_t* p_data, uint16_t len);
typedef uint8_t (*ble_lora_cfg_power_write_handler_t) (uint8_t* p_data, uint16_t len);
typedef uint8_t (*ble_lora_cfg_bw_write_handler_t) (uint8_t* p_data, uint16_t len);
typedef uint8_t (*ble_lora_cfg_sf_write_handler_t) (uint8_t* p_data, uint16_t len);
typedef uint8_t (*ble_lora_cfg_code_rate_write_handler_t) (uint8_t* p_data, uint16_t len);
typedef uint8_t (*ble_lora_cfg_preamble_write_handler_t) (uint8_t* p_data, uint16_t len);
typedef uint8_t (*ble_lora_cfg_header_write_handler_t) (uint8_t* p_data, uint16_t len);
typedef uint8_t (*ble_lora_cfg_crc_write_handler_t) (uint8_t* p_data, uint16_t len);

/** @brief LED Button Service init structure. This structure contains all options and data needed for
 *        initialization of the service.*/
typedef struct
{
    ble_lora_cfg_freq_write_handler_t lora_freq_write_handler; /**< Event handler to be called when the Characteristic is written. */
	ble_lora_cfg_power_write_handler_t lora_power_write_handler;
	ble_lora_cfg_bw_write_handler_t lora_bw_write_handler;
	ble_lora_cfg_sf_write_handler_t lora_sf_write_handler;
	ble_lora_cfg_code_rate_write_handler_t lora_code_rate_write_handler;
	ble_lora_cfg_preamble_write_handler_t lora_preamble_write_handler;
	ble_lora_cfg_header_write_handler_t lora_header_write_handler;
	ble_lora_cfg_crc_write_handler_t lora_crc_write_handler;
} ble_lora_cfg_init_t;

/**@brief LED Button Service structure. This structure contains various status information for the service. */
struct ble_lora_cfg_s
{
    uint16_t                    service_handle;      /**< Handle of Service (as provided by the BLE stack). */
    ble_gatts_char_handles_t    lora_freq_char_handles;    /**< Handles related to the Characteristic. */
    ble_gatts_char_handles_t    lora_power_char_handles;
    ble_gatts_char_handles_t    lora_bw_char_handles;  
    ble_gatts_char_handles_t    lora_sf_char_handles;
    ble_gatts_char_handles_t    lora_code_rate_char_handles;  
    ble_gatts_char_handles_t    lora_preamble_char_handles; 
    ble_gatts_char_handles_t    lora_header_char_handles; 
    ble_gatts_char_handles_t    lora_crc_char_handles;
    uint8_t                     uuid_type;           /**< UUID type for the Service. */
    ble_lora_cfg_freq_write_handler_t lora_freq_write_handler; /**< Event handler to be called when the Characteristic is written. */
	ble_lora_cfg_power_write_handler_t lora_power_write_handler;
	ble_lora_cfg_bw_write_handler_t lora_bw_write_handler;
	ble_lora_cfg_sf_write_handler_t lora_sf_write_handler;
	ble_lora_cfg_code_rate_write_handler_t lora_code_rate_write_handler;
	ble_lora_cfg_preamble_write_handler_t lora_preamble_write_handler;
	ble_lora_cfg_header_write_handler_t lora_header_write_handler;
	ble_lora_cfg_crc_write_handler_t lora_crc_write_handler;
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
uint32_t ble_lora_cfg_init(ble_lora_cfg_t * p_lora_cfg, const ble_lora_cfg_init_t * p_lora_cfg_init);


/**@brief Function for handling the application's BLE stack events.
 *
 * @details This function handles all events from the BLE stack that are of interest to the LED Button Service.
 *
 * @param[in] p_ble_evt  Event received from the BLE stack.
 * @param[in] p_context  LED Button Service structure.
 */
void ble_lora_cfg_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context);

#ifdef __cplusplus
}
#endif

#endif

/** @} */
