#ifndef __PROJECT_THERMOSTAT_TIME_MENU_H
#define __PROJECT_THERMOSTAT_TIME_MENU_H
#include "room_control_v3.h"


const Element_Button_1 button_time_plan_new PROGMEM = {
  .name = new_text_novy_time_plan,
  .x = 10,
  .y = 40,
  .size_x = 190,
  .size_y = 40,
  .font_size = 2,
  .args = 0,
  .onclick = nullfce, //MenuHistoryNextMenu,
  .redraw_class = (1 << REDRAW_ONCE),
  .enable_show = display_enable_show,
};

/*
const Element_Dyn_Button_1 button_time_plan_show_all PROGMEM = {
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
  .slider_args = MENU_SLIDER_RTDS,
  .args = INDEX_DYN_MENU_RTDS,
  .get_status_string = get_function_rtds_text_button,
  .dyn_button_onclick =  click_rtds_setting_sensor,
  .function_for_max_items = get_function_rtds_last_index_for_menu,
  .redraw_class = REDRAW_BUTTON,
};
*/

const Menu1 New_ThermostatTimeMenu PROGMEM = {
  .name = new_text_set_time_program,
  .button_1 = {button_back, button_time_plan_new},
  .button_2 = {NULL},
  .function_1 = {f_show_date},
  .switch_1 = {NULL},
  .dyn_button = {NULL},
  .symbol_button_1 = {NULL},
  .dyn_symbol_1 = {NULL},
  .dyn_select_box_1 = {NULL},
  .len_button_1 = 2,
  .len_button_2 = 0,
  .len_function_1 = 1,
  .len_switch_1 = 0,
  .len_dyn_button_1 = 0,
  .len_symbol_button_1 = 0,
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
  .redraw_class_1 = returnnullfceargs,
  .redraw_class_2 = returnnullfceargs,
  .redraw_class_3 = menu_redraw_date,
  .preload_function = returnnullfceargs,
};
  
#endif
