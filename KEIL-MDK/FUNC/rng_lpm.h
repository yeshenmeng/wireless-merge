#ifndef __RNG_LPM_H__
#define __RNG_LPM_H__
#include "main.h"


#define RNG_RANDOM_UPPER			10000u
#define RNG_RANDOM_LOWER			1u

typedef struct {
	uint32_t ramdom_upper;
	uint32_t ramdom_lower;
	
	void (*generate_random_number)(uint32_t* random_buf, uint32_t size);
}rng_lpm_mod_t;


void rng_lpm_init(void);
rng_lpm_mod_t* rng_lpm_get_handle(void);

#endif



