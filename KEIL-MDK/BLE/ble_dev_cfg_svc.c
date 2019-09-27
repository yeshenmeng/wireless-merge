#include "sdk_common.h"
#include "ble_dev_cfg_svc.h"
#include "ble_srv_common.h"
#include "app_error.h"
#include "main.h"
#include "sys_param.h"


static void on_write(ble_dev_cfg_t * p_dev_cfg, ble_evt_t const * p_ble_evt)
{
    ble_gatts_evt_write_t const * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;

    if ((p_evt_write->handle == p_dev_cfg->dev_gateway_addr_char_handles.value_handle)
        && (p_dev_cfg->dev_gateway_write_handler != NULL))
    {
        p_dev_cfg->dev_gateway_write_handler((uint8_t*)&p_evt_write->data[0], p_evt_write->len);
    }
    else if ((p_evt_write->handle == p_dev_cfg->dev_long_addr_char_handles.value_handle)
			 && (p_dev_cfg->dev_long_addr_write_handler != NULL))
	{
		p_dev_cfg->dev_long_addr_write_handler((uint8_t*)&p_evt_write->data[0], p_evt_write->len);
	}
    else if ((p_evt_write->handle == p_dev_cfg->dev_short_addr_char_handles.value_handle)
			 && (p_dev_cfg->dev_short_addr_write_handler != NULL))
	{
		p_dev_cfg->dev_short_addr_write_handler((uint8_t*)&p_evt_write->data[0], p_evt_write->len);
	}
//    else if ((p_evt_write->handle == p_dev_cfg->dev_interval_char_handles.value_handle)
//			 && (p_dev_cfg->dev_interval_write_handler != NULL))
//	{
//		p_dev_cfg->dev_interval_write_handler((uint8_t*)&p_evt_write->data[0], p_evt_write->len);
//	}
//    else if ((p_evt_write->handle == p_dev_cfg->dev_time_stamp_char_handles.value_handle)
//			 && (p_dev_cfg->dev_time_stamp_write_handler != NULL))
//	{
//		p_dev_cfg->dev_time_stamp_write_handler((uint8_t*)&p_evt_write->data[0], p_evt_write->len);
//	}
//    else if ((p_evt_write->handle == p_dev_cfg->dev_battery_char_handles.value_handle)
//			 && (p_dev_cfg->dev_battery_write_handler != NULL))
//	{
//		p_dev_cfg->dev_battery_write_handler((uint8_t*)&p_evt_write->data[0], p_evt_write->len);
//	}
//    else if ((p_evt_write->handle == p_dev_cfg->dev_sw_version_char_handles.value_handle)
//			 && (p_dev_cfg->dev_sw_version_write_handler != NULL))
//	{
//		p_dev_cfg->dev_sw_version_write_handler((uint8_t*)&p_evt_write->data[0], p_evt_write->len);
//	}
//    else if ((p_evt_write->handle == p_dev_cfg->dev_hw_version_char_handles.value_handle)
//			 && (p_dev_cfg->dev_hw_version_write_handler != NULL))
//	{
//		p_dev_cfg->dev_hw_version_write_handler((uint8_t*)&p_evt_write->data[0], p_evt_write->len);
//	}	
}

static void on_read_write_auth(ble_dev_cfg_t * p_dev_cfg, ble_evt_t const * p_ble_evt)
{
	ble_gatts_evt_rw_authorize_request_t const * req = &p_ble_evt->evt.gatts_evt.params.authorize_request;
	
	if (req->type != BLE_GATTS_AUTHORIZE_TYPE_INVALID)
	{
		/* 写权限操作 */
		if (req->type == BLE_GATTS_AUTHORIZE_TYPE_WRITE)
		{
		}
		/* 读权限操作 */
		else if(req->type == BLE_GATTS_AUTHORIZE_TYPE_READ)
		{
			if(req->request.read.handle == p_dev_cfg->dev_time_stamp_char_handles.value_handle)
			{
				uint8_t value[4] = {0};
				uint16_t len;
				p_dev_cfg->dev_time_stamp_read_handler(value, &len);
				ble_gatts_rw_authorize_reply_params_t auth_reply = {0};
				auth_reply.type = BLE_GATTS_AUTHORIZE_TYPE_READ;
				auth_reply.params.read.update = 1;
				auth_reply.params.read.len = len;
				auth_reply.params.read.p_data = value;
				auth_reply.params.read.offset = 0;
				auth_reply.params.read.gatt_status = BLE_GATT_STATUS_SUCCESS;
				ret_code_t err_code = sd_ble_gatts_rw_authorize_reply(p_ble_evt->evt.gatts_evt.conn_handle, &auth_reply);
				APP_ERROR_CHECK(err_code);
			}
		}
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
		
		case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
			on_read_write_auth(p_dev_cfg, p_ble_evt);
			break;

        default:
            // No implementation needed.
            break;
    }
}

