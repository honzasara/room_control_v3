#ifndef __PROJECT_THERMOSTAT_MENU_H
#define __PROJECT_THERMOSTAT_MENU_H
#include "room_control_v3.h"


void button_get_default_ring_labels(uint8_t args1, uint8_t args2, uint8_t args3, char *line1, char *line2);
void button_change_default_ring_via_dialog_onclick(uint16_t args1, uint16_t args2, uint8_t args3);


void button_change_default_ring_labels_in_dialog(uint8_t args1, uint8_t args2, uint8_t args3, char *line1, char *line2);
void button_click_set_new_default_ring_in_dialog(uint16_t args1, uint16_t args2, uint8_t args3);
void button_no_default_ring_onclick(uint16_t args1, uint16_t args2, uint8_t args3);

uint8_t button_get_term_ring_is_selected(uint16_t args1, uint16_t args2, uint8_t args3);

void dyn_button_show_all_thermostat_get_status_string(uint8_t args1, uint8_t args2, uint8_t args3, char *line1, char *line2);
void dyn_button_show_all_thermostat_dyn_button_onclick(uint16_t args1, uint16_t args2, uint8_t args3);

void thermostat_button_set_ring_name_onclick(uint16_t args1, uint16_t args2, uint8_t args3);
void display_element_dialog_default_ring(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint16_t args1, uint8_t args2, char *text);
void thermostat_button_deactivate_ring_onclick(uint16_t args1, uint16_t args2, uint8_t args3);
void thermostat_button_activate_ring_onclick(uint16_t args1, uint16_t args2, uint8_t args3);

/* primitivni pomocne funkce pro nastavovani manualniho modu topi/chladi */
void button_click_set_term_heat_or_cool(uint16_t args1, uint16_t args2, uint8_t args3);
uint8_t button_get_term_heat_or_cool(uint16_t args1, uint16_t args2, uint8_t args3);
uint8_t display_enable_show_term_mode_man(uint16_t args1, uint16_t args2, uint8_t args3);
uint8_t display_enable_show_term_mode_prog(uint16_t args1, uint16_t args2, uint8_t args3);

void button_deassociate_input_sensor_for_term_onclick(uint16_t args1, uint16_t args2, uint8_t args3);


void button_deassociate_virtual_output_for_term_onclick(uint16_t args1, uint16_t args2, uint8_t args3);
void button_get_show_virtual_output(uint8_t args1, uint8_t args2, uint8_t args3, char *line1, char *line2);
void button_select_term_ring_virtual_output_in_dialog_onclick(uint16_t args1, uint16_t args2, uint8_t args3);
uint8_t button_select_term_ring_virtual_output_in_dialog_status_fnt(uint16_t args1, uint16_t args2, uint8_t args3);
uint8_t button_get_virtual_output_max_items(uint16_t args1, uint16_t args2, uint8_t args3);
uint8_t menu_redraw_change_virtual_output(uint16_t args1, uint16_t args2, uint8_t args3);


void button_get_show_default_temp_1(uint8_t args1, uint8_t args2, uint8_t args3, char *line1, char *line2);
uint8_t button_select_term_ring_input_in_dialog_status_fnt(uint16_t args1, uint16_t args2, uint8_t args3);
void button_select_term_ring_input_in_dialog_onclick(uint16_t args1, uint16_t args2, uint8_t args3);

uint8_t display_enable_show_thermostat_active_ring(uint16_t args1, uint16_t args2, uint8_t args3);
uint8_t display_enable_show_thermostat_deactive_ring(uint16_t args1, uint16_t args2, uint8_t args3);


/* primitivni funkce obsluha tlacitek dialogu vyberu modu regurlatoru */
void button_click_term_set_mode_via_dialog(uint16_t args1, uint16_t args2, uint8_t args3);
///
uint8_t button_select_term_mode_get_status_fnt(uint16_t args1, uint16_t args2, uint8_t args3);
void button_select_term_mode_onclick(uint16_t args1, uint16_t args2, uint8_t args3);
void button_select_term_mode_get_status_string(uint8_t args1, uint8_t args2, uint8_t args3, char *line1, char *line2);


uint8_t menu_redraw_change_default_ring(uint16_t args1, uint16_t args2, uint8_t args3);

uint8_t menu_redraw_change_term_mode(uint16_t args1, uint16_t args2, uint8_t args3);
uint8_t menu_redraw_change_term_mode_cool_heat(uint16_t args1, uint16_t args2, uint8_t args3);
uint8_t menu_redraw_change_term_input(uint16_t args1, uint16_t args2, uint8_t args3);

uint8_t preload_regulator_menu(uint16_t args1, uint16_t args2, uint8_t args3);


void display_element_show_thermostat_statistics(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint16_t args1, uint8_t args2, char *text);

