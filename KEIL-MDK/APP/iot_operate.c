#include "iot_operate.h"
#include "calendar.h"
#include "ble_init.h"
#include "host_net_swap.h"
#include "lora_transmission.h"
#include "wireless_comm_services.h"
#include "sw_bat_soc.h"


static iot_dev_t IoT_dev;

/* ��IoT����BUF��д�볤��ַ */
void iot_write_long_addr(uint8_t* value)
{
	sys_param_t* param = sys_param_get_handle();
	if(param->object_version == INCLINOMETER_VERSION)
	{
		IoT_dev.sensor->writePropFromBuf(IOT_I_LONG_ADDR_ID, value);
		IoT_dev.sensor->resetPropChangeFlag(IOT_I_LONG_ADDR_ID);
	}
	else if(param->object_version == COLLAPSE_VERSION)
	{
		IoT_dev.sensor->writePropFromBuf(IOT_C_LONG_ADDR_ID, value);
		IoT_dev.sensor->resetPropChangeFlag(IOT_C_LONG_ADDR_ID);
	}
}

/* ��IoT����BUF��д��̵�ַ */
void iot_write_short_addr(uint8_t* value)
{
	sys_param_t* param = sys_param_get_handle();
	if(param->object_version == INCLINOMETER_VERSION)
	{
		IoT_dev.sensor->writePropFromBuf(IOT_I_SHORT_ADDR_ID, value);
		IoT_dev.sensor->resetPropChangeFlag(IOT_I_SHORT_ADDR_ID);
	}
	else if(param->object_version == COLLAPSE_VERSION)
	{
		IoT_dev.sensor->writePropFromBuf(IOT_C_SHORT_ADDR_ID, value);
		IoT_dev.sensor->resetPropChangeFlag(IOT_C_SHORT_ADDR_ID);
	}
}

/* ��IoT����BUF��д�����ģʽ */
static void iot_write_sample_mode(uint8_t value)
{
	sys_param_t* param = sys_param_get_handle();
	if(param->object_version == INCLINOMETER_VERSION)
	{
		IoT_dev.sensor->writePropFromBuf(IOT_I_SAMPLE_MODE_ID, (uint8_t *)&value);
		IoT_dev.sensor->resetPropChangeFlag(IOT_I_SAMPLE_MODE_ID);
	}
	else if(param->object_version == COLLAPSE_VERSION)
	{
		IoT_dev.sensor->writePropFromBuf(IOT_C_SAMPLE_MODE_ID, (uint8_t *)&value);
		IoT_dev.sensor->resetPropChangeFlag(IOT_C_SAMPLE_MODE_ID);
	}
}

/* ��IoT����BUF��д�������� */
static void iot_write_sample_interval(uint32_t value)
{
	sys_param_t* param = sys_param_get_handle();
	if(param->object_version == INCLINOMETER_VERSION)
	{
		IoT_dev.sensor->writePropFromBuf(IOT_I_SAMPLE_INTERVAL_ID, (uint8_t *)&value);
		IoT_dev.sensor->resetPropChangeFlag(IOT_I_SAMPLE_INTERVAL_ID);
	}
	else if(param->object_version == COLLAPSE_VERSION)
	{
		IoT_dev.sensor->writePropFromBuf(IOT_C_SAMPLE_INTERVAL_ID, (uint8_t *)&value);
		IoT_dev.sensor->resetPropChangeFlag(IOT_C_SAMPLE_INTERVAL_ID);
	}
}

/* ��IoT����BUF��д��ʱ��� */
static void iot_write_time_stamp(uint32_t value)
{
	sys_param_t* param = sys_param_get_handle();
	if(param->object_version == INCLINOMETER_VERSION)
	{
		IoT_dev.sensor->writePropFromBuf(IOT_I_TIME_STAMP_ID, (uint8_t *)&value);
		IoT_dev.sensor->resetPropChangeFlag(IOT_I_TIME_STAMP_ID);
	}
	else if(param->object_version == COLLAPSE_VERSION)
	{
		IoT_dev.sensor->writePropFromBuf(IOT_C_TIME_STAMP_ID, (uint8_t *)&value);
		IoT_dev.sensor->resetPropChangeFlag(IOT_C_TIME_STAMP_ID);
	}
}

/* ��IoT����BUF��д��ʱ��ƫ�� */
static void iot_write_time_offset(uint16_t value)
{
	sys_param_t* param = sys_param_get_handle();
	if(param->object_version == INCLINOMETER_VERSION)
	{
		IoT_dev.sensor->writePropFromBuf(IOT_I_TIME_OFFSET_ID, (uint8_t *)&value);
		IoT_dev.sensor->resetPropChangeFlag(IOT_I_TIME_OFFSET_ID);
	}
	else if(param->object_version == COLLAPSE_VERSION)
	{
		IoT_dev.sensor->writePropFromBuf(IOT_C_TIME_OFFSET_ID, (uint8_t *)&value);
		IoT_dev.sensor->resetPropChangeFlag(IOT_C_TIME_OFFSET_ID);
	}
}

