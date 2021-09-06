#ifndef __PROJECT_H
#define __PROJECT_H

#include "UserSettings.h"
#include <LCDWIKI_GUI.h> //Core graphics library
#include <LCDWIKI_KBV.h> //Hardware-specific library
#include <LCDWIKI_TOUCH.h> //touch screen library
#include <SoftSPIB.h>
#include <SRAM_23LC.h>
#include <EEPROM_CAT25.h>
#include <SPI.h>
#include <Ethernet2.h>
#include <EthernetUdp2.h>
#include <utility/w5500.h>

#include "saric_utils.h"
#include <PID_v1.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <avr/wdt.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "RF24Network.h"
#include "RF24Mesh.h"
#include "printf.h"

#include <avr/wdt.h>
#include <avr/boot.h>
#include <RTClib.h>
#include <EEPROM.h>
#include <Temperature_LM75_Derived.h>
#include "saric_thermostat.h"
#include "saric_tds_function.h"
#include "saric_mqtt_network.h"
#include "saric_nrf.h"
#include "saric_ha2d.h"

#include <avr/pgmspace.h>



#define ETH_CS 15
#define ETH_RST 6
#define STORAGE_MOSI  24
#define STORAGE_MISO  26
#define STORAGE_CLK 25
#define STORAGE_RAM_CS  22
#define STORAGE_EEPROM_CS  23
#define PIEZO 12

#define NRF_CS 2
#define NRF_CE 3
#define NRF_INT 5

#define LED 8
#define JUMPER 14


#define A_PROUD 3
#define A_TRIV 6
#define A_PETV 5
#define A_DVANACTV 4
#define A_LIGHT 7


#define CONST_PREVOD_PETV 8
#define CONST_PREVOD_TRIV 5
#define CONST_PREVOD_DVANACTV 59
#define CONST_PREVOD_PROUD 1


#define MAX_TEMP_BUFFER 16


#define SELFTEST_ERR_RTC 0
#define SELFTEST_ERR_NTP 1
#define SELFTEST_ERR_ONEWIRE 2
#define SELFTEST_NRF_SCAN 4
#define SELFTEST_MQTT_LINK 5
#define SELFTEST_ETH_LINK 6
#define SELFTEST_RESTART_NEEDED 7

#define DISPLAY_MODE_AUTO_BRIGHTNESS 7
#define DISPLAY_MODE_AUTO_SHUTDOWN_DISPLAY 6



//#define REDRAW_FORCE 3
#define REDRAW_EVERY_500MS 0
#define REDRAW_EXTERNAL_EVENT 1
#define REDRAW_EVERY_10SEC 2

#define REDRAW_CLASS_SHOW_TIME (1<<REDRAW_FORCE | 1<<REDRAW_EVERY_500MS)

#define REDRAW_PROGRAM_BUTTON (1<<REDRAW_EXTERNAL_EVENT| 1<<REDRAW_FORCE)
#define REDRAW_CLASS_SHOW (1<<REDRAW_FORCE | 1<< REDRAW_EVERY_10SEC)


#define REDRAW_BUTTON (1<<REDRAW_FORCE)

#define REDRAW_FORCE 4
#define REDRAW_CLASS_0 0
#define REDRAW_CLASS_1 1
#define REDRAW_CLASS_2 2
#define REDRAW_CLASS_3 3

#define REDRAW_ONCE 5

#define BUTTON_ACTIVE 1
#define BUTTON_NO_ACTIVE 0
#define BUTTON_NO_SHOW 2
#define BUTTON_INACTIVE 3

/// null fake funkce ///
uint8_t returnnullfceargs(uint16_t args1, uint16_t args2, uint8_t args3);
void nullfceargs(uint16_t args1, uint16_t args2, uint8_t args3);
void nullfce(uint16_t args1, uint16_t args2, uint8_t args3);

/// primitivni funkce
void MenuHistoryNextMenu(uint16_t id, uint16_t args1, uint8_t args3);
void MenuHistoryPrevMenu(uint16_t args1, uint16_t args2, uint8_t args3);

