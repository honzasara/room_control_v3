#ifndef __PROJECT_PERIFERIE_SETTINGS_MENU_H
#define __PROJECT_PERIFERIE_SETTINGS_MENU_H
#include "room_control_v3.h"
#include "UserSettings.h"


const Element_Button_1 button_nastaveni_onewire PROGMEM = {
  .name = nastaveni_onewire,
  .x = 10,
  .y = 40,
  .size_x = 190,
  .size_y = 40,
  .font_size = 2,
  .args = MENU_NASTAVENI_ONEWIRE,
  .onclick = MenuHistoryNextMenu,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};

const Element_Button_1 button_nastaveni_mqtt_topic PROGMEM = {
  .name = new_nastaveni_mqtt_topic,
  .x = 10,
  .y = 100,
  .size_x = 190,
  .size_y = 40,
  .font_size = 2,
  .args = MENU_LIST_NASTAVENI_RTDS,
  .onclick = MenuHistoryNextMenu,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};


const Element_Button_1 button_nastaveni_nrf_input PROGMEM = {
  .name = new_nastaveni_nrf_input,
  .x = 10,
  .y = 160,
  .size_x = 190,
  .size_y = 40,
  .font_size = 2,
  .args = 0, //MENU_LIST_NASTAVENI_RTDS,
  .onclick = nullfce, //MenuHistoryNextMenu,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};


const Menu1 PeriferieSettingsMenu PROGMEM = {
  .name = new_text_nastaveni_periferii_full,
  .button_1 = {button_back, button_nastaveni_onewire, button_nastaveni_mqtt_topic, button_nastaveni_nrf_input},
  .button_2 = {NULL},
  .function_1 = {f_show_date},
  .switch_1 = {NULL},
  .dyn_button = {NULL},
  .symbol_button_1 = {NULL},
  .dyn_symbol_1 = {NULL},
  .dyn_select_box_1 = {NULL},
  .len_button_1 = 4,
  .len_button_2 = 0,
  .len_function_1 = 1,
  .len_switch_1 = 0,
  .len_dyn_button_1 = 0,
  .len_symbol_button_1 = 0,
  .len_dyn_symbol_1 = 0,
  .len_dyn_select_box_1 = 0,
  .idx = NEW_MENU_PERIFERIE_NASTAVENI_SCREEN,
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
