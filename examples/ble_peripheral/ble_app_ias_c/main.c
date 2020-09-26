/******************************************************************************/
/*! \file main.c
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
#include "main.h"
#include "sh_ble.h"
#include "sh_gpio.h"
#include "sh_timer.h"

int main(void)
{
	  nrf_pwr_mgmt_init();
		sh_gpio_init();
    sh_timer_init();  
		sh_ble_init();

    for (;;)
    {
			if(m_is_timer_send)
			{
			 sh_ble_event_handler(BSP_EVENT_KEY_0);
			}
		}
}

