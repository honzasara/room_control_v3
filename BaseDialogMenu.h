#ifndef __PROJECT_BASE_DIALOG_MENU_H
#define __PROJECT_BASE_DIALOG_MENU_H
#include "room_control_v3.h"

const Element_Button_1 dialog_yes PROGMEM = {
  .name = text_yes,
  .x = 30,
  .y = 60,
  .size_x = 100,
  .size_y = 40,
  .font_size = 2,
  .args = 0,
  .onclick = click_dialog_button_yes,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};


const Element_Button_1 dialog_no PROGMEM = {
  .name = text_no,
  .x = 250,
  .y = 60,
  .size_x = 100,
  .size_y = 40,
  .font_size = 2,
  .args = 0,
  .onclick = MenuHistoryPrevMenu,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};


const Element_Button_1 dialog_ok PROGMEM = {
  .name = new_text_ok,
  .x = 250,
  .y = 60,
  .size_x = 100,
  .size_y = 40,
  .font_size = 2,
  .args = 0,
  .onclick = MenuHistoryPrevMenu,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};

const Element_Button_1 dialog_set_string_save PROGMEM =  {
  .name = text_ulozit,
  .x = 10,
  .y = 200,
  .size_x = 130,
  .size_y = 40,
  .font_size = 2,
  .args = 0,
  .onclick = dialog_set_string_button_click,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};

const Element_Button_1 dialog_set_variable_save PROGMEM =  {
  .name = text_ulozit,
  .x = 10,
  .y = 200,
  .size_x = 130,
  .size_y = 40,
  .font_size = 2,
  .args = DIALOG_SET_VARIABLE_GENERAL,
  .onclick = dialog_set_variable_button_click,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};

const Element_Button_1 dialog_set_variable_cancel PROGMEM =  {
  .name = text_zrusit,
  .x = 150,
  .y = 200,
  .size_x = 130,
  .size_y = 40,
  .font_size = 2,
  .args = 0,
  .onclick = MenuHistoryPrevMenu,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};

const Element_Function_1 f_dialog_set_string PROGMEM = {
  .x = 10,
  .y = 35,
  .args = 0,
  .fnt_coordinate_xy = display_element_dialog_set_string,
  .size_x = 300,
  .size_y = 40,
  .redraw_class = REDRAW_BUTTON,
  .onclick = nullfce,
  .enable_show = display_enable_show,
  .name = char_NULL,
};

const Element_Dyn_Symbol_1 keyboard_number PROGMEM = {
  .first_x = 10,
  .first_y = 90,
  .size_x = 35,
  .size_y = 35,
  .font_size = 2,
  .step_x = 45,
  .step_y = 45,
  .direction = HORIZONTAL_NEW_LINE,
  .max_items_count = 10,
  .max_row_count = 0,
  .slider_args = 0,
  .args = INDEX_DYN_MENU_KEYBOARD_NUMBER,
  .get_status_string = get_function_keyboard_number_char,
  .dyn_symbol_onclick =  click_keyboard_number,
  .function_for_max_items = get_function_keyboard_number_max_keys,
  .redraw_class = REDRAW_BUTTON,
};

const Element_Dyn_Symbol_1 keyboard_alfa PROGMEM = {
  .first_x = 10,
  .first_y = 80,
  .size_x = 30,
  .size_y = 30,
  .font_size = 2,
  .step_x = 40,
  .step_y = 40,
  .direction = HORIZONTAL_NEW_LINE,
  .max_items_count = 10,
  .max_row_count = 0,
  .slider_args = 0,
  .args = INDEX_DYN_MENU_KEYBOARD_ALFA,
  .get_status_string = get_function_keyboard_alfa_char,
  .dyn_symbol_onclick =  click_keyboard_alfa,
  .function_for_max_items = get_function_keyboard_alfa_max_keys,
  .redraw_class = REDRAW_BUTTON,
};

const Element_Button_1 button_keyboard_upper PROGMEM = {
  .name = text_upper_case,
  .x = 320,
  .y = 35,
  .size_x = 40,
  .size_y = 40,
  .font_size = 2,
  .args = KEYBOARD_TYPE_UPPER,
  .onclick = click_keyboard_type,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};

const Element_Button_1 button_keyboard_lower PROGMEM = {
  .name = text_lower_case,
  .x = 365,
  .y = 35,
  .size_x = 40,
  .size_y = 40,
  .font_size = 2,
  .args = KEYBOARD_TYPE_LOWER,
  .onclick = click_keyboard_type,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};

