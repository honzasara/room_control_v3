#ifndef __PROJECT_MAIN_MENU_H
#define __PROJECT_MAIN_MENU_H
#include "room_control_v3.h"


/* primitivni funkce*/
/// tato funkce slozi ke zjisteni stavu tlacitka
uint8_t button_status_default_ring_term_has_mode(uint16_t args1, uint16_t args2, uint8_t args3);
/// tato funkce nastavi termostat do pozadovaneho modu
void button_click_default_term_set_mode(uint16_t args1, uint16_t args2, uint8_t args3);

uint8_t menu_redraw_date(uint16_t args1, uint16_t args2, uint8_t args3);

void display_element_show_time_1(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint8_t args1, uint8_t args2, char *text);
void display_element_show_time_decorate_1(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint8_t args1, uint8_t args2, char *text);
void display_element_show_temp_1(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint8_t args1, uint8_t args2, char *text);
void display_element_show_temp_decorate_1(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint8_t args1, uint8_t args2, char *text);


const Element_Function_1 f_show_temp PROGMEM = {
  .x = 10,
  .y = 130,
  .args = 0,
  .fnt_coordinate_xy = display_element_show_temp_1,
  .size_x = 0,
  .size_y = 0,
  .redraw_class = (1 << REDRAW_CLASS_2 | 1 << REDRAW_FORCE),
  .onclick = nullfce,
  .enable_show = display_enable_show,
  .name = char_NULL,
};

const Element_Function_1 f_show_temp_decorate PROGMEM = {
  .x = 10,
  .y = 130,
  .args = 0,
  .fnt_coordinate_xy = display_element_show_temp_decorate_1,
  .size_x = 0,
  .size_y = 0,
  .redraw_class = (1 << REDRAW_ONCE ),
  .onclick = nullfce,
  .enable_show = display_enable_show,
  .name = char_NULL,
};

const Element_Function_1 f_show_time PROGMEM = {
  .x = 10,
  .y = 30,
  .args = 0,
  .fnt_coordinate_xy = display_element_show_time_1,
  .size_x = 0,
  .size_y = 0,
  .redraw_class = (1 << REDRAW_CLASS_0 | 1 << REDRAW_FORCE),
  .onclick = nullfce,
  .enable_show = display_enable_show,
  .name = char_NULL,
};

const Element_Function_1 f_show_time_decorate PROGMEM = {
  .x = 10,
  .y = 30,
  .args = 0,
  .fnt_coordinate_xy = display_element_show_time_decorate_1,
  .size_x = 0,
  .size_y = 0,
  .redraw_class = (1 << REDRAW_ONCE ),
  .onclick = nullfce,
  .enable_show = display_enable_show,
  .name = char_NULL,
};


const Element_Button_2 button_term_state_off PROGMEM = {
  .name = text_button_term_off,
  .x = 10,
  .y = 220,
  .size_x = 82,
  .size_y = 60,
  .font_size = 3,
  .color_active = GRAY,
  .color_inactive = WHITE,
  .args = TERM_MODE_OFF,
  .onclick = button_click_default_term_set_mode,
  .get_status_fnt = button_status_default_ring_term_has_mode,
  .redraw_class = (1 << REDRAW_CLASS_1 | 1 << REDRAW_ONCE ),
  .enable_show = display_enable_show,
};

const Element_Button_2 button_term_state_max PROGMEM = {
  .name = text_button_term_max,
  .x = 100,
  .y = 220,
  .size_x = 82,
  .size_y = 60,
  .font_size = 3,
  .color_active = RED,
  .color_inactive = WHITE,
  .args = TERM_MODE_MAX,
  .onclick = button_click_default_term_set_mode,
  .get_status_fnt = button_status_default_ring_term_has_mode,
  .redraw_class = (1 << REDRAW_CLASS_1 | 1 << REDRAW_ONCE ),
  .enable_show = display_enable_show,
};

const Element_Button_2 button_term_state_min PROGMEM = {
  .name = text_button_term_min,
  .x = 190,
  .y = 220,
  .size_x = 82,
  .size_y = 60,
  .font_size = 3,
  .color_active = BLUE,
  .color_inactive = WHITE,
  .args = TERM_MODE_MIN,
  .onclick = button_click_default_term_set_mode,
  .get_status_fnt = button_status_default_ring_term_has_mode,
  .redraw_class = (1 << REDRAW_CLASS_1  | 1 << REDRAW_ONCE ),
  .enable_show = display_enable_show,
};