const Element_Function_1 f_show_fill_rectangle_term_mode_dialog PROGMEM = {
  .x = 10,
  .y = 40,
  .args = YELLOW,
  .fnt_coordinate_xy = display_element_fill_rectangle,
  .size_x = 190,
  .size_y = 160,
  .redraw_class = ( 1 << REDRAW_CLASS_1),
  .onclick = nullfce,
  .enable_show = display_enable_show,
  .name = char_NULL,
};

const Element_Button_1 button_nastaveni_ring_input_sensor PROGMEM = {
  .name = text_assocoivat,
  .x = 280,
  .y = 100,
  .size_x = 190,
  .size_y = 40,
  .font_size = 2,
  .args = MENU_NASTAVENI_SELECT_INPUT_SENSORS,
  .onclick = MenuHistoryNextMenu,
  .redraw_class = (1 << REDRAW_ONCE),
  .enable_show = display_enable_show,
};

const Element_Button_1 button_nastaveni_ring_output PROGMEM = {
  .name = text_assocoivat_output,
  .x = 280,
  .y = 160,
  .size_x = 190,
  .size_y = 40,
  .font_size = 1,
  .args = MENU_NASTAVENI_SELECT_VIRTUAL_OUTPUT,
  .onclick = MenuHistoryNextMenu,
  .redraw_class = (1 << REDRAW_ONCE),
  .enable_show = display_enable_show,
};

const Element_Button_1 button_nastaveni_ring_pid PROGMEM = {
  .name = text_nastavit_pid,
  .x = 280,
  .y = 220,
  .size_x = 190,
  .size_y = 40,
  .font_size = 2,
  .args = MENU_NASTAVENI_SELECT_PID_PARAMETRS,
  .onclick = MenuHistoryNextMenu,
  .redraw_class = (1 << REDRAW_ONCE),
  .enable_show = display_enable_show,
};

const Element_Button_1 thermostat_button_show_statistic PROGMEM = {
  .name = new_text_termostat_statistika,
  .x = 10,
  .y = 160,
  .size_x = 190,
  .size_y = 40,
  .font_size = 1,
  .args = NEW_MENU_THERMOSTAT_STATISTICS,
  .onclick = MenuHistoryNextMenu,
  .redraw_class = (1 << REDRAW_ONCE),
  .enable_show = display_enable_show,
};


const Element_Button_2 button_term_ring_mode_heat PROGMEM = {
  .name = text_button_mode_heat,
  .x = 10,
  .y = 160,
  .size_x = 90,
  .size_y = 40,
  .font_size = 1,
  .color_active = RED,
  .color_inactive = WHITE,
  .args = TERM_MODE_MAN_HEAT,
  .onclick = button_click_set_term_heat_or_cool,
  .get_status_fnt = button_get_term_heat_or_cool,
  .redraw_class = (1 << REDRAW_ONCE | 1 << REDRAW_CLASS_2 | 1 << REDRAW_CLASS_1 ),
  .enable_show = display_enable_show_term_mode_man
};

const Element_Button_2 button_term_ring_mode_cool PROGMEM = {
  .name = text_button_mode_cool,
  .x = 110,
  .y = 160,
  .size_x = 90,
  .size_y = 40,
  .font_size = 1,
  .color_active = BLUE,
  .color_inactive = WHITE,
  .args = TERM_MODE_MAN_COOL,
  .onclick = button_click_set_term_heat_or_cool,
  .get_status_fnt = button_get_term_heat_or_cool,
  .redraw_class = (1 << REDRAW_ONCE | 1 << REDRAW_CLASS_2 | 1 << REDRAW_CLASS_1),
  .enable_show = display_enable_show_term_mode_man
};

const Element_Function_1 f_default_ring_set_temp PROGMEM = {
  .x = 50,
  .y = 40,
  .args = DIALOG_SET_VARIABLE_GENERAL,
  .fnt_coordinate_xy = display_element_dialog_set_variable,
  .size_x = 110,
  .size_y = 40,
  .redraw_class = (1 << REDRAW_ONCE | 1 << REDRAW_CLASS_0 | 1 << REDRAW_CLASS_1),
  .onclick = nullfce,
  .enable_show = display_enable_show_term_mode_man,
  .name = char_NULL,
};

const Element_Symbol_1 dialog_set_default_ring_temp_plus PROGMEM =  {
  .znak = '+',
  .x = 160,
  .y = 40,
  .size_x = 40,
  .size_y = 40,
  .znak_size = 2,
  .args = DIALOG_SET_VARIABLE_GENERAL,
  .onclick = display_function_set_variable_plus,
  .redraw_class = (1 << REDRAW_ONCE | 1 << REDRAW_CLASS_1),
  .enable_show = display_enable_show_term_mode_man
};
const Element_Symbol_1 dialog_set_default_ring_temp_minus PROGMEM =  {
  .znak = '-',
  .x = 10,
  .y = 40,
  .size_x = 40,
  .size_y = 40,
  .znak_size = 2,
  .args = DIALOG_SET_VARIABLE_GENERAL,
  .onclick = display_function_set_variable_minus,
  .redraw_class = (1 << REDRAW_ONCE | 1 << REDRAW_CLASS_1),
  .enable_show = display_enable_show_term_mode_man
};


