#ifndef __PROJECT_FUNCTIONS_MENU_H
#define __PROJECT_FUNKTIONS_MENU_H
#include "room_control_v3.h"

uint8_t get_function_budik_enabled(uint16_t args1, uint16_t args2, uint8_t args3);
void get_function_budik_text_state(uint8_t args1, uint8_t args2, uint8_t args3, char *line1, char *line2);

const Element_Switch_1 switch_budik PROGMEM = {
  .name = budik_text,
  .x = 10,
  .y = 40,
  .size_x = 190,
  .size_y = 120,
  .font_size = 2,
  .color_active = 0,
  .color_inactive = 0,
  .args = SWITCH_BUDIK,
  .onclick = nullfce,
  .get_status_fnt = get_function_budik_enabled,
  .get_status_string = get_function_budik_text_state,
  .redraw_class = REDRAW_BUTTON,
};

const Menu1 FunkceMenu PROGMEM = {
  .name = funkce_text,
  .button_1 = {button_back},
  .button_2 = {NULL},
  .function_1 = {f_show_date},
  .switch_1 = {switch_budik},
  .dyn_button = {NULL},
  .symbol_button_1 = {NULL},
  .dyn_symbol_1 = {NULL},
  .dyn_select_box_1 = {NULL},
  .len_button_1 = 1,
  .len_button_2 = 0,
  .len_function_1 = 1,
  .len_switch_1 = 1,
  .len_dyn_button_1 = 0,
  .len_symbol_button_1 = 0,
  .len_dyn_symbol_1 = 0,
  .len_dyn_select_box_1 = 0,
  .idx = MENU_FUNKCE_SCREEN,
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
  .preload_function = returnnullfceargs,
};

#endif
