/******************************************************************************/
/*! \file sh_ble.c
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

BLE_IAS_C_DEF(m_ias_c);                                 									/**< Structure used to identify the client to the Immediate Alert Service at peer. */
NRF_BLE_GATT_DEF(m_gatt);                               							/**< GATT module instance. */
NRF_BLE_QWR_DEF(m_qwr);                                 						/**< Context for the Queued Write module.*/
BLE_ADVERTISING_DEF(m_advertising);                     			/**< Advertising module instance. */
BLE_DB_DISCOVERY_DEF(m_ble_db_discovery);            /**< DB discovery module instance. */
NRF_BLE_GQ_DEF(m_ble_gatt_queue,                        			/**< BLE GATT Queue instance. */
               NRF_SDH_BLE_PERIPHERAL_LINK_COUNT,
               NRF_BLE_GQ_QUEUE_SIZE);

ble_uuid_t m_adv_uuids[] =                       														/**< Universally unique service identifiers. */
{
    {BLE_UUID_IMMEDIATE_ALERT_SERVICE, BLE_UUID_TYPE_BLE}
};

void sh_ble_init(void)
{
    bool erase_bonds = true;
	
    sh_ble_stack_init();
    sh_gap_params_init();
    sh_gatt_init();
    sh_advertising_init();
    sh_db_discovery_init();
    sh_services_init();
    sh_conn_params_init();
    sh_peer_manager_init();
		
	  // Start execution.
    sh_advertising_start(erase_bonds);
}

void sh_gap_params_init(void)
{
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *)DEVICE_NAME,
                                          strlen(DEVICE_NAME));

    sd_ble_gap_appearance_set(BLE_APPEARANCE_GENERIC_KEYRING);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    sd_ble_gap_ppcp_set(&gap_conn_params);
}

void sh_gatt_init(void)
{
    nrf_ble_gatt_init(&m_gatt, NULL);
}

void sh_on_adv_evt(ble_adv_evt_t ble_adv_evt)
{
    switch (ble_adv_evt)
    {
        case BLE_ADV_EVT_FAST:
            NRF_LOG_INFO("Fast advertising.");
            bsp_indication_set(BSP_INDICATE_ADVERTISING);
            break; // BLE_ADV_EVT_FAST

        case BLE_ADV_EVT_IDLE:
            sleep_mode_enter();
            break; // BLE_ADV_EVT_IDLE

        default:
            break;
    }
}

void sh_advertising_init(void)
{
    ble_advertising_init_t init;

    // Build and set advertising data.
    memset(&init, 0, sizeof(init));

    init.advdata.name_type               						= BLE_ADVDATA_FULL_NAME;
    init.advdata.include_appearance      			= true;
    init.advdata.flags                   									= BLE_GAP_ADV_FLAGS_LE_ONLY_LIMITED_DISC_MODE;
    init.advdata.uuids_complete.uuid_cnt		= sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
    init.advdata.uuids_complete.p_uuids  		= m_adv_uuids;

    init.config.ble_adv_fast_enabled  					= true;
    init.config.ble_adv_fast_interval 							= APP_ADV_INTERVAL;
    init.config.ble_adv_fast_timeout  						= APP_ADV_DURATION;

    init.evt_handler = sh_on_adv_evt;

    ble_advertising_init(&m_advertising, &init);

    ble_advertising_conn_cfg_tag_set(&m_advertising, APP_BLE_CONN_CFG_TAG);
}

void conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}

void sh_conn_params_init(void)
{
    ble_conn_params_init_t cp_init;

    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params                  						= NULL;
    cp_init.first_conn_params_update_delay 	= FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle    			= BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail             							= true;
    cp_init.evt_handler                    									= NULL;
    cp_init.error_handler                  									= conn_params_error_handler;

    ble_conn_params_init(&cp_init);
}

void sh_ble_stack_init(void)
{
    nrf_sdh_enable_request();

    // Configure the BLE stack using the default settings.
    // Fetch the start address of the application RAM.
    uint32_t ram_start = 0;
    nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);

    // Enable BLE stack.
    nrf_sdh_ble_enable(&ram_start);

    // Register a handler for BLE events.
    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_evt_handler, NULL);
}

void sh_peer_manager_init(void)
{
    ble_gap_sec_params_t sec_param;

    pm_init();

    memset(&sec_param, 0, sizeof(ble_gap_sec_params_t));

    // Security parameters to be used for all security procedures.
    sec_param.bond           = SEC_PARAM_BOND;
    sec_param.mitm           = SEC_PARAM_MITM;
    sec_param.lesc           = SEC_PARAM_LESC;
    sec_param.keypress       = SEC_PARAM_KEYPRESS;
    sec_param.io_caps        = SEC_PARAM_IO_CAPABILITIES;
    sec_param.oob            = SEC_PARAM_OOB;
    sec_param.min_key_size   = SEC_PARAM_MIN_KEY_SIZE;
    sec_param.max_key_size   = SEC_PARAM_MAX_KEY_SIZE;
    sec_param.kdist_own.enc  = 1;
    sec_param.kdist_own.id   = 1;
    sec_param.kdist_peer.enc = 1;
    sec_param.kdist_peer.id  = 1;

    pm_sec_params_set(&sec_param);
    pm_register(pm_evt_handler);
}

void qwr_init(void)
{
    nrf_ble_qwr_init_t qwr_init_obj = {0};

    // Initialize Queued Write Module.
    qwr_init_obj.error_handler = nrf_qwr_error_handler;

    nrf_ble_qwr_init(&m_qwr, &qwr_init_obj);
}

