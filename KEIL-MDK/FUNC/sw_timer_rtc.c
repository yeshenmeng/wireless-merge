#include "sw_timer_rtc.h"


static swt_mod_t swt_mod;


/**********************************系统空闲软件定时器**********************************/
static sw_timer_t swt_sys_idle;
APP_TIMER_DEF(sys_idle_id);
__weak void swt_idle_cb(void* param)
{
	return;
}

static void swt_sys_idle_start(uint32_t time)
{
	app_timer_start(sys_idle_id, APP_TIMER_TICKS(time), NULL);
}

static void swt_sys_idle_stop(void)
{
	app_timer_stop(sys_idle_id);
}

static void swt_sys_idle_del(void)
{
	return;
}

sw_timer_t* swt_sys_idle_create(void)
{
	swt_sys_idle.process_id = 100;
	swt_sys_idle.mode = APP_TIMER_MODE_SINGLE_SHOT/*APP_TIMER_MODE_REPEATED*/;
	swt_sys_idle.timeout_cb = swt_idle_cb;
	swt_sys_idle.start = swt_sys_idle_start;
	swt_sys_idle.stop = swt_sys_idle_stop;
	swt_sys_idle.del = swt_sys_idle_del;
	
	app_timer_create(&sys_idle_id,
					 swt_sys_idle.mode,
					 swt_sys_idle.timeout_cb);
	
	return &swt_sys_idle;
}


/**********************************系统低功耗时间软件定时器**********************************/
static sw_timer_t swt_sys_low_power;
APP_TIMER_DEF(sys_low_power_id);
__weak void swt_slp_cb(void* param)
{
	return;
}

static void swt_sys_lp_start(uint32_t time)
{
	swt_sys_idle_stop();
	app_timer_start(sys_low_power_id, APP_TIMER_TICKS(time), NULL);
}

static void swt_sys_lp_stop(void)
{
	app_timer_stop(sys_low_power_id);
}

static void swt_sys_lp_del(void)
{
	return;
}

sw_timer_t* swt_sys_lp_create(void)
{
	swt_sys_low_power.process_id = 101;
	swt_sys_low_power.mode = APP_TIMER_MODE_SINGLE_SHOT/*APP_TIMER_MODE_REPEATED*/;
	swt_sys_low_power.timeout_cb = swt_slp_cb;
	swt_sys_low_power.start = swt_sys_lp_start;
	swt_sys_low_power.stop = swt_sys_lp_stop;
	swt_sys_low_power.del = swt_sys_lp_del;
	
	app_timer_create(&sys_low_power_id,
					 swt_sys_low_power.mode,
					 swt_sys_low_power.timeout_cb);
	
	return &swt_sys_low_power;
}


/**********************************蓝牙广播时间软件定时器**********************************/
static sw_timer_t swt_ble_adv;
APP_TIMER_DEF(ble_adv_id);
__weak void swt_ble_adv_cb(void* param)
{
	return;
}

static void swt_ble_adv_start(uint32_t time)
{
	swt_sys_idle_stop();
	app_timer_start(ble_adv_id, APP_TIMER_TICKS(time), NULL);
}

static void swt_ble_adv_stop(void)
{
	app_timer_stop(ble_adv_id);
}

static void swt_ble_adv_del(void)
{
	return;
}

sw_timer_t* swt_ble_adv_create(void)
{
	swt_ble_adv.process_id = 102;
	swt_ble_adv.mode = APP_TIMER_MODE_SINGLE_SHOT/*APP_TIMER_MODE_REPEATED*/;
	swt_ble_adv.timeout_cb = swt_ble_adv_cb;
	swt_ble_adv.start = swt_ble_adv_start;
	swt_ble_adv.stop = swt_ble_adv_stop;
	swt_ble_adv.del = swt_ble_adv_del;
	
	app_timer_create(&ble_adv_id,
					 swt_ble_adv.mode,
					 swt_ble_adv.timeout_cb);
	
	return &swt_ble_adv;
}


/**********************************蓝牙广播LED指示灯软件定时器**********************************/
static sw_timer_t swt_ble_adv_led;
APP_TIMER_DEF(ble_adv_led_id);
__weak void swt_ble_adv_led_cb(void* param)
{
	return;
}

static void swt_ble_adv_led_start(uint32_t time)
{
	swt_sys_idle_stop();
	app_timer_start(ble_adv_led_id, APP_TIMER_TICKS(time), NULL);
}

static void swt_ble_adv_led_stop(void)
{
	app_timer_stop(ble_adv_led_id);
}

