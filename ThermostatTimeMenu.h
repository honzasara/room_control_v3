#ifndef __PROJECT_THERMOSTAT_TIME_MENU_H
#define __PROJECT_THERMOSTAT_TIME_MENU_H
#include "room_control_v3.h"

void click_button_time_program_new(uint16_t args1, uint16_t args2, uint8_t args3);

const Element_Button_1 button_time_program_new PROGMEM = {
  .name = new_text_novy_time_program,
  .x = 10,
  .y = 40,
  .size_x = 190,
  .size_y = 40,
  .font_size = 2,
  .args = 0,
  .onclick = click_button_time_program_new,
  .redraw_class = (1 << REDRAW_ONCE),
  .enable_show = display_enable_show,
};

uint8_t get_function_list_all_program_max_items(uint16_t idx, uint16_t args2, uint8_t args3);
void click_list_all_program(uint16_t args1, uint16_t args2, uint8_t idx);
void get_function_list_all_program_label(uint8_t args1, uint8_t args2 , uint8_t args3, char *line1, char *line2);


uint8_t get_function_list_program_interval_max_items(uint16_t idx, uint16_t args2, uint8_t args3);
void click_list_program_interval(uint16_t args1, uint16_t args2, uint8_t idx);
void get_function_list_program_interval_label(uint8_t args1, uint8_t args2 , uint8_t args3, char *line1, char *line2);


void click_button_time_program_name(uint16_t args1, uint16_t args2, uint8_t idx);
void click_button_time_program_remove(uint16_t args1, uint16_t args2, uint8_t idx);
void click_button_time_program_new_interval(uint16_t args1, uint16_t args2, uint8_t args3);

void display_element_show_program_interval_statistics(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint16_t args1, uint8_t args2, char *text);
void click_button_time_program_interval_remove(uint16_t args1, uint16_t args2, uint8_t idx);

const Element_Symbol_1 slider_menu_plus_all_program PROGMEM =  {
  .znak = '+',
  .x = 410,
  .y = 30,
  .size_x = 40,
  .size_y = 40,
  .znak_size = 2,
  .args = MENU_SLIDER_LIST_ALL_PROGRAM,
  .onclick = display_function_vertical_slider_dec,
  .redraw_class = 1 << REDRAW_ONCE,
  .enable_show = display_enable_show,
};

const Element_Symbol_1 slider_menu_minus_all_program PROGMEM =  {
  .znak = '-',
  .x = 410,
  .y = 200,
  .size_x = 40,
  .size_y = 40,
  .znak_size = 2,
  .args = MENU_SLIDER_LIST_ALL_PROGRAM,
  .onclick = display_function_vertical_slider_inc,
  .redraw_class = 1 << REDRAW_ONCE,
  .enable_show = display_enable_show,
};

const Element_Function_1 f_vertical_slider_all_program PROGMEM = {
  .x = 410,
  .y = 72,
  .args = MENU_SLIDER_LIST_ALL_PROGRAM,
  .fnt_coordinate_xy = display_element_vertical_slider,
  .size_x = 40,
  .size_y = 126,
  .redraw_class = 1 << REDRAW_ONCE | 1 << REDRAW_CLASS_2,
  .onclick = nullfce,
  .enable_show = display_enable_show,
  .name = char_NULL,
};



const Element_Symbol_1 slider_menu_plus_program_interval PROGMEM =  {
  .znak = '+',
  .x = 410,
  .y = 30,
  .size_x = 40,
  .size_y = 40,
  .znak_size = 2,
  .args = MENU_SLIDER_LIST_PROGRAM_INTERVAL,
  .onclick = display_function_vertical_slider_dec,
  .redraw_class = 1 << REDRAW_ONCE,
  .enable_show = display_enable_show,
};

const Element_Symbol_1 slider_menu_minus_program_interval PROGMEM =  {
  .znak = '-',
  .x = 410,
  .y = 200,
  .size_x = 40,
  .size_y = 40,
  .znak_size = 2,
  .args = MENU_SLIDER_LIST_PROGRAM_INTERVAL,
  .onclick = display_function_vertical_slider_inc,
  .redraw_class = 1 << REDRAW_ONCE,
  .enable_show = display_enable_show,
};

