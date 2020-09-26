/******************************************************************************/
/*! \file sh_timer.c
*
* @brief Bologna University Electronic Engineering Thesis main file.
*
* This file contains is the source code for a sample application using the
* Immediate Alert Client service.
*
* This application would accept pairing requests from any peer device.
*
*   \author  	Sinan ALTINSOY
*   \version 	1.00
*   \date    	31/08/2020
*******************************************************************************/
#include "sh_ble.h"
#include "sh_gpio.h"
#include "sh_timer.h"

volatile bool m_is_timer_send = false; 

void sh_timer_init()
{
  app_timer_init();
	sh_start_timer();
}

void sh_start_timer(void)
{
 	// 32-bit timer
	NRF_TIMER2->BITMODE = TIMER_BITMODE_BITMODE_32Bit << TIMER_BITMODE_BITMODE_Pos;
	
	// 1us timer period
	NRF_TIMER2->PRESCALER = 4 << TIMER_PRESCALER_PRESCALER_Pos;
	
	// 1000 us compare value, generates EVENTS_COMPARE[0]
	NRF_TIMER2->CC[0] = 3000000;
	
	// Enable IRQ on EVENTS_COMPARE[0]
	NRF_TIMER2->INTENSET = TIMER_INTENSET_COMPARE0_Enabled << TIMER_INTENSET_COMPARE0_Pos;
	
	// Clear the timer when COMPARE0 event is triggered
	NRF_TIMER2->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Enabled << TIMER_SHORTS_COMPARE0_CLEAR_Pos;
	
	NVIC_EnableIRQ(TIMER2_IRQn);
	
	NRF_TIMER2->TASKS_STOP = 1;
}

void TIMER2_IRQHandler(void)
{
	if (NRF_TIMER2->EVENTS_COMPARE[0] == 1)
	{
		m_is_timer_send = true;
		NRF_TIMER2->TASKS_STOP = 1;
	  NRF_TIMER2->TASKS_START = 0;
		NRF_TIMER2->EVENTS_COMPARE[0] = 0;
	}
}