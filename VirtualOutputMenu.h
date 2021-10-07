#ifndef __PROJECT_VIRTUAL_OUTPUT_SETTINGS_MENU_H
#define __PROJECT_VIRTUAL_OUTPUT_SETTINGS_MENU_H
#include "room_control_v3.h"
#include "UserSettings.h"

uint8_t get_function_list_virtual_output_max_active_items(uint16_t idx, uint16_t args2, uint8_t args3);
void click_virtual_output_associate_action(uint16_t args1, uint16_t args2, uint8_t idx);
void get_function_list_virtual_output_labels(uint8_t args1, uint8_t args2 , uint8_t args3, char *line1, char *line2);

void display_element_show_virtual_output_detail(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint16_t args1, uint8_t args2, char *text);

void clik_button_virtual_output_manual(uint16_t args1, uint16_t args2, uint8_t args3);
void clik_button_virtual_output_test(uint16_t args1, uint16_t args2, uint8_t args3);
void clik_button_virtual_output_associate(uint16_t args1, uint16_t args2, uint8_t args3);
void clik_button_virtual_output_unassociate(uint16_t args1, uint16_t args2, uint8_t args3);

const Element_Button_1 button_virtual_output_associate PROGMEM = {
  .name = new_text_login,
  .x = 280,
  .y = 40,
  .size_x = 190,
  .size_y = 40,
  .font_size = 2,
  .args = 0,
  .onclick = clik_button_virtual_output_associate,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};

const Element_Button_1 button_virtual_output_unassociate PROGMEM = {
  .name = new_text_logout,
  .x = 280,
  .y = 100,
  .size_x = 190,
  .size_y = 40,
  .font_size = 2,
  .args = 0,
  .onclick = clik_button_virtual_output_unassociate,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};

const Element_Button_1 button_virtual_output_test PROGMEM = {
  .name = new_text_virtual_output_test,
  .x = 280,
  .y = 160,
  .size_x = 190,
  .size_y = 40,
  .font_size = 2,
  .args = 0,
  .onclick = clik_button_virtual_output_test,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};

const Element_Button_1 button_virtual_output_manual PROGMEM = {
  .name = new_text_virtual_output_manual,
  .x = 280,
  .y = 220,
  .size_x = 190,
  .size_y = 40,
  .font_size = 2,
  .args = 0,
  .onclick = clik_button_virtual_output_manual,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};

const Element_Function_1 f_show_virtual_output_detail PROGMEM = {
  .x = 10,
  .y = 30,
  .args = 0,
  .fnt_coordinate_xy = display_element_show_virtual_output_detail,
  .size_x = 200,
  .size_y = 170,
  .redraw_class = 1 << REDRAW_ONCE | 1 << REDRAW_CLASS_2 | 1 << REDRAW_CLASS_0,
  .onclick = nullfce,
  .enable_show = display_enable_show,
  .name = char_NULL,
};

const Element_Function_1 f_vertical_slider_virtual_output PROGMEM = {
  .x = 410,
  .y = 72,
  .args = MENU_SLIDER_LIST_VIRTUAL_OUTPUT,
  .fnt_coordinate_xy = display_element_vertical_slider,
  .size_x = 40,
  .size_y = 126,
  .redraw_class = 1 << REDRAW_ONCE | 1 << REDRAW_CLASS_2,
  .onclick = nullfce,
  .enable_show = display_enable_show,
  .name = char_NULL,
};

const Element_Symbol_1 slider_menu_plus_virtual_output PROGMEM =  {
  .znak = '+',
  .x = 410,
  .y = 30,
  .size_x = 40,
  .size_y = 40,
  .znak_size = 2,
  .args = MENU_SLIDER_LIST_VIRTUAL_OUTPUT,
  .onclick = display_function_vertical_slider_dec,
  .redraw_class = 1 << REDRAW_ONCE,
  .enable_show = display_enable_show,
};

