#include "main.h"

#define DEVICE_NAME                     													"SHelmet"                         					 										/**< Name of device. Will be included in the advertising data. */
#define APP_ADV_INTERVAL_FAST           									0x0028                                  													/**< Fast advertising interval (in units of 0.625 ms. This value corresponds to 25 ms.). */
#define APP_ADV_INTERVAL_SLOW           								0x0C80                                 													/**< Slow advertising interval (in units of 0.625 ms. This value corresponds to 2 seconds). */

#define APP_BLE_OBSERVER_PRIO           							  3                                       															/**< Application's BLE observer priority. You shouldn't need to modify this value. */
#define APP_BLE_CONN_CFG_TAG            								1		                                       													/**< A tag identifying the SoftDevice BLE configuration. */

#define MIN_CONN_INTERVAL               											MSEC_TO_UNITS(500, UNIT_1_25_MS)        		/**< Minimum acceptable connection interval (0.5 seconds).  */
#define MAX_CONN_INTERVAL               											MSEC_TO_UNITS(1000, UNIT_1_25_MS)      		/**< Maximum acceptable connection interval (1 second). */
#define SLAVE_LATENCY                  						 							0                                       															/**< Slave latency. */
#define CONN_SUP_TIMEOUT                											MSEC_TO_UNITS(4000, UNIT_10_MS)        		 	/**< Connection supervisory timeout (4 seconds). */

#define APP_ADV_INTERVAL                											40                                      														/**< The advertising interval (in units of 0.625 ms. This value corresponds to 25 ms). */
#define APP_ADV_DURATION                											18000                                   													/**< The advertising duration (180 seconds) in units of 10 milliseconds. */

#define FIRST_CONN_PARAMS_UPDATE_DELAY  			APP_TIMER_TICKS(5000)                   							/**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY  				APP_TIMER_TICKS(30000)                  						/**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT    			3                                       															/**< Number of attempts before giving up the connection parameter negotiation. */

#define SEC_PARAM_BOND                  											1                                       															/**< Perform bonding. */
#define SEC_PARAM_MITM                  												0                                       															/**< Man In The Middle protection not required. */
#define SEC_PARAM_LESC                  											0                                       															/**< LE Secure Connections not enabled. */
#define SEC_PARAM_KEYPRESS              									0                                      															/**< Keypress notifications not enabled. */
#define SEC_PARAM_IO_CAPABILITIES       							  BLE_GAP_IO_CAPS_NONE                    				/**< No I/O capabilities. */
#define SEC_PARAM_OOB                   											0                                       															/**< Out Of Band data not available. */
#define SEC_PARAM_MIN_KEY_SIZE          								7                                       															/**< Minimum encryption key size. */
#define SEC_PARAM_MAX_KEY_SIZE          								16                                      														/**< Maximum encryption key size. */

#define DEAD_BEEF                       														0xDEADBEEF                              										/**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

static volatile bool m_is_ias_present = false;          																																								/**< Variable to indicate whether the immediate alert service has been discovered at the connected peer. */
static volatile bool m_is_alert_signalled = false;      																																								/**< Variable to indicate whether a high alert has been signalled to the peer. */
static volatile bool m_is_no_alert_signalled = true;  																																								/**< Variable to indicate whether a mild alert has been signalled to the peer. */

/**@brief Function for the BLE functions initialization.
 *
 * @details 
 *          
 */
extern void sh_ble_init(void);
	
/**@brief Function for the GAP initialization.
 *
 * @details This function sets up all the necessary GAP (Generic Access Profile) parameters of the
 *          device including the device name, appearance, and the preferred connection parameters.
 */
extern void sh_gap_params_init(void);

/**@brief Function for initializing the GATT module.
 */
extern void sh_gatt_init(void);

/**@brief Function for handling the Application's BLE Stack events.
 *
 * @details This function will be called for advertising events which are passed to the application.
 *
 * @param[in] ble_adv_evt  Advertising event.
 */
extern void sh_on_adv_evt(ble_adv_evt_t ble_adv_evt);

/**@brief Function for initializing the Advertising functionality.
 *
 * @details Encodes the required advertising data and passes it to the stack.
 *          Also builds a structure to be passed to the stack when starting advertising.
 */
extern void sh_advertising_init(void);

/**@brief Function for handling a Connection Parameters error.
 *
 * @param[in] nrf_error  Error code containing information about what went wrong.
 */
extern void conn_params_error_handler(uint32_t nrf_error);

/**@brief Function for initializing the Connection Parameters module.
 */
extern void sh_conn_params_init(void);

/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
extern void sh_ble_stack_init(void);

/**@brief Function for the Peer Manager initialization.
 */
extern void sh_peer_manager_init(void);

/**@brief Function for initializing the Queued Write module.
 */
extern void qwr_init(void);

/**@brief Function for initializing the immediate alert service client.
 *
 * @details This will initialize the client side functionality of the Find Me profile.
 */
extern void ias_client_init(void);

/**@brief Function for initializing the services that will be used by the application.
 */
extern void sh_services_init(void);

/** @brief Database discovery module initialization.
 */
extern void sh_db_discovery_init(void);

/**@brief Callback function for asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] line_num   Line number of the failing ASSERT call.
 * @param[in] file_name  File name of the failing ASSERT call.
 */
extern void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name);

/**@brief Function for handling Service errors.
 *
 * @details A pointer to this function will be passed to each service which may need to inform the
 *          application about an error.
 *
 * @param[in] nrf_error   Error code containing information about what went wrong.
 */
extern void service_error_handler(uint32_t nrf_error);

/**@brief Function for handling Peer Manager events.
 *
 * @param[in] p_evt  Peer Manager event.
 */
extern void pm_evt_handler(pm_evt_t const * p_evt);

/**@brief Function for handling database discovery events.
 *
 * @details This function is callback function to handle events from the database discovery module.
 *          Depending on the UUIDs that are discovered, this function should forward the events
 *          to their respective services.
 *
 * @param[in] p_event  Pointer to the database discovery event.
 */
extern void db_disc_handler(ble_db_discovery_evt_t * p_evt);

/**@brief Function for putting the chip into sleep mode.
 *
 * @note This function will not return.
 */
extern void sleep_mode_enter(void);

/**@brief Function for handling Queued Write Module errors.
 *
 * @details A pointer to this function will be passed to each service which may need to inform the
 *          application about an error.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
extern void nrf_qwr_error_handler(uint32_t nrf_error);

/**@brief Function for handling IAS Client events.
 *
 * @details This function will be called for all IAS Client events which are passed to the
 *          application.
 *
 * @param[in] p_ias_c  IAS Client structure.
 * @param[in] p_evt    Event received.
 */
extern void on_ias_c_evt(ble_ias_c_t * p_ias_c, ble_ias_c_evt_t * p_evt);

/**@brief Function for handling BLE events.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 * @param[in]   p_context   Unused.
 */
extern void ble_evt_handler(ble_evt_t const * p_ble_evt, void * p_context);

/**@brief Function for starting advertising.
 */
extern void sh_advertising_start(bool erase_bonds);

/**@brief Function for handling events from the BSP module.
 *
 * @param[in]   event   Event generated when button is pressed.
 */
extern void sh_ble_event_handler();

