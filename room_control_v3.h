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
#define CONST_PREVOD_DVANACTV 57
#define CONST_PREVOD_PROUD 1





#define SELFTEST_ERR_RTC 0
#define SELFTEST_ERR_NTP 1
#define SELFTEST_ERR_ONEWIRE 2
#define SELFTEST_NRF_SCAN 4
#define SELFTEST_MQTT_LINK 5
#define SELFTEST_ETH_LINK 6
#define SELFTEST_RESTART_NEEDED 7

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


void display_element_show_time_1(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint8_t args1, uint8_t args2, char *text);
void display_element_show_temp_1(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint8_t args1, uint8_t args2, char *text);
void display_element_show_date_1(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint8_t args1, uint8_t args2, char *text);
void display_element_show_tds_info_static(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint8_t args1, uint8_t args2, char *text);
void display_element_show_tds_info_dynamics(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint8_t args1, uint8_t args2, char *text);
void display_element_show_rtds_info_dynamics(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint8_t args1, uint8_t args2, char *text);
void display_element_dialog_show_text(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint8_t args1, uint8_t args2, char *text);
void display_element_dialog_set_variable(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint8_t idx, uint8_t args2, char *text);
void display_element_vertical_slider(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint8_t args1, uint8_t args2, char *text);
void display_element_dialog_default_ring(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint8_t args1, uint8_t args2, char *text);
void display_element_default_ring_set_temp(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint8_t args1, uint8_t args2, char *text);


void display_element_rectangle(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint8_t args1, uint8_t args2, char *text);


void display_element_dialog_set_string(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint8_t args1, uint8_t args2, char *text);

void display_function_set_variable(float now, float min, float max, float step, uint8_t args, uint8_t number_type, uint8_t update_now, uint8_t idx, fptr_save_function save_function);


float display_function_get_variable(uint8_t idx, uint8_t args2, uint8_t args3);
void display_function_set_variable_plus(uint16_t idx, uint16_t args2, uint8_t args3);
void display_function_set_variable_minus(uint16_t idx, uint16_t args2, uint8_t args3);












void click_thermostat_set_ring_via_dialog(uint16_t args1, uint16_t args2, uint8_t args3);

void click_rtds_add_sensor(uint16_t args1, uint16_t idx, uint8_t args3);


void display_function_vertical_slider_inc(uint16_t args1, uint16_t args2, uint8_t args3);
void display_function_vertical_slider_dec(uint16_t args1, uint16_t args2, uint8_t args3);
void display_function_vertical_slider_get(uint8_t *start, uint8_t *stop);





void click_rtds_setting_sensor(uint16_t args1, uint16_t args2, uint8_t loop_idx);

void click_tds_deassociate_onewire(uint16_t args1, uint16_t idx, uint8_t args3);
void click_rtds_deassociate_onewire(uint16_t args1, uint16_t idx, uint8_t args3);
void click_rtds_subscribe(uint16_t args1, uint16_t idx, uint8_t args3);

uint8_t get_function_rtds_active_items(uint16_t args1, uint16_t idx, uint8_t args3);
uint8_t get_function_rtds_last_index_for_menu(uint16_t args1, uint16_t args2, uint8_t args3);
void get_function_rtds_text_button(uint8_t args1, uint8_t args2, uint8_t args3, char *line1, char *line2);





void click_dialog_button_yes(uint16_t args1, uint16_t args2, uint8_t args3);

void click_keyboard_type(uint16_t args1, uint16_t args2, uint8_t args3);




void display_menu_tds_set_offset(uint16_t args1, uint16_t args2, uint8_t args3);
void display_menu_tds_set_period(uint16_t args1, uint16_t args2, uint8_t args3);
void display_menu_tds_set_name(uint16_t args1, uint16_t args2, uint8_t args3);

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

void click_select_default_temp(uint8_t args1, uint8_t args2);



void button_get_show_default_temp(uint8_t args1, uint8_t args2, uint8_t args3, char *line1, char *line2);
void button_click_set_show_default_temp(uint16_t args1, uint16_t args2, uint8_t args3);
uint8_t button_get_show_default_temp_max_items(uint16_t args1, uint16_t args2, uint8_t args3);
uint8_t button_get_show_default_temp_active(uint16_t args1, uint16_t args2, uint8_t args3);



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
uint8_t button_get_term_ring_is_selected(uint16_t args1, uint16_t args2, uint8_t args3);
void button_get_default_ring_labels(uint8_t args1, uint8_t args2, uint8_t args3, char *line1, char *line2);


void button_change_default_ring_labels_in_dialog(uint8_t args1, uint8_t args2, uint8_t args3, char *line1, char *line2);
void button_click_set_new_default_ring_in_dialog(uint16_t args1, uint16_t args2, uint8_t args3);


uint8_t button_get_show_default_ring_input_active(uint16_t args1, uint16_t args2, uint8_t args3);
void button_click_set_show_default_ring_input(uint16_t args1, uint16_t args2, uint8_t args3);

void button_click_deassociate_default_ring_input_sensor_for_term(uint16_t args1, uint16_t args2, uint8_t args3);
void button_click_deactivate_term_ring(uint16_t args1, uint16_t args2, uint8_t args3);

/* primitivni funkce nastavovani regulatoru */
void button_click_set_term_ring_name_via_keyboard(uint16_t args1, uint16_t args2, uint8_t args3);


/// pomocna funkce pro nastaveni menu, pokud je splnena podminka ze je aktivni default ring povolim menu
void button_click_nastaveni_ring_screen(uint16_t args1, uint16_t args2, uint8_t args3);

////////////
/* primitivni pomocne funkce pro nastavovani manualniho modu topi/chladi */
void button_click_set_term_heat_or_cool(uint16_t args1, uint16_t args2, uint8_t args3);
uint8_t button_get_term_heat_or_cool(uint16_t args1, uint16_t args2, uint8_t args3);
uint8_t display_enable_show_term_mode_man(uint16_t args1, uint16_t args2, uint8_t args3);
uint8_t display_enable_show_term_mode_prog(uint16_t args1, uint16_t args2, uint8_t args3);


/* primitivni pomocne funkce */
uint8_t display_enable_show(uint16_t args1, uint16_t args2, uint8_t args3);
uint8_t get_function_return_args_1(uint16_t args1, uint16_t args2, uint8_t args3);
uint8_t preload_regulator_menu(uint16_t args1, uint16_t args2, uint8_t args3);
uint8_t preload_pid_menu(uint16_t args1, uint16_t args2, uint8_t args3);
/////////////////////////////////////////////////////////////////////////////////////////



/* primitivni funkce obsluha tlacitek dialogu vyberu modu regurlatoru */
void button_click_term_set_mode_via_dialog(uint16_t args1, uint16_t args2, uint8_t args3);
///
uint8_t button_get_term_mode_is_selected(uint16_t args1, uint16_t args2, uint8_t args3);
void button_click_term_set_mode(uint16_t args1, uint16_t args2, uint8_t args3);
void button_get_term_mode_labels(uint8_t args1, uint8_t args2, uint8_t args3, char *line1, char *line2);
////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////


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
  .redraw_class = REDRAW_CLASS_SHOW,
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