uint8_t button_redraw(uint8_t args1, uint8_t args2);
uint8_t menu_redraw_time05s(uint16_t args1, uint16_t args2, uint8_t args3);



uint8_t menu_redraw_change_term_mode(uint16_t args1, uint16_t args2, uint8_t args3);
uint8_t menu_redraw_update_temp(uint16_t args1, uint16_t args2, uint8_t args3);
uint8_t menu_redraw_slider(uint16_t args1, uint16_t args2, uint8_t args3);
uint8_t menu_redraw_date(uint16_t args1, uint16_t args2, uint8_t args3);


void display_element_show_date_1(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint16_t args1, uint8_t args2, char *text);
void display_element_show_link_status(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint16_t args1, uint8_t args2, char *text);

void display_element_dialog_show_text(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint16_t args1, uint8_t args2, char *text);
void display_element_dialog_set_variable(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint16_t idx, uint8_t args2, char *text);
void display_element_vertical_slider(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint16_t args1, uint8_t args2, char *text);

void display_element_default_ring_set_temp(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint8_t args1, uint8_t args2, char *text);


void display_element_rectangle(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint16_t args1, uint8_t args2, char *text);
void display_element_fill_rectangle(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint16_t args1, uint8_t args2, char *text);


void display_element_dialog_set_string(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint16_t args1, uint8_t args2, char *text);

void display_function_set_variable(float now, float min, float max, float step, uint8_t args, uint8_t number_type, uint8_t update_now, uint8_t idx, fptr_save_function save_function);


float display_function_get_variable(uint8_t idx, uint8_t args2, uint8_t args3);
void display_function_set_variable_plus(uint16_t idx, uint16_t args2, uint8_t args3);
void display_function_set_variable_minus(uint16_t idx, uint16_t args2, uint8_t args3);














void click_rtds_add_sensor(uint16_t args1, uint16_t idx, uint8_t args3);


void display_function_vertical_slider_inc(uint16_t args1, uint16_t args2, uint8_t args3);
void display_function_vertical_slider_dec(uint16_t args1, uint16_t args2, uint8_t args3);
void display_function_vertical_slider_get(uint8_t *start, uint8_t *stop);











void click_dialog_button_yes(uint16_t args1, uint16_t args2, uint8_t args3);

void click_keyboard_type(uint16_t args1, uint16_t args2, uint8_t args3);






void display_menu_rtds_update_name(uint16_t args1, uint16_t args2, uint8_t args3);

void dialog_set_string_button_click(uint16_t args1, uint16_t args2, uint8_t args3);
void dialog_set_variable_button_click(uint16_t args1, uint16_t args2, uint8_t args3);

void display_element_get_string(char *str);
void display_element_set_string(char *str, uint8_t max_length, uint8_t idx);
void display_element_set_string_del_char(uint16_t args1, uint16_t idx, uint8_t args3);

uint8_t get_function_keyboard_number_max_keys(uint16_t args1, uint16_t args2, uint8_t args3);
void click_keyboard_number(uint16_t args1, uint16_t args2, uint8_t args3);
void get_function_keyboard_number_char(uint8_t args1, uint8_t args2, uint8_t args3, char *line1, char *line2);


void get_function_keyboard_alfa_char(uint8_t args1, uint8_t args2, uint8_t args3, char *line1, char *line2);
void click_keyboard_alfa(uint16_t args1, uint16_t args2, uint8_t args3);
uint8_t get_function_keyboard_alfa_max_keys(uint16_t args1, uint16_t args2, uint8_t args3);





/***************/
/*
   primitivni funkce pro vyber programu k regulatoru
*/
uint8_t button_get_show_default_ring_program_active(uint16_t args1, uint16_t args2, uint8_t args3);
uint8_t button_get_show_default_ring_program_max_items(uint16_t args1, uint16_t args2, uint8_t args3);
void button_click_set_show_default_ring_program(uint16_t args1, uint16_t args2, uint8_t args3);
void button_get_show_default_ring_program(uint8_t args1, uint8_t args2, uint8_t args3, char *line1, char *line2);
void button_click_deassociate_default_ring_program_for_term(uint16_t args1, uint16_t args2, uint8_t args3);
//////////////////////////



