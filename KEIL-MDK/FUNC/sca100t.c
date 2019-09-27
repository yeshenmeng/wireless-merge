#include "sca100t.h"
#include "nrf_gpio.h"


#define SCA_SW_SPI_SCK_SET()			nrf_gpio_pin_set(SCA_SW_SPI_SCK_PIN)
#define SCA_SW_SPI_SCK_CLR()			nrf_gpio_pin_clear(SCA_SW_SPI_SCK_PIN)
#define SCA_SW_SPI_MOSI_SET()			nrf_gpio_pin_set(SCA_SW_SPI_MOSI_PIN)
#define SCA_SW_SPI_MOSI_CLR()			nrf_gpio_pin_clear(SCA_SW_SPI_MOSI_PIN)
#define SCA_SW_SPI_CS_ENABLE()		nrf_gpio_pin_clear(SCA_SW_SPI_CS_PIN)
#define SCA_SW_SPI_CS_DISABLE()		nrf_gpio_pin_set(SCA_SW_SPI_CS_PIN)
#define SCA_SW_SPI_MISO_READ() 		nrf_gpio_pin_read(SCA_SW_SPI_MISO_PIN)


static void sca_delay_us(uint32_t nus)
{
	uint16_t i=0;  
	while(nus--)
	{
		i=10;
		while(i--); 
	}
}

static void sca_sw_spi_cfg(void)
{
	nrf_gpio_cfg(SCA_SW_SPI_SCK_PIN,
				 NRF_GPIO_PIN_DIR_OUTPUT,
				 NRF_GPIO_PIN_INPUT_DISCONNECT,
				 NRF_GPIO_PIN_NOPULL,
				 NRF_GPIO_PIN_S0S1,
				 NRF_GPIO_PIN_NOSENSE);
	
	nrf_gpio_cfg(SCA_SW_SPI_MOSI_PIN,
				 NRF_GPIO_PIN_DIR_OUTPUT,
				 NRF_GPIO_PIN_INPUT_DISCONNECT,
				 NRF_GPIO_PIN_NOPULL,
				 NRF_GPIO_PIN_S0S1,
				 NRF_GPIO_PIN_NOSENSE);

	nrf_gpio_cfg(SCA_SW_SPI_MISO_PIN,
				 NRF_GPIO_PIN_DIR_INPUT,
				 NRF_GPIO_PIN_INPUT_CONNECT,
//				 NRF_GPIO_PIN_NOPULL,
				 NRF_GPIO_PIN_PULLDOWN,
				 NRF_GPIO_PIN_S0S1,
				 NRF_GPIO_PIN_NOSENSE);

	nrf_gpio_cfg(SCA_SW_SPI_CS_PIN,
				 NRF_GPIO_PIN_DIR_OUTPUT,
				 NRF_GPIO_PIN_INPUT_DISCONNECT,
				 NRF_GPIO_PIN_NOPULL,
				 NRF_GPIO_PIN_S0S1,
				 NRF_GPIO_PIN_NOSENSE);
}

static void sca_sw_spi_cfg_default(void)
{
	nrf_gpio_cfg_default(SCA_SW_SPI_SCK_PIN);
	nrf_gpio_cfg_default(SCA_SW_SPI_MOSI_PIN);
	nrf_gpio_cfg_default(SCA_SW_SPI_MISO_PIN);
	nrf_gpio_cfg_default(SCA_SW_SPI_CS_PIN);
}

static void sca_sw_spi_transmit_receive(uint16_t tx_data, 
										uint8_t tx_bit_size, 
										uint16_t *p_rx_data, 
										uint8_t rx_bit_size)
{
	SCA_SW_SPI_CS_ENABLE();
	
	uint16_t _tx_data = tx_data;
	uint16_t _rx_data = 0;
	
	if(tx_bit_size != 0)
	{
		for(int i=0; i<tx_bit_size; i++)
		{
			SCA_SW_SPI_SCK_CLR();
			if(_tx_data & 0X80)
			{
				SCA_SW_SPI_MOSI_SET();
			}
			else
			{
				SCA_SW_SPI_MOSI_CLR();
			}
			_tx_data <<= 1;
			sca_delay_us(2);
			SCA_SW_SPI_SCK_SET();
			sca_delay_us(2);
			
			_rx_data <<= 1;
			if(SCA_SW_SPI_MISO_READ())
			{
				_rx_data++;
			}
		}
		SCA_SW_SPI_SCK_CLR();
	}
	
	if(rx_bit_size != 0)
	{
		_rx_data = 0;
		for(int i=0; i<rx_bit_size; i++)
		{
			SCA_SW_SPI_SCK_CLR();
			sca_delay_us(2);
			SCA_SW_SPI_SCK_SET();
			sca_delay_us(2);
			_rx_data <<= 1;
			if(SCA_SW_SPI_MISO_READ())
			{
				_rx_data++;
			}
		}
	}

	if(p_rx_data != NULL)
	{
		*p_rx_data = _rx_data;
	}
	
	SCA_SW_SPI_SCK_CLR();
	SCA_SW_SPI_CS_DISABLE();
	
	return;
}

void sca_init(void)
{
	sca_sw_spi_cfg();
}

void sca_default(void)
{
	sca_sw_spi_cfg_default();
}

void sca_write_cmd(uint8_t cmd)
{
	sca_sw_spi_transmit_receive(cmd, 8, NULL, 0);
}

uint8_t sca_read_temp(void)
{
	uint16_t temp;
	sca_sw_spi_transmit_receive(RWTR, 8, &temp, 8);
	return (uint8_t)temp;	
}

uint16_t sca_read_x_channel(void)
{
	uint16_t xChannelData;
	sca_sw_spi_transmit_receive(RDAX, 8, &xChannelData, 11);
	return xChannelData;
}

uint16_t sca_read_y_channel(void)
{
	uint16_t yChannelData;
	sca_sw_spi_transmit_receive(RDAY, 8, &yChannelData, 11);
	return yChannelData;	
}























