#ifndef __PROJECT_ONE_WIRE_MENU_H
#define __PROJECT_ONE_WIRE_MENU_H
#include "room_control_v3.h"


void get_function_one_wire_associate_or_setting_text_button(uint8_t args1, uint8_t args2, uint8_t args3, char *line1, char *line2);
uint8_t get_function_one_wire_last_index_for_menu(uint16_t idx, uint16_t args2, uint8_t args3);
void click_tds_associate_or_setting_onewire(uint16_t args1, uint16_t args2, uint8_t idx);

void display_menu_tds_set_offset(uint16_t args1, uint16_t args2, uint8_t args3);
void display_menu_tds_set_period(uint16_t args1, uint16_t args2, uint8_t args3);
void display_menu_tds_set_name(uint16_t args1, uint16_t args2, uint8_t args3);
void click_tds_deassociate_onewire(uint16_t args1, uint16_t idx, uint8_t args3);

void display_element_show_tds_info_static(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint16_t args1, uint8_t args2, char *text);
void display_element_show_tds_info_dynamics(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint16_t args1, uint8_t args2, char *text);

void clik_button_onewire_scan_bus(uint16_t args1, uint16_t args2, uint8_t args3);

const Element_Button_1 button_tds_offset PROGMEM = {
  .name = nastaveni_tds_offset, 
  .x = 280, 
  .y = 40, 
  .size_x = 190, 
  .size_y = 40, 
  .font_size = 2, 
  .args = 0, 
  .onclick = display_menu_tds_set_offset,  
  .redraw_class = REDRAW_BUTTON, 
  .enable_show = display_enable_show,
  };
  
const Element_Button_1 button_tds_name PROGMEM = {
  .name = nastaveni_name_sensor, 
  .x = 280, 
  .y = 90, 
  .size_x = 190, 
  .size_y = 40, 
  .font_size = 2, 
  .args = 0, 
  .onclick = display_menu_tds_set_name,  
  .redraw_class = REDRAW_BUTTON, 
  .enable_show = display_enable_show,
  };
  
const Element_Button_1 button_tds_period PROGMEM = {
  .name = nastaveni_tds_period, 
  .x = 280, 
  .y = 140, 
  .size_x = 190, 
  .size_y = 40,
  .font_size = 2, 
  .args = 0, 
  .onclick = display_menu_tds_set_period,  
  .redraw_class = REDRAW_BUTTON, 
  .enable_show = display_enable_show,
  };
  
const Element_Button_1 button_tds_delete PROGMEM = {
  .name = nastaveni_delete_sensor, 
  .x = 280, 
  .y = 190, 
  .size_x = 190, 
  .size_y = 40, 
  .font_size = 2, 
  .args = 0, 
  .onclick = click_tds_deassociate_onewire,  
  .redraw_class = REDRAW_BUTTON, 
  .enable_show = display_enable_show,
  };
  
const Element_Function_1 f_show_tds_info_static PROGMEM = {
  .x = 20, 
  .y = 20, 
  .args = 0, 
  .fnt_coordinate_xy = display_element_show_tds_info_static,  
  .size_x = 0, 
  .size_y = 0,
  .redraw_class = REDRAW_BUTTON, 
  .onclick = nullfce, 
  .enable_show = display_enable_show, 
  .name = char_NULL,
  };
  
const Element_Function_1 f_show_tds_info_dynamics PROGMEM = {
  .x = 20, 
  .y = 20, 
  .args = 0, 
  .fnt_coordinate_xy = display_element_show_tds_info_dynamics,  
  .size_x = 0, 
  .size_y = 0, 
  .redraw_class = REDRAW_CLASS_SHOW, 
  .onclick = nullfce, 
  .enable_show = display_enable_show, 
  .name = char_NULL,
  };



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
  .enable_show = display_enable_show,
  .redraw_class = REDRAW_BUTTON,
};

const Element_Button_1 button_nastaveni_scan_onewire PROGMEM = {
  .name = new_text_scan_onewire_bus,
  .x = 10,
  .y = 40,
  .size_x = 190,
  .size_y = 40,
  .font_size = 2,
  .args = 0,
  .onclick = clik_button_onewire_scan_bus,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};

const Menu1 OneWireMenu PROGMEM = {
  .name = nastaveni_onewire,
  .button_1 = {button_back, button_nastaveni_scan_onewire},
  .button_2 = {NULL},
  .function_1 = {f_show_date, f_vertical_slider_one_wire_menu, f_show_rectangle_decorate},
  .switch_1 = {NULL},
  .dyn_button = {onewire_associace_button},
  .symbol_button_1 = {slider_menu_plus_one_wire_menu, slider_menu_minus_one_wire_menu},
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
  .redraw_class_3 = returnnullfceargs,
  .preload_function = returnnullfceargs,
};


const Menu1 TDSMenu PROGMEM = {
  .name = text_tds_sensors,
  .button_1 = {button_back, button_tds_delete, button_tds_period, button_tds_name, button_tds_offset},
  .button_2 = {NULL},
  .function_1 = {f_show_date, f_show_tds_info_static, f_show_tds_info_dynamics},
  .switch_1 = {NULL},
  .dyn_button = {NULL},
  .symbol_button_1 = {NULL},
  .dyn_symbol_1 = {NULL},
  .dyn_select_box_1 = {NULL},
  .len_button_1 = 5,
  .len_button_2 = 0,
  .len_function_1 = 3,
  .len_switch_1 = 0,
  .len_dyn_button_1 = 0,
  .len_symbol_button_1 = 0,
  .len_dyn_symbol_1 = 0,
  .len_dyn_select_box_1 = 0,
  .idx = MENU_NASTAVENI_TDS,
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
