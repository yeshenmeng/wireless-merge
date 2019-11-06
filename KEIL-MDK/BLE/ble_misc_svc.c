#include "sdk_common.h"
#include "ble_misc_svc.h"
#include "ble_srv_common.h"
#include "app_error.h"


static void on_write(ble_misc_t * p_misc, ble_evt_t const * p_ble_evt)
{
    ble_gatts_evt_write_t const * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;

    if ((p_evt_write->handle == p_misc->payload_length_char_handles.value_handle)
        && (p_misc->payload_length_write_handler != NULL))
    {
        p_misc->payload_length_write_handler((uint8_t*)&p_evt_write->data[0], p_evt_write->len);
    }
    else if ((p_evt_write->handle == p_misc->counting_mode_char_handles.value_handle)
			 && (p_misc->counting_mode_write_handler != NULL))
	{
		p_misc->counting_mode_write_handler((uint8_t*)&p_evt_write->data[0], p_evt_write->len);
	}
    else if ((p_evt_write->handle == p_misc->timer_mode_char_handles.value_handle)
			 && (p_misc->timer_mode_write_handler != NULL))
	{
		p_misc->timer_mode_write_handler((uint8_t*)&p_evt_write->data[0], p_evt_write->len);
	}
    else if ((p_evt_write->handle == p_misc->comm_interval_char_handles.value_handle)
			 && (p_misc->comm_interval_write_handler != NULL))
	{
		p_misc->comm_interval_write_handler((uint8_t*)&p_evt_write->data[0], p_evt_write->len);
	}
    else if ((p_evt_write->handle == p_misc->comm_ctrl_char_handles.value_handle)
			 && (p_misc->comm_ctrl_write_handler != NULL))
	{
		p_misc->comm_ctrl_write_handler((uint8_t*)&p_evt_write->data[0], p_evt_write->len);
	}
    else if ((p_evt_write->handle == p_misc->ota_char_handles.value_handle)
			 && (p_misc->comm_ota_write_handler != NULL))
	{
		p_misc->comm_ota_write_handler((uint8_t*)&p_evt_write->data[0], p_evt_write->len);
	}
}

void ble_misc_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context)
{
    ble_misc_t * p_misc = (ble_misc_t *)p_context;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GATTS_EVT_WRITE:
            on_write(p_misc, p_ble_evt);
            break;

        default:
            // No implementation needed.
            break;
    }
}

uint32_t ble_misc_init(ble_misc_t * p_misc, const ble_misc_init_t * p_misc_init)
{
    uint32_t              err_code;
    ble_uuid_t            ble_uuid;
    ble_add_char_params_t add_char_params;

    // Initialize service structure.
	p_misc->payload_length_write_handler = p_misc_init->payload_length_write_handler;
	p_misc->counting_mode_write_handler = p_misc_init->counting_mode_write_handler;
	p_misc->timer_mode_write_handler = p_misc_init->timer_mode_write_handler;
	p_misc->comm_interval_write_handler = p_misc_init->comm_interval_write_handler;
	p_misc->comm_ctrl_write_handler = p_misc_init->comm_ctrl_write_handler;
	p_misc->comm_ota_write_handler = p_misc_init->comm_ota_write_handler;

    // Add service.
    ble_uuid128_t base_uuid = {MISC_UUID_BASE};
    err_code = sd_ble_uuid_vs_add(&base_uuid, &p_misc->uuid_type);
    VERIFY_SUCCESS(err_code);

    ble_uuid.type = p_misc->uuid_type;
    ble_uuid.uuid = MISC_UUID_SERVICE;

    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_misc->service_handle);
    VERIFY_SUCCESS(err_code);

    // Add characteristic.
    memset(&add_char_params, 0, sizeof(add_char_params));
    add_char_params.uuid              = MISC_UUID_PAYLOAD_LENGTH_CHAR;
    add_char_params.uuid_type         = p_misc->uuid_type;
    add_char_params.init_len          = 1;
    add_char_params.max_len           = 1;
    add_char_params.char_props.read   = 1;
    add_char_params.char_props.write  = 1;
    add_char_params.read_access       = SEC_OPEN;
    add_char_params.write_access 	  = SEC_OPEN;
    err_code = characteristic_add(p_misc->service_handle, &add_char_params, &p_misc->payload_length_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
	
	add_char_params.uuid = MISC_UUID_COUNTING_MODE_CHAR;
	add_char_params.init_len = 4;
	add_char_params.max_len = 4;
    err_code = characteristic_add(p_misc->service_handle, &add_char_params, &p_misc->counting_mode_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
	
	add_char_params.uuid = MISC_UUID_TIMER_MODE_CHAR;
    err_code = characteristic_add(p_misc->service_handle, &add_char_params, &p_misc->timer_mode_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
	
	add_char_params.uuid = MISC_UUID_COMM_INTERVAL_CHAR;
    err_code = characteristic_add(p_misc->service_handle, &add_char_params, &p_misc->comm_interval_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

	add_char_params.uuid = MISC_UUID_COMM_CTRL_CHAR;
	add_char_params.init_len = 1;
	add_char_params.max_len = 1;
	uint8_t buf[4] = {0};
	add_char_params.p_init_value = buf;
	add_char_params.char_props.notify = 1;
	add_char_params.cccd_write_access = SEC_OPEN;
    err_code = characteristic_add(p_misc->service_handle, &add_char_params, &p_misc->comm_ctrl_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
	
	add_char_params.uuid = MISC_UUID_LOST_RATE_CHAR;
	add_char_params.init_len = 4;
	add_char_params.max_len = 4;
	add_char_params.p_init_value = buf;
	add_char_params.char_props.write  = 0;
	add_char_params.char_props.notify = 1;
	add_char_params.cccd_write_access = SEC_OPEN;
    err_code = characteristic_add(p_misc->service_handle, &add_char_params, &p_misc->lost_rate_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
	
	add_char_params.uuid = MISC_UUID_TEST_PROGRESS_CHAR;
	add_char_params.cccd_write_access = SEC_OPEN;
    err_code = characteristic_add(p_misc->service_handle, &add_char_params, &p_misc->test_progress_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
	
    add_char_params.uuid              = MISC_UUID_OTA_CHAR;
    add_char_params.init_len          = 1;
    add_char_params.max_len           = 1;
    add_char_params.char_props.read   = 1;
    add_char_params.char_props.write  = 1;
	add_char_params.char_props.notify = 0;
    add_char_params.read_access       = SEC_OPEN;
    add_char_params.write_access 	  = SEC_OPEN;
    err_code = characteristic_add(p_misc->service_handle, &add_char_params, &p_misc->ota_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
	
	return 0;
}



