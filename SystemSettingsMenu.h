#ifndef __PROJECT_SYSTEM_SETTINGS_MENU_H
#define __PROJECT_SYSTEM_SETTINGS_MENU_H
#include "room_control_v3.h"

void button_nastaveni_default_value(uint16_t args1, uint16_t args2, uint8_t args3);
void display_element_show_about_device(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint16_t args1, uint8_t args2, char *text);
void button_nastaveni_reload_onclick(uint16_t args1, uint16_t args2, uint8_t args3);


uint8_t preload_display_setting_nrf(uint16_t args1, uint16_t args2, uint8_t args3);

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
  .args = NEW_MENU_NRF_SETTING,
  .onclick = MenuHistoryNextMenu,
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
  .font_size = 1,
  .args = 0,
  .onclick = button_nastaveni_default_value,
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
  .redraw_class = (1 << REDRAW_ONCE | 1 << REDRAW_CLASS_2 | 1 << REDRAW_CLASS_3),
  .onclick = nullfce,
  .enable_show = display_enable_show,
  .name = char_NULL,
};

const Element_Function_1 f_dialog_set_variable_nrf_channel PROGMEM = {
  .x = 50,
  .y = 40,
  .args = 0,
  .fnt_coordinate_xy = display_element_dialog_set_variable,
  .size_x = 140,
  .size_y = 40,
  .redraw_class = REDRAW_BUTTON,
  .onclick = nullfce,
  .enable_show = display_enable_show,
  .name = char_K,
};

const Element_Symbol_1 dialog_set_variable_plus_nrf_channel PROGMEM =  {
  .znak = '+',
  .x = 190,
  .y = 40,
  .size_x = 40,
  .size_y = 40,
  .znak_size = 2,
  .args = 0,
  .onclick = display_function_set_variable_plus,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};

const Element_Symbol_1 dialog_set_variable_minus_nrf_channel PROGMEM =  {
  .znak = '-',
  .x = 10,
  .y = 40,
  .size_x = 40,
  .size_y = 40,
  .znak_size = 2,
  .args = 0,
  .onclick = display_function_set_variable_minus,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};

const Element_Function_1 f_dialog_set_variable_nrf_power PROGMEM = {
  .x = 50,
  .y = 100,
  .args = 1,
  .fnt_coordinate_xy = display_element_dialog_set_variable,
  .size_x = 140,
  .size_y = 40,
  .redraw_class = REDRAW_BUTTON,
  .onclick = nullfce,
  .enable_show = display_enable_show,
  .name = char_P,
};

const Element_Symbol_1 dialog_set_variable_plus_nrf_power PROGMEM =  {
  .znak = '+',
  .x = 190,
  .y = 100,
  .size_x = 40,
  .size_y = 40,
  .znak_size = 2,
  .args = 1,
  .onclick = display_function_set_variable_plus,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};

const Element_Symbol_1 dialog_set_variable_minus_nrf_power PROGMEM =  {
  .znak = '-',
  .x = 10,
  .y = 100,
  .size_x = 40,
  .size_y = 40,
  .znak_size = 2,
  .args = 1,
  .onclick = display_function_set_variable_minus,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
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
  .redraw_class_3 = menu_redraw_date,
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
  .redraw_class_2 = menu_redraw_update_temp,
  .redraw_class_3 = menu_redraw_date,
  .preload_function = returnnullfceargs,
};







const Menu1 SetNRFMenu PROGMEM = {
  .name = nastaveni_nrf,
  .button_1 = {button_back},
  .button_2 = {NULL},
  .function_1 = {f_show_date, f_dialog_set_variable_nrf_channel, f_dialog_set_variable_nrf_power},
  .switch_1 = {NULL},
  .dyn_button = {NULL},
  .symbol_button_1 = {dialog_set_variable_plus_nrf_channel, dialog_set_variable_minus_nrf_channel, dialog_set_variable_plus_nrf_power, dialog_set_variable_minus_nrf_power},
  .dyn_symbol_1 = {NULL},
  .dyn_select_box_1 = {NULL},
  .len_button_1 = 1,
  .len_button_2 = 0,
  .len_function_1 = 3,
  .len_switch_1 = 0,
  .len_dyn_button_1 = 0,
  .len_symbol_button_1 = 4,
  .len_dyn_symbol_1 = 0,
  .len_dyn_select_box_1 = 0,
  .idx = NEW_MENU_NRF_SETTING,
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
  .preload_function = preload_display_setting_nrf,
};

#endif
