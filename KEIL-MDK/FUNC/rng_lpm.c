#include "rng_lpm.h"
#include "nrf_drv_rng.h"


rng_lpm_mod_t rng_lpm_mod;


static void rng_lpm_generate_random_number(uint32_t* random_buf, uint32_t size)
{
	uint32_t err_code;
	uint8_t  available;

	nrf_drv_rng_bytes_available(&available);
	uint8_t length = MIN(size*4, available);

	err_code = nrf_drv_rng_rand((uint8_t*)random_buf, length);
	APP_ERROR_CHECK(err_code);
	
	for(int32_t i=0; i<size; i++)
	{
		random_buf[i] = random_buf[i] % (rng_lpm_mod.ramdom_upper - rng_lpm_mod.ramdom_lower) + rng_lpm_mod.ramdom_lower;
	}

	return;
}

void rng_lpm_init(void)
{
	uint32_t err_code = nrf_drv_rng_init(NULL);
	APP_ERROR_CHECK(err_code);
	
	rng_lpm_mod.ramdom_upper = RNG_RANDOM_UPPER;
	rng_lpm_mod.ramdom_lower = RNG_RANDOM_LOWER;
	
	rng_lpm_mod.generate_random_number = rng_lpm_generate_random_number;
}

rng_lpm_mod_t* rng_lpm_get_handle(void)
{
	return &rng_lpm_mod;
}


__weak void rng_err_handler(void)
{
	return;
}  









