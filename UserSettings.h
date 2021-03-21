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
#define set_default_values 90
#define my_default_ring 92
#define my_default_show_temp 93


#define MAX_RTDS 10
#define RTDS_DEVICE_TOTAL_LEN 20
#define RTDS_DEVICE_STRING_LEN 18
#define RTDS_DEVICE_ACTIVE_BYTE_POS 19

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

#define ram_remote_tds_store_size 3
#define ram_remote_tds_store_data_low 100
#define ram_remote_tds_store_data_high 101
#define ram_remote_tds_store_last_update 102

#define ram_remote_tds_store_last (ram_remote_tds_store_data_low+(ram_remote_tds_store_size*MAX_RTDS))


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



#define MENU_DIALOG_YES_NO  100
#define MENU_DIALOG_KEYBOARD_NUMBER 101
#define MENU_DIALOG_SET_VARIABLE 102
#define MENU_DIALOG_KEYBOARD_ALFA 103
#define MENU_DIALOG_OK 104

#define MENU_DIALOG_SELECT_TERM_MODE 104



#define NEW_MENU_NASTAVENI_SCREEN  2
#define NEW_MENU_SYSTEM_NASTAVENI_SCREEN 150
#define NEW_MENU_PERIFERIE_NASTAVENI_SCREEN 151
#define NEW_MENU_DISPLAY_NASTAVENI_SCREEN 152
#define NEW_MENU_NASTAVENI_CASU_SCREEN 153


#define MENU_ATTRIBUTES_CLEAN_DISPLAY 0
#define MENU_ATTRIBUTES_FILL_COLOR_RECTANGLE 1
#define MENU_ATTRIBUTES_DECORATE_MENU 2
#define MENU_REFRESH_ALL_SCREEN 3


#define SWITCH_BUDIK 1

#define MENU_SLIDER_ONE_WIRE 0
#define MENU_SLIDER_RTDS 1
#define MENU_SLIDER_DEFAULT_TEMP 2
#define MENU_SLIDER_INPUT_TEMP 3
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

const char term_title[] PROGMEM = "Pokojovy regulator v3";
const char text_touchscreen[] PROGMEM = "touchscreen";
const char text_nastaveni_systemu[] PROGMEM = "nastaveni systemu";
const char text_onewire_rozhrani[] PROGMEM = "1wire rozhrani";
const char text_onewire_hledam[] PROGMEM = "1wire hledam";
const char text_vnitrni_ochrany[] PROGMEM = "Vnitrni ochrany";
const char text_ethernet_rozhrani[] PROGMEM = "Ethernet rozhrani";
const char text_mqtt_rozhrani[] PROGMEM = "MQTT rozhrani";
const char text_rest_rozhrani[] PROGMEM = "REST API rozhrani";
const char text_nrf_rozhrani[] PROGMEM = "NRF rozhrani";
const char text_pid_regulator[] PROGMEM = "PID regulator";
const char text_ntp_cas[] PROGMEM = "NTP internetovy cas";
const char text_rtc_cas[] PROGMEM = "RTC vnitrni hodiny";
const char text_vychozi_nastaveni[] PROGMEM = "Vychozi nastaveni";
const char text_bezny_start[] PROGMEM = "Bezny start";
const char text_link_status[] PROGMEM = "Link status";
const char text_test_ram[] PROGMEM = "Test SPI-RAM";
const char text_test_eeprom[] PROGMEM = "Test SPI-EEPROM";
const char current_time[] PROGMEM = "Aktualni cas";
const char current_temp[] PROGMEM = "Aktualni teplota:";
const char current_temp_short[] PROGMEM = "teplota:";
const char temp_offset_short[] PROGMEM = "offset:";
const char nastaveni_text[] PROGMEM = "Nastaveni";
const char nastaveni_ring_text[] PROGMEM = "Nastaveni regulatoru";
const char text_current_ring_mode[] PROGMEM =  "Aktualni mod regulatoru";
const char text_change_default_ring[] PROGMEM = "Zmenit vychozi regulatoru";
const char funkce_text[] PROGMEM = "Funkce";
const char regulator_text[] PROGMEM = "Regulator";
const char regulator_default_text[] PROGMEM = "Vychozi regulator:";
const char ring_text_setup[] PROGMEM = "Nastaveni";
const char text_set_default[] PROGMEM = "Vychozi hodnoty";