static void swt_ble_adv_led_del(void)
{
	return;
}

sw_timer_t* swt_ble_adv_led_del_create(void)
{
	swt_ble_adv_led.process_id = 103;
	swt_ble_adv_led.mode = APP_TIMER_MODE_REPEATED/*APP_TIMER_MODE_SINGLE_SHOT*/;
	swt_ble_adv_led.timeout_cb = swt_ble_adv_led_cb;
	swt_ble_adv_led.start = swt_ble_adv_led_start;
	swt_ble_adv_led.stop = swt_ble_adv_led_stop;
	swt_ble_adv_led.del = swt_ble_adv_led_del;
	
	app_timer_create(&ble_adv_led_id,
					 swt_ble_adv_led.mode,
					 swt_ble_adv_led.timeout_cb);

	return &swt_ble_adv_led;
}


/**********************************LORA活动时间片软件定时器**********************************/
static sw_timer_t swt_lora_task_time_slice;
APP_TIMER_DEF(lora_task_time_slice_id);
__weak void swt_lora_task_time_slice_cb(void* param)
{
	return;
}

static void swt_lora_task_time_slice_start(uint32_t time)
{
	swt_sys_idle_stop();
	app_timer_start(lora_task_time_slice_id, APP_TIMER_TICKS(time), NULL);
}

static void swt_lora_task_time_slice_stop(void)
{
	app_timer_stop(lora_task_time_slice_id);
}

static void swt_lora_task_time_slice_del(void)
{
	return;
}

sw_timer_t* swt_lora_task_time_slice_create(void)
{
	swt_lora_task_time_slice.process_id = 104;
	swt_lora_task_time_slice.mode = APP_TIMER_MODE_SINGLE_SHOT/*APP_TIMER_MODE_REPEATED*/;
	swt_lora_task_time_slice.timeout_cb = swt_lora_task_time_slice_cb;
	swt_lora_task_time_slice.start = swt_lora_task_time_slice_start;
	swt_lora_task_time_slice.stop = swt_lora_task_time_slice_stop;
	swt_lora_task_time_slice.del = swt_lora_task_time_slice_del;
	
	app_timer_create(&lora_task_time_slice_id,
					 swt_lora_task_time_slice.mode,
					 swt_lora_task_time_slice.timeout_cb);
	
	return &swt_lora_task_time_slice;
}


/**********************************LORA数据发送超时软件定时器**********************************/
static sw_timer_t swt_lora_tx_timeout;
APP_TIMER_DEF(lora_tx_timeout_id);
__weak void swt_lora_tx_timeout_cb(void* param)
{
	return;
}

static void swt_lora_tx_timeout_start(uint32_t time)
{
	swt_sys_idle_stop();
	app_timer_start(lora_tx_timeout_id, APP_TIMER_TICKS(time), NULL);
}

static void swt_lora_tx_timeout_stop(void)
{
	app_timer_stop(lora_tx_timeout_id);
}

static void swt_lora_tx_timeout_del(void)
{
	return;
}

sw_timer_t* swt_lora_tx_timeout_create(void)
{
	swt_lora_tx_timeout.process_id = 105;
	swt_lora_tx_timeout.mode = APP_TIMER_MODE_SINGLE_SHOT/*APP_TIMER_MODE_REPEATED*/;
	swt_lora_tx_timeout.timeout_cb = swt_lora_tx_timeout_cb;
	swt_lora_tx_timeout.start = swt_lora_tx_timeout_start;
	swt_lora_tx_timeout.stop = swt_lora_tx_timeout_stop;
	swt_lora_tx_timeout.del = swt_lora_tx_timeout_del;
	
	app_timer_create(&lora_tx_timeout_id,
					 swt_lora_tx_timeout.mode,
					 swt_lora_tx_timeout.timeout_cb);

	return &swt_lora_tx_timeout;
}


/**********************************LORA空闲时间软件定时器**********************************/
static sw_timer_t swt_lora_idle;
APP_TIMER_DEF(lora_idle_id);
__weak void swt_lora_idle_cb(void* param)
{
	return;
}

static void swt_lora_idle_start(uint32_t time)
{
	swt_sys_idle_stop();
	app_timer_start(lora_idle_id, APP_TIMER_TICKS(time), NULL);
}

static void swt_lora_idle_stop(void)
{
	app_timer_stop(lora_idle_id);
}

static void swt_lora_idle_del(void)
{
	return;
}

