#ifndef __PROJECT_DEFAULT_H
#define __PROJECT_DEFAULT_H

#include <avr/pgmspace.h>

#define H_TRUE 1
#define H_FALSE 0

#define MAX_THERMOSTAT 5

#define HW_ONEWIRE_MAXROMS 6
#define HW_ONEWIRE_MAXDEVICES 6
#define DS2482_COUNT 1

#define eeprom_wire_know_rom 200
#define eeprom_thermostat 332
#define eeprom_nrf_start 1670


#define bootloader_tag 0
#define time_offset 1
#define bootloader_set_1 13
#define bootloader_set_2 13

#define build_time_0 86
#define build_time_1 87
#define build_time_2 88
#define build_time_3 89

#define set_default_values 90
#define my_default_ring 92
#define my_default_show_temp 93
#define my_brightness_values 94
#define my_brightness_mode 95
#define my_display_auto_shutdown 96


#define MAX_RTDS 10
#define RTDS_DEVICE_TOTAL_LEN 20
#define RTDS_DEVICE_STRING_LEN 18
#define RTDS_DEVICE_ACTIVE_BYTE_POS 19

#define RTDS_REMOTE_TYPE_TEMP 1
#define RTDS_REMOTE_TYPE_FREE 255

#define remote_tds_name0  1450
#define remote_tds_name1  1470
#define remote_tds_name2  1490
#define remote_tds_name3  1510
#define remote_tds_name4  1530
#define remote_tds_name5  1550
#define remote_tds_name6  1570
#define remote_tds_name7  1610
#define remote_tds_name8  1630
#define remote_tds_name9  1650

#define ram_remote_tds_store_size 4
#define ram_remote_tds_store_data_low 100
#define ram_remote_tds_store_data_high 101
#define ram_remote_tds_store_last_update 102
#define ram_remote_tds_store_type 103


#define ram_remote_tds_store_last (ram_remote_tds_store_data_low+(ram_remote_tds_store_size*MAX_RTDS))
///#define ram_remote_tds_store_last 140



#define ram_nrf_device_start 150
#define ram_nrf_name_length 10
#define ram_nrf_devices 20
#define ram_nrf_device_store_size 14
#define ram_nrf_device_index 0
#define ram_nrf_device_used 1
#define ram_nrf_last_update 2
#define ram_nrf_device_name 3
///ram_nrf_device_last 430

#define ram_nrf_device_info_start 430
#define ram_nrf_device_len 12
#define ram_nrf_device_info_uptime 0
#define ram_nrf_device_info_send 4
#define ram_nrf_device_info_recv 6
#define ram_nrf_device_info_err 8
#define ram_nrf_device_info_renew 10
///#define ram_nrf_device_info_last 670



#define ram_nrf_meas_store_start 670
#define ram_nrf_meas_store_size 18
#define ram_nrf_meas_store_count 20
#define ram_nrf_meas_name_length 10

#define ram_nrf_meas_store_associate_device 0
#define ram_nrf_meas_store_last_update 1
#define ram_nrf_meas_store_name 2
#define ram_nrf_meas_store_value 12
#define ram_nrf_meas_store_value_type 16
#define ram_nrf_meas_store_used 17



#define tds_extend_temp_history_first_pos 1
#define tds_extend_temp_count 60



#define MENU_MAX_HISTORY 10

#define VERTICAL  0
#define HORIZONTAL 1
#define HORIZONTAL_NEW_LINE 2
#define VERTICAL_NEW_LINE 3


#define INDEX_DYN_MENU_ASSOCIATE_ONEWIRE 1
#define INDEX_DYN_MENU_KEYBOARD_NUMBER 2
#define INDEX_DYN_MENU_KEYBOARD_ALFA 3
#define INDEX_DYN_MENU_RTDS 4


#define MENU_DEFAULT_SCREEN  0
#define MENU_FUNKCE_SCREEN  1