const Element_Symbol_1 slider_menu_minus_virtual_output PROGMEM =  {
  .znak = '-',
  .x = 410,
  .y = 200,
  .size_x = 40,
  .size_y = 40,
  .znak_size = 2,
  .args = MENU_SLIDER_LIST_VIRTUAL_OUTPUT,
  .onclick = display_function_vertical_slider_inc,
  .redraw_class = 1 << REDRAW_ONCE,
  .enable_show = display_enable_show,
};


const Element_Dyn_Button_1 list_all_virtual_output PROGMEM = {
  .first_x = 230,
  .first_y = 35,
  .size_x = 170,
  .size_y = 55,
  .font_size = 1,
  .step_x = 60, .step_y = 60,
  .direction = VERTICAL,
  .max_items_count = 3,
  .max_row_count = 1,
  .slider_args = MENU_SLIDER_LIST_VIRTUAL_OUTPUT,
  .args = NEW_MENU_VIRTUAL_OUTPUT_CONFIG,
  .get_status_string = get_function_list_virtual_output_labels,
  .dyn_button_onclick =  click_virtual_output_associate_action,
  .function_for_max_items = get_function_list_virtual_output_max_active_items,
  .redraw_class = 1 << REDRAW_ONCE | 1 << REDRAW_CLASS_2 | 1 << REDRAW_CLASS_1 | 1 << REDRAW_CLASS_0,
};





const Menu1 VirtualOutputSettingsMenu PROGMEM = {
  .name = new_nastaveni_virtual_output,
  .button_1 = {button_back},
  .button_2 = {NULL},
  .function_1 = {f_show_rectangle_decorate, f_show_date, f_vertical_slider_virtual_output},
  .switch_1 = {NULL},
  .dyn_button = {list_all_virtual_output},
  .symbol_button_1 = {slider_menu_minus_virtual_output, slider_menu_plus_virtual_output},
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
  .idx = NEW_MENU_VIRTUAL_OUTPUT_SETTING,
  .x = 0,
  .y = 0,
  .size_x = 480,
  .size_y = 320,
  .atributes = (1 << MENU_ATTRIBUTES_CLEAN_DISPLAY),
  .color_background = WHITE,
  .redraw_class = (1 << REDRAW_FORCE),
  .redraw_class_0 = menu_redraw_update_temp,
  .redraw_class_1 = menu_redraw_slider,
  .redraw_class_2 = menu_redraw_change_virtual_output,
  .redraw_class_3 = menu_redraw_date,
  .preload_function = returnnullfceargs,
};


const Menu1 VirtualOutputSettingsDevice PROGMEM = {
  .name = new_text_virtual_output,
  .button_1 = {button_back, button_virtual_output_associate, button_virtual_output_unassociate, button_virtual_output_test, button_virtual_output_manual},
  .button_2 = {NULL},
  .function_1 = {f_show_date, f_show_virtual_output_detail},
  .switch_1 = {NULL},
  .dyn_button = {NULL},
  .symbol_button_1 = {NULL},
  .dyn_symbol_1 = {NULL},
  .dyn_select_box_1 = {NULL},
  .len_button_1 = 5,
  .len_button_2 = 0,
  .len_function_1 = 2,
  .len_switch_1 = 0,
  .len_dyn_button_1 = 0,
  .len_symbol_button_1 = 0,
  .len_dyn_symbol_1 = 0,
  .len_dyn_select_box_1 = 0,
  .idx = NEW_MENU_VIRTUAL_OUTPUT_CONFIG,
  .x = 0,
  .y = 0,
  .size_x = 480,
  .size_y = 320,
  .atributes = (1 << MENU_ATTRIBUTES_CLEAN_DISPLAY),
  .color_background = WHITE,
  .redraw_class = (1 << REDRAW_FORCE),
  .redraw_class_0 = menu_redraw_update_temp,
  .redraw_class_1 = returnnullfceargs,
  .redraw_class_2 = menu_redraw_change_virtual_output,
  .redraw_class_3 = menu_redraw_date,
  .preload_function = returnnullfceargs,
};

#endif
