/******************************************************************************/
/*! \file sh_gpio.c
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

void sh_gpio_init()
{
    nrf_drv_gpiote_init();

    nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_TOGGLE(true);
    in_config.pull = NRF_GPIO_PIN_PULLUP;

    nrf_drv_gpiote_in_init(PIN_IN, &in_config, sh_ble_event_handler);

   	nrf_drv_gpiote_in_event_enable(PIN_IN, true);
}