const Element_Button_2 button_term_state_prog PROGMEM = {
  .name = text_button_term_prog,
  .x = 280,
  .y = 220,
  .size_x = 82,
  .size_y = 60,
  .font_size = 3,
  .color_active = YELLOW,
  .color_inactive = WHITE,
  .args = TERM_MODE_PROG,
  .onclick = button_click_default_term_set_mode,
  .get_status_fnt = button_status_default_ring_term_has_mode,
  .redraw_class = (1 << REDRAW_CLASS_1  | 1 << REDRAW_ONCE ),
  .enable_show = display_enable_show,
};

const Element_Button_2 button_term_state_man PROGMEM = {
  .name = text_button_term_man,
  .x = 370,
  .y = 220,
  .size_x = 82,
  .size_y = 60,
  .font_size = 3,
  .color_active = GREEN,
  .color_inactive = WHITE,
  .args = TERM_MODE_MAN,
  .onclick = button_click_default_term_set_mode,
  .get_status_fnt = button_status_default_ring_term_has_mode,
  .redraw_class = (1 << REDRAW_CLASS_1  | 1 << REDRAW_ONCE ),
  .enable_show = display_enable_show,
};

const Element_Button_1 button_global_setting PROGMEM = {
  .name = nastaveni_text,
  .x = 290,
  .y = 30,
  .size_x = 170,
  .size_y = 40,
  .font_size = 2,
  .args = NEW_MENU_NASTAVENI_SCREEN,
  .onclick = MenuHistoryNextMenu,
  .redraw_class = (1 << REDRAW_FORCE | 1 << REDRAW_ONCE),
  .enable_show = display_enable_show,
};

const Element_Button_1 button_global_functions PROGMEM = {
  .name = funkce_text,
  .x = 290,
  .y = 90,
  .size_x = 170,
  .size_y = 40,
  .font_size = 2,
  .args = MENU_FUNKCE_SCREEN,
  .onclick = nullfce, //MenuHistoryNextMenu,
  .redraw_class = (1 << REDRAW_FORCE | 1 << REDRAW_ONCE),
  .enable_show = display_enable_show,
};

const Element_Button_1 button_global_regulator PROGMEM = {
  .name = regulator_text,
  .x = 290,
  .y = 150,
  .size_x = 170,
  .size_y = 40,
  .font_size = 2,
  .args = MENU_REGULATOR,
  .onclick = nullfce, //MenuHistoryNextMenu,
  .redraw_class = (1 << REDRAW_FORCE | 1 << REDRAW_ONCE),
  .enable_show = display_enable_show,
};

const Menu1 HlavniMenu PROGMEM = {
  .name = term_title,
  .button_1 = {button_global_regulator, button_global_functions, button_global_setting},
  .button_2 = {button_term_state_off, button_term_state_max, button_term_state_min, button_term_state_prog, button_term_state_man},
  .function_1 = {f_show_temp, f_show_date, f_show_time, f_show_time_decorate, f_show_temp_decorate},
  .switch_1 = {NULL},
  .dyn_button = {NULL},
  .symbol_button_1 = {NULL},
  .dyn_symbol_1 = {NULL},
  .dyn_select_box_1 = {NULL},
  .len_button_1 = 3,
  .len_button_2 = 5,
  .len_function_1 = 5,
  .len_switch_1 = 0,
  .len_dyn_button_1 = 0,
  .len_symbol_button_1 = 0,
  .len_dyn_symbol_1 = 0,
  .len_dyn_select_box_1 = 0,
  .idx = MENU_DEFAULT_SCREEN,
  .x = 0,
  .y = 0,
  .size_x = 480,
  .size_y = 320,
  .atributes = (1 << MENU_ATTRIBUTES_CLEAN_DISPLAY),
  .color_background = WHITE,
  .redraw_class = (1 << REDRAW_ONCE ),
  .redraw_class_0 = menu_redraw_time05s,
  .redraw_class_1 = menu_redraw_change_term_mode,
  .redraw_class_2 = menu_redraw_update_temp,
  .redraw_class_3 = menu_redraw_date,
  .preload_function = returnnullfceargs,
};

#endif
