/***************************************************************************
 * @file   sensor_thread.c
 * @author Cosmin-Andrei Tamas (cat1g19@soton.ac.uk)
 * @author Tim Brewis (tab1g19@soton.ac.uk)
 * @date   2021-11-30
 * @brief  Sensor thread
 ***************************************************************************/

#include "sensor_thread.h"
#include "gpio.h"
#include "adc.h"
#include "messaging_system.h"

#include <stdint.h>

/**
 * @brief Thread managing the sensor measurements
 */
TX_THREAD sensor_thread;
uint32_t adc_raw[10];


/*
 * return codes
 */
#define ADC_OK	0x0000
#define ADC_ERR	0x0001


/*
 * function prototypes
 */
UINT read_adc_blocking(UINT* data_ptr);

/**
 * @brief Sensor thread entry function
 *
 * @param[in]	thread_input	Unused input
 */
void sensor_thread_entry(ULONG thread_input)
{
	// prevent compiler warnings as input is not used for the moment
	(VOID) thread_input;

	// loop forever
	while(1)
	{
		// read from the ADC
		control_input_message_t message;

		if (read_adc_blocking(&message.input) == ADC_OK)
		{
			// timestamp the message
			message_set_timestamp(&message.timestamp);

			// send message to the control thread
			message_post((VOID*) &message, &control_input_queue);
		}

		// sleep thread to allow other threads to run
		tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND / 10);
	}

}

/**
 * @brief 		Demo code for blocking read from ADC
 *
 * @param[in]	data_ptr		Pointer to UINT to store 16 bit reading
 *
 * @return		ADC_OK if ADC read successfully
 * 				ADC_ERR otherwise
 */
UINT read_adc_blocking(UINT* data_ptr)
{
	/*
	 * Read from ADC
	 * - 16 bit resolution, max 0xFFFF
	 * - 3v3 reference
	 * - ADC1 IN3 single ended reads from pin D12 (PA6)
	 * - connect to 1.8v on JP5 as a simple test
	 */

	// start ADC
	// wait for conversion to complete
	if (HAL_ADC_Start(&hadc1) == HAL_OK)
	{
		// block until conversion complete
		HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);

		// read conversion data
		*data_ptr = HAL_ADC_GetValue(&hadc1);

		/*
		 * alternatively to convert to float:
		 * *data_ptr = 3.3f * ((float) adc_raw / (float) 0xFFFF);
		 */
		return ADC_OK;
	}

	return ADC_ERR;
}
