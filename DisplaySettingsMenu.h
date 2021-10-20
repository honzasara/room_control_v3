#ifndef __PROJECT_DISPLAY_SETTINGS_MENU_H
#define __PROJECT_DISPLAY_SETTINGS_MENU_H
#include "room_control_v3.h"






void button_get_show_default_temp(uint8_t args1, uint8_t args2, uint8_t args3, char *line1, char *line2);
void button_click_set_show_default_temp(uint16_t args1, uint16_t args2, uint8_t args3);
uint8_t button_get_show_default_temp_max_items(uint16_t args1, uint16_t args2, uint8_t args3);
uint8_t button_get_show_default_temp_active(uint16_t args1, uint16_t args2, uint8_t args3);


void button_change_brightness_display_get_status_string(uint8_t args1, uint8_t args2, uint8_t args3, char *line1, char *line2);
void button_change_brightness_display_dyn_button_onclick(uint16_t args1, uint16_t args2, uint8_t args3);


void button_set_brightness_auto_shutdown_get_status_string(uint8_t args1, uint8_t args2, uint8_t args3, char *line1, char *line2);
void button_set_brightness_auto_shutdown_dyn_symbol_onclick(uint16_t args1, uint16_t args2, uint8_t args3);
uint8_t button_set_brightness_auto_shutdown_get_status_fnt(uint16_t args1, uint16_t args2, uint8_t args3);


uint8_t switch_brightness_automode_get_status_fnt(uint16_t args1, uint16_t args2, uint8_t args3);
void switch_brightness_automode_get_status_string(uint8_t args1, uint8_t args2, uint8_t args3, char *line1, char *line2);
void switch_brightness_automode_onclick(uint16_t args1, uint16_t args2, uint8_t args3);

uint8_t preload_display_setting_brightness(uint16_t args1, uint16_t args2, uint8_t args3);

uint8_t display_enable_show_brightness_manual_mode(uint16_t args1, uint16_t args2, uint8_t args3);
uint8_t display_enable_show_brightness_auto_mode(uint16_t args1, uint16_t args2, uint8_t args3);

void button_set_brightness_auto_shutdown_select_time_get_status_string(uint8_t args1, uint8_t args2, uint8_t args3, char *line1, char *line2);
void button_set_brightness_auto_shutdown_select_time_dyn_symbol_onclick(uint16_t args1, uint16_t args2, uint8_t args3);
uint8_t button_set_brightness_auto_shutdown_select_time_get_status_fnt(uint16_t args1, uint16_t args2, uint8_t args3);

uint8_t menu_redraw_change_auto_brightness(uint16_t args1, uint16_t args2, uint8_t args3);
uint8_t menu_redraw_change_display_brightness_mode(uint16_t args1, uint16_t args2, uint8_t args3);

const Element_Button_1 button_default_show_temp PROGMEM = {
  .name = text_select_default_temp,
  .x = 10,
  .y = 40,
  .size_x = 190,
  .size_y = 40,
  .font_size = 1,
  .args = MENU_SELECT_DEFAULT_TEMP,
  .onclick = MenuHistoryNextMenu,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};

const Element_Dyn_Button_1 button_change_brightness_display PROGMEM = {
  .first_x = 10,
  .first_y = 100,
  .size_x = 190,
  .size_y = 40,
  .font_size = 1,
  .step_x = 1,
  .step_y = 1,
  .direction = VERTICAL,
  .max_items_count = 1,
  .max_row_count = 1,
  .slider_args = 0,
  .args = 1,
  .get_status_string = button_change_brightness_display_get_status_string,
  .dyn_button_onclick = button_change_brightness_display_dyn_button_onclick,
  .function_for_max_items = get_function_return_args_1,
  .enable_show = display_enable_show,
  .redraw_class = (1 << REDRAW_FORCE | 1 << REDRAW_CLASS_0)
};

