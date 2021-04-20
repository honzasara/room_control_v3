#ifndef __PROJECT_NASTAVENI_MQTT_MENU_H
#define __PROJECT_NASTAVENI_MQTT_MENU_H
#include "room_control_v3.h"

void button_set_mqtt_broker_onclick(uint16_t args1, uint16_t args2, uint8_t args3);
void button_set_mqtt_user_onclick(uint16_t args1, uint16_t args2, uint8_t args3);
void button_set_mqtt_pass_onclick(uint16_t args1, uint16_t args2, uint8_t args3);
void button_check_mqtt_connection_onclick(uint16_t args1, uint16_t args2, uint8_t args3);


const Element_Button_1 button_set_mqtt_broker PROGMEM = {
  .name = new_text_mqtt_broker,
  .x = 10,
  .y = 40,
  .size_x = 190,
  .size_y = 40,
  .font_size = 1,
  .args = 0,
  .onclick = button_set_mqtt_broker_onclick,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};


const Element_Button_1 button_set_mqtt_user PROGMEM = {
  .name = new_text_mqtt_user,
  .x = 10,
  .y = 100,
  .size_x = 190,
  .size_y = 40,
  .font_size = 1,
  .args = 0,
  .onclick = button_set_mqtt_user_onclick,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};

const Element_Button_1 button_set_mqtt_pass PROGMEM = {
  .name = new_text_mqtt_pass,
  .x = 10,
  .y = 160,
  .size_x = 190,
  .size_y = 40,
  .font_size = 1,
  .args = 0,
  .onclick = button_set_mqtt_pass_onclick,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};

const Element_Button_1 button_check_mqtt_connection PROGMEM = {
  .name = new_text_mqtt_check,
  .x = 280,
  .y = 40,
  .size_x = 190,
  .size_y = 40,
  .font_size = 1,
  .args = 0,
  .onclick = button_check_mqtt_connection_onclick,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};


const Menu1 MenuNastaveniMQTT PROGMEM = {
  .name = new_text_nastaveni_mqtt_pripojeni,
  .button_1 = {button_back, button_set_mqtt_broker, button_set_mqtt_user, button_set_mqtt_pass, button_check_mqtt_connection},
  .button_2 = {NULL},
  .function_1 = {f_show_date},
  .switch_1 = {NULL},
  .dyn_button = {NULL},
  .symbol_button_1 = {NULL},
  .dyn_symbol_1 = {NULL},
  .dyn_select_box_1 = {NULL},
  .len_button_1 = 6,
  .len_button_2 = 0,
  .len_function_1 = 1,
  .len_switch_1 = 0,
  .len_dyn_button_1 = 0,
  .len_symbol_button_1 = 0,
  .len_dyn_symbol_1 = 0,
  .len_dyn_select_box_1 = 0,
  .idx = NEW_MENU_NASTAVENI_MQTT_SCREEN,
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