/* ��IoT����BUF��д����ٶȱ仯��б����ֵ���� */
static void iot_write_accel_slope_threshold(uint16_t value)
{
	IoT_dev.sensor->writePropFromBuf(IOT_C_ACCEL_SLOPE_THRESHOLD_ID, (uint8_t *)&value);
	IoT_dev.sensor->resetPropChangeFlag(IOT_C_ACCEL_SLOPE_THRESHOLD_ID);
}

/* ��IoT����BUF��д���������ݵ����� */
static void iot_write_consecutive_data_points(uint16_t value)
{
	IoT_dev.sensor->writePropFromBuf(IOT_C_CONSECUTIVE_DATA_POINTS_ID, (uint8_t *)&value);
	IoT_dev.sensor->resetPropChangeFlag(IOT_C_CONSECUTIVE_DATA_POINTS_ID);
}

/* ��IoT����BUF��д���ص��� */
static void iot_write_battery_level(uint8_t value)
{
	sys_param_t* param = sys_param_get_handle();
	if(param->object_version == INCLINOMETER_VERSION)
	{
		IoT_dev.sensor->writePropFromBuf(IOT_I_BATTERY_LEVEL_ID, (uint8_t *)&value);
		IoT_dev.sensor->resetPropChangeFlag(IOT_I_BATTERY_LEVEL_ID);
	}
	else if(param->object_version == COLLAPSE_VERSION)
	{
		IoT_dev.sensor->writePropFromBuf(IOT_C_BATTERY_LEVEL_ID, (uint8_t *)&value);
		IoT_dev.sensor->resetPropChangeFlag(IOT_C_BATTERY_LEVEL_ID);
	}
}

/* ��IoT����BUF��д���¶����� */
static void iot_write_temp(float value)
{
	sys_param_t* param = sys_param_get_handle();
	if(param->object_version == INCLINOMETER_VERSION)
	{
		IoT_dev.sensor->writePropFromBuf(IOT_I_TEMPERATURE_ID, (uint8_t *)&value);
		IoT_dev.sensor->resetPropChangeFlag(IOT_I_TEMPERATURE_ID);
	}
	else if(param->object_version == COLLAPSE_VERSION)
	{
		IoT_dev.sensor->writePropFromBuf(IOT_C_TEMPERATURE_ID, (uint8_t *)&value);
		IoT_dev.sensor->resetPropChangeFlag(IOT_C_TEMPERATURE_ID);
	}
}

/* ��IoT����BUF��д��X����ٶ����� */
static void iot_write_x_accel(float value)
{
	IoT_dev.sensor->writePropFromBuf(IOT_C_DATA_X_ACCEL_ID, (uint8_t *)&value);
	IoT_dev.sensor->resetPropChangeFlag(IOT_C_DATA_X_ACCEL_ID);
}

/* ��IoT����BUF��д��Y����ٶ����� */
static void iot_write_y_accel(float value)
{
	IoT_dev.sensor->writePropFromBuf(IOT_C_DATA_Y_ACCEL_ID, (uint8_t *)&value);
	IoT_dev.sensor->resetPropChangeFlag(IOT_C_DATA_Y_ACCEL_ID);
}

/* ��IoT����BUF��д��Z����ٶ����� */
static void iot_write_z_accel(float value)
{
	IoT_dev.sensor->writePropFromBuf(IOT_C_DATA_Z_ACCEL_ID, (uint8_t *)&value);
	IoT_dev.sensor->resetPropChangeFlag(IOT_C_DATA_Z_ACCEL_ID);
}

/* ��IoT����BUF��д��X��Ƕ����� */
static void iot_write_x_angle(float value)
{
	sys_param_t* param = sys_param_get_handle();
	if(param->object_version == INCLINOMETER_VERSION)
	{
		IoT_dev.sensor->writePropFromBuf(IOT_I_DATA_X_ANGLE_ID, (uint8_t *)&value);
		IoT_dev.sensor->resetPropChangeFlag(IOT_I_DATA_X_ANGLE_ID);
	}
	else if(param->object_version == COLLAPSE_VERSION)
	{
#if (IOT_PROTOCOL_C_WITH_ANGLE == 1)
		IoT_dev.sensor->writePropFromBuf(IOT_C_DATA_X_ANGLE_ID, (uint8_t *)&value);
		IoT_dev.sensor->resetPropChangeFlag(IOT_C_DATA_X_ANGLE_ID);
#endif
	}
}

/* ��IoT����BUF��д��Y��Ƕ����� */
static void iot_write_y_angle(float value)
{
	sys_param_t* param = sys_param_get_handle();
	if(param->object_version == INCLINOMETER_VERSION)
	{
		IoT_dev.sensor->writePropFromBuf(IOT_I_DATA_Y_ANGLE_ID, (uint8_t *)&value);
		IoT_dev.sensor->resetPropChangeFlag(IOT_I_DATA_Y_ANGLE_ID);
	}
	else if(param->object_version == COLLAPSE_VERSION)
	{
#if (IOT_PROTOCOL_C_WITH_ANGLE == 1)
		IoT_dev.sensor->writePropFromBuf(IOT_C_DATA_Y_ANGLE_ID, (uint8_t *)&value);
		IoT_dev.sensor->resetPropChangeFlag(IOT_C_DATA_Y_ANGLE_ID);
#endif
	}
}