const Element_Dyn_Select_1 button_set_brightness_auto_shutdown PROGMEM = {
  .first_x = 10,
  .first_y = 160,
  .size_x = 190,
  .size_y = 40,
  .font_size_1 = 1,
  .font_size_2 = 1,
  .color_active = GREEN,
  .color_inactive = WHITE,
  .step_x = 1,
  .step_y = 50,
  .direction = VERTICAL,
  .max_items_count = 1,
  .max_row_count = 1,
  .slider_args = MENU_SLIDER_OFF,
  .args = 1,
  .get_status_string = button_set_brightness_auto_shutdown_get_status_string,
  .dyn_symbol_onclick =  button_set_brightness_auto_shutdown_dyn_symbol_onclick,
  .function_for_max_items = get_function_return_args_1,
  .get_status_fnt = button_set_brightness_auto_shutdown_get_status_fnt,
  .enable_show = display_enable_show,
  .redraw_class = REDRAW_BUTTON,
};


const Element_Dyn_Select_1 button_set_brightness_auto_shutdown_select_time PROGMEM = {
  .first_x = 10,
  .first_y = 40,
  .size_x = 100,
  .size_y = 40,
  .font_size_1 = 1,
  .font_size_2 = 1,
  .color_active = GREEN,
  .color_inactive = WHITE,
  .step_x = 110,
  .step_y = 50,
  .direction = HORIZONTAL_NEW_LINE,
  .max_items_count = 4,
  .max_row_count = 2,
  .slider_args = MENU_SLIDER_OFF,
  .args = 6,
  .get_status_string = button_set_brightness_auto_shutdown_select_time_get_status_string,
  .dyn_symbol_onclick =  button_set_brightness_auto_shutdown_select_time_dyn_symbol_onclick,
  .function_for_max_items = get_function_return_args_1,
  .get_status_fnt = button_set_brightness_auto_shutdown_select_time_get_status_fnt,
  .enable_show = display_enable_show,
  .redraw_class = REDRAW_BUTTON,
};


/*************************************************************************************/
/*
   slider pro menu vyber vychozi teplomeru pro zobrazeni
*/
const Element_Function_1 f_vertical_slider_default_temp PROGMEM = {
  .x = 410,
  .y = 72,
  .args = MENU_SLIDER_DEFAULT_TEMP,
  .fnt_coordinate_xy = display_element_vertical_slider,
  .size_x = 40,
  .size_y = 86,
  .redraw_class = REDRAW_BUTTON,
  .onclick = nullfce,
  .enable_show = display_enable_show,
  .name = char_NULL,
};
const Element_Symbol_1 slider_menu_plus_default_temp PROGMEM =  {
  .znak = '+',
  .x = 410,
  .y = 30,
  .size_x = 40,
  .size_y = 40,
  .znak_size = 2,
  .args = MENU_SLIDER_DEFAULT_TEMP,
  .onclick = display_function_vertical_slider_dec,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};
const Element_Symbol_1 slider_menu_minus_default_temp PROGMEM =  {
  .znak = '-',
  .x = 410,
  .y = 160,
  .size_x = 40,
  .size_y = 40,
  .znak_size = 2,
  .args = MENU_SLIDER_DEFAULT_TEMP,
  .onclick = display_function_vertical_slider_inc,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};


const Element_Function_1 f_show_default_temp_decorate PROGMEM = {
  .x = 5,
  .y = 30,
  .args = 0,
  .fnt_coordinate_xy = display_element_rectangle,
  .size_x = 460,
  .size_y = 180,
  .redraw_class = REDRAW_BUTTON,
  .onclick = nullfce,
  .enable_show = display_enable_show,
  .name = char_NULL,
};

const Element_Dyn_Select_1 button_select_show_default_temp PROGMEM = {
  .first_x = 10,
  .first_y = 40,
  .size_x = 120,
  .size_y = 60,
  .font_size_1 = 1,
  .font_size_2 = 1,
  .color_active = GREEN,
  .color_inactive = WHITE,
  .step_x = 130,
  .step_y = 80,
  .direction = HORIZONTAL_NEW_LINE,
  .max_items_count = 3,
  .max_row_count = 2,
  .slider_args = MENU_SLIDER_DEFAULT_TEMP,
  .args = INPUT_SENSOR_SHOW_ACTIVE,
  .get_status_string = button_get_show_default_temp,
  .dyn_symbol_onclick =  button_click_set_show_default_temp,
  .function_for_max_items = button_get_show_default_temp_max_items,
  .get_status_fnt = button_get_show_default_temp_active,
  .enable_show = display_enable_show,
  .redraw_class = REDRAW_BUTTON,
};


