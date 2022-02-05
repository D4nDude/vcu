/***************************************************************************
 * @file   messaging.h
 * @author Rory Wilson (rw7g20@soton.ac.uk)
 * @author Tim Brewis (tab1g19@soton.ac.uk)
 * @date   2021-12-07
 * @brief  Definitions for message types and priorities used by messaging system
 ***************************************************************************/

#ifndef MESSAGE_TYPES_H
#define MESSAGE_TYPES_H

/**
 * @brief Torque request message
 */
typedef struct {
	UINT timestamp;
	UINT value;
} torque_request_message_t;

/**
 * @brief Control input message
 */
typedef struct {
	UINT timestamp;
	UINT input;
} control_input_message_t;

#endif