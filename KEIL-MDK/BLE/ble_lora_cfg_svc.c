#include "sdk_common.h"
#include "ble_lora_cfg_svc.h"
#include "ble_srv_common.h"
#include "app_error.h"


typedef enum {
	VERIFY_DATA_SUCCESS,
	VERIFY_DATA_FAIL,
	VERIFY_DATA_INVALID,
}verify_data;

static void on_write(ble_lora_cfg_t * p_lora_cfg, ble_evt_t const * p_ble_evt)
{
    ble_gatts_evt_write_t const * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;

    if ((p_evt_write->handle == p_lora_cfg->lora_freq_char_handles.value_handle)
        && (p_lora_cfg->lora_freq_write_handler != NULL))
    {
        p_lora_cfg->lora_freq_write_handler((uint8_t*)&p_evt_write->data[0], p_evt_write->len);
    }
    else if ((p_evt_write->handle == p_lora_cfg->lora_power_char_handles.value_handle)
			 && (p_lora_cfg->lora_power_write_handler != NULL))
	{
		p_lora_cfg->lora_power_write_handler((uint8_t*)&p_evt_write->data[0], p_evt_write->len);
	}
    else if ((p_evt_write->handle == p_lora_cfg->lora_bw_char_handles.value_handle)
			 && (p_lora_cfg->lora_bw_write_handler != NULL))
	{
		p_lora_cfg->lora_bw_write_handler((uint8_t*)&p_evt_write->data[0], p_evt_write->len);
	}
    else if ((p_evt_write->handle == p_lora_cfg->lora_sf_char_handles.value_handle)
			 && (p_lora_cfg->lora_sf_write_handler != NULL))
	{
		p_lora_cfg->lora_sf_write_handler((uint8_t*)&p_evt_write->data[0], p_evt_write->len);
	}
    else if ((p_evt_write->handle == p_lora_cfg->lora_code_rate_char_handles.value_handle)
			 && (p_lora_cfg->lora_code_rate_write_handler != NULL))
	{
		p_lora_cfg->lora_code_rate_write_handler((uint8_t*)&p_evt_write->data[0], p_evt_write->len);
	}
    else if ((p_evt_write->handle == p_lora_cfg->lora_preamble_char_handles.value_handle)
			 && (p_lora_cfg->lora_preamble_write_handler != NULL))
	{
		p_lora_cfg->lora_preamble_write_handler((uint8_t*)&p_evt_write->data[0], p_evt_write->len);
	}
    else if ((p_evt_write->handle == p_lora_cfg->lora_header_char_handles.value_handle)
			 && (p_lora_cfg->lora_header_write_handler != NULL))
	{
		p_lora_cfg->lora_header_write_handler((uint8_t*)&p_evt_write->data[0], p_evt_write->len);
	}
    else if ((p_evt_write->handle == p_lora_cfg->lora_crc_char_handles.value_handle)
			 && (p_lora_cfg->lora_crc_write_handler != NULL))
	{
		p_lora_cfg->lora_crc_write_handler((uint8_t*)&p_evt_write->data[0], p_evt_write->len);
	}
}