const Element_Switch_1 switch_brightness_automode PROGMEM = {
  .name = new_text_switch_brightness_automode,
  .x = 10,
  .y = 40,
  .size_x = 190,
  .size_y = 40,
  .font_size = 1,
  .color_active = GREEN,
  .color_inactive = WHITE,
  .args = 0,
  .onclick = switch_brightness_automode_onclick,
  .get_status_fnt = switch_brightness_automode_get_status_fnt,
  .get_status_string = switch_brightness_automode_get_status_string,
  .redraw_class = (REDRAW_BUTTON | 1 << REDRAW_CLASS_0 ),
};

const Element_Function_1 f_show_fill_rectangle_display_auto_brightness_mode PROGMEM = {
  .x = 230,
  .y = 40,
  .args = YELLOW,
  .fnt_coordinate_xy = display_element_fill_rectangle,
  .size_x = 220,
  .size_y = 40,
  .redraw_class = ( 1 << REDRAW_CLASS_1),
  .onclick = nullfce,
  .enable_show = display_enable_show_brightness_auto_mode,
  .name = char_NULL,
};

const Element_Function_1 f_dialog_set_variable_display_brightness PROGMEM = {
  .x = 270,
  .y = 40,
  .args = 0,
  .fnt_coordinate_xy = display_element_dialog_set_variable,
  .size_x = 140,
  .size_y = 40,
  .redraw_class = (1 << REDRAW_ONCE | 1 << REDRAW_CLASS_1 | 1 << REDRAW_CLASS_2),
  .onclick = nullfce,
  .enable_show = display_enable_show_brightness_manual_mode,
  .name = char_NULL,
};
const Element_Symbol_1 dialog_set_variable_plus_display_brightness PROGMEM =  {
  .znak = '+',
  .x = 410,
  .y = 40,
  .size_x = 40,
  .size_y = 40,
  .znak_size = 2,
  .args = 0,
  .onclick = display_function_set_variable_plus,
  .redraw_class = (1 << REDRAW_ONCE | 1 << REDRAW_CLASS_1),
  .enable_show = display_enable_show_brightness_manual_mode,
};
const Element_Symbol_1 dialog_set_variable_minus_display_brightness PROGMEM =  {
  .znak = '-',
  .x = 230,
  .y = 40,
  .size_x = 40,
  .size_y = 40,
  .znak_size = 2,
  .args = 0,
  .onclick = display_function_set_variable_minus,
  .redraw_class = (1 << REDRAW_ONCE | 1 << REDRAW_CLASS_1),
  .enable_show = display_enable_show_brightness_manual_mode,
};