void button_get_default_ring_term_mode_labels(uint8_t args1, uint8_t args2, uint8_t args3, char *line1, char *line2);












/* primitivni funkce nastavovani regulatoru */



/// pomocna funkce pro nastaveni menu, pokud je splnena podminka ze je aktivni default ring povolim menu
void button_click_nastaveni_ring_screen(uint16_t args1, uint16_t args2, uint8_t args3);

////////////



/* primitivni pomocne funkce */
uint8_t display_enable_show(uint16_t args1, uint16_t args2, uint8_t args3);
uint8_t get_function_return_args_1(uint16_t args1, uint16_t args2, uint8_t args3);

/////////////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

const char new_text_show_all_temp[] PROGMEM = "Zobrazeni vsech teplot";

const char new_text_zapnuto[] PROGMEM = "Zapnoto";
const char new_text_vypnuto[] PROGMEM = "Vypnuto";

const char seznam_server[] PROGMEM = "www.seznam.cz";

const char new_text_nastaveni_system_full[] PROGMEM = "Nastaveni systemu";
const char new_text_nastaveni_system[] PROGMEM = "N. systemu";

const char new_text_nastaveni_periferii_full[] PROGMEM = "Nastaveni periferii";
const char new_text_nastaveni_periferie[] PROGMEM = "N. periferii";

const char new_text_nastaveni_zobrazeni_full[] PROGMEM = "Nastaveni zobrazeni";
const char new_text_nastaveni_zobrazeni[] PROGMEM = "N. zobrazeni";

const char new_text_nastaveni_regulatoru_full[] PROGMEM = "Nastaveni regulatoru";
const char new_text_nastaveni_regulatoru[] PROGMEM = "N. regulatoru";

const char new_nastaveni_virtual_output[] PROGMEM = "Vystupni cleny";

const char new_text_nastaveni_casu[] PROGMEM = "Nastaveni casu";
const char new_text_o_zarizeni[] PROGMEM = "O zarizeni";

const char new_nastaveni_mqtt_topic[] PROGMEM = "MQTT topic";
const char new_nastaveni_nrf_input[] PROGMEM = "NRF vstup";

const char new_text_scan_onewire_bus[] PROGMEM = "Scan sbernice";

const char new_text_nastaveni_reload[] PROGMEM = "Restart zarizeni";

const char new_text_ok[] PROGMEM = "OK";

const char new_text_ok_ntp_time[] PROGMEM = "Aktualizace NTP OK";
const char new_text_error_ntp_time[] PROGMEM = "Aktualizace NTP ERROR";

const char new_text_nastaveni_mqtt_pripojeni[] PROGMEM = "Nastaveni MQTT";
const char new_text_mqtt_broker[] PROGMEM = "MQTT server";
const char new_text_mqtt_user[] PROGMEM = "MQTT uzivatel";
const char new_text_mqtt_check[] PROGMEM = "MQTT overit spojeni";
const char new_text_mqtt_pass[] PROGMEM = "MQTT heslo";


const char new_text_jas_display[] PROGMEM = "Nastaveni jasu displaye";


const char new_text_device_nazev[] PROGMEM = "Nazev zarizeni";
const char new_text_device_ip[] PROGMEM = "IP adresa";
const char new_text_device_mask[] PROGMEM = "IP maska";
const char new_text_device_gw[] PROGMEM = "Vychozi brana";
const char new_text_device_dns[] PROGMEM = "DNS server";
const char new_text_device_mac[] PROGMEM = "MAC adresa";

const char new_text_set_network_ip[] PROGMEM = "Nastaveni IP";
const char new_text_set_network_mask[] PROGMEM = "Nastaveni masky site";
const char new_text_set_network_gw[] PROGMEM = "Nastaveni vychozi brany";
const char new_text_set_network_dns[] PROGMEM = "Nastaveni DNS serveru";
const char new_text_set_network_mac[] PROGMEM = "Nastaveni MAC adresy";
const char new_text_set_network_name[] PROGMEM = "Nastaveni nazvu zarizeni";