#define MENU_NASTAVENI_ONEWIRE 3
#define MENU_NASTAVENI_TDS 4
#define MENU_REGULATOR 5
#define MENU_LIST_NASTAVENI_RTDS 6
#define MENU_NASTAVENI_RTDS_DETAIL 7
#define MENU_SELECT_DEFAULT_TEMP 8
#define MENU_NASTAVENI_RING_SCREEN 9
#define MENU_NASTAVENI_SELECT_RING_SCREEN 10
#define MENU_NASTAVENI_SELECT_INPUT_SENSORS 11
#define MENU_NASTAVENI_SELECT_PID_PARAMETRS 12
#define MENU_NASTAVENI_SELECT_PROGRAM_TERM 13
#define MENU_NASTAVENI_SETUP_PROGRAM_TERM 14
#define MENU_NASTAVENI_PROGRAMATOR 15
#define MENU_NASTAVENI_SITE 16
#define NEW_MENU_NRF_SETTING 17
#define NEW_MENU_VIRTUAL_OUTPUT_SETTING 18
#define MENU_NASTAVENI_SELECT_VIRTUAL_OUTPUT 19


#define MENU_DIALOG_YES_NO  100
#define MENU_DIALOG_KEYBOARD_NUMBER 101
#define MENU_DIALOG_SET_VARIABLE 102
#define MENU_DIALOG_KEYBOARD_ALFA 103
#define MENU_DIALOG_OK 104

//#define MENU_DIALOG_SELECT_TERM_MODE 104



#define NEW_MENU_NASTAVENI_SCREEN  2
#define NEW_MENU_SYSTEM_NASTAVENI_SCREEN 150
#define NEW_MENU_PERIFERIE_NASTAVENI_SCREEN 151
#define NEW_MENU_DISPLAY_NASTAVENI_SCREEN 152
#define NEW_MENU_NASTAVENI_CASU_SCREEN 153
#define NEW_MENU_NASTAVENI_MQTT_SCREEN 154
#define NEW_MENU_DISPLAY_NASTAVENI_BRIGTHNESS_SCREEN 155
#define NEW_MENU_ABOUT_DEVICE 156
#define NEW_MENU_DISPLAY_NASTAVENI_AUTO_SHUTDOWN_SCREEN 157
#define NEW_MENU_THERMOSTAT_SETTINGS 158
#define NEW_MENU_DIALOG_SELECT_TERM_MODE 159
#define NEW_MENU_THERMOSTAT_TIME_MENU 160
#define NEW_MENU_SHOW_ALL_TEMP 161
#define NEW_MENU_THERMOSTAT 162
#define NEW_MENU_THERMOSTAT_MAN_TEMP 163



#define MENU_ATTRIBUTES_CLEAN_DISPLAY 0
#define MENU_ATTRIBUTES_FILL_COLOR_RECTANGLE 1
#define MENU_ATTRIBUTES_DECORATE_MENU 2
#define MENU_REFRESH_ALL_SCREEN 3


#define SWITCH_BUDIK 1

#define MAX_SLIDERS 6
#define MENU_SLIDER_ONE_WIRE 0
#define MENU_SLIDER_RTDS 1
#define MENU_SLIDER_DEFAULT_TEMP 2
#define MENU_SLIDER_INPUT_TEMP 3
#define MENU_SLIDER_ALL_THERMOSTAT 4
#define MENU_SLIDER_VIRTUAL_OUTPUT 5
#define MENU_SLIDER_OFF 255


#define INPUT_SENSOR_SHOW_ALL 1
#define INPUT_SENSOR_SHOW_ACTIVE 0
#define INPUT_SENSOR_SHOW_DEACTIVE 2

#define NUMBER_TYPE_FLOAT 0
#define NUMBER_TYPE_INT 1

