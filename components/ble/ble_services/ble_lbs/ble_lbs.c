/**
 * Copyright (c) 2013 - 2019, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include "sdk_common.h"
#if NRF_MODULE_ENABLED(BLE_LBS)
#include "ble_lbs.h"
#include "ble_srv_common.h"


/**@brief Function for handling the Write event.
 *
 * @param[in] p_dev_cfg      LED Button Service structure.
 * @param[in] p_ble_evt  Event received from the BLE stack.
 */
static void on_write(ble_dev_cfg_t * p_dev_cfg, ble_evt_t const * p_ble_evt)
{
    ble_gatts_evt_write_t const * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;

    if ((p_evt_write->handle == p_dev_cfg->ble_char_handles.value_handle)
        && (p_dev_cfg->ble_write_handler != NULL))
    {
        p_dev_cfg->ble_write_handler((uint8_t*)&p_evt_write->data[0], p_evt_write->len);
    }
    else if ((p_evt_write->handle == p_dev_cfg->lora_char_handles.value_handle)
			 && (p_dev_cfg->lora_write_handler != NULL))
	{
		p_dev_cfg->lora_write_handler((uint8_t*)&p_evt_write->data[0], p_evt_write->len);
	}
}


void ble_dev_cfg_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context)
{
    ble_dev_cfg_t * p_dev_cfg = (ble_dev_cfg_t *)p_context;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GATTS_EVT_WRITE:
            on_write(p_dev_cfg, p_ble_evt);
            break;

        default:
            // No implementation needed.
            break;
    }
}


uint32_t ble_dev_cfg_init(ble_dev_cfg_t * p_dev_cfg, const ble_dev_cfg_init_t * p_dev_cfg_init)
{
    uint32_t              err_code;
    ble_uuid_t            ble_uuid;
    ble_add_char_params_t add_char_params;

    // Initialize service structure.
    p_dev_cfg->ble_write_handler = p_dev_cfg_init->ble_write_handler;
	p_dev_cfg->lora_write_handler = p_dev_cfg_init->lora_write_handler;

    // Add service.
    ble_uuid128_t base_uuid = {DEV_CFG_UUID_BASE};
    err_code = sd_ble_uuid_vs_add(&base_uuid, &p_dev_cfg->uuid_type);
    VERIFY_SUCCESS(err_code);

    ble_uuid.type = p_dev_cfg->uuid_type;
    ble_uuid.uuid = DEV_CFG_UUID_SERVICE;

    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_dev_cfg->service_handle);
    VERIFY_SUCCESS(err_code);

    // Add BLE characteristic.
    memset(&add_char_params, 0, sizeof(add_char_params));
    add_char_params.uuid              = DEV_CFG_UUID_BLE_CHAR;
    add_char_params.uuid_type         = p_dev_cfg->uuid_type;
    add_char_params.init_len          = sizeof(uint8_t);
    add_char_params.max_len           = 25;
    add_char_params.char_props.read   = 1;
    add_char_params.char_props.write  = 1;
	
    add_char_params.read_access       = SEC_OPEN;
    add_char_params.write_access 	  = SEC_OPEN;

    err_code = characteristic_add(p_dev_cfg->service_handle,
                                  &add_char_params,
                                  &p_dev_cfg->ble_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    // Add LORA characteristic.
    memset(&add_char_params, 0, sizeof(add_char_params));
    add_char_params.uuid             = DEV_CFG_UUID_LORA_CHAR;
    add_char_params.uuid_type        = p_dev_cfg->uuid_type;
    add_char_params.init_len         = sizeof(uint8_t);
    add_char_params.max_len          = 20;
    add_char_params.char_props.read  = 1;
    add_char_params.char_props.write = 1;

    add_char_params.read_access  = SEC_OPEN;
    add_char_params.write_access = SEC_OPEN;

    return characteristic_add(p_dev_cfg->service_handle, 
							  &add_char_params, 
							  &p_dev_cfg->lora_char_handles);
}
#endif // NRF_MODULE_ENABLED(BLE_DEV_CFG)