const char new_text_set_network_test_connection[] PROGMEM = "Test pripojeni";
const char new_text_ntp_set_server[] PROGMEM = "Nastaveni NTP serveru";
const char new_text_time_set_offset[] PROGMEM = "NTP posunuti casu";
const char new_text_time_set_time_manualy[] PROGMEM = "Rucni nastaveni casu";
const char new_text_time_set_date_manualy[] PROGMEM = "Rucni nastaveni datumu";


const char new_text_ok_connect_seznam[] PROGMEM = "Konektivita OK";
const char new_text_err_connect_seznam[] PROGMEM = "Konektivita ERROR";

const char new_text_switch_brightness_automode[] PROGMEM = "Automaticky jas";
const char new_text_jas_display_automat[] PROGMEM = "Automaticke rizeni";
const char new_text_jas_display_manual[] PROGMEM =  "Manualni rizeni ";
const char new_text_auto_display_shutdown[] PROGMEM = "Automaticke vypnuti displaye";


const char new_text_input_volt[] PROGMEM = "Vstupni napeti ";
const char new_text_pet_volt[] PROGMEM = "Interni napeti 5.0 ";
const char new_text_tritri_volt[] PROGMEM = "Interni napeti 3.3 ";
const char new_text_consume_ampere[] PROGMEM = "Odber proudu: ";
const char new_text_internal_temp[] PROGMEM = "Interni teplota: ";
const char new_text_selfcheck_test[] PROGMEM = "Selfcheck: ";
const char new_text_mqtt_send[] PROGMEM = "MQTT odeslanych: ";
const char new_text_mqtt_receive[] PROGMEM = "MQTT prijatych: ";
const char new_text_mqtt_processed[] PROGMEM = "MQTT zpracovanych: ";
const char new_text_mqtt_error[] PROGMEM = "MQTT v chybe: ";
const char new_text_date_error[] PROGMEM = "--.--.----";

const char new_text_rtds_type_temp[] PROGMEM = "Typ: vzdalena teplota";

const char new_text_slash_rtds_slash[] PROGMEM = "/rtds/";
const char new_text_slash_rtds_control_list[] PROGMEM = "/rtds-control/list";


const char new_text_set_time_program[] PROGMEM = "Casove plany";
const char new_text_novy_time_plan[] PROGMEM = "Novy plan";

const char new_text_regulator[] PROGMEM = "Regulator: ";

const char new_text_termostat_statistika[] PROGMEM = "Statistika regulatoru";
const char new_text_regulator_menu[] PROGMEM = "Menu regulatoru";
const char new_text_no_default_ring[] PROGMEM = "Zadny vychozi regulator";


const char text_rf_mesh_send_total[] PROGMEM = "mesh/send";
const char text_rf_mesh_receive_total[] PROGMEM = "mesh/receive";
const char text_rf_mesh_process_total[] PROGMEM = "mesh/process";
const char text_mesh_id[] PROGMEM = "mesh/id";
const char text_mesh_neighbours[] PROGMEM = "mesh/neighbours";
const char text_mesh_neighbour[] PROGMEM = "mesh/neighbour";
const char text_rf_channel[] PROGMEM = "rf/channel";
const char text_rf_power[] PROGMEM = "rf/power";


const char text_mesh_nrf_store[] PROGMEM = "nrf/device/status";
const char text_mesh_nrf_meas_store[] PROGMEM = "nrf/measurement";


const char mesh_device_nrf_uptime[] PROGMEM = "NRF_UPTIME";
const char mesh_device_nrf_send[] PROGMEM = "NRF_SEND";
const char mesh_device_nrf_recv[] PROGMEM = "NRF_RECV";
const char mesh_device_nrf_err[] PROGMEM = "NRF_ERR";
const char mesh_device_nrf_renew[] PROGMEM = "NRF_RENEW";
const char mesh_device_nrf_name[] PROGMEM = "NRF_NAME";

