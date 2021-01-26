#ifndef __PROJECT_DEFAULT_H
#define __PROJECT_DEFAULT_H

#include <avr/pgmspace.h>


#define HW_ONEWIRE_MAXROMS 6
#define HW_ONEWIRE_MAXDEVICES 6
#define DS2482_COUNT 1

#define eeprom_wire_know_rom 200
#define eeprom_thermostat 332
#define eeprom_nrf_start 1500



#define tds_extend_temp_history_first_pos 1
#define tds_extend_temp_count 60

#define MENU_MAX_HISTORY 10

#define VERTICAL  0
#define HORIZONTAL 1

/*
#define INDEX_BUTTON_STATE_OFF 1
#define INDEX_BUTTON_STATE_MAX 2
#define INDEX_BUTTON_STATE_MIN 3
#define INDEX_BUTTON_STATE_PROG 4
#define INDEX_BUTTON_STATE_MAN 5

///#define INDEX_BUTTON_NASTAVENI 6
///#define INDEX_BUTTON_FUNKCE 7
///#define INDEX_BUTTON_REGULATOR 8

#define INDEX_FUNCTION_TIME 9
#define INDEX_FUNCTION_TEMP 10
#define INDEX_FUNCTION_DATE 11

#define INDEX_MENU_HLAVNI 12
#define INDEX_DYN_BUTTON_ASSOCIATE 13

//#define INDEX_BUTTON_NASTAVENI_SITE 13
//#define INDEX_BUTTON_NASTAVENI_NRF 14
//#define INDEX_BUTTON_NASTAVENI_DEFAULT 15
//#define INDEX_BUTTON_BACK 16
#define INDEX_MENU_NASTAVENI 17
//#define INDEX_MENU_NASTAVENI_ONEWIRE 18
//#define INDEX_MENU_NASTAVENI_RTDS 19
//#define INDEX_MENU_NASTAVENI_TDS 20

//#define INDEX_MENU_FUNKCE 21
#define INDEX_MENU_BUDIK 22
//#define FREE_23 23
//#define FREE_24 24
//#define FREE_ 25
//#define FREE_26 26
//#define FREE_27 27
//#define FREE_28 28

*/
#define INDEX_DYN_MENU_ASSOCIATE_ONEWIRE 1



#define MENU_DEFAULT_SCREEN  0
#define MENU_FUNKCE_SCREEN  1
#define MENU_NASTAVENI_SCREEN  2
#define MENU_NASTAVENI_ONEWIRE 3
#define MENU_NASTAVENI_TDS 4

#define MENU_DIALOG_YES_NO  100
#define MENU_DIALOG_KEYBOARD 101


#define MENU_ATTRIBUTES_CLEAN_DISPLAY 0
#define MENU_ATTRIBUTES_FILL_COLOR_RECTANGLE 1
#define MENU_ATTRIBUTES_DECORATE_MENU 2 
#define MENU_REFRESH_ALL_SCREEN 3
//#define MENU_REFRESH_DISABLE 4


#define SWITCH_BUDIK 1

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

const char term_title[] PROGMEM = "Pokojovy termostat v3";
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
const char nastaveni_text[] PROGMEM = "Nastaveni";
const char funkce_text[] PROGMEM = "Funkce";
const char regulator_text[] PROGMEM = "Regulator";
const char budik_text[] PROGMEM = "Budik";
const char button_zpet[] PROGMEM = "Zpet";

const char button_term_off[] PROGMEM = "OFF";
const char button_term_max[] PROGMEM = "MAX";
const char button_term_min[] PROGMEM = "MIN";
const char button_term_prog[] PROGMEM = "PROG";
const char button_term_man[] PROGMEM = "MAN";

const char nastaveni_site[] PROGMEM = "Nastaveni site";
const char nastaveni_nrf[] PROGMEM = "NRF pripojeni";
const char nastaveni_default[] PROGMEM = "Reset nastaveni";
const char nastaveni_onewire[] PROGMEM = "Lokalni cidla";
const char nastaveni_rtds[] PROGMEM = "Vzdalene cidla";

const char text_onewire_list[] PROGMEM = "Seznam nalezenych 1wire";
const char text_tds_sensors[] PROGMEM = "Vlastnosti TDS snimace: ";
const char text_onewire_asociace[] PROGMEM = "Priradit nove cidlo";
const char text_onewire_deasociace[] PROGMEM = "Vymazat cidlo";

const char text_upozorneni[] PROGMEM = "Upozorneni";

const char text_yes[] PROGMEM = "Ano";
const char text_no[] PROGMEM = "Ne";

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

typedef void (*ret_string_fptr)(uint8_t args, char *line1, char *line2);
typedef void (*fptr_xy)(uint16_t x, uint16_t y);
typedef void (*fptr_args)(uint16_t args1, uint16_t args2);

typedef void (*fptr)(uint8_t args);
typedef uint8_t (*ret_fptr)(uint8_t args);
typedef void (*function)(uint8_t args, uint8_t *ret, char *tmp);

typedef struct t_Element_Function_1
{
  uint16_t x;
  uint16_t y;
  uint8_t args;
  fptr_xy fnt;
  ret_fptr redraw;
} Element_Function_1;



typedef struct t_Element_Dyn_Button_1
{
  uint16_t first_x;
  uint16_t first_y;
  uint16_t size_x;
  uint16_t size_y;
  uint8_t font_size;
  uint8_t step_xy;
  uint8_t direction;
  uint16_t max_size;
  uint8_t args;
  ret_string_fptr get_status_string;
  fptr_args dyn_button_onclick;
  ret_fptr function_for_max_items;
  ret_fptr redraw;
} Element_Dyn_Button_1;

typedef struct t_Element_Symbol_1
{
  char znak;
  uint16_t x;
  uint16_t y;
  uint16_t size_x;
  uint16_t size_y;
  uint8_t args;
  fptr onclick;
  ret_fptr redraw;
} Element_Symbol_1;

typedef struct t_Element_Button_1
{
  char *name;
  uint16_t x;
  uint16_t y;
  uint16_t size_x;
  uint16_t size_y;
  uint8_t font_size;
  uint8_t args;
  fptr onclick;
  ret_fptr redraw;
} Element_Button_1;

typedef struct t_Element_Button_2
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
  fptr onclick;
  ret_fptr get_status_fnt;
  ret_fptr redraw;
} Element_Button_2;

typedef struct t_Element_Switch_1
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
  fptr onclick;
  ret_fptr get_status_fnt;
  ret_string_fptr get_status_string;
  ret_fptr redraw;
} Element_Switch_1;


typedef struct t_Dialog
{

}Dialog;

typedef struct t_Menu1
{
  char *name;
  Element_Button_1 button_1[10];
  Element_Button_2 button_2[5];
  Element_Function_1 function_1[3];
  Element_Switch_1 switch_1[3];
  Element_Dyn_Button_1 dyn_button[1];
  Element_Symbol_1 symbol_button_1[2];
  uint8_t len_button_1;
  uint8_t len_button_2;
  uint8_t len_function_1;
  uint8_t len_switch_1;
  uint8_t len_dyn_button_1;
  uint8_t len_symbol_button_1;
  uint8_t idx;
  uint16_t x;
  uint16_t y;
  uint16_t size_x;
  uint16_t size_y;
  uint8_t atributes;
  uint16_t color_background;
  ret_fptr redraw;
} Menu1;




typedef struct t_MenuAll
{
  uint8_t len_menu1;
  Menu1 ListMenu1[10];
} MenuAll;



#endif