const char budik_text[] PROGMEM = "Budik";
const char button_zpet[] PROGMEM = "Zpet";

const char text_button_term_off[] PROGMEM = "OFF";
const char text_button_term_off_comment[] PROGMEM = "Vypnuto, ochranny mod";
const char text_button_term_max[] PROGMEM = "MAX";
const char text_button_term_max_comment[] PROGMEM = "Zapnuto, bez regulace";
const char text_button_term_min[] PROGMEM = "MIN";
const char text_button_term_min_comment[] PROGMEM = "Chlazeni, bez regulace";
const char text_button_term_prog[] PROGMEM = "PROG";
const char text_button_term_prog_comment[] PROGMEM = "Automaticky program";
const char text_button_term_man[] PROGMEM = "MAN";
const char text_button_term_man_comment[] PROGMEM = "Rucni ovladani";


const char text_button_mode_heat[] PROGMEM = "Topeni";
const char text_button_mode_cool[] PROGMEM = "Chlazeni";
const char text_button_term_fan[] PROGMEM = "FAN";

const char text_term_active[] PROGMEM = "Aktivni";
const char text_term_deactive[] PROGMEM = "Neaktivni";
const char text_term_do_activate[] PROGMEM = "Aktivovat novy regulator?";
const char text_term_do_deactivate[] PROGMEM = "Deaktivovat regulator";

const char nastaveni_site[] PROGMEM = "Nastaveni site";
const char nastaveni_nrf[] PROGMEM = "NRF pripojeni";
const char nastaveni_default[] PROGMEM = "Reset nastaveni";
const char nastaveni_onewire[] PROGMEM = "Lokalni cidla";
const char nastaveni_rtds[] PROGMEM = "Vzdalene cidla";

const char text_onewire_list[] PROGMEM = "Seznam nalezenych 1wire";
const char text_tds_sensors[] PROGMEM = "Vlastnosti lokalniho cidla";
const char text_associovat_tds[] PROGMEM = "Priradit nove cidlo";
const char text_nastavit_tds[] PROGMEM = "Nastavit TDS cidlo";
const char text_nastavit_rtds[] PROGMEM = "Nastavit vzdalene cidlo";
const char text_assocoivat[] PROGMEM = "Nastavit cidlo";
const char text_assocoivat_output[] PROGMEM = "Nastavit vystup";
const char text_nastavit_pid[] PROGMEM = "PID volby";
const char text_rtds_prefix[] PROGMEM = "/";
const char text_not_used[] PROGMEM = "Neni pouzito";
const char text_rtds_novy[] PROGMEM = "Pridat nove vzdalene cidlo";
const char nastaveni_delete_sensor[] PROGMEM = "Vymazat cidlo";
const char nastaveni_deassociate_sensor[] PROGMEM = "Uvolnit cidlo";
const char nastaveni_deassociate_program[] PROGMEM = "Uvolnit program";
const char nastaveni_tds_period[] PROGMEM = "Merici perioda";
const char nastaveni_name_sensor[] PROGMEM = "Nastavit nazev";
const char nastaveni_tds_offset[] PROGMEM = "Nastavit offset";
const char nastaveni_rtds_subscribe[] PROGMEM = "MQTT prihlaseni";
const char text_nazev[] PROGMEM = "Nazev:";
const char text_online[] PROGMEM = "Online";
const char text_last_update[] PROGMEM = "Posledni aktualizace:";
const char text_select_default_temp[] PROGMEM = "Vyber zobrazeni teplomeru";
const char text_select_term_mode[] PROGMEM = "Vyber modu regulatoru";
const char text_select_term_default_ring[] PROGMEM = "Vyber vychoziho regulatoru";
const char text_nastaveni_ring_program[] PROGMEM = "Vyber programu";
const char text_nastaveni_setup_program[] PROGMEM = "Nastaveni programu";
const char text_nastaveni_programator[] PROGMEM = "Nastaveni programatoru";
const char text_prog[] PROGMEM = "PROG";
const char text_ntp_sync_time[] PROGMEM = "Aktualizace casu";