const Element_Button_1 button_keyboard_special PROGMEM = {
  .name = text_special_case,
  .x = 410,
  .y = 35,
  .size_x = 40,
  .size_y = 40,
  .font_size = 2,
  .args = KEYBOARD_TYPE_SPECIAL,
  .onclick = click_keyboard_type,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};

const Element_Button_1 button_delete_char PROGMEM = {
  .name = text_delete_char,
  .x = 300,
  .y = 200,
  .size_x = 130,
  .size_y = 40,
  .font_size = 2,
  .args = 0,
  .onclick = display_element_set_string_del_char,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};



/*
   graficke prvky pro dialog menu nastaveni int promene
*/
const Element_Function_1 f_dialog_set_variable PROGMEM = {
  .x = 100,
  .y = 45,
  .args = DIALOG_SET_VARIABLE_GENERAL,
  .fnt_coordinate_xy = display_element_dialog_set_variable,
  .size_x = 180,
  .size_y = 40,
  .redraw_class = REDRAW_BUTTON,
  .onclick = nullfce,
  .enable_show = display_enable_show,
  .name = char_NULL,
};
const Element_Symbol_1 dialog_set_variable_plus PROGMEM =  {
  .znak = '+',
  .x = 60,
  .y = 45,
  .size_x = 40,
  .size_y = 40,
  .znak_size = 2,
  .args = DIALOG_SET_VARIABLE_GENERAL,
  .onclick = display_function_set_variable_plus,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};
const Element_Symbol_1 dialog_set_variable_minus PROGMEM =  {
  .znak = '-',
  .x = 280,
  .y = 45,
  .size_x = 40,
  .size_y = 40,
  .znak_size = 2,
  .args = DIALOG_SET_VARIABLE_GENERAL,
  .onclick = display_function_set_variable_minus,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};


const Menu1 DialogKyeboardNumber PROGMEM = {
  .name = text_upozorneni,
  .button_1 = {dialog_set_string_save, dialog_set_variable_cancel, button_delete_char},
  .button_2 = {NULL},
  .function_1 = {f_dialog_set_string},
  .switch_1 = {NULL},
  .dyn_button = {NULL},
  .symbol_button_1 = {NULL},
  .dyn_symbol_1 = {keyboard_number},
  .dyn_select_box_1 = {NULL},
  .len_button_1 = 3,
  .len_button_2 = 0,
  .len_function_1 = 1,
  .len_switch_1 = 0,
  .len_dyn_button_1 = 0,
  .len_symbol_button_1 = 0,
  .len_dyn_symbol_1 = 1,
  .len_dyn_select_box_1 = 0,
  .idx = MENU_DIALOG_KEYBOARD_NUMBER,
  .x = 10,
  .y = 10,
  .size_x = 460,
  .size_y = 260,
  .atributes = (1 << MENU_ATTRIBUTES_FILL_COLOR_RECTANGLE | 1 << MENU_ATTRIBUTES_DECORATE_MENU),
  .color_background = YELLOW,
  .redraw_class = (1 << REDRAW_FORCE),
  .redraw_class_0 = returnnullfceargs,
  .redraw_class_1 = returnnullfceargs,
  .redraw_class_2 = returnnullfceargs,
  .preload_function = returnnullfceargs,
};

const Menu1 DialogKyeboardAlfa PROGMEM = {
  .name = text_klavesnice,
  .button_1 = {dialog_set_string_save, dialog_set_variable_cancel, button_delete_char, button_keyboard_upper, button_keyboard_lower, button_keyboard_special},
  .button_2 = {NULL},
  .function_1 = {f_dialog_set_string},
  .switch_1 = {NULL},
  .dyn_button = {NULL},
  .symbol_button_1 = {NULL},
  .dyn_symbol_1 = {keyboard_alfa},
  .dyn_select_box_1 = {NULL},
  .len_button_1 = 6,
  .len_button_2 = 0,
  .len_function_1 = 1,
  .len_switch_1 = 0,
  .len_dyn_button_1 = 0,
  .len_symbol_button_1 = 0,
  .len_dyn_symbol_1 = 1,
  .len_dyn_select_box_1 = 0,
  .idx = MENU_DIALOG_KEYBOARD_ALFA,
  .x = 10,
  .y = 10,
  .size_x = 460,
  .size_y = 260,
  .atributes = (1 << MENU_ATTRIBUTES_FILL_COLOR_RECTANGLE | 1 << MENU_ATTRIBUTES_DECORATE_MENU),
  .color_background = YELLOW,
  .redraw_class = (1 << REDRAW_FORCE),
  .redraw_class_0 = returnnullfceargs,
  .redraw_class_1 = returnnullfceargs,
  .redraw_class_2 = returnnullfceargs,
  .preload_function = returnnullfceargs,
};