const Menu1 SelectMenuDefaultTemp PROGMEM = {
  .name = text_select_default_temp,
  .button_1 = {button_back},
  .button_2 = {NULL},
  .function_1 = {f_show_date, f_vertical_slider_default_temp, f_show_default_temp_decorate},
  .switch_1 = {NULL},
  .dyn_button = {NULL},
  .symbol_button_1 = {slider_menu_plus_default_temp, slider_menu_minus_default_temp},
  .dyn_symbol_1 = {NULL},
  .dyn_select_box_1 = {button_select_show_default_temp},
  .len_button_1 = 1,
  .len_button_2 = 0,
  .len_function_1 = 3,
  .len_switch_1 = 0,
  .len_dyn_button_1 = 0,
  .len_symbol_button_1 = 2,
  .len_dyn_symbol_1 = 0,
  .len_dyn_select_box_1 = 1,
  .idx = MENU_SELECT_DEFAULT_TEMP,
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


const Menu1 New_DisplaySettingMenu PROGMEM = {
  .name = new_text_nastaveni_zobrazeni_full,
  .button_1 = {button_back, button_default_show_temp},
  .button_2 = {NULL},
  .function_1 = {f_show_date},
  .switch_1 = {NULL},
  .dyn_button = {button_change_brightness_display},
  .symbol_button_1 = {NULL},
  .dyn_symbol_1 = {NULL},
  .dyn_select_box_1 = {button_set_brightness_auto_shutdown},
  .len_button_1 = 2,
  .len_button_2 = 0,
  .len_function_1 = 1,
  .len_switch_1 = 0,
  .len_dyn_button_1 = 1,
  .len_symbol_button_1 = 0,
  .len_dyn_symbol_1 = 0,
  .len_dyn_select_box_1 = 1,
  .idx = NEW_MENU_DISPLAY_NASTAVENI_SCREEN,
  .x = 0,
  .y = 0,
  .size_x = 480,
  .size_y = 320,
  .atributes = (1 << MENU_ATTRIBUTES_CLEAN_DISPLAY),
  .color_background = WHITE,
  .redraw_class = (1 << REDRAW_FORCE),
  .redraw_class_0 = menu_redraw_change_auto_brightness,
  .redraw_class_1 = returnnullfceargs,
  .redraw_class_2 = returnnullfceargs,
  .redraw_class_3 = returnnullfceargs,
  .preload_function = returnnullfceargs,
};

const Menu1 New_DisplaySetting_Brigthness PROGMEM = {
  .name = new_text_nastaveni_zobrazeni_full,
  .button_1 = {button_back},
  .button_2 = {NULL},
  .function_1 = {f_show_fill_rectangle_display_auto_brightness_mode, f_dialog_set_variable_display_brightness},
  .switch_1 = {switch_brightness_automode},
  .dyn_button = {NULL},
  .symbol_button_1 = {dialog_set_variable_plus_display_brightness, dialog_set_variable_minus_display_brightness},
  .dyn_symbol_1 = {NULL},
  .dyn_select_box_1 = {NULL},
  .len_button_1 = 1,
  .len_button_2 = 0,
  .len_function_1 = 2,
  .len_switch_1 = 1,
  .len_dyn_button_1 = 0,
  .len_symbol_button_1 = 2,
  .len_dyn_symbol_1 = 0,
  .len_dyn_select_box_1 = 0,
  .idx = NEW_MENU_DISPLAY_NASTAVENI_BRIGTHNESS_SCREEN,
  .x = 10,
  .y = 10,
  .size_x = 460,
  .size_y = 280,
  .atributes = (1 << MENU_ATTRIBUTES_FILL_COLOR_RECTANGLE | 1 << MENU_ATTRIBUTES_DECORATE_MENU),
  .color_background = YELLOW,
  .redraw_class = (1 << REDRAW_ONCE),
  .redraw_class_0 = menu_redraw_change_auto_brightness,
  .redraw_class_1 = menu_redraw_change_display_brightness_mode,
  .redraw_class_2 = menu_redraw_change_variable,
  .redraw_class_3 = returnnullfceargs,
  .preload_function = preload_display_setting_brightness,
};



const Menu1 New_DisplaySetting_Auto_Shutdown PROGMEM = {
  .name = new_text_auto_display_shutdown,
  .button_1 = {button_back},
  .button_2 = {NULL},
  .function_1 = {NULL},
  .switch_1 = {NULL},
  .dyn_button = {NULL},
  .symbol_button_1 = {NULL},
  .dyn_symbol_1 = {NULL},
  .dyn_select_box_1 = {button_set_brightness_auto_shutdown_select_time},
  .len_button_1 = 1,
  .len_button_2 = 0,
  .len_function_1 = 0,
  .len_switch_1 = 1,
  .len_dyn_button_1 = 0,
  .len_symbol_button_1 = 0,
  .len_dyn_symbol_1 = 0,
  .len_dyn_select_box_1 = 1,
  .idx = NEW_MENU_DISPLAY_NASTAVENI_AUTO_SHUTDOWN_SCREEN,
  .x = 10,
  .y = 10,
  .size_x = 460,
  .size_y = 280,
  .atributes = (1 << MENU_ATTRIBUTES_FILL_COLOR_RECTANGLE | 1 << MENU_ATTRIBUTES_DECORATE_MENU),
  .color_background = YELLOW,
  .redraw_class = (1 << REDRAW_FORCE),
  .redraw_class_0 = returnnullfceargs,
  .redraw_class_1 = returnnullfceargs,
  .redraw_class_2 = returnnullfceargs,
  .redraw_class_3 = returnnullfceargs,
  .preload_function = preload_display_setting_brightness,
};

#endif
