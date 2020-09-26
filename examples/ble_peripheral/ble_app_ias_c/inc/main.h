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
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "ble_advertising.h"
#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "ble.h"
#include "ble_err.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "ble_conn_params.h"
#include "nrf_sdh.h"
#include "nrf_sdh_soc.h"
#include "nrf_sdh_ble.h"
#include "app_timer.h"
#include "ble_ias_c.h"
#include "bsp_btn_ble.h"
#include "ble_db_discovery.h"
#include "peer_manager.h"
#include "peer_manager_handler.h"
#include "fds.h"
#include "ble_conn_state.h"
#include "nrf_ble_gatt.h"
#include "nrf_ble_qwr.h"
#include "nrf_pwr_mgmt.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "nrf_drv_timer.h"
#include "bsp.h"