const Menu1 DialogYESNO PROGMEM = {
  .name = text_upozorneni,
  .button_1 = {dialog_yes, dialog_no},
  .button_2 = {NULL},
  .function_1 = {f_dialog_show_text},
  .switch_1 = {NULL},
  .dyn_button = {NULL},
  .symbol_button_1 = {NULL},
  .dyn_symbol_1 = {NULL},
  .dyn_select_box_1 = {NULL},
  .len_button_1 = 2,
  .len_button_2 = 0,
  .len_function_1 = 1,
  .len_switch_1 = 0,
  .len_dyn_button_1 = 0,
  .len_symbol_button_1 = 0,
  .len_dyn_symbol_1 = 0,
  .len_dyn_select_box_1 = 0,
  .idx = MENU_DIALOG_YES_NO,
  .x = 50,
  .y = 50,
  .size_x = 380,
  .size_y = 140,
  .atributes = (1 << MENU_ATTRIBUTES_FILL_COLOR_RECTANGLE | 1 << MENU_ATTRIBUTES_DECORATE_MENU),
  .color_background = YELLOW,
  .redraw_class = (1 << REDRAW_FORCE),
  .redraw_class_0 = returnnullfceargs,
  .redraw_class_1 = returnnullfceargs,
  .redraw_class_2 = returnnullfceargs,
  .preload_function = returnnullfceargs,
};

const Menu1 DialogOK PROGMEM = {
  .name = text_upozorneni,
  .button_1 = {dialog_ok},
  .button_2 = {NULL},
  .function_1 = {f_dialog_show_text},
  .switch_1 = {NULL},
  .dyn_button = {NULL},
  .symbol_button_1 = {NULL},
  .dyn_symbol_1 = {NULL},
  .dyn_select_box_1 = {NULL},
  .len_button_1 = 1,
  .len_button_2 = 0,
  .len_function_1 = 1,
  .len_switch_1 = 0,
  .len_dyn_button_1 = 0,
  .len_symbol_button_1 = 0,
  .len_dyn_symbol_1 = 0,
  .len_dyn_select_box_1 = 0,
  .idx = MENU_DIALOG_OK,
  .x = 50,
  .y = 50,
  .size_x = 380,
  .size_y = 140,
  .atributes = (1 << MENU_ATTRIBUTES_FILL_COLOR_RECTANGLE | 1 << MENU_ATTRIBUTES_DECORATE_MENU),
  .color_background = YELLOW,
  .redraw_class = (1 << REDRAW_FORCE),
  .redraw_class_0 = returnnullfceargs,
  .redraw_class_1 = returnnullfceargs,
  .redraw_class_2 = returnnullfceargs,
  .preload_function = returnnullfceargs,
};


const Menu1 DialogSetVariable PROGMEM = {
  .name = text_upozorneni,
  .button_1 = {dialog_set_variable_save, dialog_set_variable_cancel},
  .button_2 = {NULL},
  .function_1 = {f_dialog_set_variable},
  .switch_1 = {NULL},
  .dyn_button = {NULL},
  .symbol_button_1 = {dialog_set_variable_plus, dialog_set_variable_minus},
  .dyn_symbol_1 = {NULL},
  .dyn_select_box_1 = {NULL},
  .len_button_1 = 2,
  .len_button_2 = 0,
  .len_function_1 = 1,
  .len_switch_1 = 0,
  .len_dyn_button_1 = 0,
  .len_symbol_button_1 = 2,
  .len_dyn_symbol_1 = 0,
  .len_dyn_select_box_1 = 0,
  .idx = MENU_DIALOG_SET_VARIABLE,
  .x = 10,
  .y = 10,
  .size_x = 460,
  .size_y = 260,
  .atributes = (1 << MENU_ATTRIBUTES_FILL_COLOR_RECTANGLE | 1 << MENU_ATTRIBUTES_DECORATE_MENU),
  .color_background = YELLOW,
  .redraw_class = (1 << REDRAW_FORCE),
  .redraw_class_0 = returnnullfceargs,
  .redraw_class_1 = returnnullfceargs,
  .redraw_class_2 = returnnullfceargs,
  .preload_function = returnnullfceargs,
};


#endif