const Element_Function_1 f_vertical_slider_program_interval PROGMEM = {
  .x = 410,
  .y = 72,
  .args = MENU_SLIDER_LIST_PROGRAM_INTERVAL,
  .fnt_coordinate_xy = display_element_vertical_slider,
  .size_x = 40,
  .size_y = 126,
  .redraw_class = 1 << REDRAW_ONCE | 1 << REDRAW_CLASS_1,
  .onclick = nullfce,
  .enable_show = display_enable_show,
  .name = char_NULL,
};


const Element_Button_1 button_time_program_name PROGMEM = {
  .name = nastaveni_name_sensor,
  .x = 10,
  .y = 40,
  .size_x = 190,
  .size_y = 40,
  .font_size = 2,
  .args = 0,
  .onclick = click_button_time_program_name,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};

const Element_Button_1 button_time_program_remove PROGMEM = {
  .name = nastaveni_deassociate_program,
  .x = 10,
  .y = 100,
  .size_x = 190,
  .size_y = 40,
  .font_size = 2,
  .args = 0,
  .onclick = click_button_time_program_remove,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};

const Element_Button_1 button_time_program_interval_remove PROGMEM = {
  .name = nastaveni_deassociate_program_interval,
  .x = 230,
  .y = 100,
  .size_x = 190,
  .size_y = 40,
  .font_size = 1,
  .args = 0,
  .onclick = click_button_time_program_interval_remove,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};

const Element_Button_1 button_time_program_new_interval PROGMEM = {
  .name = new_text_program_interval,
  .x = 10,
  .y = 160,
  .size_x = 190,
  .size_y = 40,
  .font_size = 2,
  .args = 0,
  .onclick = click_button_time_program_new_interval,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};

const Element_Dyn_Button_1 list_program_interval PROGMEM = {
  .first_x = 230,
  .first_y = 35,
  .size_x = 170,
  .size_y = 55,
  .font_size = 1,
  .step_x = 60,
  .step_y = 60,
  .direction = VERTICAL,
  .max_items_count = 3,
  .max_row_count = 1,
  .slider_args = MENU_SLIDER_LIST_PROGRAM_INTERVAL,
  .args = 0,
  .get_status_string = get_function_list_program_interval_label,
  .dyn_button_onclick =  click_list_program_interval,
  .function_for_max_items = get_function_list_program_interval_max_items,
  .redraw_class = 1 << REDRAW_ONCE | 1 << REDRAW_CLASS_1,
};




const Element_Dyn_Button_1 list_all_program PROGMEM = {
  .first_x = 230,
  .first_y = 35,
  .size_x = 170,
  .size_y = 55,
  .font_size = 1,
  .step_x = 60, .step_y = 60,
  .direction = VERTICAL,
  .max_items_count = 3,
  .max_row_count = 1,
  .slider_args = MENU_SLIDER_LIST_ALL_PROGRAM,
  .args = 0,
  .get_status_string = get_function_list_all_program_label,
  .dyn_button_onclick =  click_list_all_program,
  .function_for_max_items = get_function_list_all_program_max_items,
  .redraw_class = 1 << REDRAW_ONCE | 1 << REDRAW_CLASS_1,
};

const Element_Function_1 f_show_program_interval_statistics PROGMEM = {
  .x = 10,
  .y = 30,
  .args = 0,
  .fnt_coordinate_xy = display_element_show_program_interval_statistics,
  .size_x = 200,
  .size_y = 170,
  .redraw_class = 1 << REDRAW_ONCE,
  .onclick = nullfce,
  .enable_show = display_enable_show,
  .name = char_NULL,
};

