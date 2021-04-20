#ifndef __PROJECT_PID_DIALOG_MENU_H
#define __PROJECT_PID_DIALOG_MENU_H
#include "room_control_v3.h"

void button_click_ring_term_set_pid_default(uint16_t args1, uint16_t args2, uint8_t args3);

uint8_t preload_pid_menu(uint16_t args1, uint16_t args2, uint8_t args3);

const Element_Button_1 button_term_ring_set_pid_default PROGMEM = {
  .name = text_set_default,
  .x = 10,
  .y = 40,
  .size_x = 190,
  .size_y = 40,
  .font_size = 1,
  .args = 0,
  .onclick = button_click_ring_term_set_pid_default,
  .redraw_class = (1 << REDRAW_FORCE),
  .enable_show = display_enable_show,
};

///p i d t
const Element_Function_1 f_dialog_set_variable_pid_p PROGMEM = {
  .x = 270,
  .y = 40,
  .args = DIALOG_SET_VARIABLE_PID_P,
  .fnt_coordinate_xy = display_element_dialog_set_variable,
  .size_x = 140,
  .size_y = 40,
  .redraw_class = REDRAW_BUTTON,
  .onclick = nullfce,
  .enable_show = display_enable_show,
  .name = char_P,
};
const Element_Symbol_1 dialog_set_variable_plus_pid_p PROGMEM =  {
  .znak = '+',
  .x = 410,
  .y = 40,
  .size_x = 40,
  .size_y = 40,
  .znak_size = 2,
  .args = DIALOG_SET_VARIABLE_PID_P,
  .onclick = display_function_set_variable_plus,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};
const Element_Symbol_1 dialog_set_variable_minus_pid_p PROGMEM =  {
  .znak = '-',
  .x = 230,
  .y = 40,
  .size_x = 40,
  .size_y = 40,
  .znak_size = 2,
  .args = DIALOG_SET_VARIABLE_PID_P,
  .onclick = display_function_set_variable_minus,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};


const Element_Function_1 f_dialog_set_variable_pid_i PROGMEM = {
  .x = 270,
  .y = 90,
  .args = DIALOG_SET_VARIABLE_PID_I,
  .fnt_coordinate_xy = display_element_dialog_set_variable,
  .size_x = 140,
  .size_y = 40,
  .redraw_class = REDRAW_BUTTON,
  .onclick = nullfce,
  .enable_show = display_enable_show,
  .name = char_I,
};
const Element_Symbol_1 dialog_set_variable_plus_pid_i PROGMEM =  {
  .znak = '+',
  .x = 410,
  .y = 90,
  .size_x = 40,
  .size_y = 40,
  .znak_size = 2,
  .args = DIALOG_SET_VARIABLE_PID_I,
  .onclick = display_function_set_variable_plus,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};
const Element_Symbol_1 dialog_set_variable_minus_pid_i PROGMEM =  {
  .znak = '-',
  .x = 230,
  .y = 90,
  .size_x = 40,
  .size_y = 40,
  .znak_size = 2,
  .args = DIALOG_SET_VARIABLE_PID_I,
  .onclick = display_function_set_variable_minus,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};


const Element_Function_1 f_dialog_set_variable_pid_d PROGMEM = {
  .x = 270,
  .y = 140,
  .args = DIALOG_SET_VARIABLE_PID_D,
  .fnt_coordinate_xy = display_element_dialog_set_variable,
  .size_x = 140,
  .size_y = 40,
  .redraw_class = REDRAW_BUTTON,
  .onclick = nullfce,
  .enable_show = display_enable_show,
  .name = char_D,
};
const Element_Symbol_1 dialog_set_variable_plus_pid_d PROGMEM =  {
  .znak = '+',
  .x = 410,
  .y = 140,
  .size_x = 40,
  .size_y = 40,
  .znak_size = 2,
  .args = DIALOG_SET_VARIABLE_PID_D,
  .onclick = display_function_set_variable_plus,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};
const Element_Symbol_1 dialog_set_variable_minus_pid_d PROGMEM =  {
  .znak = '-',
  .x = 230,
  .y = 140,
  .size_x = 40,
  .size_y = 40,
  .znak_size = 2,
  .args = DIALOG_SET_VARIABLE_PID_D,
  .onclick = display_function_set_variable_minus,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};

const Element_Function_1 f_dialog_set_variable_pid_t PROGMEM = {
  .x = 270,
  .y = 190,
  .args = DIALOG_SET_VARIABLE_PID_T,
  .fnt_coordinate_xy = display_element_dialog_set_variable,
  .size_x = 140,
  .size_y = 40,
  .redraw_class = REDRAW_BUTTON,
  .onclick = nullfce,
  .enable_show = display_enable_show,
  .name = char_T,
};
const Element_Symbol_1 dialog_set_variable_plus_pid_t PROGMEM =  {
  .znak = '+',
  .x = 410,
  .y = 190,
  .size_x = 40,
  .size_y = 40,
  .znak_size = 2,
  .args = DIALOG_SET_VARIABLE_PID_T,
  .onclick = display_function_set_variable_plus,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};
const Element_Symbol_1 dialog_set_variable_minus_pid_t PROGMEM =  {
  .znak = '-',
  .x = 230,
  .y = 190,
  .size_x = 40,
  .size_y = 40,
  .znak_size = 2,
  .args = DIALOG_SET_VARIABLE_PID_T,
  .onclick = display_function_set_variable_minus,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};

const Menu1 DialogSelectPIDSensor PROGMEM = {
  .name = text_pid_regulator,
  .button_1 = {button_back, button_term_ring_set_pid_default},
  .button_2 = {NULL},
  .function_1 = {f_dialog_set_variable_pid_p, f_dialog_set_variable_pid_i, f_dialog_set_variable_pid_d, f_dialog_set_variable_pid_t},
  .switch_1 = {NULL},
  .dyn_button = {NULL},
  .symbol_button_1 = {dialog_set_variable_plus_pid_p, dialog_set_variable_minus_pid_p, dialog_set_variable_plus_pid_i, dialog_set_variable_minus_pid_i, dialog_set_variable_plus_pid_d, dialog_set_variable_minus_pid_d, dialog_set_variable_plus_pid_t, dialog_set_variable_minus_pid_t},
  .dyn_symbol_1 = {NULL},
  .dyn_select_box_1 = {NULL},
  .len_button_1 = 2,
  .len_button_2 = 0,
  .len_function_1 = 4,
  .len_switch_1 = 0,
  .len_dyn_button_1 = 0,
  .len_symbol_button_1 = 8,
  .len_dyn_symbol_1 = 0,
  .len_dyn_select_box_1 = 0,
  .idx = MENU_NASTAVENI_SELECT_PID_PARAMETRS,
  .x = 10,
  .y = 10,
  .size_x = 460,
  .size_y = 300,
  .atributes = (1 << MENU_ATTRIBUTES_FILL_COLOR_RECTANGLE | 1 << MENU_ATTRIBUTES_DECORATE_MENU),
  .color_background = YELLOW,
  .redraw_class = (1 << REDRAW_FORCE),
  .redraw_class_0 = returnnullfceargs,
  .redraw_class_1 = returnnullfceargs,
  .redraw_class_2 = returnnullfceargs,
  .redraw_class_3 = returnnullfceargs,
  .preload_function = preload_pid_menu,
};

#endif