/*  r     g    b */
#define BLACK        0x0000  /*   0,   0,   0 */
#define BLUE         0x001F  /*   0,   0, 255 */
#define RED          0xF800  /* 255,   0,   0 */
#define GREEN        0x07E0  /*   0, 255,   0 */
#define CYAN         0x07FF  /*   0, 255, 255 */
#define MAGENTA      0xF81F  /* 255,   0, 255 */
#define YELLOW       0xFFE0  /* 255, 255,   0 */
#define WHITE        0xFFFF  /* 255, 255, 255 */
#define NAVY         0x000F  /*   0,   0, 128 */
#define DARKGREEN    0x03E0  /*   0, 128,   0 */
#define DARKCYAN     0x03EF  /*   0, 128, 128 */
#define MAROON       0x7800  /* 128,   0,   0 */
#define PURPLE       0x780F  /* 128,   0, 128 */
#define OLIVE        0x7BE0  /* 128, 128,   0 */
#define LIGHTGREY    0xC618  /* 192, 192, 192 */
//#define LIGHTGREY    0x7BEF  /* 128, 128, 128 */
#define DARKGREY     0x7BEF  /* 128, 128, 128 */
#define ORANGE       0xFD20  /* 255, 165,   0 */
#define GREENYELLOW  0xAFE5  /* 173, 255,  47 */
#define PINK         0xF81F  /* 255,   0, 255 */
#define GRAY 54938







#define DIALOG_SET_VARIABLE_GENERAL 0
#define DIALOG_SET_VARIABLE_PID_P 0
#define DIALOG_SET_VARIABLE_PID_I 1
#define DIALOG_SET_VARIABLE_PID_D 2
#define DIALOG_SET_VARIABLE_PID_T 3

#define KEYBOARD_SIZE_CHAR_UPPER 26
#define KEYBOARD_SIZE_CHAR_LOWER 26
#define KEYBOARD_SIZE_CHAR_SPECIAL 26
#define KEYBOARD_TYPE_UPPER 0
#define KEYBOARD_TYPE_LOWER 1
#define KEYBOARD_TYPE_SPECIAL 2

