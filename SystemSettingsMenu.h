#ifndef __PROJECT_SYSTEM_SETTINGS_MENU_H
#define __PROJECT_SYSTEM_SETTINGS_MENU_H
#include "room_control_v3.h"

void click_button_default_value(uint16_t args1, uint16_t args2, uint8_t args3);
void display_element_show_about_device(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint8_t args1, uint8_t args2, char *text);
void button_nastaveni_reload_onclick(uint16_t args1, uint16_t args2, uint8_t args3);

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

const Element_Button_1 button_nastaveni_mqtt PROGMEM = {
  .name = new_text_nastaveni_mqtt_pripojeni,
  .x = 10,
  .y = 160,
  .size_x = 190,
  .size_y = 40,
  .font_size = 2,
  .args = NEW_MENU_NASTAVENI_MQTT_SCREEN,
  .onclick = MenuHistoryNextMenu,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};

const Element_Button_1 button_nastaveni_default PROGMEM = {
  .name = nastaveni_default,
  .x = 280,
  .y = 160,
  .size_x = 190,
  .size_y = 40,
  .font_size = 2,
  .args = 0,
  .onclick = click_button_default_value,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};

const Element_Button_1 button_nastaveni_reload PROGMEM = {
  .name = new_text_nastaveni_reload,
  .x = 280,
  .y = 220,
  .size_x = 190,
  .size_y = 40,
  .font_size = 1,
  .args = 0,
  .onclick = button_nastaveni_reload_onclick,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};

const Element_Button_1 button_nastaveni_casu PROGMEM = {
  .name = new_text_nastaveni_casu,
  .x = 280,
  .y = 40,
  .size_x = 190,
  .size_y = 40,
  .font_size = 2,
  .args = NEW_MENU_NASTAVENI_CASU_SCREEN,
  .onclick = MenuHistoryNextMenu,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};

const Element_Button_1 button_about_device PROGMEM = {
  .name = new_text_o_zarizeni,
  .x = 280,
  .y = 100,
  .size_x = 190,
  .size_y = 40,
  .font_size = 2,
  .args = NEW_MENU_ABOUT_DEVICE,
  .onclick = MenuHistoryNextMenu,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};


const Element_Function_1 f_show_about_device PROGMEM = {
  .x = 10,
  .y = 30,
  .args = 0,
  .fnt_coordinate_xy = display_element_show_about_device,
  .size_x = 0,
  .size_y = 0,
  .redraw_class = (1 << REDRAW_FORCE),
  .onclick = nullfce,
  .enable_show = display_enable_show,
  .name = char_NULL,
};

const Menu1 SystemSettingsMenu PROGMEM = {
  .name = new_text_nastaveni_system_full,
  .button_1 = {button_back, button_nastaveni_site, button_nastaveni_nrf, button_nastaveni_default, button_nastaveni_casu, button_about_device, button_nastaveni_mqtt, button_nastaveni_reload},
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
  .idx = NEW_MENU_SYSTEM_NASTAVENI_SCREEN,
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

const Menu1 AboutDeviceMenu PROGMEM = {
  .name = new_text_o_zarizeni,
  .button_1 = {button_back},
  .button_2 = {NULL},
  .function_1 = {f_show_date, f_show_about_device},
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
  .idx = NEW_MENU_ABOUT_DEVICE,
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
