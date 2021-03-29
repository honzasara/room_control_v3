#ifndef __PROJECT_NETWORK_SETTINGS_MENU_H
#define __PROJECT_NETWORK_SETTINGS_MENU_H
#include "room_control_v3.h"

/***************************************************************/
/*
 *  primitivni funkce
 * 
 */


void display_element_show_network_detail(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint8_t args1, uint8_t args2, char *text);
void button_set_network_test_connection_onclick(uint16_t args1, uint16_t args2, uint8_t args3);

const Element_Button_1 button_set_network_ip PROGMEM = {
  .name = new_text_set_network_ip,
  .x = 280,
  .y = 20,
  .size_x = 190,
  .size_y = 30,
  .font_size = 1,
  .args = 0, //MENU_SELECT_DEFAULT_TEMP,
  .onclick = nullfce, //MenuHistoryNextMenu,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};

const Element_Button_1 button_set_network_mask PROGMEM = {
  .name = new_text_set_network_mask,
  .x = 280,
  .y = 60,
  .size_x = 190,
  .size_y = 30,
  .font_size = 1,
  .args = 0, //MENU_SELECT_DEFAULT_TEMP,
  .onclick = nullfce, //MenuHistoryNextMenu,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};

const Element_Button_1 button_set_network_gw PROGMEM = {
  .name = new_text_set_network_gw,
  .x = 280,
  .y = 100,
  .size_x = 190,
  .size_y = 30,
  .font_size = 1,
  .args = 0, //MENU_SELECT_DEFAULT_TEMP,
  .onclick = nullfce, //MenuHistoryNextMenu,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};

const Element_Button_1 button_set_network_name PROGMEM = {
  .name = new_text_set_network_name,
  .x = 280,
  .y = 140,
  .size_x = 190,
  .size_y = 30,
  .font_size = 1,
  .args = 0, //MENU_SELECT_DEFAULT_TEMP,
  .onclick = nullfce, //MenuHistoryNextMenu,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};
 
const Element_Button_1 button_set_network_dns PROGMEM = {
  .name = new_text_set_network_dns,
  .x = 280,
  .y = 180,
  .size_x = 190,
  .size_y = 30,
  .font_size = 1,
  .args = 0, //MENU_SELECT_DEFAULT_TEMP,
  .onclick = nullfce, //MenuHistoryNextMenu,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};

const Element_Button_1 button_set_network_mac PROGMEM = {
  .name = new_text_set_network_mac,
  .x = 280,
  .y = 220,
  .size_x = 190,
  .size_y = 30,
  .font_size = 1,
  .args = 0, //MENU_SELECT_DEFAULT_TEMP,
  .onclick = nullfce, //MenuHistoryNextMenu,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};


const Element_Button_1 button_set_network_dhcp PROGMEM = {
  .name = new_text_set_network_dhcp,
  .x = 280,
  .y = 260,
  .size_x = 190,
  .size_y = 30,
  .font_size = 1,
  .args = 0, //MENU_SELECT_DEFAULT_TEMP,
  .onclick = nullfce, //MenuHistoryNextMenu,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};

const Element_Button_1 button_set_network_test_connection PROGMEM = {
  .name = new_text_set_network_test_connection,
  .x = 10,
  .y = 170,
  .size_x = 190,
  .size_y = 40,
  .font_size = 1,
  .args = 0, 
  .onclick = button_set_network_test_connection_onclick,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};

const Element_Function_1 f_show_network_detail PROGMEM = {
  .x = 10,
  .y = 40,
  .args = 0,
  .fnt_coordinate_xy = display_element_show_network_detail,
  .size_x = 200,
  .size_y = 120,
  .redraw_class = (1 << REDRAW_FORCE),
  .onclick = nullfce,
  .enable_show = display_enable_show,
  .name = char_NULL,
};






const Menu1 MenuNastaveniSite PROGMEM = {
  .name = nastaveni_site,
  .button_1 = {button_back, button_set_network_mac, button_set_network_dns, button_set_network_name, button_set_network_gw, button_set_network_mask, button_set_network_ip, button_set_network_dhcp,button_set_network_test_connection},
  .button_2 = {NULL},
  .function_1 = {f_show_network_detail},
  .switch_1 = {NULL},
  .dyn_button = {NULL},
  .symbol_button_1 = {NULL},
  .dyn_symbol_1 = {NULL},
  .dyn_select_box_1 = {NULL},
  .len_button_1 = 9,
  .len_button_2 = 0,
  .len_function_1 = 1,
  .len_switch_1 = 0,
  .len_dyn_button_1 = 0,
  .len_symbol_button_1 = 0,
  .len_dyn_symbol_1 = 0,
  .len_dyn_select_box_1 = 0,
  .idx = MENU_NASTAVENI_SITE,
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