/* ��IoT����BUF��д��Z��Ƕ����� */
static void iot_write_z_angle(float value)
{
	sys_param_t* param = sys_param_get_handle();
	if(param->object_version == INCLINOMETER_VERSION)
	{
		IoT_dev.sensor->writePropFromBuf(IOT_I_DATA_Z_ANGLE_ID, (uint8_t *)&value);
		IoT_dev.sensor->resetPropChangeFlag(IOT_I_DATA_Z_ANGLE_ID);
	}
	else if(param->object_version == COLLAPSE_VERSION)
	{
#if (IOT_PROTOCOL_C_WITH_ANGLE == 1)
		IoT_dev.sensor->writePropFromBuf(IOT_C_DATA_Z_ANGLE_ID, (uint8_t *)&value);
		IoT_dev.sensor->resetPropChangeFlag(IOT_C_DATA_Z_ANGLE_ID);
#endif
	}
}

/* ��IoT����BUF��д�����X����ֵ���� */
static void iot_write_x_angle_threshold(float value)
{
	IoT_dev.sensor->writePropFromBuf(IOT_I_DATA_X_THRESHOLD_ID, (uint8_t *)&value);
	IoT_dev.sensor->resetPropChangeFlag(IOT_I_DATA_X_THRESHOLD_ID);
}

/* ��IoT����BUF��д�����Y����ֵ���� */
static void iot_write_y_angle_threshold(float value)
{
	IoT_dev.sensor->writePropFromBuf(IOT_I_DATA_Y_THRESHOLD_ID, (uint8_t *)&value);
	IoT_dev.sensor->resetPropChangeFlag(IOT_I_DATA_Y_THRESHOLD_ID);
}

/* ��IoT����BUF��д�����Z����ֵ���� */
static void iot_write_z_angle_threshold(float value)
{
	IoT_dev.sensor->writePropFromBuf(IOT_I_DATA_Z_THRESHOLD_ID, (uint8_t *)&value);
	IoT_dev.sensor->resetPropChangeFlag(IOT_I_DATA_Z_THRESHOLD_ID);
}

/* ����IoTЭ�����õĳ���ַ */
static void iot_set_long_addr(uint8_t* value)
{
	sys_param_t* param = sys_param_get_handle();
	for(int i=0; i<sizeof(value); i++)
	{
		if(value[i] != param->dev_long_addr[i])
		{
			lora_reset();
			break;
		}
	}
	sys_param_set((uint8_t*)&param->dev_long_addr, value, sizeof(param->dev_long_addr));
	
	/* ����汾�ı�󱣴������λ�豸 */
	if(param->object_version != param->dev_long_addr[0] &&
	   (param->dev_long_addr[0] == INCLINOMETER_VERSION ||
		param->dev_long_addr[0] == COLLAPSE_VERSION))
	{
		param->object_version = param->dev_long_addr[0];
		param->update_flag = 1;
		param->save_param_to_flash();
		sys_reset();
	}
}

/* ����IoTЭ�����õĶ̵�ַ */
static void iot_set_short_addr(uint8_t* value)
{
	sys_param_t* param = sys_param_get_handle();
	wireless_comm_services_t* wirelessCommSvc = Wireless_CommSvcGetHandle();
	if(wirelessCommSvc->_conn_short_addr_update_flag == 0) //����ʱ����Э���еĶ̵�ֱַ�����ò���Ҫ�뱾�ض̵�ַ�Ƚ�
	{
		for(int i=0; i<sizeof(value); i++)
		{
			if(value[i] != param->dev_short_addr[i]) //IOTЭ�������еĶ̵�ַ��Ҫ�Ƚ�
			{
				lora_reset();
				break;
			}
		}
	}
	wirelessCommSvc->_conn_short_addr_update_flag= 0;
	sys_param_set((uint8_t*)&param->dev_short_addr, value, sizeof(param->dev_short_addr));
}

/* ����IoTЭ�����õĲ���ģʽ */
static void iot_set_sample_mode(uint8_t value)
{
	sys_param_t* param = sys_param_get_handle();
	if(param->object_version == INCLINOMETER_VERSION)
	{
		sys_param_set((uint8_t*)&param->iot_clinometer.iot_mode, (uint8_t*)&value, sizeof(value));
	}
	else if(param->object_version == COLLAPSE_VERSION)
	{
		if(value != param->iot_collapse.iot_mode)
		{
			sys_param_set((uint8_t*)&param->iot_collapse.iot_mode, (uint8_t*)&value, sizeof(value));
			IoT_dev.collapse_obj->iot_set_mode();
		}
	}
}