sw_timer_t* swt_lora_idle_create(void)
{
	swt_lora_idle.process_id = 106;
	swt_lora_idle.mode = APP_TIMER_MODE_SINGLE_SHOT/*APP_TIMER_MODE_REPEATED*/;
	swt_lora_idle.timeout_cb = swt_lora_idle_cb;
	swt_lora_idle.start = swt_lora_idle_start;
	swt_lora_idle.stop = swt_lora_idle_stop;
	swt_lora_idle.del = swt_lora_idle_del;
	
	app_timer_create(&lora_idle_id,
					 swt_lora_idle.mode,
					 swt_lora_idle.timeout_cb);

	return &swt_lora_idle;
}


/**********************************信号检测时间片软件定时器**********************************/
static sw_timer_t swt_signal_detect_time_slice;
APP_TIMER_DEF(signal_det_time_slice_id);
__weak void swt_signal_detect_time_slice_cb(void* param)
{
	return;
}

static void swt_signal_detect_time_slice_start(uint32_t time)
{
	swt_sys_idle_stop();
	app_timer_start(signal_det_time_slice_id, APP_TIMER_TICKS(time), NULL);
}

static void swt_signal_detect_time_slice_stop(void)
{
	app_timer_stop(signal_det_time_slice_id);
}

static void swt_signal_detect_time_slice_del(void)
{
	return;
}

sw_timer_t* swt_signal_detect_time_slice_create(void)
{
	swt_signal_detect_time_slice.process_id = 107;
	swt_signal_detect_time_slice.mode = APP_TIMER_MODE_SINGLE_SHOT/*APP_TIMER_MODE_REPEATED*/;
	swt_signal_detect_time_slice.timeout_cb = swt_signal_detect_time_slice_cb;
	swt_signal_detect_time_slice.start = swt_signal_detect_time_slice_start;
	swt_signal_detect_time_slice.stop = swt_signal_detect_time_slice_stop;
	swt_signal_detect_time_slice.del = swt_signal_detect_time_slice_del;
	
	app_timer_create(&signal_det_time_slice_id,
					 swt_signal_detect_time_slice.mode,
					 swt_signal_detect_time_slice.timeout_cb);

	return &swt_signal_detect_time_slice;
}

/**********************************电池电量任务软件定时器**********************************/
static sw_timer_t swt_bat_soc;
APP_TIMER_DEF(bat_soc_id);
__weak void swt_bat_soc_cb(void* param)
{
	return;
}

static void swt_bat_soc_start(uint32_t time)
{
	swt_sys_idle_stop();
	app_timer_start(bat_soc_id, APP_TIMER_TICKS(time), NULL);
}

static void swt_bat_soc_stop(void)
{
	app_timer_stop(bat_soc_id);
}

static void swt_bat_soc_del(void)
{
	return;
}

sw_timer_t* swt_bat_soc_create(void)
{
	swt_bat_soc.process_id = 108;
	swt_bat_soc.mode = APP_TIMER_MODE_SINGLE_SHOT/*APP_TIMER_MODE_REPEATED*/;
	swt_bat_soc.timeout_cb = swt_bat_soc_cb;
	swt_bat_soc.start = swt_bat_soc_start;
	swt_bat_soc.stop = swt_bat_soc_stop;
	swt_bat_soc.del = swt_bat_soc_del;
	
	app_timer_create(&bat_soc_id,
					 swt_bat_soc.mode,
					 swt_bat_soc.timeout_cb);

	return &swt_bat_soc;
}

/**********************************崩塌计任务FIFO_IN软件定时器**********************************/
static sw_timer_t swt_collapse_fifo_in;
APP_TIMER_DEF(collapse_fifo_in_id);
__weak void swt_collapse_fifo_in_cb(void* param)
{
	return;
}

static void swt_collapse_fifo_in_start(uint32_t time)
{
	swt_sys_idle_stop();
	app_timer_start(collapse_fifo_in_id, APP_TIMER_TICKS(time), NULL);
}

static void swt_collapse_fifo_in_stop(void)
{
	app_timer_stop(collapse_fifo_in_id);
}

static void swt_collapse_fifo_in_del(void)
{
	return;
}

sw_timer_t* swt_collapse_fifo_in_create(void)
{
	swt_collapse_fifo_in.process_id = 109;
	swt_collapse_fifo_in.mode = APP_TIMER_MODE_SINGLE_SHOT/*APP_TIMER_MODE_REPEATED*/;
	swt_collapse_fifo_in.timeout_cb = swt_collapse_fifo_in_cb;
	swt_collapse_fifo_in.start = swt_collapse_fifo_in_start;
	swt_collapse_fifo_in.stop = swt_collapse_fifo_in_stop;
	swt_collapse_fifo_in.del = swt_collapse_fifo_in_del;
	
	app_timer_create(&collapse_fifo_in_id,
					 swt_collapse_fifo_in.mode,
					 swt_collapse_fifo_in.timeout_cb);

	return &swt_collapse_fifo_in;
}