const char mesh_device_nrf_uptime_x[] PROGMEM = "uptime";
const char mesh_device_nrf_send_x[] PROGMEM = "send";
const char mesh_device_nrf_recv_x[] PROGMEM = "recv";
const char mesh_device_nrf_err_x[] PROGMEM = "send_err";
const char mesh_device_nrf_renew_x[] PROGMEM = "mesh_renew";
const char text_name[] PROGMEM = "name";
const char mesh_device_nrf_last_seen_x[] PROGMEM = "last_seen";

const char text_value[] PROGMEM = "value";
const char text_type[] PROGMEM = "type";
const char text_device[] PROGMEM = "device";

const char text_crc_error[] PROGMEM = "CRC chyby";

const char global_time_set[] PROGMEM = "global/time/set";
const char global_time_ntp[] PROGMEM = "global/time/ntp";
const char global_time_offset[] PROGMEM = "global/time/ntp_offset";
const char thermctl_header_in[] PROGMEM  = "/thermctl-in/";
const char thermctl_header_out[] PROGMEM  = "/thermctl-out/";

const char termbig_header_in[] PROGMEM  = "/termbig-in/";
const char termbig_header_out[] PROGMEM  = "/termbig-out/";

const char lightctl_header_in[] PROGMEM  = "/lightctl-in/";
const char lightctl_header_out[] PROGMEM  = "/lightctl-out/";

const char thermctl_subscribe[] PROGMEM = "/ctl/thermctl/subscribe";
const char termbig_subscribe[] PROGMEM = "/ctl/termbig/subscribe";

const char termbig_virtual_output[] PROGMEM = "/termbig-out/virtual-output/";

const char text_mqtt_connect[] PROGMEM = " Spojeno ";
const char text_mqtt_disconnect[] PROGMEM = "Nespojeno";

const char text_find_new_onewire_devices[] PROGMEM = "Nalezeno: %d novych 1Wire";
const char text_build_version[] PROGMEM = "build version: %d.%02d.%02d %02d:%02d:%02d";
const char text_volt_consume_light_info[] PROGMEM = "3.3V=%s; 5.0V=%s; IN=%s; I=%s";
/***************************************************************/
/*
        spolecne display funkce
*/

const Element_Function_1 f_show_rectangle_decorate PROGMEM = {.x = 220, .y = 20, .args = 0, .fnt_coordinate_xy = display_element_rectangle,  .size_x = 240, .size_y = 240, .redraw_class = REDRAW_BUTTON, .onclick = nullfce, .enable_show = display_enable_show, .name = char_NULL,};


const Element_Function_1 f_show_date PROGMEM = {
  .x = 280,
  .y = 290,
  .args = 0,
  .fnt_coordinate_xy = display_element_show_date_1,
  .size_x = 0,
  .size_y = 0,
  .redraw_class = (1 << REDRAW_CLASS_3 | 1 << REDRAW_FORCE),
  .onclick = nullfce,
  .enable_show = display_enable_show,
  .name = char_NULL,
};

const Element_Function_1 f_show_link_status PROGMEM = {
  .x = 10,
  .y = 290,
  .args = 0,
  .fnt_coordinate_xy = display_element_show_link_status,
  .size_x = 0,
  .size_y = 0,
  .redraw_class = (1 << REDRAW_CLASS_0 | 1 << REDRAW_FORCE),
  .onclick = nullfce,
  .enable_show = display_enable_show,
  .name = char_NULL,
};

const Element_Function_1 f_dialog_show_text PROGMEM = {
  .x = 20,
  .y = 35,
  .args = 0,
  .fnt_coordinate_xy = display_element_dialog_show_text,
  .size_x = 0,
  .size_y = 0,
  .redraw_class = REDRAW_BUTTON,
  .onclick = nullfce,
  .enable_show = display_enable_show,
  .name = char_NULL,
};

const Element_Button_1 button_back PROGMEM = {
  .name = button_zpet,
  .x = 10,
  .y = 220,
  .size_x = 190,
  .size_y = 40,
  .font_size = 2,
  .args = 0,
  .onclick = MenuHistoryPrevMenu,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};








#endif