const Menu1 New_ThermostatTimeMenu PROGMEM = {
  .name = new_text_set_time_program,
  .button_1 = {button_back, button_time_program_new},
  .button_2 = {NULL},
  .function_1 = {f_show_date, f_show_rectangle_decorate, f_vertical_slider_all_program},
  .switch_1 = {NULL},
  .dyn_button = {list_all_program},
  .symbol_button_1 = {slider_menu_plus_all_program, slider_menu_minus_all_program},
  .dyn_symbol_1 = {NULL},
  .dyn_select_box_1 = {NULL},
  .len_button_1 = 2,
  .len_button_2 = 0,
  .len_function_1 = 3,
  .len_switch_1 = 0,
  .len_dyn_button_1 = 1,
  .len_symbol_button_1 = 2,
  .len_dyn_symbol_1 = 0,
  .len_dyn_select_box_1 = 0,
  .idx = NEW_MENU_THERMOSTAT_TIME_MENU,
  .x = 0,
  .y = 0,
  .size_x = 480,
  .size_y = 320,
  .atributes = (1 << MENU_ATTRIBUTES_CLEAN_DISPLAY),
  .color_background = WHITE,
  .redraw_class = (1 << REDRAW_ONCE),
  .redraw_class_0 = returnnullfceargs,
  .redraw_class_1 = menu_redraw_slider,
  .redraw_class_2 = returnnullfceargs,
  .redraw_class_3 = menu_redraw_date,
  .preload_function = returnnullfceargs,
};


const Menu1 New_ThermostatTimeProgramMenu PROGMEM = {
  .name = new_text_set_time_program,
  .button_1 = {button_back, button_time_program_name, button_time_program_remove, button_time_program_new_interval},
  .button_2 = {NULL},
  .function_1 = {f_show_date, f_vertical_slider_program_interval, f_show_rectangle_decorate},
  .switch_1 = {NULL},
  .dyn_button = {list_program_interval},
  .symbol_button_1 = {slider_menu_plus_program_interval, slider_menu_minus_program_interval},
  .dyn_symbol_1 = {NULL},
  .dyn_select_box_1 = {NULL},
  .len_button_1 = 4,
  .len_button_2 = 0,
  .len_function_1 = 3,
  .len_switch_1 = 0,
  .len_dyn_button_1 = 1,
  .len_symbol_button_1 = 2,
  .len_dyn_symbol_1 = 0,
  .len_dyn_select_box_1 = 0,
  .idx = NEW_MENU_THERMOSTAT_TIME_PROGRAM_MENU,
  .x = 0,
  .y = 0,
  .size_x = 480,
  .size_y = 320,
  .atributes = (1 << MENU_ATTRIBUTES_CLEAN_DISPLAY),
  .color_background = WHITE,
  .redraw_class = (1 << REDRAW_ONCE),
  .redraw_class_0 = returnnullfceargs,
  .redraw_class_1 = menu_redraw_slider,
  .redraw_class_2 = returnnullfceargs,
  .redraw_class_3 = menu_redraw_date,
  .preload_function = returnnullfceargs,
};


const Menu1 New_ThermostatTimeProgramMenuDetail PROGMEM = {
  .name = new_text_set_time_program,
  .button_1 = {button_back, button_time_program_interval_remove},
  .button_2 = {NULL},
  .function_1 = {f_show_date,f_show_program_interval_statistics},
  .switch_1 = {NULL},
  .dyn_button = {NULL},
  .symbol_button_1 = {NULL},
  .dyn_symbol_1 = {NULL},
  .dyn_select_box_1 = {NULL},
  .len_button_1 = 2,
  .len_button_2 = 0,
  .len_function_1 = 2,
  .len_switch_1 = 0,
  .len_dyn_button_1 = 0,
  .len_symbol_button_1 = 0,
  .len_dyn_symbol_1 = 0,
  .len_dyn_select_box_1 = 0,
  .idx = NEW_MENU_THERMOSTAT_TIME_PROGRAM_MENU_DETAIL,
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
  .redraw_class_3 = menu_redraw_date,
  .preload_function = returnnullfceargs,
};

#endif
