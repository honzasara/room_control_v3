#ifndef __PROJECT_SETTINGS_TIME_MENU_H
#define __PROJECT_SETTINGS_TIME_MENU_H
#include "room_control_v3.h"


const Element_Button_1 button_ntp_sync_time PROGMEM = {
  .name = text_ntp_sync_time,
  .x = 10,
  .y = 40,
  .size_x = 190,
  .size_y = 40,
  .font_size = 1,
  .args = 0,
  .onclick = button_click_ntp_sync_time,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};


const Menu1 MenuNastaveniCas PROGMEM = {
  .name = new_text_nastaveni_casu,
  .button_1 = {button_back, button_ntp_sync_time},
  .button_2 = {NULL},
  .function_1 = {NULL},
  .switch_1 = {NULL},
  .dyn_button = {NULL},
  .symbol_button_1 = {NULL},
  .dyn_symbol_1 = {NULL},
  .dyn_select_box_1 = {NULL},
  .len_button_1 = 2,
  .len_button_2 = 0,
  .len_function_1 = 0,
  .len_switch_1 = 0,
  .len_dyn_button_1 = 0,
  .len_symbol_button_1 = 0,
  .len_dyn_symbol_1 = 0,
  .len_dyn_select_box_1 = 0,
  .idx = NEW_MENU_NASTAVENI_CASU_SCREEN,
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