/* ����IoTЭ�����õĲ������ */
static void iot_set_sample_interval(uint32_t value)
{
	sys_param_t* param = sys_param_get_handle();
	if(param->object_version == INCLINOMETER_VERSION)
	{
		sys_param_set((uint8_t*)&param->iot_clinometer.iot_sample_interval, (uint8_t*)&value, sizeof(value));
	}
	else if(param->object_version == COLLAPSE_VERSION)
	{
		if(value != 0)
		{
			sys_param_set((uint8_t*)&param->iot_collapse.iot_period, (uint8_t*)&value, sizeof(value));
			if(param->iot_collapse.iot_mode == PERIOD_MODE)
			{
				sys_param_set((uint8_t*)&param->iot_collapse.iot_sample_period, (uint8_t*)&value, sizeof(value));
			}
			else if(param->iot_collapse.iot_mode == TRIGGER_MODE)
			{
				sys_param_set((uint8_t*)&param->iot_collapse.iot_trigger_period, (uint8_t*)&value, sizeof(value));
			}
			IoT_dev.collapse_obj->iot_set_period();
		}
	}
}

/* ����IoTЭ�����õ�ʱ��� */
static void iot_set_time_stamp(uint32_t value)
{
	calendar_mod_t* calendar = calendar_get_handle();
	calendar->set_time_stamp(value);
}

/* ����IoTЭ�����õ�ʱ��ƫ�� */
static void iot_set_time_offset(uint16_t value)
{
	sys_param_t* param = sys_param_get_handle();
	if(param->object_version == INCLINOMETER_VERSION)
	{
		sys_param_set((uint8_t*)&param->iot_clinometer.time_offset, (uint8_t*)&value, sizeof(value));
	}
	else if(param->object_version == COLLAPSE_VERSION)
	{
		if(value != param->iot_collapse.iot_mode)
		{
			sys_param_set((uint8_t*)&param->iot_collapse.time_offset, (uint8_t*)&value, sizeof(value));
			IoT_dev.collapse_obj->iot_set_mode();
		}
	}
}

/* ����IoTЭ�����õļ��ٶȱ仯��б����ֵ */
static void iot_set_accel_slope_threshold(uint16_t value)
{
	(value > 1000) ? (value = 1000) : (1);
	sys_param_t* param = sys_param_get_handle();
	sys_param_set((uint8_t*)&param->iot_collapse.iot_accel_slope_threshold, (uint8_t*)&value, sizeof(value));
	IoT_dev.collapse_obj->iot_set_accel_slope_threshold();
}

/* ����IoTЭ�����õ��������ݵ� */
static void iot_set_consecutive_data_points(uint16_t value)
{
	(value > 65200) ? (value = 65200) : (1);
	sys_param_t* param = sys_param_get_handle();
	sys_param_set((uint8_t*)&param->iot_collapse.iot_consecutive_data_points, (uint8_t*)&value, sizeof(value));
	IoT_dev.collapse_obj->iot_set_consecutive_data_points();
}

/* ����IoTЭ�����õ����X����ֵ */
static void iot_set_x_angle_threshold(float value)
{
	sys_param_t* param = sys_param_get_handle();
	sys_param_set((uint8_t*)&param->iot_clinometer.iot_x_angle_threshold, (uint8_t*)&value, sizeof(value));
}

/* ����IoTЭ�����õ����Y����ֵ */
static void iot_set_y_angle_threshold(float value)
{
	sys_param_t* param = sys_param_get_handle();
	sys_param_set((uint8_t*)&param->iot_clinometer.iot_y_angle_threshold, (uint8_t*)&value, sizeof(value));
}

/* ����IoTЭ�����õ����Z����ֵ */
static void iot_set_z_angle_threshold(float value)
{
	sys_param_t* param = sys_param_get_handle();
	sys_param_set((uint8_t*)&param->iot_clinometer.iot_z_angle_threshold, (uint8_t*)&value, sizeof(value));
}