static verify_data on_write_auth(ble_lora_cfg_t * p_lora_cfg, ble_evt_t const * p_ble_evt)
{
	ble_gatts_evt_rw_authorize_request_t const * p_evt_auth_req = &p_ble_evt->evt.gatts_evt.params.authorize_request;

    if ((p_evt_auth_req->request.write.handle == p_lora_cfg->lora_freq_char_handles.value_handle)
        && (p_lora_cfg->lora_freq_write_handler != NULL))
    {
        return (verify_data)p_lora_cfg->lora_freq_write_handler((uint8_t*)&p_evt_auth_req->request.write.data[0], p_evt_auth_req->request.write.len);
    }
    else if ((p_evt_auth_req->request.write.handle == p_lora_cfg->lora_power_char_handles.value_handle)
			 && (p_lora_cfg->lora_power_write_handler != NULL))
	{
		return (verify_data)p_lora_cfg->lora_power_write_handler((uint8_t*)&p_evt_auth_req->request.write.data[0], p_evt_auth_req->request.write.len);
	}
    else if ((p_evt_auth_req->request.write.handle == p_lora_cfg->lora_bw_char_handles.value_handle)
			 && (p_lora_cfg->lora_bw_write_handler != NULL))
	{
		return (verify_data)p_lora_cfg->lora_bw_write_handler((uint8_t*)&p_evt_auth_req->request.write.data[0], p_evt_auth_req->request.write.len);
	}
    else if ((p_evt_auth_req->request.write.handle == p_lora_cfg->lora_sf_char_handles.value_handle)
			 && (p_lora_cfg->lora_sf_write_handler != NULL))
	{
		return (verify_data)p_lora_cfg->lora_sf_write_handler((uint8_t*)&p_evt_auth_req->request.write.data[0], p_evt_auth_req->request.write.len);
	}
    else if ((p_evt_auth_req->request.write.handle == p_lora_cfg->lora_code_rate_char_handles.value_handle)
			 && (p_lora_cfg->lora_code_rate_write_handler != NULL))
	{
		return (verify_data)p_lora_cfg->lora_code_rate_write_handler((uint8_t*)&p_evt_auth_req->request.write.data[0], p_evt_auth_req->request.write.len);
	}
    else if ((p_evt_auth_req->request.write.handle == p_lora_cfg->lora_preamble_char_handles.value_handle)
			 && (p_lora_cfg->lora_preamble_write_handler != NULL))
	{
		return (verify_data)p_lora_cfg->lora_preamble_write_handler((uint8_t*)&p_evt_auth_req->request.write.data[0], p_evt_auth_req->request.write.len);
	}
    else if ((p_evt_auth_req->request.write.handle == p_lora_cfg->lora_header_char_handles.value_handle)
			 && (p_lora_cfg->lora_header_write_handler != NULL))
	{
		return (verify_data)p_lora_cfg->lora_header_write_handler((uint8_t*)&p_evt_auth_req->request.write.data[0], p_evt_auth_req->request.write.len);
	}
    else if ((p_evt_auth_req->request.write.handle == p_lora_cfg->lora_crc_char_handles.value_handle)
			 && (p_lora_cfg->lora_crc_write_handler != NULL))
	{
		return (verify_data)p_lora_cfg->lora_crc_write_handler((uint8_t*)&p_evt_auth_req->request.write.data[0], p_evt_auth_req->request.write.len);
	}
	
	return VERIFY_DATA_INVALID;
}

static void on_read_write_auth(ble_lora_cfg_t * p_lora_cfg, ble_evt_t const * p_ble_evt)
{
	ble_gatts_evt_rw_authorize_request_t const * req = &p_ble_evt->evt.gatts_evt.params.authorize_request;
	
	if (req->type != BLE_GATTS_AUTHORIZE_TYPE_INVALID)
	{
		/* 写权限操作 */
		if (req->type == BLE_GATTS_AUTHORIZE_TYPE_WRITE)
		{
			ble_gatts_rw_authorize_reply_params_t auth_reply = {0};
			auth_reply.type = BLE_GATTS_AUTHORIZE_TYPE_WRITE;
			
			verify_data verify_data_ret = on_write_auth(p_lora_cfg, p_ble_evt);
			if(verify_data_ret == VERIFY_DATA_SUCCESS)
			{
				auth_reply.params.write.update = 1;
				auth_reply.params.write.offset = req->request.write.offset;
				auth_reply.params.write.len = req->request.write.len;
				auth_reply.params.write.p_data = req->request.write.data;
				auth_reply.params.write.gatt_status = BLE_GATT_STATUS_SUCCESS;
				uint32_t err_code = sd_ble_gatts_rw_authorize_reply(p_ble_evt->evt.gatts_evt.conn_handle, &auth_reply);
				APP_ERROR_CHECK(err_code);
			}
			else if(verify_data_ret == VERIFY_DATA_FAIL)
			{
				auth_reply.params.write.update = 0;
				auth_reply.params.write.gatt_status = BLE_GATT_STATUS_ATTERR_INVALID_ATT_VAL_LENGTH;
				uint32_t err_code = sd_ble_gatts_rw_authorize_reply(p_ble_evt->evt.gatts_evt.conn_handle, &auth_reply);
				APP_ERROR_CHECK(err_code);
			}
			else if(verify_data_ret == VERIFY_DATA_INVALID)
			{
				
			}
		}
		/* 读权限操作 */
		else if(req->type == BLE_GATTS_AUTHORIZE_TYPE_READ)
		{
		}
	}
}

