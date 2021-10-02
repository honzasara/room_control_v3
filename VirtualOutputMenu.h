#ifndef __PROJECT_VIRTUAL_OUTPUT_SETTINGS_MENU_H
#define __PROJECT_VIRTUAL_OUTPUT_SETTINGS_MENU_H
#include "room_control_v3.h"
#include "UserSettings.h"

uint8_t get_function_list_virtual_output_max_active_items(uint16_t idx, uint16_t args2, uint8_t args3);
void click_virtual_output_associate_action(uint16_t args1, uint16_t args2, uint8_t idx);
void get_function_list_virtual_output_labels(uint8_t args1, uint8_t args2 , uint8_t args3, char *line1, char *line2);

const Element_Dyn_Button_1 list_all_virtual_output PROGMEM = {
  .first_x = 230,
  .first_y = 35,
  .size_x = 170,
  .size_y = 55,
  .font_size = 1,
  .step_x = 60, .step_y = 60,
  .direction = VERTICAL,
  .max_items_count = 3,
  .max_row_count = 1,
  .slider_args = MENU_SLIDER_LIST_VIRTUAL_OUTPUT,
  .args = 0,
  .get_status_string = get_function_list_virtual_output_labels,
  .dyn_button_onclick =  click_virtual_output_associate_action,
  .function_for_max_items = get_function_list_virtual_output_max_active_items,
  .redraw_class = REDRAW_BUTTON,
};



const Menu1 VirtualOutputSettingsMenu PROGMEM = {
  .name = new_nastaveni_virtual_output,
  .button_1 = {button_back},
  .button_2 = {NULL},
  .function_1 = {f_show_date},
  .switch_1 = {NULL},
  .dyn_button = {list_all_virtual_output},
  .symbol_button_1 = {NULL},
  .dyn_symbol_1 = {NULL},
  .dyn_select_box_1 = {NULL},
  .len_button_1 = 1,
  .len_button_2 = 0,
  .len_function_1 = 1,
  .len_switch_1 = 0,
  .len_dyn_button_1 = 1,
  .len_symbol_button_1 = 0,
  .len_dyn_symbol_1 = 0,
  .len_dyn_select_box_1 = 0,
  .idx = NEW_MENU_VIRTUAL_OUTPUT_SETTING,
  .x = 0,
  .y = 0,
  .size_x = 480,
  .size_y = 320,
  .atributes = (1 << MENU_ATTRIBUTES_CLEAN_DISPLAY),
  .color_background = WHITE,
  .redraw_class = (1 << REDRAW_FORCE),
  .redraw_class_0 = returnnullfceargs,
  .redraw_class_1 = returnnullfceargs,
  .redraw_class_2 = returnnullfceargs,
  .redraw_class_3 = returnnullfceargs,
  .preload_function = returnnullfceargs,
};

#endif