const char text_upozorneni[] PROGMEM = "Upozorneni";
const char text_klavesnice[] PROGMEM = "Klavesnice";

const char text_yes[] PROGMEM = "Ano";
const char text_no[] PROGMEM = "Ne";

const char text_zrusit[] PROGMEM = "Zrusit";
const char text_ulozit[] PROGMEM = "Ulozit";
const char text_delete_char[] PROGMEM = "Smaz znak";
const char text_err[] PROGMEM = "Error";
const char text_ok[] PROGMEM = "OK";


const char text_upper_case[] PROGMEM = "A";
const char text_lower_case[] PROGMEM = "a";
const char text_special_case[] PROGMEM = "@";
const char text_dashdash[] PROGMEM = "---";

const char char_P[] PROGMEM = "P";
const char char_I[] PROGMEM = "I";
const char char_D[] PROGMEM = "D";
const char char_T[] PROGMEM = "T";
const char char_NULL[] PROGMEM = "";


const char global_time_set[] PROGMEM = "global/time/set";
const char global_time_ntp[] PROGMEM = "global/time/ntp";
const char global_time_offset[] PROGMEM = "global/time/ntp_offset";
const char thermctl_header_in[] PROGMEM  = "/thermctl-in/";
const char thermctl_header_out[] PROGMEM  = "/thermctl-out/";

const char termbig_header_in[] PROGMEM  = "/termbig-in/";
const char termbig_header_out[] PROGMEM  = "/termbig-out/";

const char lightctl_header_in[] PROGMEM  = "/lightctl-in/";
const char lightctl_header_out[] PROGMEM  = "/lightctl-out/";

const char thermctl_subscribe[] PROGMEM = "/ctl/thermctl/subscribe";
const char termbig_subscribe[] PROGMEM = "/ctl/termbig/subscribe";


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
typedef void (*fptr_coordinate_xy)(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint8_t args1, uint8_t args2, char *text);
typedef void (*fptr_args)(uint16_t args1, uint16_t args2, uint8_t args3);
typedef uint8_t (*ret_fptr)(uint16_t args1, uint16_t args2, uint8_t args3);

typedef void (*fptr_save_function)(uint16_t args, float now, uint8_t now1);


//#define REDRAW_FORCE 3
#define REDRAW_EVERY_500MS 0
#define REDRAW_EXTERNAL_EVENT 1
#define REDRAW_EVERY_10SEC 2

#define REDRAW_CLASS_SHOW_TIME (1<<REDRAW_FORCE | 1<<REDRAW_EVERY_500MS)

#define REDRAW_PROGRAM_BUTTON (1<<REDRAW_EXTERNAL_EVENT| 1<<REDRAW_FORCE)
#define REDRAW_CLASS_SHOW (1<<REDRAW_FORCE | 1<< REDRAW_EVERY_10SEC)


#define REDRAW_BUTTON (1<<REDRAW_FORCE)

#define REDRAW_FORCE 3
#define REDRAW_CLASS_0 0
#define REDRAW_CLASS_1 1
#define REDRAW_CLASS_2 2

typedef struct t_Element_Function_1
{
  uint16_t x;
  uint16_t y;
  uint8_t args;
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
  Element_Function_1 function_1[4];
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
  ret_fptr preload_function;
} Menu1;




typedef struct t_MenuAll
{
  uint8_t len_menu1;
  uint8_t len_menu2;
  uint8_t len_menu3;
  uint8_t len_menu4;
  Menu1 ListMenu1[10];
  Menu1 ListMenu2[6];
  Menu1 ListMenu3[6];
  Menu1 ListMenu4[6];
} MenuAll;




#endif
