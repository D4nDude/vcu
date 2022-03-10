/***************************************************************************
 * @file   testbench.c
 * @author Tim Brewis (tab1g19@soton.ac.uk)
 * @date   2022-02-08
 * @brief  Testbench implementations
***************************************************************************/

#include "Test/testbench.h"

#include <stdbool.h>

/***************************************************************************
 * throttle input testbench
 ***************************************************************************/

#if (RUN_THROTTLE_TESTBENCH)

static UINT throttle = 0;

/**
 * @brief 	Testbench to simulate throttle inputs using data from a look-up table
 *
 * @details If THROTTLE_TESTBENCH_LAPS (config.h) is set to 1, only the first lap
 * 			with a standing start will be simulated. For values of 2 or more, additional
 * 			flying laps are added. The simulated throttle is the same for laps 2, 3, 4, etc.
 *
 * @return	Simulated throttle input for current system timestamp
 */
UINT testbench_throttle()
{
	// persistent variables
	static UINT lap_count = 0;
	static UINT lookup_index = 0;
	static UINT last_lap_finish_time = 0;

	if (lap_count < THROTTLE_TESTBENCH_LAPS)
	{
		// work out which lookup table to use
		// -> standing start data in first lap
		// -> flying lap data for any further laps
		const uint16_t* time_data;
		const uint16_t* throttle_data;
		UINT final_lookup_index;

		{
			// first lap (standing start)
			if (lap_count == 0)
			{
				time_data = standing_start_time_lookup;
				throttle_data = standing_start_throttle_lookup;
				final_lookup_index = (sizeof(standing_start_time_lookup) / sizeof(standing_start_time_lookup[0])) - 1;
			}
			// flying laps
			else
			{
				time_data = flying_lap_time_lookup;
				throttle_data = flying_lap_throttle_lookup;
				final_lookup_index = (sizeof(flying_lap_time_lookup) / sizeof(flying_lap_time_lookup[0])) - 1;
			}
		}

		// timestamps in ms
		const UINT current_time = tx_time_get() * 1000 / TX_TIMER_TICKS_PER_SECOND;
		const UINT lap_finish_time = time_data[final_lookup_index];
		const UINT current_lap_time = current_time - last_lap_finish_time;

		// progress to next throttle input if in next time slot
		UINT next_time;
		bool end_of_lap = false;

		while (current_lap_time > (next_time = time_data[lookup_index + 1]))
		{
			lookup_index++;

			// check for end of lap
			if (next_time == lap_finish_time)
			{
				end_of_lap = true;
				break;
			}
		}

		throttle = throttle_data[lookup_index];

		// end of lap handling
		if (end_of_lap)
		{
			end_of_lap = false;
			lookup_index = 0;
			lap_count++;
			last_lap_finish_time = current_time;
		}
	}
	else
	{
		throttle = 0;
	}

	return throttle;
}

#endif