const Element_Button_1 button_set_time_program PROGMEM = {
  .name = new_text_set_time_program,
  .x = 10,
  .y = 120,
  .size_x = 190,
  .size_y = 40,
  .font_size = 1,
  .args = NEW_MENU_THERMOSTAT_TIME_MENU,
  .onclick = MenuHistoryNextMenu,
  .redraw_class = (1 << REDRAW_ONCE),
  .enable_show = display_enable_show,
};

const Element_Button_1 button_no_default_ring PROGMEM = {
  .name = new_text_no_default_ring,
  .x = 220,
  .y = 220,
  .size_x = 190,
  .size_y = 40,
  .font_size = 1,
  .args = 0,
  .onclick = button_no_default_ring_onclick,
  .redraw_class = (1 << REDRAW_ONCE),
  .enable_show = display_enable_show,
};

const Element_Dyn_Button_1 button_change_default_ring_via_dialog PROGMEM = {
  .first_x = 10,
  .first_y = 40,
  .size_x = 190,
  .size_y = 55,
  .font_size = 1,
  .step_x = 1,
  .step_y = 1,
  .direction = VERTICAL,
  .max_items_count = 1,
  .max_row_count = 1,
  .slider_args = 0,
  .args = 1,
  .get_status_string = button_get_default_ring_labels,
  .dyn_button_onclick =  button_change_default_ring_via_dialog_onclick,
  .function_for_max_items = get_function_return_args_1,
  .redraw_class = (1 << REDRAW_ONCE),
};

const Element_Dyn_Select_1 button_change_default_ring_in_dialog PROGMEM = {
  .first_x = 10,
  .first_y = 40,
  .size_x = 120,
  .size_y = 50,
  .font_size_1 = 2,
  .font_size_2 = 1,
  .color_active = GREEN,
  .color_inactive = WHITE,
  .step_x = 130,
  .step_y = 60,
  .direction = HORIZONTAL_NEW_LINE,
  .max_items_count = 3,
  .max_row_count = 3,
  .slider_args = MENU_SLIDER_OFF,
  .args = MAX_THERMOSTAT,
  .get_status_string =  button_change_default_ring_labels_in_dialog,
  .dyn_symbol_onclick =  button_click_set_new_default_ring_in_dialog,
  .function_for_max_items = get_function_return_args_1,
  .get_status_fnt = button_get_term_ring_is_selected,
  .redraw_class = (1 << REDRAW_ONCE | 1 << REDRAW_CLASS_0),
};

const Element_Function_1 f_vertical_slider_show_all_thermostat PROGMEM = {
  .x = 410,
  .y = 72,
  .args = MENU_SLIDER_ALL_THERMOSTAT,
  .fnt_coordinate_xy = display_element_vertical_slider,
  .size_x = 40,
  .size_y = 126,
  .redraw_class = (1 << REDRAW_ONCE | 1 << REDRAW_CLASS_2),
  .onclick = nullfce,
  .enable_show = display_enable_show,
  .name = char_NULL,
};
const Element_Symbol_1 slider_menu_plus_all_thermostat PROGMEM =  {
  .znak = '+',
  .x = 410,
  .y = 30,
  .size_x = 40,
  .size_y = 40,
  .znak_size = 2,
  .args = MENU_SLIDER_ALL_THERMOSTAT,
  .onclick = display_function_vertical_slider_dec,
  .redraw_class = (1 << REDRAW_ONCE),
  .enable_show = display_enable_show,
};

const Element_Symbol_1 slider_menu_minus_all_thermostat PROGMEM =  {
  .znak = '-',
  .x = 410,
  .y = 200,
  .size_x = 40,
  .size_y = 40,
  .znak_size = 2,
  .args = MENU_SLIDER_ALL_THERMOSTAT,
  .onclick = display_function_vertical_slider_inc,
  .redraw_class = (1 << REDRAW_ONCE),
  .enable_show = display_enable_show,
};

const Element_Dyn_Button_1 dyn_button_show_all_thermostat PROGMEM = {
  .first_x = 230,
  .first_y = 40,
  .size_x = 170,
  .size_y = 55,
  .font_size = 1,
  .step_x = 60,
  .step_y = 60,
  .direction = VERTICAL,
  .max_items_count = 3,
  .max_row_count = 1,
  .slider_args = MENU_SLIDER_ALL_THERMOSTAT,
  .args = MAX_THERMOSTAT,
  .get_status_string = dyn_button_show_all_thermostat_get_status_string,
  .dyn_button_onclick =  dyn_button_show_all_thermostat_dyn_button_onclick,
  .function_for_max_items = get_function_return_args_1,
  .redraw_class = (1 << REDRAW_ONCE | 1 << REDRAW_CLASS_2),
};