void ble_lora_cfg_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context)
{
    ble_lora_cfg_t * p_lora_cfg = (ble_lora_cfg_t *)p_context;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GATTS_EVT_WRITE:
            on_write(p_lora_cfg, p_ble_evt);
            break;
		
		case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
			on_read_write_auth(p_lora_cfg, p_ble_evt);
			break;

        default:
            // No implementation needed.
            break;
    }
}

uint32_t ble_lora_cfg_init(ble_lora_cfg_t * p_lora_cfg, const ble_lora_cfg_init_t * p_lora_cfg_init)
{
    uint32_t              err_code;
    ble_uuid_t            ble_uuid;
    ble_add_char_params_t add_char_params;

    // Initialize service structure.
	p_lora_cfg->lora_freq_write_handler = p_lora_cfg_init->lora_freq_write_handler;
	p_lora_cfg->lora_power_write_handler = p_lora_cfg_init->lora_power_write_handler;
	p_lora_cfg->lora_bw_write_handler = p_lora_cfg_init->lora_bw_write_handler;
	p_lora_cfg->lora_sf_write_handler = p_lora_cfg_init->lora_sf_write_handler;
	p_lora_cfg->lora_code_rate_write_handler = p_lora_cfg_init->lora_code_rate_write_handler;
	p_lora_cfg->lora_preamble_write_handler = p_lora_cfg_init->lora_preamble_write_handler;
	p_lora_cfg->lora_header_write_handler = p_lora_cfg_init->lora_header_write_handler;
	p_lora_cfg->lora_crc_write_handler = p_lora_cfg_init->lora_crc_write_handler;

    // Add service.
    ble_uuid128_t base_uuid = {LORA_CFG_UUID_BASE};
    err_code = sd_ble_uuid_vs_add(&base_uuid, &p_lora_cfg->uuid_type);
    VERIFY_SUCCESS(err_code);

    ble_uuid.type = p_lora_cfg->uuid_type;
    ble_uuid.uuid = LORA_CFG_UUID_SERVICE;

    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_lora_cfg->service_handle);
    VERIFY_SUCCESS(err_code);

    // Add characteristic.
    memset(&add_char_params, 0, sizeof(add_char_params));
    add_char_params.uuid              = LORA_CFG_UUID_FREQ_CHAR;
    add_char_params.uuid_type         = p_lora_cfg->uuid_type;
    add_char_params.init_len          = 2;
    add_char_params.max_len           = 2;
    add_char_params.char_props.read   = 1;
    add_char_params.char_props.write  = 1;
    add_char_params.read_access       = SEC_OPEN;
    add_char_params.write_access 	  = SEC_OPEN;
	add_char_params.is_defered_write  = 1;
    err_code = characteristic_add(p_lora_cfg->service_handle, &add_char_params, &p_lora_cfg->lora_freq_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
	
	add_char_params.uuid = LORA_CFG_UUID_POWER_CHAR;
	add_char_params.init_len = 1;
	add_char_params.max_len = 1;
    err_code = characteristic_add(p_lora_cfg->service_handle, &add_char_params, &p_lora_cfg->lora_power_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
	
	add_char_params.uuid = LORA_CFG_UUID_BW_CHAR;
    err_code = characteristic_add(p_lora_cfg->service_handle, &add_char_params, &p_lora_cfg->lora_bw_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
	
	add_char_params.uuid = LORA_CFG_UUID_SF_CHAR;
    err_code = characteristic_add(p_lora_cfg->service_handle, &add_char_params, &p_lora_cfg->lora_sf_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

	add_char_params.uuid = LORA_CFG_UUID_CODE_RATE_CHAR;
    err_code = characteristic_add(p_lora_cfg->service_handle, &add_char_params, &p_lora_cfg->lora_code_rate_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

	add_char_params.uuid = LORA_CFG_UUID_PREAMBLE_CHAR;
    err_code = characteristic_add(p_lora_cfg->service_handle, &add_char_params, &p_lora_cfg->lora_preamble_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
	
	add_char_params.uuid = LORA_CFG_UUID_HEADER_CHAR;
    err_code = characteristic_add(p_lora_cfg->service_handle, &add_char_params, &p_lora_cfg->lora_header_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
	
	add_char_params.uuid = LORA_CFG_UUID_CRC_CHAR;
    return characteristic_add(p_lora_cfg->service_handle, &add_char_params, &p_lora_cfg->lora_crc_char_handles);
}