/* ���IoTЭ�鴦�� */
static void iot_i_operate(void)
{
	/* ����IoTЭ�����õĶ̵�ַͬʱ�������ݵ�����Э���� */
	if(IoT_dev.sensor->isPropChanged(IOT_I_SHORT_ADDR_ID))
	{
		uint8_t value[2];
		IoT_dev.sensor->readPropToBuf(IOT_I_SHORT_ADDR_ID, (uint8_t *)&value);
		IoT_dev.sensor->resetPropChangeFlag(IOT_I_SHORT_ADDR_ID);
		iot_set_short_addr(value);
		ble_char_update_t* ble_char_update_handle = ble_char_update_handle_get();
		ble_char_update_handle->dev_short_addr_update(value);
	}
	
	/* ����IoTЭ�����õĲ���ģʽͬʱ�������ݵ�����Э���� */
	if(IoT_dev.sensor->isPropChanged(IOT_I_SAMPLE_MODE_ID))
	{
		uint8_t value;
		IoT_dev.sensor->readPropToBuf(IOT_I_SAMPLE_MODE_ID, (uint8_t *)&value);
		IoT_dev.sensor->resetPropChangeFlag(IOT_I_SAMPLE_MODE_ID);
		iot_set_sample_mode(value);
		ble_char_update_t* ble_char_update_handle = ble_char_update_handle_get();
		ble_char_update_handle->dev_sample_mode_update(value);
	}
	
	/* ����IoTЭ�����õĲ������ͬʱ�������ݵ�����Э���� */
	if(IoT_dev.sensor->isPropChanged(IOT_I_SAMPLE_INTERVAL_ID))
	{
		uint32_t value;
		IoT_dev.sensor->readPropToBuf(IOT_I_SAMPLE_INTERVAL_ID, (uint8_t *)&value);
		IoT_dev.sensor->resetPropChangeFlag(IOT_I_SAMPLE_INTERVAL_ID);
#if COMM_TRANSMISSION_FORMAT == 1
		swap_reverse((uint8_t*)&value, sizeof(value));
#endif
		iot_set_sample_interval(value);
		ble_char_update_t* ble_char_update_handle = ble_char_update_handle_get();
		ble_char_update_handle->dev_sample_interval_update(value);
	}
	
	/* ����IoTЭ�����õ�ʱ���ͬʱ�������ݵ�����Э���� */
	if(IoT_dev.sensor->isPropChanged(IOT_I_TIME_STAMP_ID))
	{
		uint32_t value;
		IoT_dev.sensor->readPropToBuf(IOT_I_TIME_STAMP_ID, (uint8_t *)&value);
		IoT_dev.sensor->resetPropChangeFlag(IOT_I_TIME_STAMP_ID);
#if COMM_TRANSMISSION_FORMAT == 1
		swap_reverse((uint8_t*)&value, sizeof(value));
#endif
		iot_set_time_stamp(value);
		ble_char_update_t* ble_char_update_handle = ble_char_update_handle_get();
		ble_char_update_handle->dev_time_stamp_update(value);
	}
	
	/* ����IoTЭ�����õ�ʱ��ƫ��ʱ�������ݵ�����Э���� */
	if(IoT_dev.sensor->isPropChanged(IOT_I_TIME_OFFSET_ID))
	{
		uint16_t value;
		IoT_dev.sensor->readPropToBuf(IOT_I_TIME_OFFSET_ID, (uint8_t *)&value);
		IoT_dev.sensor->resetPropChangeFlag(IOT_I_TIME_OFFSET_ID);
#if COMM_TRANSMISSION_FORMAT == 1
		swap_reverse((uint8_t*)&value, sizeof(value));
#endif
		iot_set_time_offset(value);
		ble_char_update_t* ble_char_update_handle = ble_char_update_handle_get();
		ble_char_update_handle->dev_time_offset_update(value);
	}
	
	/* ����IoTЭ�����õ����X����ֵͬʱ�������ݵ�����Э���� */
	if(IoT_dev.sensor->isPropChanged(IOT_I_DATA_X_THRESHOLD_ID))
	{
		float value;
		IoT_dev.sensor->readPropToBuf(IOT_I_DATA_X_THRESHOLD_ID, (uint8_t *)&value);
		IoT_dev.sensor->resetPropChangeFlag(IOT_I_DATA_X_THRESHOLD_ID);
#if COMM_TRANSMISSION_FORMAT == 1
		swap_reverse((uint8_t*)&value, sizeof(value));
#endif
		iot_set_x_angle_threshold(value);
		ble_char_update_t* ble_char_update_handle = ble_char_update_handle_get();
		ble_char_update_handle->dev_x_angle_threshold_update(value);
	}
	
	/* ����IoTЭ�����õ����Y����ֵͬʱ�������ݵ�����Э���� */
	if(IoT_dev.sensor->isPropChanged(IOT_I_DATA_Y_THRESHOLD_ID))
	{
		float value;
		IoT_dev.sensor->readPropToBuf(IOT_I_DATA_Y_THRESHOLD_ID, (uint8_t *)&value);
		IoT_dev.sensor->resetPropChangeFlag(IOT_I_DATA_Y_THRESHOLD_ID);
#if COMM_TRANSMISSION_FORMAT == 1
		swap_reverse((uint8_t*)&value, sizeof(value));
#endif
		iot_set_y_angle_threshold(value);
		ble_char_update_t* ble_char_update_handle = ble_char_update_handle_get();
		ble_char_update_handle->dev_y_angle_threshold_update(value);
	}
	
	/* ����IoTЭ�����õ����Z����ֵͬʱ�������ݵ�����Э���� */
	if(IoT_dev.sensor->isPropChanged(IOT_I_DATA_Z_THRESHOLD_ID))
	{
		float value;
		IoT_dev.sensor->readPropToBuf(IOT_I_DATA_Z_THRESHOLD_ID, (uint8_t *)&value);
		IoT_dev.sensor->resetPropChangeFlag(IOT_I_DATA_Z_THRESHOLD_ID);
#if COMM_TRANSMISSION_FORMAT == 1
		swap_reverse((uint8_t*)&value, sizeof(value));
#endif
		iot_set_z_angle_threshold(value);
		ble_char_update_t* ble_char_update_handle = ble_char_update_handle_get();
		ble_char_update_handle->dev_z_angle_threshold_update(value);
	}
	
	/* ����IoTЭ�����õĳ���ַͬʱ�������ݵ�����Э���� */
	if(IoT_dev.sensor->isPropChanged(IOT_I_LONG_ADDR_ID))
	{
		uint8_t value[8];
		IoT_dev.sensor->readPropToBuf(IOT_I_LONG_ADDR_ID, (uint8_t *)&value);
		IoT_dev.sensor->resetPropChangeFlag(IOT_I_LONG_ADDR_ID);
		iot_set_long_addr(value);
		ble_char_update_t* ble_char_update_handle = ble_char_update_handle_get();
		ble_char_update_handle->dev_long_addr_update(value);
	}

#if BAT_SOC_DET_SW == 1
	if(sw_bat_soc_get_handle()->get_update_flag())
	{
		IoT_dev.gas_gauge_flag = 1;
		IoT_dev.gas_gauge = sw_bat_soc_get_handle()->get_gas_gauge();
	}
#endif
	
	/* �����豸�����������ͬʱ�������ݵ�����Э���� */
	if(IoT_dev.gas_gauge_flag == 1)
	{
		IoT_dev.gas_gauge_flag = 0;
		iot_write_battery_level(IoT_dev.gas_gauge);
		ble_char_update_t* ble_char_update_handle = ble_char_update_handle_get();
		ble_char_update_handle->dev_battery_update(IoT_dev.gas_gauge);
	}	
	
	/* �����豸�¶ȡ�X��Ƕȡ�Y��Ƕ���������ͬʱ�������ݵ�����Э���� */
	if(IoT_dev.collapse_obj->update_flag == 1)
	{
		IoT_dev.collapse_obj->update_flag = 0;
		iot_write_temp(IoT_dev.collapse_obj->data.temp_c);
		iot_write_x_angle(IoT_dev.collapse_obj->data.angle.x_angle);
		iot_write_y_angle(IoT_dev.collapse_obj->data.angle.y_angle);
		iot_write_z_angle(IoT_dev.collapse_obj->data.angle.z_angle);
		ble_char_update_t* ble_char_update_handle = ble_char_update_handle_get();
		ble_char_update_handle->dev_temperature_update(IoT_dev.collapse_obj->data.temp_c);
		ble_char_update_handle->dev_x_angle_update(IoT_dev.collapse_obj->data.angle.x_angle);
		ble_char_update_handle->dev_y_angle_update(IoT_dev.collapse_obj->data.angle.y_angle);
		ble_char_update_handle->dev_z_angle_update(IoT_dev.collapse_obj->data.angle.z_angle);
	}
	
	/* ����LORA�ź�ǿ�� */
	static int8_t lora_rssi = -127;
	if(lora_rssi != IoT_dev.lora_obj->get_rssi())
	{
		lora_rssi = IoT_dev.lora_obj->get_rssi();
		ble_char_update_t* ble_char_update_handle = ble_char_update_handle_get();
		ble_char_update_handle->dev_lora_rssi_update(lora_rssi);
	}
}