const char keyboad_number_char_upper[KEYBOARD_SIZE_CHAR_UPPER] PROGMEM =     {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
const char keyboad_number_char_lower[KEYBOARD_SIZE_CHAR_LOWER] PROGMEM =     {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
const char keyboad_number_char_special[KEYBOARD_SIZE_CHAR_SPECIAL] PROGMEM = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '!', '@', '#', '$', '%', '^', '&', '*', '|', '+', '-', '_', '?', ',', '.', '/'};



typedef void (*ret_string_fptr)(uint8_t args1, uint8_t args2, uint8_t args3, char *line1, char *line2);
typedef void (*fptr_coordinate_xy)(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint16_t args1, uint8_t args2, char *text);
typedef void (*fptr_args)(uint16_t args1, uint16_t args2, uint8_t args3);
typedef uint8_t (*ret_fptr)(uint16_t args1, uint16_t args2, uint8_t args3);
typedef uint8_t (*ret_fptr_no_args)(void);

typedef void (*fptr_save_function)(uint16_t args, float now, uint8_t now1);




typedef struct t_Element_Function_1
{
  uint16_t x;
  uint16_t y;
  uint16_t args;
  fptr_coordinate_xy fnt_coordinate_xy;
  uint16_t size_x;
  uint16_t size_y;
  uint8_t redraw_class;
  fptr_args onclick;
  ret_fptr enable_show;
  const char *name;
} Element_Function_1;




typedef struct t_Element_Dyn_Button_1
{
  uint16_t first_x;
  uint16_t first_y;
  uint16_t size_x;
  uint16_t size_y;
  uint8_t font_size;
  uint8_t step_x;
  uint8_t step_y;
  uint8_t direction;
  uint8_t max_items_count;
  uint8_t max_row_count;
  uint8_t slider_args;
  uint8_t args;
  ret_string_fptr get_status_string;
  fptr_args dyn_button_onclick;
  ret_fptr function_for_max_items;
  uint8_t redraw_class;
} Element_Dyn_Button_1;


typedef struct t_Element_Dyn_Symbol_1
{
  uint16_t first_x;
  uint16_t first_y;
  uint16_t size_x;
  uint16_t size_y;
  uint8_t font_size;
  uint8_t step_x;
  uint8_t step_y;
  uint8_t direction;
  uint8_t max_items_count;
  uint8_t max_row_count;
  uint8_t slider_args;
  uint8_t args;
  ret_string_fptr get_status_string;
  fptr_args dyn_symbol_onclick;
  ret_fptr function_for_max_items;
  uint8_t redraw_class;
} Element_Dyn_Symbol_1;

typedef struct t_Element_Dyn_Select_1
{
  uint16_t first_x;
  uint16_t first_y;
  uint16_t size_x;
  uint16_t size_y;
  uint8_t font_size_1;
  uint8_t font_size_2;
  uint16_t color_active;
  uint16_t color_inactive;
  uint8_t step_x;
  uint8_t step_y;
  uint8_t direction;
  uint8_t max_items_count;
  uint8_t max_row_count;
  uint8_t slider_args;
  uint8_t args;
  ret_string_fptr get_status_string;
  fptr_args dyn_symbol_onclick;
  ret_fptr function_for_max_items;
  ret_fptr get_status_fnt;
  uint8_t redraw_class;
} Element_Dyn_Select_1;


typedef struct t_Element_Symbol_1
{
  char znak;
  uint16_t x;
  uint16_t y;
  uint16_t size_x;
  uint16_t size_y;
  uint8_t znak_size;
  uint8_t args;
  fptr_args onclick;
  uint8_t redraw_class;
  ret_fptr enable_show;
} Element_Symbol_1;

typedef struct t_Element_Button_1
{
  const char *name;
  uint16_t x;
  uint16_t y;
  uint16_t size_x;
  uint16_t size_y;
  uint8_t font_size;
  uint8_t args;
  fptr_args onclick;
  uint8_t redraw_class;
  ret_fptr enable_show;
} Element_Button_1;

typedef struct t_Element_Button_2
{
  const char *name;
  uint16_t x;
  uint16_t y;
  uint16_t size_x;
  uint16_t size_y;
  uint8_t font_size;
  uint16_t color_active;
  uint16_t color_inactive;
  uint8_t args;
  fptr_args onclick;
  ret_fptr get_status_fnt;
  uint8_t redraw_class;
  ret_fptr enable_show;
} Element_Button_2;

/*
  typedef struct t_Element_Select_Box_1
  {
  char *name;
  uint16_t x;
  uint16_t y;
  uint16_t size_x;
  uint16_t size_y;
  uint8_t font_size;
  uint16_t color_active;
  uint16_t color_inactive;
  uint8_t args;
  fptr_args onclick;
  ret_fptr get_status_fnt;
  uint8_t redraw_class;
  } Element_Select_Box_1;
*/

typedef struct t_Element_Switch_1
{
  const char *name;
  uint16_t x;
  uint16_t y;
  uint16_t size_x;
  uint16_t size_y;
  uint8_t font_size;
  uint16_t color_active;
  uint16_t color_inactive;
  uint8_t args;
  fptr_args onclick;
  ret_fptr get_status_fnt;
  ret_string_fptr get_status_string;
  uint8_t redraw_class;
} Element_Switch_1;




typedef struct t_Menu1
{
  const char *name;
  Element_Button_1 button_1[10];
  Element_Button_2 button_2[5];
  Element_Function_1 function_1[6];
  Element_Switch_1 switch_1[3];
  Element_Dyn_Button_1 dyn_button[2];
  Element_Symbol_1 symbol_button_1[8];
  Element_Dyn_Symbol_1 dyn_symbol_1[3];
  Element_Dyn_Select_1 dyn_select_box_1[1];
  uint8_t len_button_1;
  uint8_t len_button_2;
  uint8_t len_function_1;
  uint8_t len_switch_1;
  uint8_t len_dyn_button_1;
  uint8_t len_symbol_button_1;
  uint8_t len_dyn_symbol_1;
  uint8_t len_dyn_select_box_1;
  uint8_t idx;
  uint16_t x;
  uint16_t y;
  uint16_t size_x;
  uint16_t size_y;
  uint8_t atributes;
  uint16_t color_background;
  uint8_t redraw_class;
  ret_fptr redraw_class_0;
  ret_fptr redraw_class_1;
  ret_fptr redraw_class_2;
  ret_fptr redraw_class_3;
  ret_fptr preload_function;
} Menu1;




typedef struct t_MenuAll
{
  uint8_t len_menu1;
  uint8_t len_menu2;
  uint8_t len_menu3;
  uint8_t len_menu4;
  Menu1 ListMenu1[10];
  Menu1 ListMenu2[10];
  Menu1 ListMenu3[10];
  Menu1 ListMenu4[10];
} MenuAll;




#endif