const Element_Function_1 f_show_default_ring PROGMEM = {
  .x = 140,
  .y = 5,
  .args = 0,
  .fnt_coordinate_xy = display_element_dialog_default_ring,
  .size_x = 0,
  .size_y = 0,
  .redraw_class = (1 << REDRAW_ONCE),
  .onclick = nullfce,
  .enable_show = display_enable_show,
  .name = char_NULL,
};

const Element_Button_1 thermostat_button_set_ring_name PROGMEM = {
  .name = nastaveni_name_sensor,
  .x = 280,
  .y = 40,
  .size_x = 190,
  .size_y = 40,
  .font_size = 2,
  .args = 0,
  .onclick = thermostat_button_set_ring_name_onclick,
  .redraw_class = (1 << REDRAW_ONCE),
  .enable_show = display_enable_show,
};





const Element_Button_1 thermostat_button_deactivate_ring PROGMEM = {
  .name = text_term_do_deactivate,
  .x = 10,
  .y = 40,
  .size_x = 190,
  .size_y = 40,
  .font_size = 1,
  .args = 0,
  .onclick = thermostat_button_deactivate_ring_onclick,
  .redraw_class = (1 << REDRAW_ONCE),
  .enable_show = display_enable_show_thermostat_deactive_ring,
};

const Element_Button_1 thermostat_button_activate_ring PROGMEM = {
  .name = text_term_do_activate1,
  .x = 10,
  .y = 40,
  .size_x = 190,
  .size_y = 40,
  .font_size = 1,
  .args = 0,
  .onclick = thermostat_button_activate_ring_onclick,
  .redraw_class = (1 << REDRAW_ONCE),
  .enable_show = display_enable_show_thermostat_active_ring,
};

