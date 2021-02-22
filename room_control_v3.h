#ifndef __PROJECT_H
#define __PROJECT_H

#include "UserSettings.h"


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
uint8_t returnnullfceargs(uint16_t args1, uint16_t args2);
void nullfceargs(uint16_t args1, uint16_t args2);
void nullfce(void);

/// primitivni funkce
void MenuHistoryNextMenu(uint8_t id, uint8_t args1);
void MenuHistoryPrevMenu(void);

uint8_t button_redraw(uint8_t args1, uint8_t args2);
uint8_t menu_redraw_time05s(uint8_t args1, uint8_t args2);



uint8_t menu_redraw_change_term_mode(uint8_t args1, uint8_t args2);
uint8_t menu_redraw_update_temp(uint8_t args1, uint8_t args2);


void display_element_show_time_1(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint8_t args1, uint8_t args2);
void display_element_show_temp_1(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint8_t args1, uint8_t args2);
void display_element_show_date_1(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint8_t args1, uint8_t args2);
void display_element_show_tds_info_static(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint8_t args1, uint8_t args2);
void display_element_show_tds_info_dynamics(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint8_t args1, uint8_t args2);
void display_element_show_rtds_info_dynamics(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint8_t args1, uint8_t args2);
void display_element_dialog_show_text(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y);
void display_element_dialog_set_variable(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint8_t idx, uint8_t args2, char *text);
void display_element_vertical_slider(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint8_t args1, uint8_t args2);
void display_element_dialog_default_ring(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint8_t args1, uint8_t args2);
void display_element_default_ring_set_temp(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint8_t args1, uint8_t args2);


void display_element_rectangle(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint8_t args1, uint8_t args2);


void display_element_dialog_set_string(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint8_t args1, uint8_t args2);

void display_function_set_variable(float now, float min, float max, float step, uint8_t args, uint8_t number_type, uint8_t update_now, uint8_t idx, fptr_save_function save_function);


float display_function_get_variable(uint8_t idx, uint8_t args2, uint8_t args3);
void display_function_set_variable_plus(uint8_t idx, uint8_t args2, uint8_t args3);
void display_function_set_variable_minus(uint8_t idx, uint8_t args2, uint8_t args3);








uint8_t button_status_default_ring_term_has_mode(uint8_t args1, uint8_t args2, uint8_t args3);
void button_click_default_term_set_mode(uint8_t args1, uint8_t args2, uint8_t args3);



void click_thermostat_set_ring_via_dialog(uint8_t args1, uint8_t args2, uint8_t args3);

void click_rtds_add_sensor(uint8_t args1, uint8_t args);


void display_function_vertical_slider_inc(uint8_t args1, uint8_t args2);
void display_function_vertical_slider_dec(uint8_t args1, uint8_t args2);
void display_function_vertical_slider_get(uint8_t *start, uint8_t *stop);



void get_function_one_wire_associate_or_setting_text_button(uint8_t args1, uint8_t args2, uint8_t args3, char *line1, char *line2);
uint8_t get_function_one_wire_last_index_for_menu(uint16_t idx, uint16_t args2, uint8_t args3);
void click_tds_associate_or_setting_onewire(uint8_t args1, uint8_t args2, uint8_t idx);

void click_rtds_setting_sensor(uint8_t args1, uint8_t args2, uint8_t loop_idx);

void click_tds_deassociate_onewire(uint8_t args1, uint8_t idx);
void click_rtds_deassociate_onewire(uint8_t args1, uint8_t idx);
void click_rtds_subscribe(uint8_t args1, uint8_t idx);

uint8_t get_function_rtds_active_items(uint8_t args1, uint8_t args2);
uint8_t get_function_rtds_last_index_for_menu(uint16_t args1, uint16_t args2, uint8_t args3);
void get_function_rtds_text_button(uint8_t args1, uint8_t args2, uint8_t args3, char *line1, char *line2);


uint8_t get_function_budik_enabled(uint8_t args);
void get_function_budik_text_state(uint8_t args1, uint8_t args2, uint8_t args3, char *line1, char *line2);


void click_dialog_button_yes(uint8_t args1, uint8_t args2);

void click_keyboard_type(uint8_t args1, uint8_t args2);




void display_menu_tds_set_offset(uint8_t args1, uint8_t args2);
void display_menu_tds_set_period(uint8_t args1, uint8_t args2);
void display_menu_tds_set_name(uint8_t args1, uint8_t args2);

void display_menu_rtds_update_name(uint8_t args1, uint8_t args2);

void dialog_set_string_button_click(uint8_t args1, uint8_t args2, uint8_t args3);
void dialog_set_variable_button_click(uint8_t args1, uint8_t args2, uint8_t args3);

void display_element_get_string(char *str);
void display_element_set_string(char *str, uint8_t max_length, uint8_t idx);
void display_element_set_string_del_char(uint8_t args1, uint8_t args2);

uint8_t get_function_keyboard_number_max_keys(uint16_t args1, uint16_t args2, uint8_t args3);
void click_keyboard_number(uint8_t args1, uint8_t args2, uint8_t args3);
void get_function_keyboard_number_char(uint8_t args1, uint8_t args2, uint8_t args3, char *line1, char *line2);


void get_function_keyboard_alfa_char(uint8_t args1, uint8_t args2, uint8_t args3, char *line1, char *line2);
void click_keyboard_alfa(uint8_t args1, uint8_t args2, uint8_t args3);
uint8_t get_function_keyboard_alfa_max_keys(uint16_t args1, uint16_t args2, uint8_t args3);