static uint32_t ble_dev_i_cfg_init(ble_dev_cfg_t * p_dev_cfg, const ble_dev_cfg_init_t * p_dev_cfg_init)
{
    uint32_t              err_code;
    ble_uuid_t            ble_uuid;
    ble_add_char_params_t add_char_params;

    // Initialize service structure.
	p_dev_cfg->dev_gateway_write_handler = p_dev_cfg_init->dev_gateway_write_handler;
	p_dev_cfg->dev_long_addr_write_handler = p_dev_cfg_init->dev_long_addr_write_handler;
	p_dev_cfg->dev_short_addr_write_handler = p_dev_cfg_init->dev_short_addr_write_handler;
	p_dev_cfg->dev_time_stamp_read_handler = p_dev_cfg_init->dev_time_stamp_read_handler;
//	p_dev_cfg->dev_interval_write_handler = p_dev_cfg_init->dev_interval_write_handler;
//	p_dev_cfg->dev_time_stamp_write_handler = p_dev_cfg_init->dev_time_stamp_write_handler;
//	p_dev_cfg->dev_battery_write_handler = p_dev_cfg_init->dev_battery_write_handler;
//	p_dev_cfg->dev_sw_version_write_handler = p_dev_cfg_init->dev_sw_version_write_handler;
//	p_dev_cfg->dev_hw_version_write_handler = p_dev_cfg_init->dev_hw_version_write_handler;	

    // Add service.
    ble_uuid128_t base_uuid = {DEV_CFG_UUID_BASE};
    err_code = sd_ble_uuid_vs_add(&base_uuid, &p_dev_cfg->uuid_type);
    VERIFY_SUCCESS(err_code);

    ble_uuid.type = p_dev_cfg->uuid_type;
    ble_uuid.uuid = DEV_I_CFG_UUID_SERVICE;
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_dev_cfg->service_handle);
    VERIFY_SUCCESS(err_code);
	
    // Add characteristic.
    memset(&add_char_params, 0, sizeof(add_char_params));
    add_char_params.uuid              = DEV_I_CFG_UUID_GATEWAY_ADDR_CHAR;
    add_char_params.uuid_type         = p_dev_cfg->uuid_type;
    add_char_params.init_len          = 8;
    add_char_params.max_len           = 8;
    add_char_params.char_props.read   = 1;
    add_char_params.char_props.write  = 1;
    add_char_params.read_access       = SEC_OPEN;
    add_char_params.write_access 	  = SEC_OPEN;
    err_code = characteristic_add(p_dev_cfg->service_handle, &add_char_params, &p_dev_cfg->dev_gateway_addr_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
	
	add_char_params.uuid = DEV_I_CFG_UUID_LONG_ADDR_CHAR;
	add_char_params.init_len = 8;
	add_char_params.max_len = 8;
    err_code = characteristic_add(p_dev_cfg->service_handle, &add_char_params, &p_dev_cfg->dev_long_addr_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
	
	add_char_params.uuid = DEV_I_CFG_UUID_SHORT_ADDR_CHAR;
	add_char_params.init_len = 2;
	add_char_params.max_len = 2;
    err_code = characteristic_add(p_dev_cfg->service_handle, &add_char_params, &p_dev_cfg->dev_short_addr_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
	
	add_char_params.uuid = DEV_I_CFG_UUID_MODE_CHAR;
	add_char_params.init_len = 1;
	add_char_params.max_len = 1;
	add_char_params.char_props.write = 0;
	add_char_params.write_access = SEC_NO_ACCESS;
    err_code = characteristic_add(p_dev_cfg->service_handle, &add_char_params, &p_dev_cfg->dev_mode_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }	
	
	add_char_params.uuid = DEV_I_CFG_UUID_INTERVAL_CHAR;
	add_char_params.init_len = 4;
	add_char_params.max_len = 4;
    err_code = characteristic_add(p_dev_cfg->service_handle, &add_char_params, &p_dev_cfg->dev_interval_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

	add_char_params.uuid = DEV_I_CFG_UUID_TIME_STAMP_CHAR;
	add_char_params.init_len = 4;
	add_char_params.max_len = 4;
	add_char_params.is_defered_read = 1; //读权限控制（对端设备设备访问此特征值需要程序授权）
    err_code = characteristic_add(p_dev_cfg->service_handle, &add_char_params, &p_dev_cfg->dev_time_stamp_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

	add_char_params.uuid = DEV_I_CFG_UUID_BATTERY_CHAR;
	add_char_params.init_len = 1;
	add_char_params.max_len = 1;
	add_char_params.is_defered_read = 0;
    err_code = characteristic_add(p_dev_cfg->service_handle, &add_char_params, &p_dev_cfg->dev_battery_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
	
	add_char_params.uuid = DEV_I_CFG_UUID_TEMPERATURE_CHAR;
	add_char_params.init_len = 4;
	add_char_params.max_len = 4;	
    err_code = characteristic_add(p_dev_cfg->service_handle, &add_char_params, &p_dev_cfg->dev_temperature_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
	
	add_char_params.uuid = DEV_I_CFG_UUID_X_ANGLE_CHAR;
	add_char_params.init_len = 4;
	add_char_params.max_len = 4;	
    err_code = characteristic_add(p_dev_cfg->service_handle, &add_char_params, &p_dev_cfg->dev_x_angle_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }	

	add_char_params.uuid = DEV_I_CFG_UUID_Y_ANGLE_CHAR;
	add_char_params.init_len = 4;
	add_char_params.max_len = 4;	
    err_code = characteristic_add(p_dev_cfg->service_handle, &add_char_params, &p_dev_cfg->dev_y_angle_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
	
	add_char_params.uuid = DEV_I_CFG_UUID_Z_ANGLE_CHAR;
	add_char_params.init_len = 4;
	add_char_params.max_len = 4;	
    err_code = characteristic_add(p_dev_cfg->service_handle, &add_char_params, &p_dev_cfg->dev_z_angle_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
	
	add_char_params.uuid = DEV_I_CFG_UUID_X_ANGLE_THRESHOLD_CHAR;
	add_char_params.init_len = 4;
	add_char_params.max_len = 4;	
    err_code = characteristic_add(p_dev_cfg->service_handle, &add_char_params, &p_dev_cfg->dev_x_angle_threshold_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }			

	add_char_params.uuid = DEV_I_CFG_UUID_Y_ANGLE_THRESHOLD_CHAR;
	add_char_params.init_len = 4;
	add_char_params.max_len = 4;	
    err_code = characteristic_add(p_dev_cfg->service_handle, &add_char_params, &p_dev_cfg->dev_y_angle_threshold_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }		
	
	add_char_params.uuid = DEV_I_CFG_UUID_Z_ANGLE_THRESHOLD_CHAR;
	add_char_params.init_len = 4;
	add_char_params.max_len = 4;	
    err_code = characteristic_add(p_dev_cfg->service_handle, &add_char_params, &p_dev_cfg->dev_z_angle_threshold_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
	
	add_char_params.uuid = DEV_I_CFG_UUID_LORA_RSSI_CHAR;
	add_char_params.init_len = 1;
	add_char_params.max_len = 1;	
    err_code = characteristic_add(p_dev_cfg->service_handle, &add_char_params, &p_dev_cfg->dev_lora_rssi_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
	
	add_char_params.uuid = DEV_I_CFG_UUID_SW_VERSION_CHAR;
	add_char_params.init_len = 4;
	add_char_params.max_len = 4;		
    err_code = characteristic_add(p_dev_cfg->service_handle, &add_char_params, &p_dev_cfg->dev_sw_version_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
	
	add_char_params.uuid = DEV_I_CFG_UUID_HW_VERSION_CHAR;
	add_char_params.init_len = 4;
	add_char_params.max_len = 4;		
    return characteristic_add(p_dev_cfg->service_handle, &add_char_params, &p_dev_cfg->dev_hw_version_char_handles);	
}

static uint32_t ble_dev_c_cfg_init(ble_dev_cfg_t * p_dev_cfg, const ble_dev_cfg_init_t * p_dev_cfg_init)
{
    uint32_t              err_code;
    ble_uuid_t            ble_uuid;
    ble_add_char_params_t add_char_params;

    // Initialize service structure.
	p_dev_cfg->dev_gateway_write_handler = p_dev_cfg_init->dev_gateway_write_handler;
	p_dev_cfg->dev_long_addr_write_handler = p_dev_cfg_init->dev_long_addr_write_handler;
	p_dev_cfg->dev_short_addr_write_handler = p_dev_cfg_init->dev_short_addr_write_handler;
	p_dev_cfg->dev_time_stamp_read_handler = p_dev_cfg_init->dev_time_stamp_read_handler;
//	p_dev_cfg->dev_interval_write_handler = p_dev_cfg_init->dev_interval_write_handler;
//	p_dev_cfg->dev_time_stamp_write_handler = p_dev_cfg_init->dev_time_stamp_write_handler;
//	p_dev_cfg->dev_battery_write_handler = p_dev_cfg_init->dev_battery_write_handler;
//	p_dev_cfg->dev_sw_version_write_handler = p_dev_cfg_init->dev_sw_version_write_handler;
//	p_dev_cfg->dev_hw_version_write_handler = p_dev_cfg_init->dev_hw_version_write_handler;	

    // Add service.
    ble_uuid128_t base_uuid = {DEV_CFG_UUID_BASE};
    err_code = sd_ble_uuid_vs_add(&base_uuid, &p_dev_cfg->uuid_type);
    VERIFY_SUCCESS(err_code);

    ble_uuid.type = p_dev_cfg->uuid_type;
    ble_uuid.uuid = DEV_C_CFG_UUID_SERVICE;
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_dev_cfg->service_handle);
    VERIFY_SUCCESS(err_code);
	
    // Add characteristic.
    memset(&add_char_params, 0, sizeof(add_char_params));
    add_char_params.uuid              = DEV_C_CFG_UUID_GATEWAY_ADDR_CHAR;
    add_char_params.uuid_type         = p_dev_cfg->uuid_type;
    add_char_params.init_len          = 8;
    add_char_params.max_len           = 8;
    add_char_params.char_props.read   = 1;
    add_char_params.char_props.write  = 1;
    add_char_params.read_access       = SEC_OPEN;
    add_char_params.write_access 	  = SEC_OPEN;
    err_code = characteristic_add(p_dev_cfg->service_handle, &add_char_params, &p_dev_cfg->dev_gateway_addr_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
	
	add_char_params.uuid = DEV_C_CFG_UUID_LONG_ADDR_CHAR;
	add_char_params.init_len = 8;
	add_char_params.max_len = 8;
    err_code = characteristic_add(p_dev_cfg->service_handle, &add_char_params, &p_dev_cfg->dev_long_addr_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
	
	add_char_params.uuid = DEV_C_CFG_UUID_SHORT_ADDR_CHAR;
	add_char_params.init_len = 2;
	add_char_params.max_len = 2;
    err_code = characteristic_add(p_dev_cfg->service_handle, &add_char_params, &p_dev_cfg->dev_short_addr_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
	
	add_char_params.uuid = DEV_C_CFG_UUID_MODE_CHAR;
	add_char_params.init_len = 1;
	add_char_params.max_len = 1;
	add_char_params.char_props.write = 0;
	add_char_params.write_access = SEC_NO_ACCESS;
    err_code = characteristic_add(p_dev_cfg->service_handle, &add_char_params, &p_dev_cfg->dev_mode_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }	
	
	add_char_params.uuid = DEV_C_CFG_UUID_INTERVAL_CHAR;
	add_char_params.init_len = 4;
	add_char_params.max_len = 4;
    err_code = characteristic_add(p_dev_cfg->service_handle, &add_char_params, &p_dev_cfg->dev_interval_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

	add_char_params.uuid = DEV_C_CFG_UUID_TIME_STAMP_CHAR;
	add_char_params.init_len = 4;
	add_char_params.max_len = 4;
	add_char_params.is_defered_read = 1; //读权限控制（对端设备设备访问此特征值需要程序授权）
    err_code = characteristic_add(p_dev_cfg->service_handle, &add_char_params, &p_dev_cfg->dev_time_stamp_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

	add_char_params.uuid = DEV_C_CFG_UUID_BATTERY_CHAR;
	add_char_params.init_len = 1;
	add_char_params.max_len = 1;
	add_char_params.is_defered_read = 0;
    err_code = characteristic_add(p_dev_cfg->service_handle, &add_char_params, &p_dev_cfg->dev_battery_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
	
	add_char_params.uuid = DEV_C_CFG_UUID_TEMPERATURE_CHAR;
	add_char_params.init_len = 4;
	add_char_params.max_len = 4;	
    err_code = characteristic_add(p_dev_cfg->service_handle, &add_char_params, &p_dev_cfg->dev_temperature_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
	
	add_char_params.uuid = DEV_C_CFG_UUID_X_ACCEL_CHAR;
	add_char_params.init_len = 4;
	add_char_params.max_len = 4;	
    err_code = characteristic_add(p_dev_cfg->service_handle, &add_char_params, &p_dev_cfg->dev_x_accel_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }	

	add_char_params.uuid = DEV_C_CFG_UUID_Y_ACCEL_CHAR;
	add_char_params.init_len = 4;
	add_char_params.max_len = 4;	
    err_code = characteristic_add(p_dev_cfg->service_handle, &add_char_params, &p_dev_cfg->dev_y_accel_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
	
	add_char_params.uuid = DEV_C_CFG_UUID_Z_ACCEL_CHAR;
	add_char_params.init_len = 4;
	add_char_params.max_len = 4;	
    err_code = characteristic_add(p_dev_cfg->service_handle, &add_char_params, &p_dev_cfg->dev_z_accel_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
	
	add_char_params.uuid = DEV_C_CFG_UUID_X_ANGLE_CHAR;
	add_char_params.init_len = 4;
	add_char_params.max_len = 4;	
    err_code = characteristic_add(p_dev_cfg->service_handle, &add_char_params, &p_dev_cfg->dev_x_angle_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
	
	add_char_params.uuid = DEV_C_CFG_UUID_Y_ANGLE_CHAR;
	add_char_params.init_len = 4;
	add_char_params.max_len = 4;	
    err_code = characteristic_add(p_dev_cfg->service_handle, &add_char_params, &p_dev_cfg->dev_y_angle_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
	
	add_char_params.uuid = DEV_C_CFG_UUID_Z_ANGLE_CHAR;
	add_char_params.init_len = 4;
	add_char_params.max_len = 4;	
    err_code = characteristic_add(p_dev_cfg->service_handle, &add_char_params, &p_dev_cfg->dev_z_angle_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
	
	add_char_params.uuid = DEV_C_CFG_UUID_ACCEL_SLOPE_THRESHOLD_CHAR;
	add_char_params.init_len = 2;
	add_char_params.max_len = 2;	
    err_code = characteristic_add(p_dev_cfg->service_handle, &add_char_params, &p_dev_cfg->dev_accel_slope_threshold_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }			

	add_char_params.uuid = DEV_C_CFG_UUID_CONSECUTIVE_DATA_POINTS_CHAR;
	add_char_params.init_len = 2;
	add_char_params.max_len = 2;	
    err_code = characteristic_add(p_dev_cfg->service_handle, &add_char_params, &p_dev_cfg->dev_consecutive_data_points_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
	
	add_char_params.uuid = DEV_C_CFG_UUID_LORA_RSSI_CHAR;
	add_char_params.init_len = 1;
	add_char_params.max_len = 1;	
    err_code = characteristic_add(p_dev_cfg->service_handle, &add_char_params, &p_dev_cfg->dev_lora_rssi_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
	
	add_char_params.uuid = DEV_C_CFG_UUID_SW_VERSION_CHAR;
	add_char_params.init_len = 4;
	add_char_params.max_len = 4;		
    err_code = characteristic_add(p_dev_cfg->service_handle, &add_char_params, &p_dev_cfg->dev_sw_version_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
	
	add_char_params.uuid = DEV_C_CFG_UUID_HW_VERSION_CHAR;
	add_char_params.init_len = 4;
	add_char_params.max_len = 4;		
    return characteristic_add(p_dev_cfg->service_handle, &add_char_params, &p_dev_cfg->dev_hw_version_char_handles);	
}

uint32_t ble_dev_cfg_init(ble_dev_cfg_t * p_dev_cfg, const ble_dev_cfg_init_t * p_dev_cfg_init)
{
	uint32_t err_code = 0;
	sys_param_t* param = sys_param_get_handle();
	if(param->object_version == INCLINOMETER_VERSION)
	{
		err_code = ble_dev_i_cfg_init(p_dev_cfg, p_dev_cfg_init);
	}
	else if(param->object_version == COLLAPSE_VERSION)
	{
		err_code = ble_dev_c_cfg_init(p_dev_cfg, p_dev_cfg_init);
	}
	
	return err_code;
}