/* ������IoTЭ�鴦�� */
static void iot_c_operate(void)
{
	/* ����IoTЭ�����õĶ̵�ַͬʱ�������ݵ�����Э���� */
	if(IoT_dev.sensor->isPropChanged(IOT_C_SHORT_ADDR_ID))
	{
		uint8_t value[2];
		IoT_dev.sensor->readPropToBuf(IOT_C_SHORT_ADDR_ID, (uint8_t *)&value);
		IoT_dev.sensor->resetPropChangeFlag(IOT_C_SHORT_ADDR_ID);
		iot_set_short_addr(value);
		ble_char_update_t* ble_char_update_handle = ble_char_update_handle_get();
		ble_char_update_handle->dev_short_addr_update(value);
	}
	
	/* ����IoTЭ�����õ�ʱ���ͬʱ�������ݵ�����Э���� */
	if(IoT_dev.sensor->isPropChanged(IOT_C_TIME_STAMP_ID))
	{
		uint32_t value;
		IoT_dev.sensor->readPropToBuf(IOT_C_TIME_STAMP_ID, (uint8_t *)&value);
		IoT_dev.sensor->resetPropChangeFlag(IOT_C_TIME_STAMP_ID);
#if COMM_TRANSMISSION_FORMAT == 1
		swap_reverse((uint8_t*)&value, sizeof(value));
#endif
		iot_set_time_stamp(value);
		ble_char_update_t* ble_char_update_handle = ble_char_update_handle_get();
		ble_char_update_handle->dev_time_stamp_update(value);
	}
	
	/* ����IoTЭ�����õ�ʱ��ƫ��ʱ�������ݵ�����Э���� */
	if(IoT_dev.sensor->isPropChanged(IOT_C_TIME_OFFSET_ID))
	{
		uint16_t value;
		IoT_dev.sensor->readPropToBuf(IOT_C_TIME_OFFSET_ID, (uint8_t *)&value);
		IoT_dev.sensor->resetPropChangeFlag(IOT_C_TIME_OFFSET_ID);
#if COMM_TRANSMISSION_FORMAT == 1
		swap_reverse((uint8_t*)&value, sizeof(value));
#endif
		iot_set_time_offset(value);
		ble_char_update_t* ble_char_update_handle = ble_char_update_handle_get();
		ble_char_update_handle->dev_time_offset_update(value);
	}
	
	/* ����IoTЭ�����õļ��ٶȱ仯��б����ֵͬʱ�������ݵ�����Э���� */
	if(IoT_dev.sensor->isPropChanged(IOT_C_ACCEL_SLOPE_THRESHOLD_ID))
	{
		uint16_t value;
		IoT_dev.sensor->readPropToBuf(IOT_C_ACCEL_SLOPE_THRESHOLD_ID, (uint8_t *)&value);
		IoT_dev.sensor->resetPropChangeFlag(IOT_C_ACCEL_SLOPE_THRESHOLD_ID);
#if COMM_TRANSMISSION_FORMAT == 1
		swap_reverse((uint8_t*)&value, sizeof(value));
#endif
		iot_set_accel_slope_threshold(value);
		ble_char_update_t* ble_char_update_handle = ble_char_update_handle_get();
		ble_char_update_handle->dev_accel_slope_threshold_update(value);
	}
	
	/* ����IoTЭ�����õ��������ݵ�ͬʱ�������ݵ�����Э���� */
	if(IoT_dev.sensor->isPropChanged(IOT_C_CONSECUTIVE_DATA_POINTS_ID))
	{
		uint16_t value;
		IoT_dev.sensor->readPropToBuf(IOT_C_CONSECUTIVE_DATA_POINTS_ID, (uint8_t *)&value);
		IoT_dev.sensor->resetPropChangeFlag(IOT_C_CONSECUTIVE_DATA_POINTS_ID);
#if COMM_TRANSMISSION_FORMAT == 1
		swap_reverse((uint8_t*)&value, sizeof(value));
#endif
		iot_set_consecutive_data_points(value);
		IoT_dev.collapse_obj->iot_set_consecutive_data_points();
		ble_char_update_t* ble_char_update_handle = ble_char_update_handle_get();
		ble_char_update_handle->dev_consecutive_data_points_update(value);
	}
	
	/* ����IoTЭ�����õĲ���ģʽͬʱ�������ݵ�����Э���� */
	if(IoT_dev.sensor->isPropChanged(IOT_C_SAMPLE_MODE_ID))
	{
		uint8_t value;
		IoT_dev.sensor->readPropToBuf(IOT_C_SAMPLE_MODE_ID, (uint8_t *)&value);
		IoT_dev.sensor->resetPropChangeFlag(IOT_C_SAMPLE_MODE_ID);
		iot_set_sample_mode(value);
		ble_char_update_t* ble_char_update_handle = ble_char_update_handle_get();
		ble_char_update_handle->dev_sample_mode_update(value);
	}
	
	/* ����IoTЭ�����õĲ������ͬʱ�������ݵ�����Э���� */
	if(IoT_dev.sensor->isPropChanged(IOT_C_SAMPLE_INTERVAL_ID))
	{
		uint32_t value;
		IoT_dev.sensor->readPropToBuf(IOT_C_SAMPLE_INTERVAL_ID, (uint8_t *)&value);
		IoT_dev.sensor->resetPropChangeFlag(IOT_C_SAMPLE_INTERVAL_ID);
#if COMM_TRANSMISSION_FORMAT == 1
		swap_reverse((uint8_t*)&value, sizeof(value));
#endif
		iot_set_sample_interval(value);
		ble_char_update_t* ble_char_update_handle = ble_char_update_handle_get();
		ble_char_update_handle->dev_sample_interval_update(value);
	}
	
	/* ����IoTЭ�����õĳ���ַͬʱ�������ݵ�����Э���� */
	if(IoT_dev.sensor->isPropChanged(IOT_C_LONG_ADDR_ID))
	{
		uint8_t value[8];
		IoT_dev.sensor->readPropToBuf(IOT_C_LONG_ADDR_ID, (uint8_t *)&value);
		IoT_dev.sensor->resetPropChangeFlag(IOT_C_LONG_ADDR_ID);
		iot_set_long_addr(value);
		ble_char_update_t* ble_char_update_handle = ble_char_update_handle_get();
		ble_char_update_handle->dev_long_addr_update(value);
	}

#if BAT_SOC_DET_SW == 1
	if(sw_bat_soc_get_handle()->get_update_flag())
	{
		IoT_dev.gas_gauge_flag = 1;
		IoT_dev.gas_gauge = sw_bat_soc_get_handle()->get_gas_gauge();
	}
#endif
	
	/* �����豸�����������ͬʱ�������ݵ�����Э���� */
	if(IoT_dev.gas_gauge_flag == 1)
	{
		IoT_dev.gas_gauge_flag = 0;
		iot_write_battery_level(IoT_dev.gas_gauge);
		ble_char_update_t* ble_char_update_handle = ble_char_update_handle_get();
		ble_char_update_handle->dev_battery_update(IoT_dev.gas_gauge);
	}	
	
	/* �����豸�¶ȡ�������������ͬʱ�������ݵ�����Э���� */
	if(IoT_dev.collapse_obj->update_flag == 1)
	{
		IoT_dev.collapse_obj->update_flag = 0;
		iot_write_temp(IoT_dev.collapse_obj->data.temp_c);
		iot_write_x_accel(IoT_dev.collapse_obj->data.accel.x_accel);
		iot_write_y_accel(IoT_dev.collapse_obj->data.accel.y_accel);
		iot_write_z_accel(IoT_dev.collapse_obj->data.accel.z_accel);

#if (IOT_PROTOCOL_C_WITH_ANGLE == 1)
		iot_write_x_angle(IoT_dev.collapse_obj->data.angle.x_angle);
		iot_write_y_angle(IoT_dev.collapse_obj->data.angle.y_angle);
		iot_write_z_angle(IoT_dev.collapse_obj->data.angle.z_angle);
#endif
		
		ble_char_update_t* ble_char_update_handle = ble_char_update_handle_get();
		ble_char_update_handle->dev_temperature_update(IoT_dev.collapse_obj->data.temp_c);
		ble_char_update_handle->dev_x_accel_update(IoT_dev.collapse_obj->data.accel.x_accel);
		ble_char_update_handle->dev_y_accel_update(IoT_dev.collapse_obj->data.accel.y_accel);
		ble_char_update_handle->dev_z_accel_update(IoT_dev.collapse_obj->data.accel.z_accel);
		ble_char_update_handle->dev_x_angle_update(IoT_dev.collapse_obj->data.angle.x_angle);
		ble_char_update_handle->dev_y_angle_update(IoT_dev.collapse_obj->data.angle.y_angle);
		ble_char_update_handle->dev_z_angle_update(IoT_dev.collapse_obj->data.angle.z_angle);
	}
	
	/* ����LORA�ź�ǿ�� */
	static int8_t lora_rssi = -127;
	if(lora_rssi != IoT_dev.lora_obj->get_rssi())
	{
		lora_rssi = IoT_dev.lora_obj->get_rssi();
		ble_char_update_t* ble_char_update_handle = ble_char_update_handle_get();
		ble_char_update_handle->dev_lora_rssi_update(lora_rssi);
	}
}