void ias_client_init(void)
{
    ble_ias_c_init_t ias_c_init_obj;

    memset(&ias_c_init_obj, 0, sizeof(ias_c_init_obj));

    ias_c_init_obj.evt_handler   = on_ias_c_evt;
    ias_c_init_obj.error_handler = service_error_handler;
    ias_c_init_obj.p_gatt_queue  = &m_ble_gatt_queue;

    ble_ias_c_init(&m_ias_c, &ias_c_init_obj);
}

void sh_services_init(void)
{
    qwr_init();
    ias_client_init();
}

void sh_db_discovery_init(void)
{
    ble_db_discovery_init_t db_init;

    memset(&db_init, 0, sizeof(ble_db_discovery_init_t));

    db_init.evt_handler  = db_disc_handler;
    db_init.p_gatt_queue = &m_ble_gatt_queue;

     ble_db_discovery_init(&db_init);
}

void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}

void service_error_handler(uint32_t nrf_error)
{

}

void pm_evt_handler(pm_evt_t const * p_evt)
{
    pm_handler_on_pm_evt(p_evt);
    pm_handler_flash_clean(p_evt);

    switch (p_evt->evt_id)
    {
        case PM_EVT_PEERS_DELETE_SUCCEEDED:
            sh_advertising_start(false);
            break;

        default:
            break;
    }
}

void db_disc_handler(ble_db_discovery_evt_t * p_evt)
{
    ble_ias_c_on_db_disc_evt(&m_ias_c, p_evt);
}

void sleep_mode_enter(void)
{
    bsp_indication_set(BSP_INDICATE_IDLE);

    // Prepare wakeup buttons.
    bsp_btn_ble_sleep_mode_prepare();
	
    // Go to system-off mode (this function will not return; wakeup will cause a reset).
    sd_power_system_off();
}

void nrf_qwr_error_handler(uint32_t nrf_error)
{

}

void on_ias_c_evt(ble_ias_c_t * p_ias_c, ble_ias_c_evt_t * p_evt)
{
    switch (p_evt->evt_type)
    {
        case BLE_IAS_C_EVT_DISCOVERY_COMPLETE:
            // IAS is found on peer. The Find Me Locator functionality of this app will work.
            ble_ias_c_handles_assign(&m_ias_c,
                                                p_evt->conn_handle,
                                                p_evt->alert_level.handle_value);

            m_is_ias_present = true;
            break;

        case BLE_IAS_C_EVT_DISCOVERY_FAILED:
            // IAS is not found on peer. The Find Me Locator functionality of this app will NOT work.
            break;

        case BLE_IAS_C_EVT_DISCONN_COMPLETE:
            // Disable alert buttons
            m_is_ias_present = false;
            break;

        default:
            break;
    }
}

void ble_evt_handler(ble_evt_t const * p_ble_evt, void * p_context)
{
     pm_handler_secure_on_connection(p_ble_evt);

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_DISCONNECTED:
            NRF_LOG_INFO("Disconnected.");
            // LED indication will be changed when advertising starts.
            break;

        case BLE_GAP_EVT_CONNECTED:
        {
            NRF_LOG_INFO("Connected.");
            bsp_indication_set(BSP_INDICATE_CONNECTED);

            // Assign connection handle to the Queued Write module.
            nrf_ble_qwr_conn_handle_assign(&m_qwr, p_ble_evt->evt.gap_evt.conn_handle);

            // Discover peer's services.
            ble_db_discovery_start(&m_ble_db_discovery,
                                              p_ble_evt->evt.gap_evt.conn_handle);
        } break;

        case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
        {
            NRF_LOG_DEBUG("PHY update request.");
            ble_gap_phys_t const phys =
            {
                .rx_phys = BLE_GAP_PHY_AUTO,
                .tx_phys = BLE_GAP_PHY_AUTO,
            };
            sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);

        } break;

        case BLE_GATTC_EVT_TIMEOUT:
            // Disconnect on GATT Client timeout event.
            NRF_LOG_DEBUG("GATT Client Timeout.");
            sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            break;

        case BLE_GATTS_EVT_TIMEOUT:
            // Disconnect on GATT Server timeout event.
            NRF_LOG_DEBUG("GATT Server Timeout.");
            sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            break;

        default:
            // No implementation needed.
            break;
    }
}

void sh_advertising_start(bool erase_bonds)
{
    if (erase_bonds == true){
        pm_peers_delete();
        // Advertising is started by PM_EVT_PEERS_DELETE_SUCEEDED event.
    }
    else
    {
        ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST);
    }
}


void sh_ble_event_handler()
{
    if(m_is_alert_signalled && !m_is_timer_send)
    {
        return;
    }
		else if (m_is_ias_present)
		{
				if (m_is_alert_signalled)
				{
						m_is_timer_send = false;
						ble_ias_c_send_alert_level(&m_ias_c, BLE_CHAR_ALERT_LEVEL_NO_ALERT);
						m_is_alert_signalled = false;
						m_is_no_alert_signalled = true;
				}
				else if (m_is_no_alert_signalled)
				{
						m_is_timer_send = false;
						ble_ias_c_send_alert_level(&m_ias_c, BLE_CHAR_ALERT_LEVEL_ALERT);
						m_is_alert_signalled = true;
						m_is_no_alert_signalled = false;
						NRF_TIMER2->TASKS_STOP = 0;
						NRF_TIMER2->TASKS_START = 1;
				}
    }
}

