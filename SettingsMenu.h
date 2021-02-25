#ifndef __PROJECT_SETTINGS_MENU_H
#define __PROJECT_SETTINGS_MENU_H
#include "room_control_v3.h"


const Element_Button_1 button_nastaveni_site PROGMEM = {
  .name = nastaveni_site,
  .x = 10,
  .y = 40,
  .size_x = 190,
  .size_y = 40,
  .font_size = 2,
  .args = MENU_NASTAVENI_SITE,
  .onclick = MenuHistoryNextMenu,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};

const Element_Button_1 button_nastaveni_nrf PROGMEM = {
  .name = nastaveni_nrf,
  .x = 10,
  .y = 100,
  .size_x = 190,
  .size_y = 40,
  .font_size = 2,
  .args = 0,
  .onclick = nullfce,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};

const Element_Button_1 button_nastaveni_default PROGMEM = {
  .name = nastaveni_default,
  .x = 10,
  .y = 160,
  .size_x = 190,
  .size_y = 40,
  .font_size = 2,
  .args = 0,
  .onclick = nullfce,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};


const Element_Button_1 button_nastaveni_onewire PROGMEM = {
  .name = nastaveni_onewire,
  .x = 280,
  .y = 40,
  .size_x = 190,
  .size_y = 40,
  .font_size = 2,
  .args = MENU_NASTAVENI_ONEWIRE,
  .onclick = MenuHistoryNextMenu,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};

const Element_Button_1 button_nastaveni_rtds PROGMEM = {
  .name = nastaveni_rtds,
  .x = 280,
  .y = 100,
  .size_x = 190,
  .size_y = 40,
  .font_size = 2,
  .args = MENU_LIST_NASTAVENI_RTDS,
  .onclick = MenuHistoryNextMenu,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};

const Element_Button_1 button_default_show_temp PROGMEM = {
  .name = text_select_default_temp,
  .x = 280,
  .y = 160,
  .size_x = 190,
  .size_y = 40,
  .font_size = 1,
  .args = MENU_SELECT_DEFAULT_TEMP,
  .onclick = MenuHistoryNextMenu,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};

const Element_Button_1 button_nastaveni_programator PROGMEM = {
  .name = text_nastaveni_programator,
  .x = 280,
  .y = 220,
  .size_x = 190,
  .size_y = 40,
  .font_size = 1,
  .args = MENU_NASTAVENI_PROGRAMATOR,
  .onclick = MenuHistoryNextMenu,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};


const Menu1 NastaveniMenu PROGMEM = {
  .name = nastaveni_text,
  .button_1 = {button_nastaveni_site, button_nastaveni_nrf, button_nastaveni_default, button_back, button_nastaveni_rtds, button_nastaveni_onewire, button_default_show_temp, button_nastaveni_programator},
  .button_2 = {NULL},
  .function_1 = {f_show_date},
  .switch_1 = {NULL},
  .dyn_button = {NULL},
  .symbol_button_1 = {NULL},
  .dyn_symbol_1 = {NULL},
  .dyn_select_box_1 = {NULL},
  .len_button_1 = 8,
  .len_button_2 = 0,
  .len_function_1 = 1,
  .len_switch_1 = 0,
  .len_dyn_button_1 = 0,
  .len_symbol_button_1 = 0,
  .len_dyn_symbol_1 = 0,
  .len_dyn_select_box_1 = 0,
  .idx = MENU_NASTAVENI_SCREEN,
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