/* IoTЭ�鴦����IoT�������ݴ��� */
void iot_operate(void)
{
	sys_param_t* param = sys_param_get_handle();
	if(param->object_version == INCLINOMETER_VERSION)
	{
		iot_i_operate();
	}
	else if(param->object_version == COLLAPSE_VERSION)
	{
		iot_c_operate();
	}
}

/* IoTЭ���ʼ�� */
iot_dev_t * iot_init(iot_object_t *sensor, collapse_obj_t *collapse_obj, lora_obj_t* lora_obj)
{
	IoT_dev.gas_gauge_flag = 0;
	IoT_dev.gas_gauge = 100;

	IoT_dev.sensor = sensor;
	IoT_dev.collapse_obj = collapse_obj;
	IoT_dev.lora_obj = lora_obj;
	
	/* IoT�����������ݳ�ʼ�� */
	sys_param_t* param = sys_param_get_handle();
	if(param->object_version == INCLINOMETER_VERSION)
	{
		iot_write_long_addr(param->dev_long_addr);
		iot_write_short_addr(param->dev_short_addr);
		iot_write_sample_mode(param->iot_clinometer.iot_mode);
		iot_write_sample_interval(param->iot_clinometer.iot_sample_interval);	
		iot_write_time_stamp(0);
		iot_write_time_offset(param->iot_clinometer.time_offset);
		iot_write_battery_level(IoT_dev.gas_gauge);
		iot_write_temp(25);
		iot_write_x_angle(0);
		iot_write_y_angle(0);
		iot_write_z_angle(0);
		iot_write_x_angle_threshold(param->iot_clinometer.iot_x_angle_threshold);
		iot_write_y_angle_threshold(param->iot_clinometer.iot_y_angle_threshold);
		iot_write_z_angle_threshold(param->iot_clinometer.iot_z_angle_threshold);
	}
	else if(param->object_version == COLLAPSE_VERSION)
	{
		iot_write_long_addr(param->dev_long_addr);
		iot_write_short_addr(param->dev_short_addr);
		iot_write_sample_mode(param->iot_collapse.iot_mode);
		iot_write_sample_interval(param->iot_collapse.iot_period);	
		iot_write_time_stamp(0);
		iot_write_time_offset(param->iot_collapse.time_offset);
		iot_write_accel_slope_threshold(param->iot_collapse.iot_accel_slope_threshold);
		iot_write_consecutive_data_points(param->iot_collapse.iot_consecutive_data_points);
		iot_write_battery_level(IoT_dev.gas_gauge);
		iot_write_temp(25);
		iot_write_x_accel(0);
		iot_write_y_accel(0);
		iot_write_z_accel(0);

#if (IOT_PROTOCOL_C_WITH_ANGLE == 1)
		iot_write_x_angle(0);
		iot_write_y_angle(0);
		iot_write_z_angle(0);
#endif
	}
	
	IoT_dev.operate = iot_operate;
	
	return &IoT_dev;
}