void click_select_default_temp(uint8_t args1, uint8_t args2);



void button_get_show_default_temp(uint8_t args1, uint8_t args2, uint8_t args3, char *line1, char *line2);
void button_click_set_show_default_temp(uint8_t args1, uint8_t args2, uint8_t args3);
uint8_t button_get_show_default_temp_max_items(uint16_t args1, uint16_t args2, uint8_t args3);
uint8_t button_get_show_default_temp_active(uint16_t args1, uint16_t args2, uint8_t args3);







void button_get_default_ring_term_mode_labels(uint8_t args1, uint8_t args2, uint8_t args3, char *line1, char *line2);
uint8_t button_get_term_ring_is_selected(uint16_t args1, uint16_t args2, uint8_t args3);
void button_get_default_ring_labels(uint8_t args1, uint8_t args2, uint8_t args3, char *line1, char *line2);


void button_change_default_ring_labels_in_dialog(uint8_t args1, uint8_t args2, uint8_t args3, char *line1, char *line2);
void button_click_set_new_default_ring_in_dialog(uint16_t args1, uint16_t args2, uint8_t args3);


uint8_t button_get_show_default_ring_input_active(uint16_t args1, uint16_t args2, uint8_t args3);
void button_click_set_show_default_ring_input(uint16_t args1, uint16_t args2, uint8_t args3);

/* primitivni funkce nastavovani regulatoru */
void button_click_set_term_ring_name_via_keyboard(uint16_t args1, uint16_t args2, uint8_t args3);


////////////
/* primitivni pomocne funkce pro nastavovani manualniho modu topi/chladi */
void button_click_set_term_heat_or_cool(uint8_t args1, uint8_t args2, uint8_t args3);
uint8_t button_get_term_heat_or_cool(uint16_t args1, uint16_t args2, uint8_t args3);
uint8_t display_enable_show_term_mode_man(uint16_t args1, uint16_t args2, uint8_t args3);



/* primitivni pomocne funkce */
uint8_t display_enable_show(uint8_t args1, uint8_t args2);
uint8_t get_function_return_args_1(uint16_t args1, uint16_t args2, uint8_t args3);
void preload_regulator_menu(uint16_t args1, uint16_t args2, uint8_t args3);
void preload_pid_menu(uint16_t args1, uint16_t args2, uint8_t args3);
/////////////////////////////////////////////////////////////////////////////////////////



/* primitivni funkce obsluha tlacitek dialogu vyberu modu regurlatoru */
void button_click_term_set_mode_via_dialog(uint8_t args1, uint8_t args2, uint8_t args3);
///
uint8_t button_get_term_mode_is_selected(uint16_t args1, uint16_t args2, uint8_t args3);
void button_click_term_set_mode(uint8_t args1, uint8_t args2, uint8_t args3);
void button_get_term_mode_labels(uint8_t args1, uint8_t args2, uint8_t args3, char *line1, char *line2);
////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

const Element_Dyn_Symbol_1 keyboard_number PROGMEM = {
  .first_x = 10,
  .first_y = 60,
  .size_x = 30,
  .size_y = 30,
  .font_size = 2,
  .step_x = 40,
  .step_y = 40,
  .direction = HORIZONTAL_NEW_LINE,
  .max_items_count = 9,
  .max_row_count = 0,
  .slider_args = 0,
  .args = INDEX_DYN_MENU_KEYBOARD_NUMBER,
  .get_status_string = get_function_keyboard_number_char,
  .dyn_symbol_onclick =  click_keyboard_number,
  .function_for_max_items = get_function_keyboard_number_max_keys,
  .redraw_class = REDRAW_BUTTON,
};

const Element_Dyn_Symbol_1 keyboard_alfa PROGMEM = {
  .first_x = 10,
  .first_y = 80,
  .size_x = 30,
  .size_y = 30,
  .font_size = 2,
  .step_x = 40,
  .step_y = 40,
  .direction = HORIZONTAL_NEW_LINE,
  .max_items_count = 10,
  .max_row_count = 0,
  .slider_args = 0,
  .args = INDEX_DYN_MENU_KEYBOARD_ALFA,
  .get_status_string = get_function_keyboard_alfa_char,
  .dyn_symbol_onclick =  click_keyboard_alfa,
  .function_for_max_items = get_function_keyboard_alfa_max_keys,
  .redraw_class = REDRAW_BUTTON,
};



const Element_Button_1 button_keyboard_upper PROGMEM = {
  .name = text_upper_case,
  .x = 320,
  .y = 35,
  .size_x = 40,
  .size_y = 40,
  .font_size = 2,
  .args = KEYBOARD_TYPE_UPPER,
  .onclick = click_keyboard_type,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};

const Element_Button_1 button_keyboard_lower PROGMEM = {
  .name = text_lower_case,
  .x = 365,
  .y = 35,
  .size_x = 40,
  .size_y = 40,
  .font_size = 2,
  .args = KEYBOARD_TYPE_LOWER,
  .onclick = click_keyboard_type,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};

const Element_Button_1 button_keyboard_special PROGMEM = {
  .name = text_special_case,
  .x = 410,
  .y = 35,
  .size_x = 40,
  .size_y = 40,
  .font_size = 2,
  .args = KEYBOARD_TYPE_SPECIAL,
  .onclick = click_keyboard_type,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};

const Element_Button_1 button_delete_char PROGMEM = {
  .name = text_delete_char,
  .x = 300,
  .y = 200,
  .size_x = 130,
  .size_y = 40,
  .font_size = 2,
  .args = 0,
  .onclick = display_element_set_string_del_char,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};

#endif