/**********************************崩塌计任务FIFO_OUT软件定时器**********************************/
static sw_timer_t swt_collapse_fifo_out;
APP_TIMER_DEF(collapse_fifo_out_id);
__weak void swt_collapse_fifo_out_cb(void* param)
{
	return;
}

static void swt_collapse_fifo_out_start(uint32_t time)
{
	swt_sys_idle_stop();
	app_timer_start(collapse_fifo_out_id, APP_TIMER_TICKS(time), NULL);
}

static void swt_collapse_fifo_out_stop(void)
{
	app_timer_stop(collapse_fifo_out_id);
}

static void swt_collapse_fifo_out_del(void)
{
	return;
}

sw_timer_t* swt_collapse_fifo_out_create(void)
{
	swt_collapse_fifo_out.process_id = 110;
	swt_collapse_fifo_out.mode = APP_TIMER_MODE_SINGLE_SHOT/*APP_TIMER_MODE_REPEATED*/;
	swt_collapse_fifo_out.timeout_cb = swt_collapse_fifo_out_cb;
	swt_collapse_fifo_out.start = swt_collapse_fifo_out_start;
	swt_collapse_fifo_out.stop = swt_collapse_fifo_out_stop;
	swt_collapse_fifo_out.del = swt_collapse_fifo_out_del;
	
	app_timer_create(&collapse_fifo_out_id,
					 swt_collapse_fifo_out.mode,
					 swt_collapse_fifo_out.timeout_cb);

	return &swt_collapse_fifo_out;
}

/**********************************LORA通信质量测试软件定时器**********************************/
static sw_timer_t swt_llrt_timer;
APP_TIMER_DEF(llrt_timer_id);
__weak void swt_llrt_timer_cb(void* param)
{
	return;
}

static void swt_llrt_timer_start(uint32_t time)
{
	swt_sys_idle_stop();
	app_timer_start(llrt_timer_id, APP_TIMER_TICKS(time), NULL);
}

static void swt_llrt_timer_stop(void)
{
	app_timer_stop(llrt_timer_id);
}

static void swt_llrt_timer_del(void)
{
	return;
}

sw_timer_t* swt_llrt_timer_create(void)
{
	swt_llrt_timer.process_id = 111;
	swt_llrt_timer.mode = APP_TIMER_MODE_SINGLE_SHOT/*APP_TIMER_MODE_REPEATED*/;
	swt_llrt_timer.timeout_cb = swt_llrt_timer_cb;
	swt_llrt_timer.start = swt_llrt_timer_start;
	swt_llrt_timer.stop = swt_llrt_timer_stop;
	swt_llrt_timer.del = swt_llrt_timer_del;
	
	app_timer_create(&llrt_timer_id,
					 swt_llrt_timer.mode,
					 swt_llrt_timer.timeout_cb);

	return &swt_llrt_timer;
}

void swt_init(void)
{
	swt_mod.sys_idle = &swt_sys_idle;
	swt_mod.sys_low_power = &swt_sys_low_power;
	swt_mod.ble_adv = &swt_ble_adv;
	swt_mod.ble_adv_led = &swt_ble_adv_led;
	swt_mod.lora_task_time_slice = &swt_lora_task_time_slice;
	swt_mod.lora_tx_timeout = &swt_lora_tx_timeout;
	swt_mod.lora_idle = &swt_lora_idle;
	swt_mod.signal_detect_time_slice = &swt_signal_detect_time_slice;
	swt_mod.bat_soc = &swt_bat_soc;
	swt_mod.collapse_fifo_in = &swt_collapse_fifo_in;
	swt_mod.collapse_fifo_out = &swt_collapse_fifo_out;
	swt_mod.llrt_timer = &swt_llrt_timer;
	
	swt_sys_idle_create();
	swt_sys_lp_create();
	swt_ble_adv_create();
	swt_lora_task_time_slice_create();
	swt_lora_tx_timeout_create();
	swt_lora_idle_create();
	swt_signal_detect_time_slice_create();
	swt_ble_adv_led_del_create();
	swt_bat_soc_create();
	swt_collapse_fifo_in_create();
	swt_collapse_fifo_out_create();
	swt_llrt_timer_create();
}

swt_mod_t* swt_get_handle(void)
{
	return &swt_mod;
}










