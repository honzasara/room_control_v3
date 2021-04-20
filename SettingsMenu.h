#ifndef __PROJECT_SETTINGS_MENU_H
#define __PROJECT_SETTINGS_MENU_H
#include "room_control_v3.h"




const Element_Button_1 button_nastaveni_systemu PROGMEM = {
  .name = new_text_nastaveni_system,
  .x = 10,
  .y = 40,
  .size_x = 190,
  .size_y = 40,
  .font_size = 2,
  .args = NEW_MENU_SYSTEM_NASTAVENI_SCREEN,
  .onclick = MenuHistoryNextMenu,
  .redraw_class = (1 << REDRAW_ONCE),
  .enable_show = display_enable_show,
};

const Element_Button_1 button_nastaveni_periferie PROGMEM = {
  .name = new_text_nastaveni_periferie,
  .x = 10,
  .y = 100,
  .size_x = 190,
  .size_y = 40,
  .font_size = 2,
  .args = NEW_MENU_PERIFERIE_NASTAVENI_SCREEN,
  .onclick = MenuHistoryNextMenu,
  .redraw_class = (1 << REDRAW_ONCE),
  .enable_show = display_enable_show,
};

const Element_Button_1 button_nastaveni_zobrazeni PROGMEM = {
  .name = new_text_nastaveni_zobrazeni,
  .x = 10,
  .y = 160,
  .size_x = 190,
  .size_y = 40,
  .font_size = 2,
  .args =  NEW_MENU_DISPLAY_NASTAVENI_SCREEN,
  .onclick = MenuHistoryNextMenu,
  .redraw_class = (1 << REDRAW_ONCE),
  .enable_show = display_enable_show,
};

const Element_Button_1 button_nastaveni_regulator PROGMEM = {
  .name = new_text_nastaveni_regulatoru,
  .x = 280,
  .y = 40,
  .size_x = 190,
  .size_y = 40,
  .font_size = 2,
  .args = NEW_MENU_THERMOSTAT_SETTINGS,
  .onclick = MenuHistoryNextMenu,
  .redraw_class = (1 << REDRAW_ONCE),
  .enable_show = display_enable_show,
};

/*










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
*/



const Menu1 New_NastaveniMenu PROGMEM = {
  .name = nastaveni_text,
  .button_1 = {button_back, button_nastaveni_systemu, button_nastaveni_regulator, button_nastaveni_zobrazeni, button_nastaveni_periferie},
  .button_2 = {NULL},
  .function_1 = {f_show_date},
  .switch_1 = {NULL},
  .dyn_button = {NULL},
  .symbol_button_1 = {NULL},
  .dyn_symbol_1 = {NULL},
  .dyn_select_box_1 = {NULL},
  .len_button_1 = 5,
  .len_button_2 = 0,
  .len_function_1 = 1,
  .len_switch_1 = 0,
  .len_dyn_button_1 = 0,
  .len_symbol_button_1 = 0,
  .len_dyn_symbol_1 = 0,
  .len_dyn_select_box_1 = 0,
  .idx = NEW_MENU_NASTAVENI_SCREEN,
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

#endif
