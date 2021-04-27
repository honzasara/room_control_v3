#ifndef __PROJECT_RTDS_MENU_H
#define __PROJECT_RTDS_MENU_H
#include "room_control_v3.h"


void click_rtds_setting_sensor(uint16_t args1, uint16_t args2, uint8_t loop_idx);


void click_rtds_deassociate_onewire(uint16_t args1, uint16_t idx, uint8_t args3);
void click_rtds_subscribe(uint16_t args1, uint16_t idx, uint8_t args3);

uint8_t get_function_rtds_active_items(uint16_t args1, uint16_t idx, uint8_t args3);
uint8_t get_function_rtds_last_index_for_menu(uint16_t args1, uint16_t args2, uint8_t args3);
void get_function_rtds_text_button(uint8_t args1, uint8_t args2, uint8_t args3, char *line1, char *line2);


void display_element_show_rtds_info_dynamics(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint16_t args1, uint8_t args2, char *text);


const Element_Button_1 button_rtds_novy PROGMEM = {
  .name = text_rtds_novy,
  .x = 10,
  .y = 40,
  .size_x = 190,
  .size_y = 40,
  .font_size = 1,
  .args = 0,
  .onclick = click_rtds_add_sensor,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};

const Element_Button_1 button_rtds_name PROGMEM = {
  .name = nastaveni_name_sensor,
  .x = 280,
  .y = 40,
  .size_x = 190,
  .size_y = 40,
  .font_size = 2,
  .args = 0,
  .onclick = display_menu_rtds_update_name,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};


const Element_Button_1 button_rtds_delete PROGMEM = {
  .name = nastaveni_delete_sensor,
  .x = 280,
  .y = 90,
  .size_x = 190,
  .size_y = 40,
  .font_size = 2,
  .args = 0,
  .onclick = click_rtds_deassociate_onewire,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};


const Element_Button_1 button_rtds_subscribe PROGMEM = {
  .name = nastaveni_rtds_subscribe,
  .x = 280,
  .y = 140,
  .size_x = 190,
  .size_y = 40,
  .font_size = 2,
  .args = 0,
  .onclick = click_rtds_subscribe,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};


const Element_Function_1 f_show_rtds_info_dynamics PROGMEM = {
  .x = 20,
  .y = 20,
  .args = 0,
  .fnt_coordinate_xy = display_element_show_rtds_info_dynamics,
  .size_x = 0,
  .size_y = 0,
  .redraw_class = REDRAW_CLASS_SHOW,
  .onclick = nullfce,
  .enable_show = display_enable_show,
  .name = char_NULL,
};


const Element_Function_1 f_vertical_slider_rtds PROGMEM = {
  .x = 410,
  .y = 72,
  .args = MENU_SLIDER_RTDS,
  .fnt_coordinate_xy = display_element_vertical_slider,
  .size_x = 40,
  .size_y = 126,
  .redraw_class = REDRAW_BUTTON,
  .onclick = nullfce,
  .enable_show = display_enable_show,
  .name = char_NULL,
};

const Element_Symbol_1 slider_menu_plus_rtds PROGMEM =  {
  .znak = '+',
  .x = 410,
  .y = 30,
  .size_x = 40,
  .size_y = 40,
  .znak_size = 2,
  .args = MENU_SLIDER_RTDS,
  .onclick = display_function_vertical_slider_dec,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};

const Element_Symbol_1 slider_menu_minus_rtds PROGMEM =  {
  .znak = '-',
  .x = 410,
  .y = 200,
  .size_x = 40,
  .size_y = 40,
  .znak_size = 2,
  .args = MENU_SLIDER_RTDS,
  .onclick = display_function_vertical_slider_inc,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};

const Element_Dyn_Button_1 rtds_stat_button PROGMEM = {
  .first_x = 230,
  .first_y = 35,
  .size_x = 170,
  .size_y = 55,
  .font_size = 1,
  .step_x = 60,
  .step_y = 60,
  .direction = VERTICAL,
  .max_items_count = 3,
  .max_row_count = 1,
  .slider_args = MENU_SLIDER_RTDS,
  .args = INDEX_DYN_MENU_RTDS,
  .get_status_string = get_function_rtds_text_button,
  .dyn_button_onclick =  click_rtds_setting_sensor,
  .function_for_max_items = get_function_rtds_last_index_for_menu,
  .redraw_class = 1 << REDRAW_ONCE | 1 << REDRAW_CLASS_2,
};

const Menu1 List_RTDS_Menu PROGMEM = {
  .name = nastaveni_rtds,
  .button_1 = {button_back, button_rtds_novy},
  .button_2 = {NULL},
  .function_1 = {f_show_rectangle_decorate, f_show_date, f_vertical_slider_rtds},
  .switch_1 = {NULL},
  .dyn_button = {rtds_stat_button},
  .symbol_button_1 = {slider_menu_plus_rtds, slider_menu_minus_rtds},
  .dyn_symbol_1 = {NULL},
  .dyn_select_box_1 = {NULL},
  .len_button_1 = 2,
  .len_button_2 = 0,
  .len_function_1 = 3,
  .len_switch_1 = 0,
  .len_dyn_button_1 = 1,
  .len_symbol_button_1 = 2,
  .len_dyn_symbol_1 = 0,
  .len_dyn_select_box_1 = 0,
  .idx = MENU_LIST_NASTAVENI_RTDS,
  .x = 0,
  .y = 0,
  .size_x = 480,
  .size_y = 320,
  .atributes = (1 << MENU_ATTRIBUTES_CLEAN_DISPLAY),
  .color_background = WHITE,
  .redraw_class = (1 << REDRAW_ONCE),
  .redraw_class_0 = returnnullfceargs,
  .redraw_class_1 = returnnullfceargs,
  .redraw_class_2 = menu_redraw_slider,
  .redraw_class_3 = menu_redraw_date,
  .preload_function = returnnullfceargs,
};


const Menu1 RTDS_Menu_Detail PROGMEM = {
  .name = nastaveni_rtds,
  .button_1 = {button_back, button_rtds_delete, button_rtds_name, button_rtds_subscribe},
  .button_2 = {NULL},
  .function_1 = {f_show_rtds_info_dynamics, f_show_date},
  .switch_1 = {NULL},
  .dyn_button = {NULL},
  .symbol_button_1 = {NULL},
  .dyn_symbol_1 = {NULL},
  .dyn_select_box_1 = {NULL},
  .len_button_1 = 4,
  .len_button_2 = 0,
  .len_function_1 = 2,
  .len_switch_1 = 0,
  .len_dyn_button_1 = 0,
  .len_symbol_button_1 = 0,
  .len_dyn_symbol_1 = 0,
  .len_dyn_select_box_1 = 0,
  .idx = MENU_NASTAVENI_RTDS_DETAIL,
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
  .redraw_class_3 = menu_redraw_date,
  .preload_function = returnnullfceargs,
};

#endif
