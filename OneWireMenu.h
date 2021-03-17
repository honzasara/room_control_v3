#ifndef __PROJECT_ONE_WIRE_MENU_H
#define __PROJECT_ONE_WIRE_MENU_H
#include "room_control_v3.h"


void get_function_one_wire_associate_or_setting_text_button(uint8_t args1, uint8_t args2, uint8_t args3, char *line1, char *line2);
uint8_t get_function_one_wire_last_index_for_menu(uint16_t idx, uint16_t args2, uint8_t args3);
void click_tds_associate_or_setting_onewire(uint16_t args1, uint16_t args2, uint8_t idx);


const Element_Function_1 f_vertical_slider_one_wire_menu PROGMEM = {
  .x = 410,
  .y = 72,
  .args = MENU_SLIDER_ONE_WIRE,
  .fnt_coordinate_xy = display_element_vertical_slider,
  .size_x = 40,
  .size_y = 126,
  .redraw_class = REDRAW_BUTTON,
  .onclick = nullfce,
  .enable_show = display_enable_show,
  .name = char_NULL,
};
const Element_Symbol_1 slider_menu_plus_one_wire_menu PROGMEM =  {
  .znak = '+',
  .x = 410,
  .y = 30,
  .size_x = 40,
  .size_y = 40,
  .znak_size = 2,
  .args = MENU_SLIDER_ONE_WIRE,
  .onclick = display_function_vertical_slider_dec,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};
const Element_Symbol_1 slider_menu_minus_one_wire_menu PROGMEM =  {
  .znak = '-',
  .x = 410,
  .y = 200,
  .size_x = 40,
  .size_y = 40,
  .znak_size = 2,
  .args = MENU_SLIDER_ONE_WIRE,
  .onclick = display_function_vertical_slider_inc,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};


const Element_Dyn_Button_1 onewire_associace_button PROGMEM = {
  .first_x = 230,
  .first_y = 35,
  .size_x = 170,
  .size_y = 55,
  .font_size = 1,
  .step_x = 60, .step_y = 60,
  .direction = VERTICAL,
  .max_items_count = 3,
  .max_row_count = 1,
  .slider_args = MENU_SLIDER_ONE_WIRE,
  .args = INDEX_DYN_MENU_ASSOCIATE_ONEWIRE,
  .get_status_string = get_function_one_wire_associate_or_setting_text_button,
  .dyn_button_onclick =  click_tds_associate_or_setting_onewire,
  .function_for_max_items = get_function_one_wire_last_index_for_menu,
  .redraw_class = REDRAW_BUTTON,
};

const Menu1 OneWireMenu PROGMEM = {
  .name = nastaveni_onewire,
  .button_1 = {button_back},
  .button_2 = {NULL},
  .function_1 = {f_show_date, f_vertical_slider_one_wire_menu, f_show_rectangle_decorate},
  .switch_1 = {NULL},
  .dyn_button = {onewire_associace_button},
  .symbol_button_1 = {slider_menu_plus_one_wire_menu, slider_menu_minus_one_wire_menu},
  .dyn_symbol_1 = {NULL},
  .dyn_select_box_1 = {NULL},
  .len_button_1 = 1,
  .len_button_2 = 0,
  .len_function_1 = 3,
  .len_switch_1 = 0,
  .len_dyn_button_1 = 1,
  .len_symbol_button_1 = 2,
  .len_dyn_symbol_1 = 0,
  .len_dyn_select_box_1 = 0,
  .idx = MENU_NASTAVENI_ONEWIRE,
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