const Element_Button_1 thermostat_button_set_ring_mode PROGMEM = {
  .name = text_current_ring_mode,
  .x = 10,
  .y = 100,
  .size_x = 190,
  .size_y = 40,
  .font_size = 1,
  .args = NEW_MENU_DIALOG_SELECT_TERM_MODE,
  .onclick = MenuHistoryNextMenu,
  .redraw_class = (1 << REDRAW_ONCE),
  .enable_show = display_enable_show,
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const Element_Button_1 button_deassociate_input_sensor_for_term = {
  .name = nastaveni_deassociate_sensor,
  .x = 210,
  .y = 220,
  .size_x = 170,
  .size_y = 40,
  .font_size = 2,
  .args = 0,
  .onclick = button_deassociate_input_sensor_for_term_onclick,
  .redraw_class = (1 << REDRAW_ONCE),
  .enable_show = display_enable_show,
};


/*
   definice polozek menu pro virtualni vystupy
*/
const Element_Function_1 f_vertical_slider_term_ring_virtual_output PROGMEM = {
  .x = 410,
  .y = 82,
  .args = MENU_SLIDER_LIST_VIRTUAL_OUTPUT,
  .fnt_coordinate_xy = display_element_vertical_slider,
  .size_x = 40,
  .size_y = 86,
  .redraw_class = (1 << REDRAW_ONCE | 1 << REDRAW_CLASS_1),
  .onclick = nullfce,
  .enable_show = display_enable_show,
  .name = char_NULL,
};
const Element_Symbol_1 slider_menu_plus_term_ring_virtual_output PROGMEM =  {
  .znak = '+',
  .x = 410,
  .y = 40,
  .size_x = 40,
  .size_y = 40,
  .znak_size = 2,
  .args = MENU_SLIDER_LIST_VIRTUAL_OUTPUT,
  .onclick = display_function_vertical_slider_dec,
  .redraw_class = (1 << REDRAW_ONCE),
  .enable_show = display_enable_show,
};
const Element_Symbol_1 slider_menu_minus_term_ring_virtual_output PROGMEM =  {
  .znak = '-',
  .x = 410,
  .y = 170,
  .size_x = 40,
  .size_y = 40,
  .znak_size = 2,
  .args = MENU_SLIDER_LIST_VIRTUAL_OUTPUT,
  .onclick = display_function_vertical_slider_inc,
  .redraw_class = (1 << REDRAW_ONCE),
  .enable_show = display_enable_show,
};

///////////////////////////
const Element_Button_1 button_deassociate_virtual_output_for_term = {
  .name = nastaveni_deassociate_virtual_output,
  .x = 210,
  .y = 220,
  .size_x = 170,
  .size_y = 40,
  .font_size = 1,
  .args = 0,
  .onclick = button_deassociate_virtual_output_for_term_onclick,
  .redraw_class = (1 << REDRAW_ONCE),
  .enable_show = display_enable_show,
};


const Element_Dyn_Select_1 button_select_term_ring_virtual_output_in_dialog PROGMEM = {
  .first_x = 10,
  .first_y = 40,
  .size_x = 120,
  .size_y = 50,
  .font_size_1 = 1,
  .font_size_2 = 1,
  .color_active = GREEN,
  .color_inactive = WHITE,
  .step_x = 130,
  .step_y = 60,
  .direction = HORIZONTAL_NEW_LINE,
  .max_items_count = 3,
  .max_row_count = 3,
  .slider_args = MENU_SLIDER_LIST_VIRTUAL_OUTPUT,
  .args = 0,
  .get_status_string =  button_get_show_virtual_output,
  .dyn_symbol_onclick =  button_select_term_ring_virtual_output_in_dialog_onclick,
  .function_for_max_items =  button_get_virtual_output_max_items,
  .get_status_fnt = button_select_term_ring_virtual_output_in_dialog_status_fnt,
  .redraw_class = (1 << REDRAW_ONCE | 1 << REDRAW_CLASS_1 | 1 << REDRAW_CLASS_2),
};






/*********************************************************/
/*
    slider pro vyber vstupniho cidla pro regulatory
*/
const Element_Function_1 f_vertical_slider_term_ring_input_temp PROGMEM = {
  .x = 410,
  .y = 82,
  .args = MENU_SLIDER_INPUT_TEMP,
  .fnt_coordinate_xy = display_element_vertical_slider,
  .size_x = 40,
  .size_y = 86,
  .redraw_class = (1 << REDRAW_ONCE | 1 << REDRAW_CLASS_1),
  .onclick = nullfce,
  .enable_show = display_enable_show,
  .name = char_NULL,
};

const Element_Symbol_1 slider_menu_plus_term_ring_input_temp PROGMEM =  {
  .znak = '+',
  .x = 410,
  .y = 40,
  .size_x = 40,
  .size_y = 40,
  .znak_size = 2,
  .args = MENU_SLIDER_INPUT_TEMP,
  .onclick = display_function_vertical_slider_dec,
  .redraw_class = (1 << REDRAW_ONCE),
  .enable_show = display_enable_show,
};
const Element_Symbol_1 slider_menu_minus_term_ring_input_temp PROGMEM =  {
  .znak = '-',
  .x = 410,
  .y = 170,
  .size_x = 40,
  .size_y = 40,
  .znak_size = 2,
  .args = MENU_SLIDER_INPUT_TEMP,
  .onclick = display_function_vertical_slider_inc,
  .redraw_class = (1 << REDRAW_ONCE),
  .enable_show = display_enable_show,
};


const Element_Dyn_Select_1 button_select_term_ring_input_in_dialog PROGMEM = {
  .first_x = 10,
  .first_y = 40,
  .size_x = 120,
  .size_y = 50,
  .font_size_1 = 1,
  .font_size_2 = 1,
  .color_active = GREEN,
  .color_inactive = WHITE,
  .step_x = 130,
  .step_y = 60,
  .direction = HORIZONTAL_NEW_LINE,
  .max_items_count = 3,
  .max_row_count = 3,
  .slider_args = MENU_SLIDER_INPUT_TEMP,
  //.args = INPUT_SENSOR_SHOW_ACTIVE,
  .args = INPUT_SENSOR_SHOW_ALL,
  .get_status_string =  button_get_show_default_temp_1,
  .dyn_symbol_onclick =  button_select_term_ring_input_in_dialog_onclick,
  .function_for_max_items =  button_get_show_default_temp_max_items,
  .get_status_fnt = button_select_term_ring_input_in_dialog_status_fnt,
  .redraw_class = (1 << REDRAW_ONCE | 1 << REDRAW_CLASS_1 | 1 << REDRAW_CLASS_2),
};

const Element_Dyn_Select_1 button_select_term_mode PROGMEM = {
  .first_x = 230,
  .first_y = 40,
  .size_x = 200,
  .size_y = 40,
  .font_size_1 = 2,
  .font_size_2 = 1,
  .color_active = GREEN,
  .color_inactive = WHITE,
  .step_x = 1,
  .step_y = 50,
  .direction = VERTICAL,
  .max_items_count = 5,
  .max_row_count = 1,
  .slider_args = MENU_SLIDER_OFF,
  .args = 5,
  .get_status_string = button_select_term_mode_get_status_string,
  .dyn_symbol_onclick =  button_select_term_mode_onclick,
  .function_for_max_items = get_function_return_args_1,
  .get_status_fnt = button_select_term_mode_get_status_fnt,
  .redraw_class = (1 << REDRAW_ONCE | 1 << REDRAW_CLASS_1),
};


const Element_Function_1 f_show_thermostat_statistics PROGMEM = {
  .x = 10,
  .y = 30,
  .args = 0,
  .fnt_coordinate_xy = display_element_show_thermostat_statistics,
  .size_x = 200,
  .size_y = 170,
  .redraw_class = 1 << REDRAW_ONCE,
  .onclick = nullfce,
  .enable_show = display_enable_show,
  .name = char_NULL,
};

const Menu1 DialogSetManualyTemp PROGMEM = {
  .name = text_nastaveni_mezni_temp,
  .button_1 = {button_back },
  .button_2 = {NULL},
  .function_1 = {f_default_ring_set_temp},
  .switch_1 = {NULL},
  .dyn_button = {NULL},
  .symbol_button_1 = {dialog_set_default_ring_temp_plus, dialog_set_default_ring_temp_minus},
  .dyn_symbol_1 = {NULL},
  .dyn_select_box_1 = {NULL},
  .len_button_1 = 1,
  .len_button_2 = 0,
  .len_function_1 = 1,
  .len_switch_1 = 0,
  .len_dyn_button_1 = 0,
  .len_symbol_button_1 = 2,
  .len_dyn_symbol_1 = 0,
  .len_dyn_select_box_1 = 0,
  .idx = NEW_MENU_THERMOSTAT_MAN_TEMP,
  .x = 10,
  .y = 10,
  .size_x = 460,
  .size_y = 300,
  .atributes = (1 << MENU_ATTRIBUTES_FILL_COLOR_RECTANGLE | 1 << MENU_ATTRIBUTES_DECORATE_MENU),
  .color_background = YELLOW,
  .redraw_class = (1 << REDRAW_ONCE),
  .redraw_class_0 = menu_redraw_change_variable,
  .redraw_class_1 = returnnullfceargs,
  .redraw_class_2 = returnnullfceargs,
  .redraw_class_3 = returnnullfceargs,
  .preload_function = preload_regulator_menu,
};


const Menu1 DialogSelectTermMode PROGMEM = {
  .name = text_select_term_mode,
  .button_1 = {button_back },
  .button_2 = {button_term_ring_mode_heat, button_term_ring_mode_cool},
  .function_1 = {f_show_fill_rectangle_term_mode_dialog, f_default_ring_set_temp},
  .switch_1 = {NULL},
  .dyn_button = {NULL},
  .symbol_button_1 = {dialog_set_default_ring_temp_plus, dialog_set_default_ring_temp_minus},
  .dyn_symbol_1 = {NULL},
  .dyn_select_box_1 = {button_select_term_mode},
  .len_button_1 = 1,
  .len_button_2 = 2,
  .len_function_1 = 2,
  .len_switch_1 = 0,
  .len_dyn_button_1 = 0,
  .len_symbol_button_1 = 2,
  .len_dyn_symbol_1 = 0,
  .len_dyn_select_box_1 = 1,
  .idx = NEW_MENU_DIALOG_SELECT_TERM_MODE,
  .x = 10,
  .y = 10,
  .size_x = 460,
  .size_y = 300,
  .atributes = (1 << MENU_ATTRIBUTES_FILL_COLOR_RECTANGLE | 1 << MENU_ATTRIBUTES_DECORATE_MENU),
  .color_background = YELLOW,
  .redraw_class = (1 << REDRAW_ONCE),
  .redraw_class_0 = menu_redraw_change_variable,
  .redraw_class_1 = menu_redraw_change_term_mode,
  .redraw_class_2 = menu_redraw_change_term_mode_cool_heat,
  .redraw_class_3 = returnnullfceargs,
  .preload_function = preload_regulator_menu,
};

const Menu1 DialogSelectInputSensorsForTerm PROGMEM = {
  .name = text_assocoivat,
  .button_1 = {button_back, button_deassociate_input_sensor_for_term},
  .button_2 = {NULL},
  .function_1 = {f_vertical_slider_term_ring_input_temp},
  .switch_1 = {NULL},
  .dyn_button = {NULL},
  .symbol_button_1 = {slider_menu_plus_term_ring_input_temp, slider_menu_minus_term_ring_input_temp},
  .dyn_symbol_1 = {NULL},
  .dyn_select_box_1 = {button_select_term_ring_input_in_dialog},
  .len_button_1 = 2,
  .len_button_2 = 0,
  .len_function_1 = 1,
  .len_switch_1 = 0,
  .len_dyn_button_1 = 0,
  .len_symbol_button_1 = 2,
  .len_dyn_symbol_1 = 0,
  .len_dyn_select_box_1 = 1,
  .idx = MENU_NASTAVENI_SELECT_INPUT_SENSORS,
  .x = 10,
  .y = 10,
  .size_x = 460,
  .size_y = 300,
  .atributes = (1 << MENU_ATTRIBUTES_FILL_COLOR_RECTANGLE | 1 << MENU_ATTRIBUTES_DECORATE_MENU),
  .color_background = YELLOW,
  .redraw_class = (1 << REDRAW_ONCE),
  .redraw_class_0 = returnnullfceargs,
  .redraw_class_1 = menu_redraw_slider,
  .redraw_class_2 = menu_redraw_change_term_input,
  .redraw_class_3 = returnnullfceargs,
  .preload_function = returnnullfceargs,
};


const Menu1 MenuThermostatRingSetup PROGMEM = {
  .name = ring_text_setup,
  .button_1 = {button_back, thermostat_button_show_statistic, thermostat_button_set_ring_name, thermostat_button_deactivate_ring, thermostat_button_activate_ring, button_nastaveni_ring_input_sensor, button_nastaveni_ring_output, button_nastaveni_ring_pid, thermostat_button_set_ring_mode},
  .button_2 = {},
  .function_1 = {f_show_date, f_show_default_ring},
  .switch_1 = {NULL},
  .dyn_button = {NULL},
  .symbol_button_1 = {NULL},
  .dyn_symbol_1 = {NULL},
  .dyn_select_box_1 = {NULL},
  .len_button_1 = 9,
  .len_button_2 = 0,
  .len_function_1 = 2,
  .len_switch_1 = 0,
  .len_dyn_button_1 = 0,
  .len_symbol_button_1 = 0,
  .len_dyn_symbol_1 = 0,
  .len_dyn_select_box_1 = 0,
  .idx = MENU_NASTAVENI_RING_SCREEN,
  .x = 0,
  .y = 0,
  .size_x = 480,
  .size_y = 320,
  .atributes = (1 << MENU_ATTRIBUTES_CLEAN_DISPLAY),
  .color_background = WHITE,
  .redraw_class = (1 << REDRAW_ONCE),
  .redraw_class_0 = returnnullfceargs,
  .redraw_class_1 = returnnullfceargs,
  .redraw_class_2 = returnnullfceargs,
  .redraw_class_3 = returnnullfceargs,
  .preload_function = returnnullfceargs,
};

const Menu1 DialogSelectRing PROGMEM = {
  .name = text_select_term_default_ring,
  .button_1 = {button_back, button_no_default_ring},
  .button_2 = {NULL},
  .function_1 = {NULL},
  .switch_1 = {NULL},
  .dyn_button = {NULL},
  .symbol_button_1 = {NULL},
  .dyn_symbol_1 = {NULL},
  .dyn_select_box_1 = {button_change_default_ring_in_dialog},
  .len_button_1 = 2,
  .len_button_2 = 0,
  .len_function_1 = 0,
  .len_switch_1 = 0,
  .len_dyn_button_1 = 0,
  .len_symbol_button_1 = 0,
  .len_dyn_symbol_1 = 0,
  .len_dyn_select_box_1 = 1,
  .idx = MENU_NASTAVENI_SELECT_RING_SCREEN,
  .x = 10,
  .y = 10,
  .size_x = 460,
  .size_y = 300,
  .atributes = (1 << MENU_ATTRIBUTES_FILL_COLOR_RECTANGLE | 1 << MENU_ATTRIBUTES_DECORATE_MENU),
  .color_background = YELLOW,
  .redraw_class = (1 << REDRAW_ONCE),
  .redraw_class_0 = menu_redraw_change_default_ring,
  .redraw_class_1 = returnnullfceargs,
  .redraw_class_2 = returnnullfceargs,
  .redraw_class_3 = returnnullfceargs,
  .preload_function = returnnullfceargs,
};

const Menu1 MenuThermostat_Setting PROGMEM = {
  .name = new_text_nastaveni_regulatoru_full,
  .button_1 = {button_back, button_set_time_program},
  .button_2 = {NULL},
  .function_1 = {f_show_rectangle_decorate, f_show_date, f_vertical_slider_show_all_thermostat },
  .switch_1 = {NULL},
  .dyn_button = {button_change_default_ring_via_dialog, dyn_button_show_all_thermostat},
  .symbol_button_1 = {slider_menu_plus_all_thermostat, slider_menu_minus_all_thermostat},
  .dyn_symbol_1 = {NULL},
  .dyn_select_box_1 = {NULL},
  .len_button_1 = 2,
  .len_button_2 = 0,
  .len_function_1 = 3,
  .len_switch_1 = 0,
  .len_dyn_button_1 = 2,
  .len_symbol_button_1 = 2,
  .len_dyn_symbol_1 = 0,
  .len_dyn_select_box_1 = 0,
  .idx = NEW_MENU_THERMOSTAT_SETTINGS,
  .x = 0,
  .y = 0,
  .size_x = 480,
  .size_y = 320,
  .atributes = (1 << MENU_ATTRIBUTES_CLEAN_DISPLAY),
  .color_background = WHITE,
  .redraw_class = (1 << REDRAW_ONCE),
  .redraw_class_0 = returnnullfceargs,
  .redraw_class_1 = returnnullfceargs,
  .redraw_class_2 = menu_redraw_slider,
  .redraw_class_3 = menu_redraw_date,
  .preload_function = returnnullfceargs,
};


const Menu1 MenuThermostat PROGMEM = {
  .name = new_text_regulator_menu,
  .button_1 = {button_back},
  .button_2 = {NULL},
  .function_1 = {NULL},
  .switch_1 = {NULL},
  .dyn_button = {NULL},
  .symbol_button_1 = {NULL},
  .dyn_symbol_1 = {NULL},
  .dyn_select_box_1 = {NULL},
  .len_button_1 = 1,
  .len_button_2 = 0,
  .len_function_1 = 0,
  .len_switch_1 = 0,
  .len_dyn_button_1 = 0,
  .len_symbol_button_1 = 0,
  .len_dyn_symbol_1 = 0,
  .len_dyn_select_box_1 = 0,
  .idx = NEW_MENU_THERMOSTAT,
  .x = 0,
  .y = 0,
  .size_x = 480,
  .size_y = 320,
  .atributes = (1 << MENU_ATTRIBUTES_CLEAN_DISPLAY),
  .color_background = WHITE,
  .redraw_class = (1 << REDRAW_ONCE),
  .redraw_class_0 = returnnullfceargs,
  .redraw_class_1 = returnnullfceargs,
  .redraw_class_2 = menu_redraw_slider,
  .redraw_class_3 = menu_redraw_date,
  .preload_function = returnnullfceargs,
};





const Menu1 DialogSelectVirtualOutputForTerm PROGMEM = {
  .name = text_assocoivat_output,
  .button_1 = {button_back, button_deassociate_virtual_output_for_term},
  .button_2 = {NULL},
  .function_1 = {f_vertical_slider_term_ring_virtual_output},
  .switch_1 = {NULL},
  .dyn_button = {NULL},
  .symbol_button_1 = {slider_menu_plus_term_ring_virtual_output, slider_menu_minus_term_ring_virtual_output},
  .dyn_symbol_1 = {NULL},
  .dyn_select_box_1 = {button_select_term_ring_virtual_output_in_dialog},
  .len_button_1 = 2,
  .len_button_2 = 0,
  .len_function_1 = 1,
  .len_switch_1 = 0,
  .len_dyn_button_1 = 0,
  .len_symbol_button_1 = 2,
  .len_dyn_symbol_1 = 0,
  .len_dyn_select_box_1 = 1,
  .idx = MENU_NASTAVENI_SELECT_VIRTUAL_OUTPUT,
  .x = 10,
  .y = 10,
  .size_x = 460,
  .size_y = 300,
  .atributes = (1 << MENU_ATTRIBUTES_FILL_COLOR_RECTANGLE | 1 << MENU_ATTRIBUTES_DECORATE_MENU),
  .color_background = YELLOW,
  .redraw_class = (1 << REDRAW_ONCE),
  .redraw_class_0 = returnnullfceargs,
  .redraw_class_1 = menu_redraw_slider,
  .redraw_class_2 = menu_redraw_change_virtual_output,
  .redraw_class_3 = returnnullfceargs,
  .preload_function = returnnullfceargs,
};



const Menu1 DialogShowThermostatStatistics PROGMEM = {
  .name = new_text_termostat_statistika,
  .button_1 = {button_back},
  .button_2 = {NULL},
  .function_1 = {f_show_date, f_show_thermostat_statistics},
  .switch_1 = {NULL},
  .dyn_button = {NULL},
  .symbol_button_1 = {NULL},
  .dyn_symbol_1 = {NULL},
  .dyn_select_box_1 = {NULL},
  .len_button_1 = 1,
  .len_button_2 = 0,
  .len_function_1 = 2,
  .len_switch_1 = 0,
  .len_dyn_button_1 = 0,
  .len_symbol_button_1 = 0,
  .len_dyn_symbol_1 = 0,
  .len_dyn_select_box_1 = 0,
  .idx = NEW_MENU_THERMOSTAT_STATISTICS,
  .x = 10,
  .y = 10,
  .size_x = 480,
  .size_y = 320,
  .atributes = (1 << MENU_ATTRIBUTES_CLEAN_DISPLAY),
  .color_background = WHITE,
  .redraw_class = (1 << REDRAW_ONCE),
  .redraw_class_0 = returnnullfceargs,
  .redraw_class_1 = returnnullfceargs,
  .redraw_class_2 = returnnullfceargs,
  .redraw_class_3 =  menu_redraw_date,
  .preload_function = returnnullfceargs,
};


#endif
