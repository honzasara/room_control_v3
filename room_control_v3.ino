#include "room_control_v3.h"
#include <LCDWIKI_GUI.h> //Core graphics library
#include <LCDWIKI_KBV.h> //Hardware-specific library
#include <LCDWIKI_TOUCH.h> //touch screen library
#include <SoftSPIB.h>
#include <SRAM_23LC.h>
#include <EEPROM_CAT25.h>
#include <SPI.h>
#include <Ethernet2.h>
#include <EthernetUdp2.h>
#include "saric_utils.h"
#include <PID_v1.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <avr/wdt.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "RF24Network.h"
#include "RF24Mesh.h"
#include "printf.h"

#include <avr/wdt.h>
#include <avr/boot.h>
#include <RTClib.h>
#include <EEPROM.h>
#include <Temperature_LM75_Derived.h>
#include "saric_thermostat.h"
#include "saric_tds_function.h"
#include "saric_mqtt_network.h"
#include "saric_nrf.h"
#include "saric_ha2d.h"

#include <avr/pgmspace.h>

#define ETH_CS 15
#define ETH_RST 6
#define STORAGE_MOSI  24
#define STORAGE_MISO  26
#define STORAGE_CLK 25
#define STORAGE_RAM_CS  22
#define STORAGE_EEPROM_CS  23
#define PIEZO 12
#define NRF_CS 2
#define NRF_CE 3
#define NRF_INT 5
#define LED 8
#define JUMPER 14


#define A_PROUD 3
#define A_TRIV 6
#define A_PETV 5
#define A_DVANACTV 4
#define A_LIGHT 7


#define CONST_PREVOD_PETV 8
#define CONST_PREVOD_TRIV 5
#define CONST_PREVOD_DVANACTV 57
#define CONST_PREVOD_PROUD 1





#define SELFTEST_ERR_RTC 0
#define SELFTEST_ERR_NTP 1
#define SELFTEST_ERR_ONEWIRE 2
#define SELFTEST_NRF_SCAN 4
#define SELFTEST_MQTT_LINK 5
#define SELFTEST_ETH_LINK 6
#define SELFTEST_RESTART_NEEDED 7

#define TDS_MEMORY_MAP_TDS 0
#define TDS_MEMORY_MAP_RTDS 16


#define STATUS_BIT_HEAT_OR_COOL 0

SoftSPIB swSPI(STORAGE_MOSI, STORAGE_MISO, STORAGE_CLK);


RTC_DS1307 rtc;
DateTime now;

EthernetClient ethClient;
EthernetUDP udpClient;
PubSubClient mqtt_client(ethClient);


RF24 radio(NRF_CE, NRF_CS);
RF24Network network(radio);
RF24Mesh mesh(radio, network);
Generic_LM75_11Bit lm75_temp;


LCDWIKI_KBV my_lcd(ST7796S, 33, 35, 36, 32, 34); //model,cs,cd,wr,rd,reset
LCDWIKI_TOUCH my_touch(28, 27, 29, 30, 7); //tcs,tclk,tdout,tdin,tirq

StaticJsonDocument<256> doc;

SRAM_23LC SRAM(&swSPI, STORAGE_RAM_CS, SRAM_23LC1024);
EEPROM_CAT25 SROM(&swSPI, STORAGE_EEPROM_CS , CAT25M02);




uint8_t last_output_update[MAX_THERMOSTAT];
uint8_t selftest_data = 0;

uint16_t tritri = 0;
uint16_t petnula = 0;
uint16_t dvanact = 0;
uint16_t proud = 0;
uint16_t light_curr = 0;
uint8_t a2d_run_now = 0;

uint32_t uptime = 0;
long int milis = 0;
long int milis_005s = 0;
long int milis_05s = 0;
long int milis_1s = 0;
long int milis_10s = 0;
long int milis_1ms = 0;



uint8_t default_ring = 0;
uint8_t default_show_temp = 0;

unsigned long load = 0;
unsigned long load_max = 0;
unsigned long load_min = 0xffffffff;

uint16_t light_min = 0;
uint16_t light_max = 0;
uint8_t auto_jas = 0;
uint8_t watchdog_state = 0;

uint8_t use_rtds = 0;
uint8_t use_tds = 0;

uint8_t menu_slider_data_current[5];
uint8_t menu_slider_data_max;
uint8_t menu_slider_data_min;
uint8_t menu_slider_data_max_element;

int menu_dialog_set_variable_now;
int menu_dialog_set_variable_max;
int menu_dialog_set_variable_min;
uint8_t menu_dialog_set_variable_update_now;
uint16_t menu_dialog_set_divider;
uint8_t menu_dialog_set_variable_step;
uint8_t menu_dialog_set_variable_args;


uint8_t dialog_yes_args1;
char dialog_text[32];

char dialog_set_string[16];
uint8_t dialog_set_string_max_length;
uint8_t dialog_set_string_args;
uint8_t dialog_set_string_keyboard_type = 0;

uint8_t menu_redraw05s = 0;
uint8_t menu_redraw10s = 0;
uint8_t change_term_mode = 0;

uint8_t MenuHistory[MENU_MAX_HISTORY];
uint8_t Global_menu_args1[MENU_MAX_HISTORY];
uint8_t MenuHistoryIndex = 0;





////////////////////////////////////////////////////////////////////////////////////////

fptr_args dialog_yes_function;
fptr_args dialog_save_variable_function;

/// null fake funkce ///
uint8_t returnnullfceargs(uint16_t args1, uint16_t args2);
void nullfceargs(uint16_t args1, uint16_t args2);
void nullfce(void);

/// primitivni funkce
void MenuHistoryNextMenu(uint8_t id, uint8_t args1);
void MenuHistoryPrevMenu(void);

uint8_t button_redraw(uint8_t args1, uint8_t args2);
uint8_t menu_redraw_time05s(uint8_t args1, uint8_t args2);



uint8_t menu_redraw_change_term_mode(uint8_t args1, uint8_t args2);
uint8_t menu_redraw_update_temp(uint8_t args1, uint8_t args2);


void display_element_show_time_1(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint8_t args1, uint8_t args2);
void display_element_show_temp_1(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint8_t args1, uint8_t args2);
void display_element_show_date_1(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint8_t args1, uint8_t args2);
void display_element_show_tds_info_static(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint8_t args1, uint8_t args2);
void display_element_show_tds_info_dynamics(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint8_t args1, uint8_t args2);
void display_element_show_rtds_info_dynamics(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint8_t args1, uint8_t args2);
void display_element_dialog_show_text(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y);
void display_element_dialog_set_variable(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint8_t args1, uint8_t args2);
void display_element_vertical_slider(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint8_t args1, uint8_t args2);
void display_element_dialog_default_ring(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint8_t args1, uint8_t args2);
void display_element_default_ring_set_temp(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint8_t args1, uint8_t args2);


void display_element_rectangle(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint8_t args1, uint8_t args2);


void display_element_dialog_set_string(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint8_t args1, uint8_t args2);

void display_function_set_variable(uint16_t now, uint16_t min, uint16_t max, uint8_t step, uint8_t args, uint16_t divider, uint8_t update_now);

uint16_t display_function_get_variable(void);
void display_function_set_variable_plus(void);
void display_function_set_variable_minus(void);








uint8_t get_function_thermostat_has_mode(uint8_t args);
void click_thermostat_set_mode(uint8_t args);
void click_thermostat_set_mode_via_dialog(uint8_t args1, uint8_t args2);

void click_rtds_add_sensor(uint8_t args1, uint8_t args);


void display_function_vertical_slider_inc(uint8_t args1, uint8_t args2);
void display_function_vertical_slider_dec(uint8_t args1, uint8_t args2);
void display_function_vertical_slider_get(uint8_t *start, uint8_t *stop);



void get_function_one_wire_associate_or_setting_text_button(uint8_t args1, uint8_t args2, char *line1, char *line2);
uint8_t get_function_one_wire_last_index_for_menu(uint8_t idx);
void click_tds_associate_or_setting_onewire(uint8_t args1, uint8_t idx);

void click_rtds_setting_sensor(uint8_t args1, uint8_t idx);

void click_tds_deassociate_onewire(uint8_t args1, uint8_t idx);
void click_rtds_deassociate_onewire(uint8_t args1, uint8_t idx);
void click_rtds_subscribe(uint8_t args1, uint8_t idx);

uint8_t get_function_rtds_active_items(uint8_t args1, uint8_t args2);
uint8_t get_function_rtds_last_index_for_menu(uint8_t args1, uint8_t args2);
void get_function_rtds_text_button(uint8_t args1, uint8_t args2, char *line1, char *line2);


uint8_t get_function_budik_enabled(uint8_t args);
void get_function_budik_text_state(uint8_t args1, uint8_t args2, char *text);


void click_dialog_button_yes(uint8_t args1, uint8_t args2);

void click_keyboard_type(uint8_t args1, uint8_t args2);




void display_menu_tds_set_offset(uint8_t args1, uint8_t args2);
void display_menu_tds_set_period(uint8_t args1, uint8_t args2);
void display_menu_tds_set_name(uint8_t args1, uint8_t args2);

void display_menu_rtds_update_name(uint8_t args1, uint8_t args2);


void dialog_set_variable_button_click(uint8_t args1, uint8_t args2);

void display_element_get_string(char *str);
void display_element_set_string(char *str, uint8_t max_length);
void display_element_set_string_del_char(uint8_t args1, uint8_t args2);

uint8_t get_function_keyboard_number_max_keys(uint8_t args1, uint8_t args2, uint8_t args3);
void click_keyboard_number(uint8_t args1, uint8_t args2);
void get_function_keyboard_number_char(uint8_t args1, uint8_t args2, char *line1, char *line2);


void get_function_keyboard_alfa_char(uint8_t args1, uint8_t args2, char *line1, char *line2);
void click_keyboard_alfa(uint8_t args1, uint8_t args2);
uint8_t get_function_keyboard_alfa_max_keys(uint8_t idx);

void click_select_default_temp(uint8_t args1, uint8_t args2);



void get_function_default_temp(uint8_t args1, uint8_t args2, char *line1, char *line2);
void click_function_default_temp(uint8_t args1, uint8_t args2);
uint8_t get_function_default_temp_max_items(uint8_t args1, uint8_t args2);
uint8_t get_function_default_temp_active(uint8_t args1, uint8_t args2, uint8_t args3);

uint8_t get_function_thermostat_mode_max_items(uint8_t args1, uint8_t args2, args3);

void get_funtion_default_ring_temp_mode(uint8_t args1, uint8_t args2, char *line1, char *line2);
void get_funtion_default_ring(uint8_t args1, uint8_t args2, char *line1, char *line2);
void set_function_thermostat_heat_or_cool(uint8_t args);
uint8_t get_function_thermostat_heat_or_cool(uint8_t args);
uint8_t display_enable_show_term_mode_man(uint16_t args1, uint16_t args2, uint8_t args3);

uint8_t display_enable_show(uint8_t args1, uint8_t args2);
uint8_t get_function_return_args_1(uint8_t args1, uint8_t args2);

uint8_t preload_regulator_menu(uint8_t args1, uint8_t args2, uint8_t args3);

///////////////////////////////////////////////////


const Element_Dyn_Symbol_1 keyboard_number PROGMEM = {.first_x = 10, .first_y = 60, .size_x = 30, .size_y = 30, .font_size = 2, .step_x = 40, .step_y = 40, .direction = HORIZONTAL_NEW_LINE, .max_items_count = 9, .max_row_count = 0, .slider_args = 0, .args = INDEX_DYN_MENU_KEYBOARD_NUMBER, .get_status_string = get_function_keyboard_number_char, .dyn_symbol_onclick =  click_keyboard_number, .function_for_max_items = get_function_keyboard_number_max_keys, .redraw_class = REDRAW_BUTTON,};
const Element_Dyn_Symbol_1 keyboard_alfa PROGMEM = {.first_x = 10, .first_y = 80, .size_x = 30, .size_y = 30, .font_size = 2, .step_x = 40, .step_y = 40, .direction = HORIZONTAL_NEW_LINE, .max_items_count = 10, .max_row_count = 0, .slider_args = 0, .args = INDEX_DYN_MENU_KEYBOARD_ALFA, .get_status_string = get_function_keyboard_alfa_char, .dyn_symbol_onclick =  click_keyboard_alfa, .function_for_max_items = get_function_keyboard_alfa_max_keys, .redraw_class = REDRAW_BUTTON,};
const Element_Button_1 button_keyboard_upper PROGMEM = {.name = text_upper_case, .x = 320, .y = 35, .size_x = 40, .size_y = 40, .font_size = 2, .args = KEYBOARD_TYPE_UPPER, .onclick = click_keyboard_type, .redraw_class = REDRAW_BUTTON, .enable_show = display_enable_show, };
const Element_Button_1 button_keyboard_lower PROGMEM = {.name = text_lower_case, .x = 365, .y = 35, .size_x = 40, .size_y = 40, .font_size = 2, .args = KEYBOARD_TYPE_LOWER, .onclick = click_keyboard_type, .redraw_class = REDRAW_BUTTON, .enable_show = display_enable_show,};
const Element_Button_1 button_keyboard_special PROGMEM = {.name = text_special_case, .x = 410, .y = 35, .size_x = 40, .size_y = 40, .font_size = 2, .args = KEYBOARD_TYPE_SPECIAL, .onclick = click_keyboard_type, .redraw_class = REDRAW_BUTTON, .enable_show = display_enable_show,};
const Element_Button_1 button_delete_char PROGMEM = {.name = text_delete_char, .x = 300, .y = 200, .size_x = 130, .size_y = 40, .font_size = 2, .args = 0, .onclick = display_element_set_string_del_char,  .redraw_class = REDRAW_BUTTON, .enable_show = display_enable_show,};

const Element_Dyn_Select_1 select_default_temp PROGMEM = {
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
  .args = 0,
  .get_status_string = get_function_default_temp,
  .dyn_symbol_onclick =  click_function_default_temp,
  .function_for_max_items = get_function_default_temp_max_items,
  .get_status_fnt = get_function_default_temp_active,
  .redraw_class = REDRAW_BUTTON,
};


const Element_Dyn_Select_1 select_term_mode PROGMEM = {
  .first_x = 10,
  .first_y = 20,
  .size_x = 120,
  .size_y = 40,
  .font_size_1 = 1,
  .font_size_2 = 1,
  .color_active = GREEN,
  .color_inactive = WHITE,
  .step_x = 1,
  .step_y = 50,
  .direction = VERTICAL,
  .max_items_count = 5,
  .max_row_count = 1,
  .slider_args = MENU_SLIDER_OFF,
  .args = 0,
  .get_status_string = get_function_default_temp,
  .dyn_symbol_onclick =  click_function_default_temp,
  .function_for_max_items = get_function_thermostat_mode_max_items,
  .get_status_fnt = get_function_default_temp_active,
  .redraw_class = REDRAW_BUTTON,
};


const Element_Dyn_Button_1 onewire_associace_button PROGMEM = {.first_x = 230, .first_y = 35, .size_x = 170, .size_y = 55, .font_size = 1, .step_x = 60, .step_y = 60, .direction = VERTICAL, .max_items_count = 3, .max_row_count = 1, .slider_args = MENU_SLIDER_ONE_WIRE, .args = INDEX_DYN_MENU_ASSOCIATE_ONEWIRE, .get_status_string = get_function_one_wire_associate_or_setting_text_button, .dyn_button_onclick =  click_tds_associate_or_setting_onewire, .function_for_max_items = get_function_one_wire_last_index_for_menu, .redraw_class = REDRAW_BUTTON,};

const Element_Switch_1 switch_budik PROGMEM = {.name = budik_text, .x = 10, .y = 40, .size_x = 190, .size_y = 120, .font_size = 2, .color_active = 0, .color_inactive = 0, .args = SWITCH_BUDIK, .onclick = nullfce, .get_status_fnt = get_function_budik_enabled, .get_status_string = get_function_budik_text_state, .redraw_class = REDRAW_BUTTON,};



const Element_Button_2 button_term_state_off PROGMEM = { .name = button_term_off, .x = 10, .y = 220, .size_x = 82, .size_y = 60, .font_size = 3, .color_active = GRAY, .color_inactive = WHITE, .args = TERM_MODE_OFF, .onclick = click_thermostat_set_mode, .get_status_fnt = get_function_thermostat_has_mode,  .redraw_class = REDRAW_PROGRAM_BUTTON, .enable_show = display_enable_show,};
const Element_Button_2 button_term_state_max PROGMEM = {.name = button_term_max, .x = 100, .y = 220, .size_x = 82, .size_y = 60, .font_size = 3, .color_active = RED, .color_inactive = WHITE, .args = TERM_MODE_MAX, .onclick = click_thermostat_set_mode, .get_status_fnt = get_function_thermostat_has_mode,  .redraw_class = REDRAW_PROGRAM_BUTTON, .enable_show = display_enable_show,};
const Element_Button_2 button_term_state_min PROGMEM = {.name = button_term_min, .x = 190, .y = 220, .size_x = 82, .size_y = 60, .font_size = 3, .color_active = BLUE, .color_inactive = WHITE, .args = TERM_MODE_MIN, .onclick = click_thermostat_set_mode, .get_status_fnt = get_function_thermostat_has_mode,  .redraw_class = REDRAW_PROGRAM_BUTTON, .enable_show = display_enable_show,};
const Element_Button_2 button_term_state_prog PROGMEM = {.name = button_term_prog, .x = 280, .y = 220, .size_x = 82, .size_y = 60, .font_size = 3, .color_active = YELLOW, .color_inactive = WHITE, .args = TERM_MODE_PROG, .onclick = click_thermostat_set_mode, .get_status_fnt = get_function_thermostat_has_mode,  .redraw_class = REDRAW_PROGRAM_BUTTON, .enable_show = display_enable_show,};
const Element_Button_2 button_term_state_man PROGMEM = {.name = button_term_man, .x = 370, .y = 220, .size_x = 82, .size_y = 60, .font_size = 3, .color_active = GREEN, .color_inactive = WHITE, .args = TERM_MODE_MAN, .onclick = click_thermostat_set_mode, .get_status_fnt = get_function_thermostat_has_mode,  .redraw_class = REDRAW_PROGRAM_BUTTON, .enable_show = display_enable_show,};

const Element_Dyn_Button_1 button_change_term_mode PROGMEM = {.first_x = 10, .first_y = 100, .size_x = 190, .size_y = 40, .font_size = 1, .step_x = 1, .step_y = 1, .direction = VERTICAL, .max_items_count = 1, .max_row_count = 1, .slider_args = 0, .args = 1, .get_status_string = get_funtion_default_ring_temp_mode, .dyn_button_onclick = click_thermostat_set_mode_via_dialog,  .function_for_max_items = get_function_return_args_1, .redraw_class = (1 << REDRAW_FORCE) };
const Element_Dyn_Button_1 button_change_ring PROGMEM = {.first_x = 10, .first_y = 40, .size_x = 190, .size_y = 40, .font_size = 1, .step_x = 1, .step_y = 1, .direction = VERTICAL, .max_items_count = 1, .max_row_count = 1, .slider_args = 0, .args = 1, .get_status_string = get_funtion_default_ring, .dyn_button_onclick = nullfce,  .function_for_max_items = get_function_return_args_1, .redraw_class = (1 << REDRAW_FORCE) };
const Element_Button_2 button_term_mode_heat PROGMEM = {.name = button_mode_heat, .x = 240, .y = 90, .size_x = 100, .size_y = 40, .font_size = 1, .color_active = RED, .color_inactive = WHITE, .args = TERM_MODE_MAN_HEAT, .onclick = set_function_thermostat_heat_or_cool, .get_status_fnt = get_function_thermostat_heat_or_cool,  .redraw_class = REDRAW_PROGRAM_BUTTON, .enable_show = display_enable_show_term_mode_man};
const Element_Button_2 button_term_mode_cool PROGMEM = {.name = button_mode_cool, .x = 360, .y = 90, .size_x = 100, .size_y = 40, .font_size = 1, .color_active = BLUE, .color_inactive = WHITE, .args = TERM_MODE_MAN_COOL, .onclick = set_function_thermostat_heat_or_cool, .get_status_fnt = get_function_thermostat_heat_or_cool,  .redraw_class = REDRAW_PROGRAM_BUTTON, .enable_show = display_enable_show_term_mode_man};
const Element_Button_1 button_nastaveni_ring PROGMEM = { .name = nastaveni_ring_text, .x = 10, .y = 160, .size_x = 190, .size_y = 40, .font_size = 1, .args = MENU_NASTAVENI_RING_SCREEN, .onclick = MenuHistoryNextMenu,  .redraw_class = (1 << REDRAW_FORCE), .enable_show = display_enable_show,};

const Element_Button_1 button_nastaveni_ring_nazev PROGMEM = { .name = nastaveni_name_sensor, .x = 280, .y = 40, .size_x = 190, .size_y = 40, .font_size = 1, .args = 0, .onclick = nullfce,  .redraw_class = (1 << REDRAW_FORCE), .enable_show = display_enable_show,};
const Element_Button_1 button_nastaveni_ring_input_sensor PROGMEM = { .name = text_assocoivat, .x = 280, .y = 100, .size_x = 190, .size_y = 40, .font_size = 1, .args = 0, .onclick = nullfce,  .redraw_class = (1 << REDRAW_FORCE), .enable_show = display_enable_show,};
const Element_Button_1 button_nastaveni_ring_output PROGMEM = { .name = text_assocoivat_output, .x = 280, .y = 160, .size_x = 190, .size_y = 40, .font_size = 1, .args = 0, .onclick = nullfce,  .redraw_class = (1 << REDRAW_FORCE), .enable_show = display_enable_show,};
const Element_Button_1 button_nastaveni_ring_pid PROGMEM = { .name = text_nastavit_pid, .x = 280, .y = 220, .size_x = 190, .size_y = 40, .font_size = 1, .args = 0, .onclick = nullfce,  .redraw_class = (1 << REDRAW_FORCE), .enable_show = display_enable_show,};



const Element_Button_1 button_nastaveni PROGMEM = { .name = nastaveni_text, .x = 290, .y = 30, .size_x = 170, .size_y = 40, .font_size = 2, .args = MENU_NASTAVENI_SCREEN, .onclick = MenuHistoryNextMenu,  .redraw_class = (1 << REDRAW_FORCE), .enable_show = display_enable_show,};
const Element_Button_1 button_funkce PROGMEM = {.name = funkce_text, .x = 290, .y = 90, .size_x = 170, .size_y = 40, .font_size = 2, .args = MENU_FUNKCE_SCREEN, .onclick = MenuHistoryNextMenu,  .redraw_class = (1 << REDRAW_FORCE), .enable_show = display_enable_show,};
const Element_Button_1 button_regulator PROGMEM = {.name = regulator_text, .x = 290, .y = 150, .size_x = 170, .size_y = 40, .font_size = 2, .args = MENU_REGULATOR, .onclick = MenuHistoryNextMenu,  .redraw_class = (1 << REDRAW_FORCE), .enable_show = display_enable_show,};

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
  .redraw_class = REDRAW_BUTTON,
};



const Element_Button_1 button_nastaveni_site PROGMEM = {
  .name = nastaveni_site,
  .x = 10,
  .y = 40,
  .size_x = 190,
  .size_y = 40,
  .font_size = 2,
  .args = 0,
  .onclick = nullfce,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};

const Element_Button_1 button_nastaveni_nrf PROGMEM = {
  .name = nastaveni_nrf,
  .x = 10,
  .y = 100,
  .size_x = 190,
  .size_y = 40,
  .font_size = 2,
  .args = 0,
  .onclick = nullfce,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};

const Element_Button_1 button_nastaveni_default PROGMEM = {
  .name = nastaveni_default,
  .x = 10,
  .y = 160,
  .size_x = 190,
  .size_y = 40,
  .font_size = 2,
  .args = 0,
  .onclick = nullfce,
  .redraw_class = REDRAW_BUTTON,
  .enable_show = display_enable_show,
};

const Element_Button_1 button_back PROGMEM = {.name = button_zpet, .x = 10, .y = 220, .size_x = 190, .size_y = 40, .font_size = 2, .args = 0, .onclick = MenuHistoryPrevMenu, .redraw_class = REDRAW_BUTTON, .enable_show = display_enable_show,};

const Element_Button_1 button_nastaveni_onewire PROGMEM = {.name = nastaveni_onewire, .x = 280, .y = 40, .size_x = 190, .size_y = 40, .font_size = 2, .args = MENU_NASTAVENI_ONEWIRE, .onclick = MenuHistoryNextMenu, .redraw_class = REDRAW_BUTTON, .enable_show = display_enable_show,};

const Element_Button_1 button_nastaveni_rtds PROGMEM = {.name = nastaveni_rtds, .x = 280, .y = 100, .size_x = 190, .size_y = 40, .font_size = 2, .args = MENU_LIST_NASTAVENI_RTDS, .onclick = MenuHistoryNextMenu, .redraw_class = REDRAW_BUTTON, .enable_show = display_enable_show,};

const Element_Button_1 button_default_show_temp PROGMEM = {.name = text_select_default_temp, .x = 280, .y = 160, .size_x = 190, .size_y = 40, .font_size = 1, .args = MENU_SELECT_DEFAULT_TEMP, .onclick = MenuHistoryNextMenu, .redraw_class = REDRAW_BUTTON, .enable_show = display_enable_show,};
/////

const Element_Button_1 button_tds_offset PROGMEM = {.name = nastaveni_tds_offset, .x = 280, .y = 40, .size_x = 190, .size_y = 40, .font_size = 2, .args = 0, .onclick = display_menu_tds_set_offset,  .redraw_class = REDRAW_BUTTON, .enable_show = display_enable_show,};
const Element_Button_1 button_tds_name PROGMEM = {.name = nastaveni_name_sensor, .x = 280, .y = 90, .size_x = 190, .size_y = 40, .font_size = 2, .args = 0, .onclick = display_menu_tds_set_name,  .redraw_class = REDRAW_BUTTON, .enable_show = display_enable_show,};
const Element_Button_1 button_tds_period PROGMEM = {.name = nastaveni_tds_period, .x = 280, .y = 140, .size_x = 190, .size_y = 40, .font_size = 2, .args = 0, .onclick = display_menu_tds_set_period,  .redraw_class = REDRAW_BUTTON, .enable_show = display_enable_show,};
const Element_Button_1 button_tds_delete PROGMEM = {.name = nastaveni_delete_sensor, .x = 280, .y = 190, .size_x = 190, .size_y = 40, .font_size = 2, .args = 0, .onclick = click_tds_deassociate_onewire,  .redraw_class = REDRAW_BUTTON, .enable_show = display_enable_show,};
const Element_Function_1 f_show_tds_info_static PROGMEM = {.x = 20, .y = 20, .args = 0, .fnt_coordinate_xy = display_element_show_tds_info_static,  .size_x = 0, .size_y = 0, .redraw_class = REDRAW_BUTTON, .onclick = nullfce, .enable_show = display_enable_show,};
const Element_Function_1 f_show_tds_info_dynamics PROGMEM = {.x = 20, .y = 20, .args = 0, .fnt_coordinate_xy = display_element_show_tds_info_dynamics,  .size_x = 0, .size_y = 0, .redraw_class = REDRAW_CLASS_SHOW, .onclick = nullfce, .enable_show = display_enable_show,};
/////////

const Element_Button_1 button_rtds_novy PROGMEM = {.name = text_rtds_novy, .x = 10, .y = 40, .size_x = 190, .size_y = 40, .font_size = 1, .args = 0, .onclick = click_rtds_add_sensor, .redraw_class = REDRAW_BUTTON, .enable_show = display_enable_show,};
const Element_Button_1 button_rtds_name PROGMEM = {.name = nastaveni_name_sensor, .x = 280, .y = 40, .size_x = 190, .size_y = 40, .font_size = 2, .args = 0, .onclick = display_menu_rtds_update_name,  .redraw_class = REDRAW_BUTTON, .enable_show = display_enable_show,};
const Element_Button_1 button_rtds_delete PROGMEM = {.name = nastaveni_delete_sensor, .x = 280, .y = 90, .size_x = 190, .size_y = 40, .font_size = 2, .args = 0, .onclick = click_rtds_deassociate_onewire,  .redraw_class = REDRAW_BUTTON, .enable_show = display_enable_show,};
const Element_Button_1 button_rtds_subscribe PROGMEM = {.name = nastaveni_rtds_subscribe, .x = 280, .y = 140, .size_x = 190, .size_y = 40, .font_size = 2, .args = 0, .onclick = click_rtds_subscribe,  .redraw_class = REDRAW_BUTTON, .enable_show = display_enable_show,};
const Element_Function_1 f_show_rtds_info_dynamics PROGMEM = {.x = 20, .y = 20, .args = 0, .fnt_coordinate_xy = display_element_show_rtds_info_dynamics,  .size_x = 0, .size_y = 0, .redraw_class = REDRAW_CLASS_SHOW, .onclick = nullfce, .enable_show = display_enable_show,};
const Element_Function_1 f_show_rtds_decorate PROGMEM = {.x = 220, .y = 20, .args = 0, .fnt_coordinate_xy = display_element_rectangle,  .size_x = 240, .size_y = 240, .redraw_class = REDRAW_BUTTON, .onclick = nullfce, .enable_show = display_enable_show,};
const Element_Function_1 f_vertical_slider_rtds PROGMEM = {  .x = 410, .y = 72, .args = MENU_SLIDER_RTDS, .fnt_coordinate_xy = display_element_vertical_slider,  .size_x = 40, .size_y = 126, .redraw_class = REDRAW_BUTTON, .onclick = nullfce, .enable_show = display_enable_show,};
const Element_Symbol_1 slider_menu_plus_rtds PROGMEM =  {.znak = '+', .x = 410, .y = 30, .size_x = 40, .size_y = 40, .znak_size = 2, .args = MENU_SLIDER_RTDS, .onclick = display_function_vertical_slider_dec, .redraw_class = REDRAW_BUTTON,};
const Element_Symbol_1 slider_menu_minus_rtds PROGMEM =  {.znak = '-', .x = 410, .y = 200, .size_x = 40, .size_y = 40, .znak_size = 2, .args = MENU_SLIDER_RTDS, .onclick = display_function_vertical_slider_inc,  .redraw_class = REDRAW_BUTTON,};


const Element_Button_1 dialog_yes PROGMEM = {.name = text_yes, .x = 30, .y = 60, .size_x = 100, .size_y = 40, .font_size = 2, .args = 0, .onclick = click_dialog_button_yes, .redraw_class = REDRAW_BUTTON, .enable_show = display_enable_show,};

const Element_Button_1 dialog_no PROGMEM = {.name = text_no, .x = 250, .y = 60, .size_x = 100, .size_y = 40, .font_size = 2, .args = 0, .onclick = MenuHistoryPrevMenu, .redraw_class = REDRAW_BUTTON, .enable_show = display_enable_show,};


/////
/////
const Element_Function_1 f_show_time PROGMEM = {.x = 10, .y = 30, .args = 0, .fnt_coordinate_xy = display_element_show_time_1, .size_x = 0, .size_y = 0, .redraw_class = (1 << REDRAW_FORCE | 1 << REDRAW_CLASS_0), .onclick = nullfce, .enable_show = display_enable_show,};
const Element_Function_1 f_show_temp PROGMEM = {.x = 10, .y = 130, .args = 0, .fnt_coordinate_xy = display_element_show_temp_1,  .size_x = 0, .size_y = 0, .redraw_class = (1 << REDRAW_FORCE | 1 << REDRAW_CLASS_2), .onclick = nullfce, .enable_show = display_enable_show,};
const Element_Function_1 f_show_date PROGMEM = {.x = 280, .y = 290, .args = 0, .fnt_coordinate_xy = display_element_show_date_1,  .size_x = 0, .size_y = 0, .redraw_class = REDRAW_CLASS_SHOW, .onclick = nullfce, .enable_show = display_enable_show,};
const Element_Function_1 f_dialog_show_text PROGMEM = {.x = 20, .y = 35, .args = 0, .fnt_coordinate_xy = display_element_dialog_show_text, .size_x = 0, .size_y = 0, .redraw_class = REDRAW_BUTTON, .onclick = nullfce, .enable_show = display_enable_show,};

const Element_Function_1 f_show_default_ring PROGMEM = {.x = 140, .y = 5, .args = 0, .fnt_coordinate_xy = display_element_dialog_default_ring, .size_x = 0, .size_y = 0, .redraw_class = REDRAW_BUTTON, .onclick = nullfce, .enable_show = display_enable_show,};
const Element_Function_1 f_default_ring_set_temp PROGMEM = {.x = 280, .y = 40, .args = 0, .fnt_coordinate_xy = display_element_default_ring_set_temp,  .size_x = 140, .size_y = 40, .redraw_class = REDRAW_BUTTON, .onclick = nullfce, .enable_show = display_enable_show_term_mode_man,};
const Element_Symbol_1 dialog_set_default_ring_temp_plus PROGMEM =  {.znak = '+', .x = 420, .y = 40, .size_x = 40, .size_y = 40, .znak_size = 2, .args = 0, .onclick = display_function_set_variable_plus,  .redraw_class = REDRAW_BUTTON, .enable_show = display_enable_show_term_mode_man};
const Element_Symbol_1 dialog_set_default_ring_temp_minus PROGMEM =  {.znak = '-', .x = 240, .y = 40, .size_x = 40, .size_y = 40, .znak_size = 2, .args = 0, .onclick = display_function_set_variable_minus,  .redraw_class = REDRAW_BUTTON, .enable_show = display_enable_show_term_mode_man};




const Element_Function_1 f_vertical_slider PROGMEM = {  .x = 410, .y = 72, .args = MENU_SLIDER_ONE_WIRE, .fnt_coordinate_xy = display_element_vertical_slider,  .size_x = 40, .size_y = 126, .redraw_class = REDRAW_BUTTON, .onclick = nullfce, .enable_show = display_enable_show,};
const Element_Symbol_1 slider_menu_plus PROGMEM =  {.znak = '+', .x = 410, .y = 30, .size_x = 40, .size_y = 40, .znak_size = 2, .args = MENU_SLIDER_ONE_WIRE, .onclick = display_function_vertical_slider_dec, .redraw_class = REDRAW_BUTTON, .enable_show = display_enable_show,};
const Element_Symbol_1 slider_menu_minus PROGMEM =  {.znak = '-', .x = 410, .y = 200, .size_x = 40, .size_y = 40, .znak_size = 2, .args = MENU_SLIDER_ONE_WIRE, .onclick = display_function_vertical_slider_inc,  .redraw_class = REDRAW_BUTTON, .enable_show = display_enable_show,};

const Element_Function_1 f_show_default_decorate PROGMEM = {.x = 5, .y = 30, .args = 0, .fnt_coordinate_xy = display_element_rectangle,  .size_x = 460, .size_y = 180, .redraw_class = REDRAW_BUTTON, .onclick = nullfce, .enable_show = display_enable_show,};
const Element_Function_1 f_vertical_slider_default_temp PROGMEM = {  .x = 410, .y = 72, .args = MENU_SLIDER_DEFAULT_TEMP, .fnt_coordinate_xy = display_element_vertical_slider,  .size_x = 40, .size_y = 86, .redraw_class = REDRAW_BUTTON, .onclick = nullfce, .enable_show = display_enable_show,};
const Element_Symbol_1 slider_menu_plus_default_temp PROGMEM =  {.znak = '+', .x = 410, .y = 30, .size_x = 40, .size_y = 40, .znak_size = 2, .args = MENU_SLIDER_DEFAULT_TEMP, .onclick = display_function_vertical_slider_dec, .redraw_class = REDRAW_BUTTON,};
const Element_Symbol_1 slider_menu_minus_default_temp PROGMEM =  {.znak = '-', .x = 410, .y = 160, .size_x = 40, .size_y = 40, .znak_size = 2, .args = MENU_SLIDER_DEFAULT_TEMP, .onclick = display_function_vertical_slider_inc,  .redraw_class = REDRAW_BUTTON,};


const Element_Function_1 f_dialog_set_variable PROGMEM = {.x = 100, .y = 45, .args = 0, .fnt_coordinate_xy = display_element_dialog_set_variable,  .size_x = 180, .size_y = 40, .redraw_class = REDRAW_BUTTON, .onclick = nullfce, .enable_show = display_enable_show,};
const Element_Symbol_1 dialog_set_variable_plus PROGMEM =  {.znak = '+', .x = 60, .y = 45, .size_x = 40, .size_y = 40, .znak_size = 2, .args = 0, .onclick = display_function_set_variable_plus,  .redraw_class = REDRAW_BUTTON, .enable_show = display_enable_show,};
const Element_Symbol_1 dialog_set_variable_minus PROGMEM =  {.znak = '-', .x = 280, .y = 45, .size_x = 40, .size_y = 40, .znak_size = 2, .args = 0, .onclick = display_function_set_variable_minus,  .redraw_class = REDRAW_BUTTON, .enable_show = display_enable_show,};

const Element_Function_1 f_dialog_set_string PROGMEM = {.x = 10, .y = 35, .args = 0, .fnt_coordinate_xy = display_element_dialog_set_string,  .size_x = 300, .size_y = 40, .redraw_class = REDRAW_BUTTON, .onclick = nullfce, .enable_show = display_enable_show,};

const Element_Button_1 dialog_set_variable_save PROGMEM =  {
  .name = text_ulozit,
  .x = 10,
  .y = 200,
  .size_x = 130,
  .size_y = 40,
  .font_size = 2,
  .args = 0,
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




const Menu1 HlavniMenu PROGMEM = {
  .name = term_title,
  .button_1 = {button_regulator, button_funkce, button_nastaveni},
  .button_2 = {button_term_state_off, button_term_state_max, button_term_state_min, button_term_state_prog, button_term_state_man},
  .function_1 = {f_show_temp, f_show_date, f_show_time},
  .switch_1 = {NULL},
  .dyn_button = {NULL},
  .symbol_button_1 = {NULL},
  .dyn_symbol_1 = {NULL},
  .dyn_select_box_1 = {NULL},
  .len_button_1 = 3,
  .len_button_2 = 5,
  .len_function_1 = 3,
  .len_switch_1 = 0,
  .len_dyn_button_1 = 0,
  .len_symbol_button_1 = 0,
  .len_dyn_symbol_1 = 0,
  .len_dyn_select_box_1 = 0,
  .idx = MENU_DEFAULT_SCREEN,
  .x = 0,
  .y = 0,
  .size_x = 480,
  .size_y = 320,
  .atributes = (1 << MENU_ATTRIBUTES_CLEAN_DISPLAY),
  .color_background = WHITE,
  .redraw_class = (1 << REDRAW_FORCE),
  .redraw_class_0 = menu_redraw_time05s,
  .redraw_class_1 = menu_redraw_change_term_mode,
  .redraw_class_2 = menu_redraw_update_temp,
  .preload_function = returnnullfceargs,
};

const Menu1 NastaveniMenu PROGMEM = {
  .name = nastaveni_text,
  .button_1 = {button_nastaveni_site, button_nastaveni_nrf, button_nastaveni_default, button_back, button_nastaveni_rtds, button_nastaveni_onewire, button_default_show_temp},
  .button_2 = {NULL},
  .function_1 = {f_show_date},
  .switch_1 = {NULL},
  .dyn_button = {NULL},
  .symbol_button_1 = {NULL},
  .dyn_symbol_1 = {NULL},
  .dyn_select_box_1 = {NULL},
  .len_button_1 = 7,
  .len_button_2 = 0,
  .len_function_1 = 1,
  .len_switch_1 = 0,
  .len_dyn_button_1 = 0,
  .len_symbol_button_1 = 0,
  .len_dyn_symbol_1 = 0,
  .len_dyn_select_box_1 = 0,
  .idx = MENU_NASTAVENI_SCREEN,
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

const Menu1 FunkceMenu PROGMEM = {
  .name = funkce_text,
  .button_1 = {button_back},
  .button_2 = {NULL},
  .function_1 = {f_show_date},
  .switch_1 = {switch_budik},
  .dyn_button = {NULL},
  .symbol_button_1 = {NULL},
  .dyn_symbol_1 = {NULL},
  .dyn_select_box_1 = {NULL},
  .len_button_1 = 1,
  .len_button_2 = 0,
  .len_function_1 = 1,
  .len_switch_1 = 1,
  .len_dyn_button_1 = 0,
  .len_symbol_button_1 = 0,
  .len_dyn_symbol_1 = 0,
  .len_dyn_select_box_1 = 0,
  .idx = MENU_FUNKCE_SCREEN,
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

const Menu1 OneWireMenu PROGMEM = {
  .name = nastaveni_onewire,
  .button_1 = {button_back},
  .button_2 = {NULL},
  .function_1 = {f_show_date, f_vertical_slider, f_show_rtds_decorate},
  .switch_1 = {NULL},
  .dyn_button = {onewire_associace_button},
  .symbol_button_1 = {slider_menu_plus, slider_menu_minus},
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
  .preload_function = returnnullfceargs,
};



const Menu1 List_RTDS_Menu PROGMEM = {
  .name = nastaveni_rtds,
  .button_1 = {button_back, button_rtds_novy},
  .button_2 = {NULL},
  .function_1 = {f_show_rtds_decorate, f_show_date, f_vertical_slider_rtds},
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
  .redraw_class = (1 << REDRAW_FORCE),
  .redraw_class_0 = returnnullfceargs,
  .redraw_class_1 = returnnullfceargs,
  .redraw_class_2 = returnnullfceargs,
  .preload_function = returnnullfceargs,
};


const Menu1 RTDS_Menu_Detail PROGMEM = {
  .name = nastaveni_rtds,
  .button_1 = {button_back, button_rtds_delete, button_rtds_name, button_rtds_subscribe},
  .button_2 = {NULL},
  .function_1 = {f_show_rtds_info_dynamics},
  .switch_1 = {NULL},
  .dyn_button = {NULL},
  .symbol_button_1 = {NULL},
  .dyn_symbol_1 = {NULL},
  .dyn_select_box_1 = {NULL},
  .len_button_1 = 4,
  .len_button_2 = 0,
  .len_function_1 = 1,
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
  .redraw_class = 0,
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
  .redraw_class = 0,
  .redraw_class_0 = returnnullfceargs,
  .redraw_class_1 = returnnullfceargs,
  .redraw_class_2 = returnnullfceargs,
  .preload_function = returnnullfceargs,
};


const Menu1 DialogKyeboardNumber PROGMEM = {
  .name = text_upozorneni,
  .button_1 = {dialog_set_variable_save, dialog_set_variable_cancel, button_delete_char},
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
  .redraw_class = 0,
  .redraw_class_0 = returnnullfceargs,
  .redraw_class_1 = returnnullfceargs,
  .redraw_class_2 = returnnullfceargs,
  .preload_function = returnnullfceargs,
};

const Menu1 DialogKyeboardAlfa PROGMEM = {
  .name = text_klavesnice,
  .button_1 = {dialog_set_variable_save, dialog_set_variable_cancel, button_delete_char, button_keyboard_upper, button_keyboard_lower, button_keyboard_special},
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
  .redraw_class = 0,
  .redraw_class_0 = returnnullfceargs,
  .redraw_class_1 = returnnullfceargs,
  .redraw_class_2 = returnnullfceargs,
  .preload_function = returnnullfceargs,
};


const Menu1 RegulatorMenu PROGMEM = {
  .name = regulator_text,
  .button_1 = {button_back, button_nastaveni_ring},
  .button_2 = {button_term_mode_heat, button_term_mode_cool},
  .function_1 = {f_show_date, f_show_default_ring, f_default_ring_set_temp},
  .switch_1 = {NULL},
  .dyn_button = {button_change_ring, button_change_term_mode},
  .symbol_button_1 = {dialog_set_default_ring_temp_plus, dialog_set_default_ring_temp_minus},
  .dyn_symbol_1 = {NULL},
  .dyn_select_box_1 = {NULL},
  .len_button_1 = 2,
  .len_button_2 = 2,
  .len_function_1 = 3,
  .len_switch_1 = 0,
  .len_dyn_button_1 = 2,
  .len_symbol_button_1 = 2,
  .len_dyn_symbol_1 = 0,
  .len_dyn_select_box_1 = 0,
  .idx = MENU_REGULATOR,
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
  .preload_function = preload_regulator_menu,
};



const Menu1 SelectMenuDefaultTemp PROGMEM = {
  .name = text_select_default_temp,
  .button_1 = {button_back },
  .button_2 = {NULL},
  .function_1 = {f_show_date, f_vertical_slider_default_temp, f_show_default_decorate},
  .switch_1 = {NULL},
  .dyn_button = {NULL},
  .symbol_button_1 = {slider_menu_plus_default_temp, slider_menu_minus_default_temp},
  .dyn_symbol_1 = {NULL},
  .dyn_select_box_1 = {select_default_temp},
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
  .preload_function = returnnullfceargs,
};



const Menu1 RingSetup PROGMEM = {
  .name = ring_text_setup,
  .button_1 = {button_back, button_nastaveni_ring_nazev, button_nastaveni_ring_input_sensor, button_nastaveni_ring_output, button_nastaveni_ring_pid},
  .button_2 = {NULL},
  .function_1 = {f_show_date, f_show_default_ring},
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
  .idx = MENU_NASTAVENI_RING_SCREEN,
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


const Menu1 DialogSelectTermMode PROGMEM = {
  .name = text_select_term_mode,
  .button_1 = {button_back },
  .button_2 = {NULL},
  .function_1 = {NULL},
  .switch_1 = {NULL},
  .dyn_button = {NULL},
  .symbol_button_1 = {NULL},
  .dyn_symbol_1 = {NULL},
  .dyn_select_box_1 = {NULL},
  .len_button_1 = 1,
  .len_button_2 = 0,
  .len_function_1 = 0,
  .len_switch_1 = 0,
  .len_dyn_button_1 = 0,
  .len_symbol_button_1 = 0,
  .len_dyn_symbol_1 = 0,
  .len_dyn_select_box_1 = 0,
  .idx = MENU_DIALOG_SELECT_TERM_MODE,
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
  .preload_function = returnnullfceargs,
};




const MenuAll Menu_All PROGMEM = {
  .len_menu1 = 6,
  .len_menu2 = 6,
  .len_menu3 = 3,
  .ListMenu1 = {HlavniMenu, NastaveniMenu, FunkceMenu, OneWireMenu, TDSMenu, DialogSetVariable},
  .ListMenu2 = {DialogKyeboardAlfa, DialogKyeboardNumber, DialogYESNO, RegulatorMenu, List_RTDS_Menu, RTDS_Menu_Detail},
  .ListMenu3 = {SelectMenuDefaultTemp, RingSetup, DialogSelectTermMode},
};














void nullfce(void) {};
void nullfceargs(uint16_t args1, uint16_t args2) {};
uint8_t returnnullfceargs(uint16_t args1, uint16_t args2)
{
  return 0;
}
uint8_t display_enable_show(uint8_t args1, uint8_t args2)
{
  return 1;
}










bool enable_redraw(uint8_t mask, uint8_t data)
{
  bool ret = false;
  for (uint8_t i = 0; i < 8; i++)
  {
    uint8_t v = (1 << i);
    if ( (mask & v) != 0 )
      if ( (data & v) != 0)
      {
        ret = true;
        break;
      }
  }
  return ret;
}





bool draw_menu(bool redraw)
{
  bool ret = false;
  char str1[128];
  char str2[32];
  char str3[16];
  Element_Button_1 *button_1;
  Element_Button_2 *button_2;
  Element_Function_1 *function_1;
  Element_Switch_1 *switch_1;
  Element_Dyn_Button_1 *dyn_button_1;
  Element_Dyn_Symbol_1 *dyn_symbol_1;
  Element_Symbol_1 *symbol_button_1;
  Element_Dyn_Select_1 *dyn_select_box_1;
  ret_fptr *rfnt;
  fptr_args *fntargs;
  fptr_coordinate_xy *fnt_coordinate_xy;
  ret_string_fptr *gss;
  Menu1 *current;
  uint16_t click_x, click_y;
  bool state;
  uint8_t loop_cnt, loop_i, loop_t, loop_r;
  uint8_t index_items;
  uint16_t new_x;
  uint16_t new_y;
  uint16_t global_x;
  uint16_t global_y;
  uint8_t rdr;
  uint8_t menu_args1;
  uint8_t redraw_class;
  uint8_t active = 0;

  current = MenuHistoryGetMenu(&menu_args1);

  click_x = 0;
  click_y = 0;

  my_touch.TP_Scan(0);
  if (my_touch.TP_Get_State()&TP_PRES_DOWN && redraw == false)
  {
    click_x = my_touch.x;
    click_y = my_touch.y;
  }

  global_x = pgm_read_word(&current->x);
  global_y = pgm_read_word(&current->y);


  rfnt = pgm_read_word(&current->redraw_class_0);
  rdr = (ret_fptr(rfnt))(pgm_read_byte(&current->idx), menu_args1, -1);
  if (rdr == 0)
    cbi(redraw_class, 0);
  else
    sbi(redraw_class, 0);
  ///
  rfnt = pgm_read_word(&current->redraw_class_1);
  rdr = (ret_fptr(rfnt))(pgm_read_byte(&current->idx), menu_args1, -1);
  if (rdr == 0)
    cbi(redraw_class, 1);
  else
    sbi(redraw_class, 1);
  ///
  rfnt = pgm_read_word(&current->redraw_class_2);
  rdr = (ret_fptr(rfnt))(pgm_read_byte(&current->idx), menu_args1, -1);
  if (rdr == 0)
    cbi(redraw_class, 2);
  else
    sbi(redraw_class, 2);
  ///
  if (redraw == false)
    cbi(redraw_class, 3);
  else
    sbi(redraw_class, 3);
  /////
  /////
  rdr = pgm_read_byte(&current->redraw_class);
  if (enable_redraw(rdr, redraw_class) == true)
  {
    strcpy_P(str1, pgm_read_word(&current->name));
    show_string(str1, pgm_read_word(&current->x) + 5, pgm_read_word(&current->y) + 5, 2, BLACK, WHITE, 0);
  }
  /// tlacitko typ 1 se prekresluje s celym menu
  /// tlacitko typ 1
  for (uint8_t idx = 0; idx < pgm_read_byte(&current->len_button_1); idx++)
  {
    button_1 = &current->button_1[idx];
    //rfnt = pgm_read_word(&button_1->enable_show);
    //active = (ret_fptr(rfnt))(pgm_read_byte(&button_1->args), menu_args1, -1);
    //if (active == 0) continue;
    rdr = pgm_read_byte(&button_1->redraw_class);
    if (enable_redraw(rdr, redraw_class) == true)
    {
      strcpy_P(str2, pgm_read_word(&button_1->name));
      str1[0] = 0;
      button_click_1(global_x + pgm_read_word(&button_1->x), global_y + pgm_read_word(&button_1->y), pgm_read_word(&button_1->size_x), pgm_read_word(&button_1->size_y), pgm_read_byte(&button_1->font_size), pgm_read_byte(&button_1->font_size), str2, str1, 0);
    }
    if (click_x > 0 && click_y > 0)
      if (button_click_touch(global_x + pgm_read_word(&button_1->x), global_y + pgm_read_word(&button_1->y), pgm_read_word(&button_1->size_x), pgm_read_word(&button_1->size_y), click_x, click_y) == true)
      {
        fntargs =  pgm_read_word(&button_1->onclick);
        ((fptr_args)fntargs)(pgm_read_byte(&button_1->args), menu_args1);
        ret = true;
      }
  }
  if (ret == true)  goto draw_menu_end;
  /// tlacitko typ 2
  for (uint8_t idx = 0; idx < pgm_read_byte(&current->len_button_2); idx++)
  {
    button_2 = &current->button_2[idx];
    rfnt = pgm_read_word(&button_2->enable_show);
    active = (ret_fptr(rfnt))(pgm_read_byte(&button_2->args), menu_args1, -1);
    if (active == 0) continue;
    rdr = pgm_read_byte(&button_2->redraw_class);
    if (enable_redraw(rdr, redraw_class) == true)
    {
      strcpy_P(str2, pgm_read_word(&button_2->name));
      rfnt = pgm_read_word(&button_2->get_status_fnt);
      state = ((ret_fptr)rfnt)(pgm_read_byte(&button_2->args), menu_args1, -1);
      str1[0] = 0;
      button_click_2( global_x + pgm_read_word(&button_2->x), \
                      global_y + pgm_read_word(&button_2->y), pgm_read_word(&button_2->size_x), \
                      pgm_read_word(&button_2->size_y), pgm_read_byte(&button_2->font_size), 0,  \
                      pgm_read_word(&button_2->color_active), \
                      pgm_read_word(&button_2->color_inactive), state,  str2, str1);
    }
    if (click_x > 0 && click_y > 0)
      if (button_click_touch(global_x + pgm_read_word(&button_2->x), global_y + pgm_read_word(&button_2->y), pgm_read_word(&button_2->size_x), pgm_read_word(&button_2->size_y), click_x, click_y) == true)
      {
        fntargs =  pgm_read_word(&button_2->onclick);
        ((fptr_args)fntargs)(pgm_read_byte(&button_2->args), menu_args1);
        ret = true;
      }
  }
  if (ret == true)  goto draw_menu_end;
  /// menu programy
  for (uint8_t idx = 0; idx < pgm_read_byte(&current->len_function_1); idx++)
  {
    function_1 = &current->function_1[idx];
    rfnt = pgm_read_word(&function_1->enable_show);
    active = (ret_fptr(rfnt))(pgm_read_byte(&function_1->args), menu_args1, -1);
    if (active == 0) continue;
    rdr = pgm_read_byte(&function_1->redraw_class);
    if (enable_redraw(rdr, redraw_class) == true)
    {
      fnt_coordinate_xy =  pgm_read_dword(&function_1->fnt_coordinate_xy);
      ((fptr_coordinate_xy)fnt_coordinate_xy)(global_x + pgm_read_word(&function_1->x), global_y + pgm_read_word(&function_1->y), pgm_read_word(&function_1->size_x), pgm_read_word(&function_1->size_y), pgm_read_word(&function_1->args), menu_args1);
    }
    if (click_x > 0 && click_y > 0)
      if (button_click_touch(global_x + pgm_read_word(&function_1->x), global_y + pgm_read_word(&function_1->y), pgm_read_word(&function_1->size_x), pgm_read_word(&function_1->size_y), click_x, click_y) == true)
      {
        //fntargs =  pgm_read_word(&function_1->onclick);
        //((fptr_args)fntargs)(pgm_read_byte(&function_1->args), menu_args1);
        //ret = true;
        printf("mrdka funkce\n");
      }
  }
  if (ret == true)  goto draw_menu_end;
  /////
  /// menu switch_1
  for (uint8_t idx = 0; idx < pgm_read_byte(&current->len_switch_1); idx++)
  {
    switch_1 = &current->switch_1[idx];
    rdr = pgm_read_byte(&switch_1->redraw_class);
    if (enable_redraw(rdr, redraw_class) == true)
    {
      strcpy_P(str2, pgm_read_word(&switch_1->name));
      rfnt = pgm_read_word(&switch_1->get_status_fnt);
      gss = pgm_read_word(&switch_1->get_status_string);
      state = ((ret_fptr)rfnt)(pgm_read_byte(&switch_1->args), menu_args1, -1);
      str3[0] = 0;
      ((ret_string_fptr)gss)(pgm_read_byte(&switch_1->args), menu_args1, str1, str3);
      button_switch_1(global_x + pgm_read_word(&switch_1->x), \
                      global_y + pgm_read_word(&switch_1->y), \
                      pgm_read_word(&switch_1->size_x), \
                      pgm_read_word(&switch_1->size_y), \
                      pgm_read_word(&switch_1->font_size), state, str2, str1);

    }
    if (click_x > 0 && click_y > 0)
      if (button_click_touch(global_x + pgm_read_word(&switch_1->x), global_y + pgm_read_word(&switch_1->y), pgm_read_word(&switch_1->size_x), pgm_read_word(&switch_1->size_y), click_x, click_y) == true)
      {
        fntargs =  pgm_read_word(&switch_1->onclick);
        ((fptr_args)fntargs)(pgm_read_byte(&switch_1->args), menu_args1);
        ret = true;
      }
  }
  if (ret == true)  goto draw_menu_end;
  ////
  for (uint8_t idx = 0; idx < pgm_read_byte(&current->len_symbol_button_1); idx++)
  {
    symbol_button_1 = &current->symbol_button_1[idx];
    rfnt = pgm_read_word(&symbol_button_1->enable_show);
    active = (ret_fptr(rfnt))(pgm_read_byte(&symbol_button_1->args), menu_args1, -1);
    if (active == 0) continue;
    rdr = pgm_read_byte(&symbol_button_1->redraw_class);
    if (enable_redraw(rdr, redraw_class) == true)
    {
      button_click_3(global_x + pgm_read_word(&symbol_button_1->x), global_y + pgm_read_word(&symbol_button_1->y), \
                     pgm_read_word(&symbol_button_1->size_x), pgm_read_word(&symbol_button_1->size_y), \
                     pgm_read_word(&symbol_button_1->znak_size), pgm_read_word(&symbol_button_1->znak));
    }
    if (click_x > 0 && click_y > 0)
      if (button_click_touch(global_x + pgm_read_word(&symbol_button_1->x), global_y + pgm_read_word(&symbol_button_1->y), pgm_read_word(&symbol_button_1->size_x), pgm_read_word(&symbol_button_1->size_y), click_x, click_y) == true)
      {
        fntargs =  pgm_read_word(&symbol_button_1->onclick);
        ((fptr_args)fntargs)(pgm_read_byte(&symbol_button_1->args), menu_args1);
        ret = true;
      }
  }
  if (ret == true)  goto draw_menu_end;
  ////
  for (uint8_t idx = 0; idx < pgm_read_byte(&current->len_dyn_symbol_1); idx++)
  {
    dyn_symbol_1 = &current->dyn_symbol_1[idx];
    rdr = pgm_read_byte(&dyn_symbol_1->redraw_class);
    if (enable_redraw(rdr, redraw_class) == true)
    {
      loop_i = 0;
      loop_t = 0;
      loop_r = 0;
      rfnt =  pgm_read_word(&dyn_symbol_1->function_for_max_items);
      loop_cnt = ((ret_fptr)rfnt)(pgm_read_word(&dyn_symbol_1->args), menu_args1, loop_i);
      gss = pgm_read_word(&dyn_symbol_1->get_status_string);
      while (loop_i < loop_cnt)
      {
        new_x = global_x + pgm_read_word(&dyn_symbol_1->first_x);
        new_y = global_y + pgm_read_word(&dyn_symbol_1->first_y);
        if (pgm_read_byte(&dyn_symbol_1->direction) == VERTICAL)
        {
          new_y = global_y + pgm_read_word(&dyn_symbol_1->first_y) + (pgm_read_byte(&dyn_symbol_1->step_y) * loop_t);
          loop_t++;
        }
        if (pgm_read_byte(&dyn_symbol_1->direction) == HORIZONTAL)
        {
          new_x = global_x + pgm_read_word(&dyn_symbol_1->first_x) + (pgm_read_byte(&dyn_symbol_1->step_x) * loop_t);
          loop_t++;
        }
        if (pgm_read_byte(&dyn_symbol_1->direction) == HORIZONTAL_NEW_LINE)
        {
          new_x = global_x + pgm_read_word(&dyn_symbol_1->first_x) + (pgm_read_byte(&dyn_symbol_1->step_x) * loop_t);
          new_y = global_y + pgm_read_word(&dyn_symbol_1->first_y) + (pgm_read_byte(&dyn_symbol_1->step_y) * loop_r);
          loop_t++;
          if (loop_t > (pgm_read_byte(&dyn_symbol_1->max_items_count) - 1))
          {
            loop_t = 0;
            loop_r++;
          }
        }
        ((ret_string_fptr)gss)(loop_i, menu_args1, str1, str2);
        char z = str1[0];
        button_click_3(new_x, new_y, pgm_read_word(&dyn_symbol_1->size_x), pgm_read_word(&dyn_symbol_1->size_y), pgm_read_byte(&dyn_symbol_1->font_size), z);
        loop_i++;
      }
    }
    if (click_x > 0 && click_y > 0)
    {
      loop_i = 0;
      loop_t = 0;
      loop_r = 0;
      rfnt =  pgm_read_word(&dyn_symbol_1->function_for_max_items);
      loop_cnt = ((ret_fptr)rfnt)(pgm_read_word(&dyn_symbol_1->args), menu_args1, loop_i);
      while (loop_i < loop_cnt)
      {
        new_x = global_x + pgm_read_word(&dyn_symbol_1->first_x);
        new_y = global_y + pgm_read_word(&dyn_symbol_1->first_y);
        if (pgm_read_byte(&dyn_symbol_1->direction) == VERTICAL)
        {
          new_y = global_y + pgm_read_word(&dyn_symbol_1->first_y) + (pgm_read_byte(&dyn_symbol_1->step_y) * loop_t);
          loop_t++;
        }
        if (pgm_read_byte(&dyn_symbol_1->direction) == HORIZONTAL)
        {
          new_x = global_x + pgm_read_word(&dyn_symbol_1->first_x) + (pgm_read_byte(&dyn_symbol_1->step_x) * loop_t);
          loop_t++;
        }
        if (pgm_read_byte(&dyn_symbol_1->direction) == HORIZONTAL_NEW_LINE)
        {
          new_x = global_x + pgm_read_word(&dyn_symbol_1->first_x) + (pgm_read_byte(&dyn_symbol_1->step_x) * loop_t);
          new_y = global_y + pgm_read_word(&dyn_symbol_1->first_y) + (pgm_read_byte(&dyn_symbol_1->step_y) * loop_r);
          loop_t++;
          if (loop_t > (pgm_read_byte(&dyn_symbol_1->max_items_count) - 1))
          {
            loop_t = 0;
            loop_r++;
          }
        }
        if (button_click_touch(new_x, new_y, pgm_read_word(&dyn_symbol_1->size_x), pgm_read_word(&dyn_symbol_1->size_y), click_x, click_y) == true)
        {
          fntargs =  pgm_read_word(&dyn_symbol_1->dyn_symbol_onclick);
          ((fptr_args)fntargs)(pgm_read_byte(&dyn_symbol_1->args), loop_i);
          ret = true;
          break;
        }
        loop_i++;
      }
    }
  }
  if (ret == true)  goto draw_menu_end;
  ////
  for (uint8_t idx = 0; idx < pgm_read_byte(&current->len_dyn_button_1); idx++)
  {
    dyn_button_1 = &current->dyn_button[idx];
    rdr = pgm_read_byte(&dyn_button_1->redraw_class);
    if (enable_redraw(rdr, redraw_class) == true)
    {
      loop_i = 0;
      loop_t = 0;
      rfnt =  pgm_read_word(&dyn_button_1->function_for_max_items);
      loop_cnt = ((ret_fptr)rfnt)(pgm_read_word(&dyn_button_1->args), menu_args1, loop_i);
      gss = pgm_read_word(&dyn_button_1->get_status_string);
      process_display_element_slider(&loop_i, &loop_cnt, pgm_read_byte(&dyn_button_1->slider_args), pgm_read_byte(&dyn_button_1->max_items_count)*pgm_read_byte(&dyn_button_1->max_row_count));
      while (loop_i < loop_cnt)
      {
        new_x = global_x + pgm_read_word(&dyn_button_1->first_x);
        new_y = global_y + pgm_read_word(&dyn_button_1->first_y);
        if (pgm_read_byte(&dyn_button_1->direction) == VERTICAL)
          new_y = global_y + pgm_read_word(&dyn_button_1->first_y) + (pgm_read_byte(&dyn_button_1->step_y) * loop_t);
        if (pgm_read_byte(&dyn_button_1->direction) == HORIZONTAL)
          new_x = global_x + pgm_read_word(&dyn_button_1->first_x) + (pgm_read_byte(&dyn_button_1->step_x) * loop_t);
        ((ret_string_fptr)gss)(loop_i, menu_args1, str1, str2);
        button_click_1(new_x, new_y , pgm_read_word(&dyn_button_1->size_x), pgm_read_word(&dyn_button_1->size_y), pgm_read_byte(&dyn_button_1->font_size), pgm_read_byte(&dyn_button_1->font_size), str1, str2, 1);
        loop_t++;
        loop_i++;
      }
    }
    if (click_x > 0 && click_y > 0)
    {
      loop_i = 0;
      loop_t = 0;
      rfnt =  pgm_read_word(&dyn_button_1->function_for_max_items);
      loop_cnt = ((ret_fptr)rfnt)(pgm_read_word(&dyn_button_1->args), menu_args1, loop_i);
      process_display_element_slider(&loop_i, &loop_cnt, pgm_read_byte(&dyn_button_1->slider_args), pgm_read_byte(&dyn_button_1->max_items_count)*pgm_read_byte(&dyn_button_1->max_row_count));
      while (loop_i < loop_cnt)
      {
        new_x = global_x + pgm_read_word(&dyn_button_1->first_x);
        new_y = global_y + pgm_read_word(&dyn_button_1->first_y);
        if (pgm_read_byte(&dyn_button_1->direction) == VERTICAL)
          new_y = global_y + pgm_read_word(&dyn_button_1->first_y) + (pgm_read_byte(&dyn_button_1->step_y) * loop_t);
        if (pgm_read_byte(&dyn_button_1->direction) == HORIZONTAL)
          new_x = global_x + pgm_read_word(&dyn_button_1->first_x) + (pgm_read_byte(&dyn_button_1->step_x) * loop_t);
        if (button_click_touch(new_x, new_y, pgm_read_word(&dyn_button_1->size_x), pgm_read_word(&dyn_button_1->size_y), click_x, click_y) == true)
        {
          fntargs =  pgm_read_word(&dyn_button_1->dyn_button_onclick);
          ((fptr_args)fntargs)(pgm_read_byte(&dyn_button_1->args), loop_i);
          ret = true;
          break;
        }
        loop_t++;
        loop_i++;
      }
    }
  }
  if (ret == true)  goto draw_menu_end;
  for (uint8_t idx = 0; idx < pgm_read_byte(&current->len_dyn_select_box_1); idx++)
  {
    dyn_select_box_1 = &current->dyn_select_box_1[idx];
    rdr = pgm_read_byte(&dyn_select_box_1->redraw_class);
    if (enable_redraw(rdr, redraw_class) == true)
    {
      loop_i = 0;
      loop_t = 0;
      loop_r = 0;
      rfnt =  pgm_read_word(&dyn_select_box_1->function_for_max_items);
      loop_cnt = ((ret_fptr)rfnt)(pgm_read_word(&dyn_select_box_1->args), menu_args1, loop_i);
      gss = pgm_read_word(&dyn_select_box_1->get_status_string);
      process_display_element_slider(&loop_i, &loop_cnt, pgm_read_byte(&dyn_select_box_1->slider_args), pgm_read_byte(&dyn_select_box_1->max_items_count)*pgm_read_byte(&dyn_select_box_1->max_row_count));
      while (loop_i < loop_cnt)
      {
        new_x = global_x + pgm_read_word(&dyn_select_box_1->first_x);
        new_y = global_y + pgm_read_word(&dyn_select_box_1->first_y);
        if (pgm_read_byte(&dyn_select_box_1->direction) == VERTICAL)
        {
          new_y = global_y + pgm_read_word(&dyn_select_box_1->first_y) + (pgm_read_byte(&dyn_select_box_1->step_y) * loop_t);
          loop_t++;
        }
        if (pgm_read_byte(&dyn_select_box_1->direction) == HORIZONTAL)
        {
          new_x = global_x + pgm_read_word(&dyn_select_box_1->first_x) + (pgm_read_byte(&dyn_select_box_1->step_x) * loop_t);
          loop_t++;
        }
        if (pgm_read_byte(&dyn_select_box_1->direction) == HORIZONTAL_NEW_LINE)
        {
          new_x = global_x + pgm_read_word(&dyn_select_box_1->first_x) + (pgm_read_byte(&dyn_select_box_1->step_x) * loop_t);
          new_y = global_y + pgm_read_word(&dyn_select_box_1->first_y) + (pgm_read_byte(&dyn_select_box_1->step_y) * loop_r);
          loop_t++;
          if (loop_t > (pgm_read_byte(&dyn_select_box_1->max_items_count) - 1))
          {
            loop_t = 0;
            loop_r++;
          }
        }
        ((ret_string_fptr)gss)(loop_i, menu_args1, str1, str2);
        rfnt = pgm_read_word(&dyn_select_box_1->get_status_fnt);
        state = ((ret_fptr)rfnt)(pgm_read_byte(&dyn_select_box_1->args), menu_args1, loop_i);
        button_click_2(new_x, new_y, pgm_read_word(&dyn_select_box_1->size_x), \
                       pgm_read_word(&dyn_select_box_1->size_y), pgm_read_byte(&dyn_select_box_1->font_size_1), pgm_read_byte(&dyn_select_box_1->font_size_2), \
                       pgm_read_word(&dyn_select_box_1->color_active), \
                       pgm_read_word(&dyn_select_box_1->color_inactive), state,  str1, str2);
        loop_i++;
      }
    }
    if (click_x > 0 && click_y > 0)
    {
      loop_i = 0;
      loop_t = 0;
      loop_r = 0;
      rfnt =  pgm_read_word(&dyn_select_box_1->function_for_max_items);
      loop_cnt = ((ret_fptr)rfnt)(pgm_read_word(&dyn_select_box_1->args), menu_args1, loop_i);
      process_display_element_slider(&loop_i, &loop_cnt, pgm_read_byte(&dyn_select_box_1->slider_args), pgm_read_byte(&dyn_select_box_1->max_items_count)*pgm_read_byte(&dyn_select_box_1->max_row_count));
      while (loop_i < loop_cnt)
      {
        new_x = global_x + pgm_read_word(&dyn_select_box_1->first_x);
        new_y = global_y + pgm_read_word(&dyn_select_box_1->first_y);
        if (pgm_read_byte(&dyn_symbol_1->direction) == VERTICAL)
        {
          new_y = global_y + pgm_read_word(&dyn_select_box_1->first_y) + (pgm_read_byte(&dyn_select_box_1->step_y) * loop_t);
          loop_t++;
        }
        if (pgm_read_byte(&dyn_select_box_1->direction) == HORIZONTAL)
        {
          new_x = global_x + pgm_read_word(&dyn_select_box_1->first_x) + (pgm_read_byte(&dyn_select_box_1->step_x) * loop_t);
          loop_t++;
        }
        if (pgm_read_byte(&dyn_select_box_1->direction) == HORIZONTAL_NEW_LINE)
        {
          new_x = global_x + pgm_read_word(&dyn_select_box_1->first_x) + (pgm_read_byte(&dyn_select_box_1->step_x) * loop_t);
          new_y = global_y + pgm_read_word(&dyn_select_box_1->first_y) + (pgm_read_byte(&dyn_select_box_1->step_y) * loop_r);
          loop_t++;
          if (loop_t > (pgm_read_byte(&dyn_select_box_1->max_items_count) - 1) )
          {
            loop_t = 0;
            loop_r++;
          }
        }
        if (button_click_touch(new_x, new_y, pgm_read_word(&dyn_select_box_1->size_x), pgm_read_word(&dyn_select_box_1->size_y), click_x, click_y) == true)
        {
          fntargs =  pgm_read_word(&dyn_select_box_1->dyn_symbol_onclick);
          ((fptr_args)fntargs)(pgm_read_byte(&dyn_select_box_1->args), loop_i);
          ret = true;
          break;
        }
        loop_i++;
      }
    }
  }


draw_menu_end:
  return ret;
}






/////////////////////////////////////////////////////////////////////////////////////////////
void MenuPrepareStyle(void)
{
  uint8_t attr;
  uint16_t x;
  uint16_t y;
  uint16_t size_x;
  uint16_t size_y;
  uint16_t color_background;
  uint8_t args1;
  Menu1 *current;
  ret_fptr *rfnt;

  current = MenuHistoryGetMenu(&args1);
  attr = pgm_read_byte(&current->atributes);
  x = pgm_read_word(&current->x);
  y = pgm_read_word(&current->y);
  size_x = pgm_read_word(&current->size_x);
  size_y = pgm_read_word(&current->size_y);
  color_background = pgm_read_word(&current->color_background);

  if ((attr & (1 << MENU_ATTRIBUTES_CLEAN_DISPLAY)) != 0)
  {
    DisplayClean(color_background);
  }

  if ((attr & (1 << MENU_ATTRIBUTES_FILL_COLOR_RECTANGLE)) != 0)
  {
    my_lcd.Set_Draw_color(color_background);
    my_lcd.Fill_Rectangle(x, y, x + size_x, y + size_y);
  }

  if ((attr & (1 << MENU_ATTRIBUTES_DECORATE_MENU)) != 0)
  {
    my_lcd.Set_Draw_color(BLACK);
    for (uint8_t i = 0; i < 4; i++)
    {
      my_lcd.Draw_Rectangle(x + i, y + i, x + size_x - i, y + size_y - i);
      my_lcd.Draw_Fast_HLine(x, y + 23 + i, size_x - i);
    }
  }
  rfnt =  pgm_read_word(&current->preload_function);
  (ret_fptr(rfnt))(pgm_read_byte(&current->idx), 0, 0);

}






/////////////////////////////////////////////////////////////////////////////////////
void MenuHistoryPrevMenu(void)
{

  if (MenuHistoryIndex > 0)
  {
    MenuHistoryIndex--;
    MenuPrepareStyle();

  }
}
///
void MenuHistoryNextMenu(uint8_t id, uint8_t args1)
{
  if (MenuHistoryIndex < MENU_MAX_HISTORY)
  {
    MenuHistoryIndex++;
    MenuHistory[MenuHistoryIndex] = id;
    Global_menu_args1[MenuHistoryIndex] = args1;
    MenuPrepareStyle();
  }
}
///
void MenuHistoryInit(void)
{
  MenuHistory[MenuHistoryIndex] = MENU_DEFAULT_SCREEN;
}
///
Menu1 *MenuHistoryGetMenu(uint8_t *args1)
{
  Menu1 *menus;
  for (uint8_t idx = 0; idx < pgm_read_byte(&Menu_All.len_menu1); idx++)
  {
    menus = &Menu_All.ListMenu1[idx];
    if (pgm_read_byte(&menus->idx) == MenuHistory[MenuHistoryIndex])
    {
      *args1 = Global_menu_args1[MenuHistoryIndex];
      return menus;
    }
  }
  for (uint8_t idx = 0; idx < pgm_read_byte(&Menu_All.len_menu2); idx++)
  {
    menus = &Menu_All.ListMenu2[idx];
    if (pgm_read_byte(&menus->idx) == MenuHistory[MenuHistoryIndex])
    {
      *args1 = Global_menu_args1[MenuHistoryIndex];
      return menus;
    }
  }

  for (uint8_t idx = 0; idx < pgm_read_byte(&Menu_All.len_menu3); idx++)
  {
    menus = &Menu_All.ListMenu3[idx];
    if (pgm_read_byte(&menus->idx) == MenuHistory[MenuHistoryIndex])
    {
      *args1 = Global_menu_args1[MenuHistoryIndex];
      return menus;
    }
  }


}
///////////////////////////////////////////////////////////////////////////////////
void DisplayClean(uint16_t color)
{
  my_lcd.Fill_Screen(color);
}
//////////////////////////////////////////////////////////////////////////////////

/*
  args1 -- menu_dialog_set_variable_now, aktualni hodnota
  args2 -- menu_dialog_set_variable_args, id ringu
*/
void helper_thermostat_set_mezni(int args1, uint8_t args2)
{
  thermostat_ring_set_mezni(args2, args1);
}




uint8_t preload_regulator_menu(uint8_t args1, uint8_t args2, uint8_t args3)
{
  display_function_set_variable(thermostat_ring_get_mezni(default_ring), 160, 320, 5, default_ring, 10,  H_TRUE);
  dialog_save_variable_function = &helper_thermostat_set_mezni;
}

/////////////////////////////////////////////////////////////////////////////////

uint8_t display_function_get_variable_args(void)
{
  return menu_dialog_set_variable_args;
}

uint16_t display_function_get_variable(void)
{
  return menu_dialog_set_variable_now;
}

void display_function_set_variable_minus(void)
{
  if (menu_dialog_set_variable_now - menu_dialog_set_variable_step >= menu_dialog_set_variable_min)
  {
    menu_dialog_set_variable_now = menu_dialog_set_variable_now - menu_dialog_set_variable_step;
    if (menu_dialog_set_variable_update_now == H_TRUE)
      dialog_save_variable_function(menu_dialog_set_variable_now, menu_dialog_set_variable_args);
  }
}
void display_function_set_variable_plus(void)
{
  if ((menu_dialog_set_variable_now + menu_dialog_set_variable_step) <= menu_dialog_set_variable_max)
  {
    menu_dialog_set_variable_now = menu_dialog_set_variable_now + menu_dialog_set_variable_step;
    if (menu_dialog_set_variable_update_now == H_TRUE)
      dialog_save_variable_function(menu_dialog_set_variable_now, menu_dialog_set_variable_args);
  }
}
void display_function_set_variable(uint16_t now, uint16_t min, uint16_t max, uint8_t step, uint8_t args, uint16_t divider, uint8_t update_now)
{
  menu_dialog_set_variable_now = now;
  menu_dialog_set_variable_min = min;
  menu_dialog_set_variable_max = max;
  menu_dialog_set_variable_step = step;
  menu_dialog_set_variable_args = args;
  menu_dialog_set_divider = divider;
  dialog_save_variable_function = NULL;
  menu_dialog_set_variable_update_now = update_now;
}
/////
void display_element_set_string(char *str, uint8_t max_length, uint8_t idx)
{
  strcpy(dialog_set_string, str);
  dialog_set_string_max_length = max_length;
  dialog_set_string_args = idx;
}

uint8_t display_element_get_string_args(void)
{
  return dialog_set_string_args;
}

void display_element_get_string(char *str)
{
  strcpy(str, dialog_set_string);
}

void display_element_set_string_add_char(char znak)
{
  uint8_t len = strlen(dialog_set_string);
  if (len  < dialog_set_string_max_length - 1)
  {
    dialog_set_string[len] = znak;
    dialog_set_string[len + 1] = 0;
  }
}

void display_element_set_string_del_char(uint8_t args1, uint8_t args2)
{
  uint8_t len = strlen(dialog_set_string);
  if (len > 0)
    dialog_set_string[len - 1] = 0;
}

/////
void menu_tds_save_offset(uint8_t args1, uint8_t args2)
{
  tds_set_offset(display_function_get_variable_args(), display_function_get_variable());
}

void menu_tds_save_period(uint8_t args1, uint8_t args2)
{
  tds_set_period(display_function_get_variable_args(), display_function_get_variable());
}
/////
void menu_tds_save_name(uint8_t args1, uint8_t args2)
{
  char name[10];
  display_element_get_string(name);
  if (strlen(name) > 0)
    tds_set_name(display_element_get_string_args(), name);
}


//////////////////////////////////////////////////////////////////////////////////
/*
   funkce ktere se volaji z menu tlacitek
   args1 ... je parametr z definice tlacitka
   args2 ... je dynamicky parametr nastaveni menu

*/
/// funkce pro nastaveni offsetu tds
void display_menu_tds_set_offset(uint8_t args1, uint8_t args2)
{
  MenuHistoryNextMenu(MENU_DIALOG_SET_VARIABLE, 0);
  display_function_set_variable(tds_get_offset(args2), -10000, 10000, 100, args2, 1000, H_FALSE);
  dialog_save_variable_function = &menu_tds_save_offset;
}

/// funkce pro nastaveni merici periody
void display_menu_tds_set_period(uint8_t args1, uint8_t args2)
{
  MenuHistoryNextMenu(MENU_DIALOG_SET_VARIABLE, 0);
  display_function_set_variable(tds_get_period(args2), 1, 255, 1, args2, 0, H_FALSE);
  dialog_save_variable_function = &menu_tds_save_period;
}

/// funkce pro nastaveni nazvu cidla
void display_menu_tds_set_name(uint8_t args1, uint8_t args2)
{
  char name[10];
  tds_get_name(args2, name);
  MenuHistoryNextMenu(MENU_DIALOG_KEYBOARD_ALFA, 0);
  display_element_set_string(name, 8, args2);
  dialog_save_variable_function = &menu_tds_save_name;
}
/// funkce pro nastaveni nazvu rtds
void display_menu_rtds_update_name(uint8_t args1, uint8_t args2)
{
  char name[RTDS_DEVICE_STRING_LEN];
  uint8_t active;
  remote_tds_get_complete(args2, &active, name);
  MenuHistoryNextMenu(MENU_DIALOG_KEYBOARD_ALFA, 0);
  display_element_set_string(name, RTDS_DEVICE_STRING_LEN, args2);
  dialog_save_variable_function = &menu_rtds_update_name;
}
////////////////////////////////////////////////////////

/*
   funkce pro nastaveni budiku
*/
uint8_t get_function_budik_enabled(uint8_t args)
{
  return 1;
}
void get_function_budik_text_state(uint8_t args1, uint8_t args2, char *text)
{
  strcpy(text, "Zapnuto 06:45");
}



////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
/// funkce, ktera vraci pocet polozek dynamickeho menu
uint8_t get_function_keyboard_number_max_keys(uint8_t args1, uint8_t args2, uint8_t args3)
{
  return 11;
}
/*
   args1 ... argument ze statickeho menu nastaveni
   args2 ... index prvku
*/
void click_keyboard_number(uint8_t args1, uint8_t args2)
{
  char znak;
  znak = '0' + args2;
  if (args1 == 10) znak = '.';
  display_element_set_string_add_char(znak);
}
///////////////////////////
/*
   args1 - index prvku
   args2
*/
void get_function_keyboard_number_char(uint8_t args1, uint8_t args2, char *line1, char *line2)
{
  line1[0] = '0' + args1;
  if (args1 == 10) line1[0] = '.';
  line2[0] = 0;
}

/////
void get_function_keyboard_alfa_char(uint8_t args1, uint8_t args2, char *line1, char *line2)
{
  char znak;
  if (dialog_set_string_keyboard_type == KEYBOARD_TYPE_UPPER)
    znak = pgm_read_byte(&keyboad_number_char_upper[args1]);
  if (dialog_set_string_keyboard_type == KEYBOARD_TYPE_LOWER)
    znak = pgm_read_byte(&keyboad_number_char_lower[args1]);
  if (dialog_set_string_keyboard_type == KEYBOARD_TYPE_SPECIAL)
    znak = pgm_read_byte(&keyboad_number_char_special[args1]);
  line1[0] = znak;
  line2[0] = 0;
}
/*
   args1 ... argument ze statickeho menu nastaveni
   args2 ... index prvku
*/
void click_keyboard_alfa(uint8_t args1, uint8_t args2)
{
  char znak;
  if (dialog_set_string_keyboard_type == KEYBOARD_TYPE_UPPER)
    znak = pgm_read_byte(&keyboad_number_char_upper[args2]);
  if (dialog_set_string_keyboard_type == KEYBOARD_TYPE_LOWER)
    znak = pgm_read_byte(&keyboad_number_char_lower[args2]);
  if (dialog_set_string_keyboard_type == KEYBOARD_TYPE_SPECIAL)
    znak = pgm_read_byte(&keyboad_number_char_special[args2]);
  ///
  display_element_set_string_add_char(znak);
}
/// vraci pocet prvku pro zobrazeni
uint8_t get_function_keyboard_alfa_max_keys(uint8_t idx)
{
  if (dialog_set_string_keyboard_type == KEYBOARD_TYPE_UPPER)
    return KEYBOARD_SIZE_CHAR_UPPER;
  if (dialog_set_string_keyboard_type == KEYBOARD_TYPE_LOWER)
    return KEYBOARD_SIZE_CHAR_LOWER;
  if (dialog_set_string_keyboard_type == KEYBOARD_TYPE_SPECIAL)
    return KEYBOARD_SIZE_CHAR_SPECIAL;
}

void click_keyboard_type(uint8_t args1, uint8_t args2)
{
  dialog_set_string_keyboard_type = args1;
}

////////////////////////////////////////////////////////////
/// funkce pro obsluhu dialogu
/*
   args1 - jsou data z definice funkce
   args2 - jsou globalni data z definice menu
*/
void click_dialog_button_yes(uint8_t args1, uint8_t args2)
{
  dialog_yes_function(dialog_yes_args1, 0);
  MenuHistoryPrevMenu();
}

void dialog_set_variable_button_click(uint8_t args1, uint8_t args2)
{
  dialog_save_variable_function(args1, args2);
  MenuHistoryPrevMenu();
}







uint8_t button_redraw(uint8_t args1, uint8_t args2)
{
  return 0;
}


uint8_t menu_redraw_time05s(uint8_t args1, uint8_t args2)
{
  if (menu_redraw05s == 1)
  {
    menu_redraw05s = 0;
    return 1;
  }
  return 0;
}

uint8_t menu_redraw_change_term_mode(uint8_t args1, uint8_t args2)
{
  if (change_term_mode == 1)
  {
    change_term_mode = 0;
    return 1;
  }
  return 0;
}

uint8_t menu_redraw_update_temp(uint8_t args1, uint8_t args2)
{
  if (menu_redraw10s == 1)
  {
    menu_redraw10s = 0;
    return 1;
  }
  return 0;
}


uint8_t get_function_return_args_1(uint8_t args1, uint8_t args2)
{
  return args1;
}
/*************************************************************************************************************************/
/// resetFunc - self restart, skok na PC = 0
void(* resetFunc) (void) = 0;
/*************************************************************************************************************************/
/// show_string - obalovaci funkce pro psani textu na display
/*
 *  *str - ukazatel na pole s texte,
    x,y - souradnice
    csize - velikost pisma
    fc - foreground color
    bc - backgroud color
    mode - ?
*/
void show_string(char *str, int16_t x, int16_t y, uint8_t csize, uint16_t fc, uint16_t bc, boolean mode)
{
  my_lcd.Set_Text_Mode(mode);
  my_lcd.Set_Text_Size(csize);
  my_lcd.Set_Text_colour(fc);
  my_lcd.Set_Text_Back_colour(bc);
  my_lcd.Print_String(str, x, y);
}

uint16_t show_string_size_width(uint8_t str_len, uint8_t text_size)
{
  return str_len * 6 * text_size;
}

uint8_t show_string_size_hight(uint8_t text_size)
{
  return 6 * text_size;
}
/*************************************************************************************************************************/
/// funkce, ktera vraci jestli byla udalost v danem okne
boolean is_pressed(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t px, int16_t py)
{
  if ((px > x1 && px < x2) && (py > y1 && py < y2))
  {
    return true;
  }
  else
  {
    return false;
  }
}
/*************************************************************************************************************************/
/// ip2CharArray - jednoducha funkce prevodu objectu IPAddress na char pole
/*
   ip - object typu IP address
 * *buf - ukazatel na pole s vysledkem
*/
void ip2CharArray(IPAddress ip, char* buf) {
  sprintf(buf, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
}
/*************************************************************************************************************************/

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///   SEKCE NASTAVENI REMOTE TDS CIDEL ///
/*
  idx - index rtds
  active - ukazatel na promenou active
  name - ukazatel na nazev topicu
*/
/// ziska nazev topicu
void remote_tds_get_complete(uint8_t idx, uint8_t *active, char *name)
{
  char t;
  if (idx < MAX_RTDS)
  {
    for (uint8_t i = 0; i < RTDS_DEVICE_STRING_LEN; i++)
    {
      t = EEPROM.read(remote_tds_name0 + (RTDS_DEVICE_TOTAL_LEN * idx) + i);
      name[i] = t;
      name[i + 1] = 0;
      if (t == 0) break;
    }
    *active = EEPROM.read(remote_tds_name0 + (RTDS_DEVICE_TOTAL_LEN * idx) + RTDS_DEVICE_ACTIVE_BYTE_POS);
  }
  else
  {
    *active = 255;
  }
}
///
void remote_tds_set_complete(uint8_t idx, uint8_t active, char *name)
{
  remote_tds_set_name(idx, name);
  remote_tds_set_active(idx, active);
}
/// nastavi topic
void remote_tds_set_name(uint8_t idx,  char *name)
{
  char t;
  if (idx < MAX_RTDS)
    for (uint8_t i = 0; i < (RTDS_DEVICE_STRING_LEN); i++)
    {
      t = name[i];
      EEPROM.write(remote_tds_name0 + (RTDS_DEVICE_TOTAL_LEN * idx) + i, t);
      if (t == 0) break;
    }
}
///
/// je aktivni
void remote_tds_get_active(uint8_t idx, uint8_t *active)
{
  if (idx < MAX_RTDS)
  {
    *active = EEPROM.read(remote_tds_name0 + (RTDS_DEVICE_TOTAL_LEN * idx) + RTDS_DEVICE_ACTIVE_BYTE_POS);
  }
  else
    *active = 255;
}

void remote_tds_set_active(uint8_t idx, uint8_t active)
{
  if (idx <  MAX_RTDS)
    EEPROM.write(remote_tds_name0 + (RTDS_DEVICE_TOTAL_LEN * idx) + RTDS_DEVICE_ACTIVE_BYTE_POS, active);
}
///
void remote_tds_clear(uint8_t idx)
{
  char rtds_name[RTDS_DEVICE_STRING_LEN];
  for (uint8_t i = 0; i < RTDS_DEVICE_STRING_LEN; i++)
    rtds_name[idx] = 0;
  remote_tds_set_complete(idx, 0, rtds_name);
}
/// funkce pro nastaveni odebirani topicu vzdalenych cidel
/*
  idx - index nazvu topicu, ktery si chci subscribnout/unsubscribnout
*/
void remote_tds_subscibe_topic(uint8_t idx)
{
  char tmp1[64];
  char tmp2[64];
  uint8_t active = 0;
  remote_tds_get_complete(idx, &active, tmp1);
  if (active == 1)
  {
    strcpy(tmp2, "/rtds/");
    strcat(tmp2, tmp1);
    mqtt_client.subscribe(tmp2);
  }
}
///
/// funkce pro zruseni odebirani topicu vzdalenych cidel
void remote_tds_unsubscibe_topic(uint8_t idx)
{
  char tmp1[64];
  char tmp2[64];
  uint8_t active = 0;
  remote_tds_get_complete(idx, &active, tmp1);
  if (active == 1)
  {
    strcpy(tmp2, "/rtds/");
    strcat(tmp2, tmp1);
    mqtt_client.unsubscribe(tmp2);
  }
}
///
/// vrati prvni idx banky ktera neni aktivni
uint8_t remote_tds_find_free(void)
{
  uint8_t ret = 255;
  uint8_t active;
  for (uint8_t idx = 0; idx < MAX_RTDS; idx++)
  {
    remote_tds_get_active(idx, &active);
    if (active == 0)
    {
      ret = idx;
      break;
    }
  }
  return ret;
}
/// hleda, zda jiz odebirame topic s timto nazvem
/*
   navratove hodnoty
   255 ... nenalezeno
   1..MAX_RTDS je idx bunky, kde mame tento nazev ulozen
*/
uint8_t remote_tds_name_exist(char *name)
{
  uint8_t active;
  uint8_t found = 255;
  char tmp_name[RTDS_DEVICE_STRING_LEN];
  for (uint8_t idx = 0; idx < MAX_RTDS; idx++)
  {
    remote_tds_get_complete(idx, &active, tmp_name);
    if (strcmp(name, tmp_name) == 0)
      found = idx;
  }
  return found;
}
///
void remote_tds_set_data(uint8_t idx, int temp)
{
  if (idx < MAX_RTDS)
  {
    SRAM.writeByte(ram_remote_tds_store_data_low + (ram_remote_tds_store_size * idx), (temp & 0xff));
    SRAM.writeByte(ram_remote_tds_store_data_high + (ram_remote_tds_store_size * idx), ((temp >> 8) & 0xff));
    SRAM.writeByte(ram_remote_tds_store_last_update + (ram_remote_tds_store_size * idx), 0);
  }
}
int remote_tds_get_data(uint8_t idx)
{
  int temp = 0;
  if (idx < MAX_RTDS)
  {
    temp = SRAM.readByte(ram_remote_tds_store_data_high + (ram_remote_tds_store_size * idx)) << 8;
    temp = temp = SRAM.readByte(ram_remote_tds_store_data_low + (ram_remote_tds_store_size * idx));
  }
  return temp;
}
uint8_t remote_tds_get_last_update(uint8_t idx)
{
  uint8_t last = 255;
  if (idx < MAX_RTDS)
  {
    last = SRAM.readByte(ram_remote_tds_store_last_update + (ram_remote_tds_store_size * idx));
  }
  return last;
}
uint8_t remote_tds_set_last_update(uint8_t idx, uint8_t value)
{
  if (idx < MAX_RTDS)
  {
    SRAM.writeByte(ram_remote_tds_store_last_update + (ram_remote_tds_store_size * idx), value);
  }
}
uint8_t remote_tds_inc_last_update(uint8_t idx)
{
  uint8_t last = 255;
  if (idx < MAX_RTDS)
  {
    last = SRAM.readByte(ram_remote_tds_store_last_update + (ram_remote_tds_store_size * idx));
    if (last < 250)
      last++;
    SRAM.writeByte(ram_remote_tds_store_last_update + (ram_remote_tds_store_size * idx), last);
  }
}

void remote_tds_update_last_update(void)
{
  uint8_t active;
  for (uint8_t idx = 0; idx < MAX_RTDS; idx++)
  {
    remote_tds_get_active(idx, &active);
    if (active == 1)
    {
      remote_tds_inc_last_update(idx);
    }
  }
}
/*************************************************************************************************************************/
/// funkce pro nastaveni vychoziho ringu
/*
   ring - cislo aktivniho ringu
*/
void set_default_ring(uint8_t ring)
{
  EEPROM.write(my_default_ring, ring);
}
/// funkce pro ziskani jaky ring je prave aktivni
/*
   return
     - uint8_t s id ringu
*/
uint8_t get_default_ring(void)
{
  return EEPROM.read(my_default_ring);
}
//////////////////////////
/// funkce pro nastaveni vychoziho zobrazeni zdroje teploty
void set_default_show_temp(uint8_t default_temp)
{
  EEPROM.write(my_default_show_temp, default_temp);
}
/// funkce pro ziskani vychoziho zobrazeni zdroje teploty
/*
   return
     - uint8_t s id cislo cidla
*/
uint8_t get_default_show_temp(void)
{
  return EEPROM.read(my_default_show_temp);
}





/*************************************************************************************************************************/
void tds_extended_memory_store(void)
{
  struct_DDS18s20 tds;
  int tt;
  uint16_t active = 0;
  uint16_t pos = 0;
  for (uint8_t id = 0; id < HW_ONEWIRE_MAXROMS; id++)
    if (get_tds18s20(id, &tds) == 1)
      if (tds.used == 1) if (status_tds18s20[id].online == True)
        {
          tt = status_tds18s20[id].temp;

          //pos = SRAM.write();
        }
}

/*************************************************************************************************************************/
///SEKCE MQTT ///
/// mqtt_callback - zpracovani prichozich MQTT zprav
/*
   topic - ukazatel na nazev topicu
   payload - ukazatel na pole prichozich dat
   length - pocet prichozich dat
*/
void mqtt_callback(char* topic, byte * payload, unsigned int length)
{
  char str1[64];
  const char tmp1[16];
  char tmp2[32];
  static char my_payload[128];
  boolean ret = 0;
  uint8_t cnt = 0;
  uint8_t id = 0;
  uint8_t id_interval = 0;
  struct_DDS18s20 tds;
  char *pch;
  uint8_t active;
  NTPClient timeClient(udpClient);

  for (uint8_t j = 0; j < 128; j++) my_payload[j] = 0;
  ////
  mqtt_receive_message++; /// inkrementuji promenou celkovy pocet prijatych zprav
  strncpy(my_payload, (char*) payload, length);
  ///
  /// kamarad discovery
  strcpy_P(str1, termbig_subscribe);
  if (strcmp(str1, topic) == 0)
  {
    mqtt_process_message++; /// inkrementuji promenou celkovy pocet zpracovanych zprav
    know_mqtt_create_or_update(my_payload, TYPE_TERMBIG);
  }
  strcpy_P(str1, thermctl_subscribe);
  if (strcmp(str1, topic) == 0)
  {
    mqtt_process_message++;
    know_mqtt_create_or_update(my_payload, TYPE_THERMCTL);
  }
  ///
  /// nastavovani casu
  //// /thermctl-in/global/time/set - nastaveni casu. payload json
  strcpy_P(str1, thermctl_header_in);
  strcat_P(str1, global_time_set);
  if (strcmp(str1, topic) == 0)
  {
    mqtt_process_message++;
    deserializeJson(doc, my_payload);
    JsonObject root = doc.as<JsonObject>();
    if (root.containsKey("year") && root.containsKey("month") && root.containsKey("month") && root.containsKey("hour") && root.containsKey("minute") && root.containsKey("second"))
      rtc.adjust(DateTime(root["year"], root["month"], root["day"], root["hour"], root["minute"], root["second"]));
  }
  //// /thermctl-in/global/time/ntp - jednorazova aktualizace casu z ntp serveru
  strcpy_P(str1, thermctl_header_in);
  strcat_P(str1, global_time_ntp);
  if (strcmp(str1, topic) == 0)
  {
    mqtt_process_message++;
    if (ntp_update(&timeClient, &rtc, time_get_offset()) == 1)
      cbi(selftest_data, SELFTEST_ERR_NTP); //TODO predelat na volani funkce
    else
      sbi(selftest_data, SELFTEST_ERR_NTP); //TODO predelat na volani funkce
  }
  //// /termbig-in/global/time/offset - nastaveni offsetu casu
  strcpy_P(str1, thermctl_header_in);
  strcat_P(str1, global_time_offset);
  if (strcmp(str1, topic) == 0)
  {
    mqtt_process_message++;
    time_set_offset(atoi(my_payload));
  }
  /// nastavovani vlastnosti TDS
  //// /thermctl-in/XXXX/tds/associate - asociace do tds si pridam mac 1wire - odpoved je pod jakem ID to mam ulozeno
  strcpy_P(str1, thermctl_header_in);
  strcat(str1, device.nazev);
  strcat(str1, "/tds/associate");
  if (strcmp(str1, topic) == 0)
  {
    mqtt_process_message++;
    id = atoi(my_payload);
    if (tds_associate(id) == 255)
      log_error(&mqtt_client, "tds/associate full or bad id");
  }
  ///
  //// /thermctl-in/XXXX/tds/set/IDcko/name - nastavi cidlu nazev
  //// /thermctl-in/XXXX/tds/set/IDcko/offset
  //// /thermctl-in/XXXX/tds/set/IDcko/period
  strcpy_P(str1, thermctl_header_in);
  strcat(str1, device.nazev);
  strcat(str1, "/tds/set/");
  if (strncmp(str1, topic, strlen(str1)) == 0)
  {
    mqtt_process_message++;
    cnt = 0;
    for (uint8_t f = strlen(str1); f < strlen(topic); f++)
    {
      str1[cnt] = topic[f];
      str1[cnt + 1] = 0;
      cnt++;
    }
    cnt = 0;
    pch = strtok (str1, "/");
    while (pch != NULL)
    {
      if (cnt == 0) id = atoi(pch);
      if (id < HW_ONEWIRE_MAXROMS)
      {
        if ((cnt == 1) && (strcmp(pch, "name") == 0)) tds_set_name(id, my_payload);
        if ((cnt == 1) && (strcmp(pch, "offset") == 0)) tds_set_offset(id, atoi(my_payload));
        if ((cnt == 1) && (strcmp(pch, "period") == 0)) tds_set_period(id, atoi(my_payload));
      }
      else
      {
        log_error(&mqtt_client, "tds/set bad id");
      }
      pch = strtok (NULL, "/");
      cnt++;
    }
  }
  ////
  //// /thermctl-in/XXXX/tds/clear
  strcpy_P(str1, thermctl_header_in);
  strcat(str1, device.nazev);
  strcat(str1, "/tds/clear");
  if (strncmp(str1, topic, strlen(str1)) == 0)
  {
    mqtt_process_message++;
    id = atoi(my_payload);
    if (id < HW_ONEWIRE_MAXROMS)
      tds_set_clear(id);
    else
      log_error(&mqtt_client, "tds/clear bad id");
  }
  ////////
  ////////
  /// nastavovani vlastnosti RTDS ///

  //// thermctl-in/XXXXX/rtds/register - registruje nove vzdalene cidlo
  strcpy_P(str1, thermctl_header_in);
  strcat(str1, device.nazev);
  strcat(str1, "/rtds/register");
  if (strncmp(str1, topic, strlen(str1)) == 0)
  {
    mqtt_process_message++;
    if (remote_tds_name_exist(my_payload) == 255)
    {
      id = remote_tds_find_free();
      remote_tds_set_complete(id, 1, my_payload);
      remote_tds_subscibe_topic(id);
    }
    ///TODO - vratit ze jiz existuje
  }

  //// thermctl-in/XXXXX/rtds/set/IDX/name - nastavi a udela prihlaseni
  strcpy_P(str1, thermctl_header_in);
  strcat(str1, device.nazev);
  strcat(str1, "/rtds/set/");
  if (strncmp(str1, topic, strlen(str1)) == 0)
  {
    mqtt_process_message++;
    cnt = 0;
    for (uint8_t f = strlen(str1); f < strlen(topic); f++)
    {
      str1[cnt] = topic[f];
      str1[cnt + 1] = 0;
      cnt++;
    }
    cnt = 0;
    pch = strtok (str1, "/");
    while (pch != NULL)
    {
      if (cnt == 0) id = atoi(pch);
      if ((cnt == 1) && (strcmp(pch, "name") == 0))
      {
        remote_tds_get_active(id, &active);
        if (active == 0)
        {
          remote_tds_set_complete(id, 1, my_payload);
          remote_tds_subscibe_topic(id);
        }
        else
        {
          log_error(&mqtt_client, "rtds/set bad id");
        }
      }
      pch = strtok (NULL, "/");
      cnt++;
    }
  }

  ////
  //// /thermctl-in/XXXX/rtds/clear index vymaze a odhlasi
  strcpy_P(str1, thermctl_header_in);
  strcat(str1, device.nazev);
  strcat(str1, "/rtds/clear");
  if (strncmp(str1, topic, strlen(str1)) == 0)
  {
    mqtt_process_message++;
    id = atoi(my_payload);
    remote_tds_unsubscibe_topic(id);
    /// TODO dodelat navratovou chybu
    remote_tds_clear(id);
  }
  ///
  //// ziska nastaveni remote_tds
  strcpy_P(str1, thermctl_header_in);
  strcat(str1, device.nazev);
  strcat(str1, "/rtds/get");
  if (strncmp(str1, topic, strlen(str1)) == 0)
  {
    send_mqtt_remote_tds_status();
  }
  ////
  //// rtds/NAME - hodnota, kde NAME je nazev cidla
  strcpy(str1, "/rtds/");
  if (strncmp(str1, topic, strlen(str1)) == 0)
  {
    mqtt_process_message++;
    cnt = 0;
    for (uint8_t f = strlen(str1); f < strlen(topic); f++)
    {
      str1[cnt] = topic[f];
      str1[cnt + 1] = 0;
      cnt++;
    }
    for (uint8_t idx = 0; idx < MAX_RTDS; idx++)
    {
      uint8_t active = 0;
      remote_tds_get_complete(idx, &active, tmp1);
      if (active == 1 && strcmp(tmp1, str1) == 0)
      {
        remote_tds_set_data(idx, atoi(my_payload));
      }
    }
  }
  strcpy(str1, "/rtds/list");
  if (strncmp(str1, topic, strlen(str1)) == 0)
  {

  }
  ////////
  ////////
  ////////
  /// nastavovani vlastnosti programu
  //// thermctl-in/XXXXX/prog/set/IDX/name  - 8znaku
  //// thermctl-in/XXXXX/prog/set/IDX/active  - 0-off, 1-heat, 2-cool,3.....
  //// thermctl-in/XXXXX/prog_interval/set/IDX/IDcko/theshold - pro jednotlive casove useky ruzne teploty
  //// thermctl-in/XXXXX/prog_interval/set/IDX/IDcko/active - pro jednotlivy usek povoleni zakazani
  //// thermctl-in/XXXXX/prog_interval/set/IDX/IDcko/time - nastavi cas pro jednotlive intervaly
  //// thermctl-in/XXXXX/prog/clear - IDX
  strcpy_P(str1, thermctl_header_in);
  strcat(str1, device.nazev);
  strcat(str1, "/prog/set/");
  if (strncmp(str1, topic, strlen(str1)) == 0)
  {
    mqtt_process_message++;
    cnt = 0;
    for (uint8_t f = strlen(str1); f < strlen(topic); f++)
    {
      str1[cnt] = topic[f];
      str1[cnt + 1] = 0;
      cnt++;
    }
    cnt = 0;
    pch = strtok (str1, "/");
    while (pch != NULL)
    {
      if (cnt == 0) id = atoi(pch);
      if (id < AVAILABLE_PROGRAM)
      {
        if ((cnt == 1) && (strcmp(pch, "name") == 0))  thermostat_program_set_name(id, my_payload);
        if ((cnt == 1) && (strcmp(pch, "active") == 0))  thermostat_program_set_active(id, atoi(my_payload));
      }
      else
      {
        log_error(&mqtt_client, "prog/set bad id");
      }
      pch = strtok (NULL, "/");
      cnt++;
    }
  }
  ////
  strcpy_P(str1, thermctl_header_in);
  strcat(str1, device.nazev);
  strcat(str1, "/prog/clear");
  if (strncmp(str1, topic, strlen(str1)) == 0)
  {
    mqtt_process_message++;
    id = atoi(my_payload);
    if (id < AVAILABLE_PROGRAM)
    {
      strcpy(tmp2, "PROG");
      thermostat_program_set_name(id, tmp2);
      thermostat_program_set_active(id, 0);
      for (uint8_t progid = 0; progid < MAX_PROGRAM_INTERVAL; progid++)
      {
        thermostat_program_set_time(id, progid, 0, 0, 0, 0, 0);
        thermostat_program_set_threshold(id, progid, 220);
        thermostat_program_set_week(id, progid, 0);
      }
      for (uint8_t tix = 0; tix < MAX_THERMOSTAT; tix++)
      {
        if (thermostat_ring_get_program_id(tix) == id)
        {
          thermostat_ring_set_program_id(tix, PROG_FREE);
        }
      }
    }
    else
    {
      log_error(&mqtt_client, "prog/clear bad id");
    }
  }
  /////
  strcpy_P(str1, thermctl_header_in);
  strcat(str1, device.nazev);
  strcat(str1, "/prog_interval/set/");
  if (strncmp(str1, topic, strlen(str1)) == 0)
  {
    mqtt_process_message++;
    cnt = 0;
    for (uint8_t f = strlen(str1); f < strlen(topic); f++)
    {
      str1[cnt] = topic[f];
      str1[cnt + 1] = 0;
      cnt++;
    }
    cnt = 0;
    pch = strtok (str1, "/");
    while (pch != NULL)
    {
      if (cnt == 0) id = atoi(pch);
      if (cnt == 1) id_interval = atoi(pch);
      if (id < AVAILABLE_PROGRAM && id_interval < MAX_PROGRAM_INTERVAL)
      {
        if ((cnt == 2) && (strcmp(pch, "active") == 0))  thermostat_program_set_interval_active(id, id_interval, atoi(my_payload));
        if ((cnt == 2) && (strcmp(pch, "threshold") == 0))  thermostat_program_set_threshold(id, id_interval , atoi(my_payload));
        if ((cnt == 2) && (strcmp(pch, "time") == 0)) thermostat_program_set_parse_interval(id, id_interval, my_payload);
      }
      else
      {
        log_error(&mqtt_client, "prog_interval/set bad id");
      }
      pch = strtok (NULL, "/");
      cnt++;
    }
  }
  ///
  //// thermctl-in/XXXXX/ring/default, nastavi vychozi ring na displaji
  strcpy_P(str1, thermctl_header_in);
  strcat(str1, device.nazev);
  strcat(str1, "/ring/default");
  if (strcmp(str1, topic) == 0)
  {
    mqtt_process_message++;
    default_ring = atoi(my_payload);
    set_default_ring(default_ring);
  }
  ///
  //// thermctl-in/XXXXX/ring/get/IDcko/pid
  strcpy_P(str1, thermctl_header_in);
  strcat(str1, device.nazev);
  strcat(str1, "/ring/get/");
  if (strncmp(str1, topic, strlen(str1)) == 0)
  {
    cnt = 0;
    for (uint8_t f = strlen(str1); f < strlen(topic); f++)
    {
      str1[cnt] = topic[f];
      str1[cnt + 1] = 0;
      cnt++;
    }
    cnt = 0;
    pch = strtok (str1, "/");
    while (pch != NULL)
    {
      if (cnt == 0) id = atoi(my_payload);
      if (id < MAX_THERMOSTAT)
      {
        if ((cnt == 1) && (strcmp(pch, "pid") == 0))  mqtt_send_pid_variable(id);
      }
      else
      {
        log_error(&mqtt_client, "ring/set bad id");
      }
      pch = strtok (NULL, "/");
      cnt++;
    }
  }
  ////////
  ////////
  /// nastavovani vlastnosti regulacnich okruhu
  //// thermctl-in/XXXXX/ring/set/IDcko/name
  //// thermctl-in/XXXXX/ring/set/IDcko/program
  //// thermctl-in/XXXXX/ring/set/IDcko/threshold
  //// thermctl-in/XXXXX/ring/set/IDcko/text_mode
  //// thermctl-in/XXXXX/ring/set/IDcko/mode
  //// thermctl-in/XXXXX/ring/set/IDcko/tds
  //// thermctl-in/XXXXX/ring/set/IDcko/rtds
  //// thermctl-in/XXXXX/ring/set/IDcko/ready
  //// thermctl-in/XXXXX/ring/set/IDcko/output
  //// thermctl-in/XXXXX/ring/set/IDcko/pid_kp
  //// thermctl-in/XXXXX/ring/set/IDcko/pid_ki
  //// thermctl-in/XXXXX/ring/set/IDcko/pid_kd
  //// thermctl-in/XXXXX/ring/set/IDcko/pid_time
  strcpy_P(str1, thermctl_header_in);
  strcat(str1, device.nazev);
  strcat(str1, "/ring/set/");
  if (strncmp(str1, topic, strlen(str1)) == 0)
  {
    cnt = 0;
    for (uint8_t f = strlen(str1); f < strlen(topic); f++)
    {
      str1[cnt] = topic[f];
      str1[cnt + 1] = 0;
      cnt++;
    }
    cnt = 0;
    pch = strtok (str1, "/");
    while (pch != NULL)
    {
      if (cnt == 0) id = atoi(pch);
      if (id < MAX_THERMOSTAT)
      {
        if ((cnt == 1) && (strcmp(pch, "name") == 0)) thermostat_ring_set_name(id, my_payload);
        if ((cnt == 1) && (strcmp(pch, "program") == 0))
          if (atoi(my_payload) < AVAILABLE_PROGRAM)
            thermostat_ring_set_program_id(id, atoi(my_payload));
        if ((cnt == 1) && (strcmp(pch, "threshold-f") == 0)) thermostat_ring_set_mezni(id, atof(my_payload) * 10); /// thereshold hodnota presne ve floatu * 10
        if ((cnt == 1) && (strcmp(pch, "threshold") == 0)) thermostat_ring_set_mezni(id, atoi(my_payload));
        if ((cnt == 1) && (strcmp(pch, "text_mode") == 0))
        {
          active = convert_text_mode(my_payload);
          thermostat_ring_set_mode(id, active);
          if (active == TERM_MODE_MAN_HEAT)
            thermostat_ring_update_bites(id, STATUS_BIT_HEAT_OR_COOL, 0);
          if (active == TERM_MODE_MAN_COOL)
            thermostat_ring_update_bites(id, STATUS_BIT_HEAT_OR_COOL, 1);
        }
        if ((cnt == 1) && (strcmp(pch, "mode") == 0))
        {
          active = atoi(my_payload);
          thermostat_ring_set_mode(id, active);
          if (active == TERM_MODE_MAN_HEAT)
            thermostat_ring_update_bites(id, STATUS_BIT_HEAT_OR_COOL, 0);
          if (active == TERM_MODE_MAN_COOL)
            thermostat_ring_update_bites(id, STATUS_BIT_HEAT_OR_COOL, 1);
        }
        if ((cnt == 1) && (strcmp(pch, "tds") == 0)) thermostat_ring_set_asociate_tds(id, atoi(my_payload));
        if ((cnt == 1) && (strcmp(pch, "rtds") == 0)) thermostat_ring_set_asociate_tds(id, atoi(my_payload) + TDS_MEMORY_MAP_RTDS);
        if ((cnt == 1) && (strcmp(pch, "active") == 0)) thermostat_ring_set_active(id, atoi(my_payload));
        if ((cnt == 1) && (strcmp(pch, "output") == 0)) thermostat_ring_set_output(id, atoi(my_payload));
        if ((cnt == 1) && (strcmp(pch, "pid_kp") == 0)) thermostat_ring_pid_set_kp(id, atof(my_payload));
        if ((cnt == 1) && (strcmp(pch, "pid_ki") == 0)) thermostat_ring_pid_set_ki(id, atof(my_payload));
        if ((cnt == 1) && (strcmp(pch, "pid_kd") == 0)) thermostat_ring_pid_set_kd(id, atof(my_payload));
        if ((cnt == 1) && (strcmp(pch, "pid_time") == 0)) thermostat_ring_pid_set_time(id, atoi(my_payload));
      }
      else
      {
        log_error(&mqtt_client, "ring/set bad id");
      }
      pch = strtok (NULL, "/");
      cnt++;
    }

  }
  //// nastaveni ringu do vychoziho stavu
  strcpy_P(str1, thermctl_header_in);
  strcat(str1, device.nazev);
  strcat(str1, "/ring/clear");
  if (strncmp(str1, topic, strlen(str1)) == 0)
  {
    mqtt_process_message++;
    id = atoi(my_payload);
    if (id < MAX_THERMOSTAT)
    {
      thermostat_ring_clear(id);
    }
    else
    {
      log_error(&mqtt_client, "ring/clear bad id");
    }
  }
  ////////
  ////////
  //// nastavovani vlastnosti site
  //// ziskani nastaveni site
  strcpy_P(str1, thermctl_header_in);
  strcat(str1, device.nazev);
  strcat(str1, "/network/get/config");
  if (strncmp(str1, topic, strlen(str1)) == 0)
  {
    mqtt_process_message++;
    send_network_config(&mqtt_client);
  }
  /// nastaveni site
  //// thermctl-in/XXXXX/network/set/mac
  //// thermctl-in/XXXXX/network/set/ip
  //// thermctl-in/XXXXX/network/set/netmask
  //// thermctl-in/XXXXX/network/set/gw
  //// thermctl-in/XXXXX/network/set/dns
  //// thermctl-in/XXXXX/network/set/ntp
  //// thermctl-in/XXXXX/network/set/mqtt_host
  //// thermctl-in/XXXXX/network/set/mqtt_port
  //// thermctl-in/XXXXX/network/set/mqtt_user
  //// thermctl-in/XXXXX/network/set/mqtt_key
  //// thermctl-in/XXXXX/network/set/name
  strcpy_P(str1, thermctl_header_in);
  strcat(str1, device.nazev);
  strcat(str1, "/network/set/");
  if (strncmp(str1, topic, strlen(str1)) == 0)
  {
    mqtt_process_message++;
    cnt = 0;
    for (uint8_t f = strlen(str1); f < strlen(topic); f++)
    {
      str1[cnt] = topic[f];
      str1[cnt + 1] = 0;
      cnt++;
    }
    cnt = setting_network(str1, my_payload);
    if (cnt == 1)
    {
      save_setup_network();
      sbi(selftest_data, SELFTEST_RESTART_NEEDED); //TODO predelat na volani funkce
    }
    if (cnt == 2)
    {
      sbi(selftest_data, SELFTEST_RESTART_NEEDED); //TODO predelat na volani funkce
    }
  }
  //// priradi k menu pro rizeni intezity svetla vystup
  //// thermctl-in/XXXXX//light/set/IDX/output
  strcpy_P(str1, thermctl_header_in);
  strcat(str1, device.nazev);
  strcat(str1, "/light/set/");
  if (strncmp(str1, topic, strlen(str1)) == 0)
  {
    mqtt_process_message++;
    //strncpy(str2, payload, length);
    cnt = 0;
    for (uint8_t f = strlen(str1); f < strlen(topic); f++)
    {
      str1[cnt] = topic[f];
      str1[cnt + 1] = 0;
      cnt++;
    }
    cnt = 0;
    pch = strtok (str1, "/");
    while (pch != NULL)
    {
      if (cnt == 0) id = atoi(pch);
      //if ((cnt == 1) && (strcmp(pch, "output") == 0)) light_set_output(id, atoi(my_payload));
      //if ((cnt == 1) && (strcmp(pch, "value") == 0)) light_value[id] = atoi(my_payload);
      pch = strtok (NULL, "/");
      cnt++;
    }
  }
  ///
  /// zpetna vazba od vystupu
  strcpy_P(str1, termbig_header_out);
  strcat(str1, "output/");
  if (strncmp(str1, topic, strlen(str1)) == 0)
  {
    mqtt_process_message++;
    cnt = 0;
    for (uint8_t f = strlen(str1); f < strlen(topic); f++)
    {
      str1[cnt] = topic[f];
      str1[cnt + 1] = 0;
      cnt++;
    }
    cnt = 0;
    pch = strtok (str1, "/");
    while (pch != NULL)
    {
      if (cnt == 0) id = atoi(pch);
      if (cnt == 1)
      {
        for (uint8_t idx = 0; idx < MAX_THERMOSTAT; idx++)
          if (thermostat_ring_get_output(idx) == id)
          {
            if (strcmp(pch, "pwm")) last_output_update[id] = 0;
            if (strcmp(pch, "state")) last_output_update[id] = 0;
            break;
          }
      }
      pch = strtok (NULL, "/");
      cnt++;
    }
  }

  //// thermctl-in/XXXXX/reload
  strcpy_P(str1, thermctl_header_in);
  strcat(str1, device.nazev);
  strcat(str1, "/reload");
  if (strcmp(str1, topic) == 0)
  {
    mqtt_process_message++;
    log_error(&mqtt_client, "reload ..... ");
    resetFunc();
  }

  //// thermctl-in/XXXXX/reload
  strcpy_P(str1, thermctl_header_in);
  strcat(str1, device.nazev);
  strcat(str1, "/bootloader");
  if (strcmp(str1, topic) == 0)
  {
    mqtt_process_message++;
    log_error(&mqtt_client, "bootloader ..... ");
    EEPROM.write(bootloader_tag, 255);
    wdt_enable(WDTO_1S);
    while (1);
  }

  //// /thermctl-in/XXXXX/reset_default
  strcpy_P(str1, thermctl_header_in);
  strcat(str1, device.nazev);
  strcat(str1, "/default");
  if (strcmp(str1, topic) == 0)
  {
    mqtt_process_message++;
    //strncpy(str2, payload, length);
    EEPROM.write(set_default_values, atoi(my_payload));
  }

}
/*************************************************************************************************************************/
/// mqtt_reconent - funkce pro prihlaseni k MQTT topicum
byte mqtt_reconnect(void)
{
  char nazev[10];
  char topic[26];
  byte ret = MQTT_DISCONNECTED;
  ///  /thermctl/xxxxxxxx/#
  ///  /thermctl/global/#
  device_get_name(nazev);
  if (!mqtt_client.connected())
    if (mqtt_client.connect(nazev))
    {
      strcpy_P(topic, thermctl_header_in);
      strcat(topic, nazev);
      strcat(topic, "/#");
      mqtt_client.subscribe(topic);
      strcpy_P(topic, thermctl_header_in);
      strcat(topic, "global/#");
      mqtt_client.subscribe(topic);
      //// /rtds/xxxxx
      for (uint8_t idx = 0; idx < MAX_RTDS; idx++)
        remote_tds_subscibe_topic(idx);
      //// svetelny controller
      strcpy_P(topic, lightctl_header_out);
      strcat(topic, "/#");
      mqtt_client.subscribe(topic);
      /// zpetna vazba od ostatnich room controleru
      strcpy_P(topic, thermctl_subscribe);
      mqtt_client.subscribe(topic);
      /// zpetna vazba od ostatnich term controlleru
      strcpy_P(topic, termbig_subscribe);
      mqtt_client.subscribe(topic);
      /// zpetna vazba od vystupu
      strcpy_P(topic, termbig_header_out);
      mqtt_client.subscribe(topic);
    }
  ret = mqtt_client.state();
  return ret;
}
/*************************************************************************************************************************/
///
/// funkce pro odeslani informaci o 1wire zarizenich
/*
   /thermctl_out/XXXXX/1wire/count
   /thermctl_out/XXXXX/1wire/IDcko/rom
*/
void send_mqtt_onewire(void)
{
  char str_topic[64];
  char hostname[10];
  char payload[64];
  char tmp1[4];
  itoa(Global_HWwirenum, payload, 10);
  send_mqtt_general_payload(&mqtt_client, "1wire/count", payload);
  for (uint8_t i = 0; i < Global_HWwirenum; i++)
  {
    createString(payload, ':', w_rom[i].rom, 8, 16);
    send_mqtt_message_prefix_id_topic_payload(&mqtt_client, "1wire", i, "rom", payload);
    ///
    itoa(w_rom[i].assigned_ds2482, payload, 10);
    send_mqtt_message_prefix_id_topic_payload(&mqtt_client, "1wire", i, "assigned", payload);
    ///
    itoa(w_rom[i].tds_idx, payload, 10);
    send_mqtt_message_prefix_id_topic_payload(&mqtt_client, "1wire", i, "tds_idx", payload);
  }
}
///
/// funkce pro odeslani zakladnich informaci o tomto zarizeni
void send_device_status(void)
{
  char str_topic[64];
  char payload[64];
  if (mqtt_client.connected())
  {
    strcpy(str_topic, "status/uptime");
    itoa(uptime, payload, 10);
    send_mqtt_general_payload(&mqtt_client, str_topic, payload);
    ///
    strcpy(str_topic, "status/brigthness");
    itoa(light_curr, payload, 10);
    send_mqtt_general_payload(&mqtt_client, str_topic, payload);
    ///
    //strcpy(str_topic, "status/light");
    //itoa(jas_disp, payload, 10);
    //send_mqtt_general_payload(&mqtt_client, str_topic, payload);
    ///
    strcpy(str_topic, "status/auto_brigthness");
    itoa(auto_jas, payload, 10);
    send_mqtt_general_payload(&mqtt_client, str_topic, payload);
    ///
    strcpy(str_topic, "status/load_min");
    itoa(load_min, payload, 10);
    send_mqtt_general_payload(&mqtt_client, str_topic, payload);
    ///
    strcpy(str_topic, "status/load_max");
    itoa(load_max, payload, 10);
    send_mqtt_general_payload(&mqtt_client, str_topic, payload);
    ///
    load_max = 0;
    load_min = 0xffffffff;
    ///
    strcpy(str_topic, "status/default_ring");
    itoa(default_ring, payload, 10);
    send_mqtt_general_payload(&mqtt_client, str_topic, payload);
    ///
    strcpy(str_topic, "status/selftest");
    itoa(selftest_data, payload, 10);
    send_mqtt_general_payload(&mqtt_client, str_topic, payload);

    strcpy(str_topic, "status/rtds/count");
    itoa(use_rtds, payload, 10);
    send_mqtt_general_payload(&mqtt_client, str_topic, payload);
    /*
      strcpy(str_topic, "status/light/count");
      itoa(use_light_curr, payload, 10);
      send_mqtt_general_payload(&mqtt_client, str_topic, payload);
    */
    itoa(time_get_offset(), payload, 10);
    send_mqtt_general_payload(&mqtt_client, "time/ntp_offset", payload);
    ///
    dtostrf(prepocet_napeti(tritri, CONST_PREVOD_TRIV), 4, 2, payload);
    send_mqtt_general_payload(&mqtt_client, "status/voltage/33V", payload);
    ///
    dtostrf(prepocet_napeti(petnula, CONST_PREVOD_PETV), 4, 2, payload);
    send_mqtt_general_payload(&mqtt_client, "status/voltage/50V", payload);
    ///
    dtostrf(prepocet_napeti(dvanact, CONST_PREVOD_DVANACTV), 4, 2, payload);
    send_mqtt_general_payload(&mqtt_client, "status/voltage/12V", payload);
    ///
    dtostrf(prepocet_proudu(proud), 4, 2, payload);
    send_mqtt_general_payload(&mqtt_client, "status/current/consume", payload);
  }
}
///
/// funkce pro odeslani informaci o stavu ringu
/*
  /thermctl-out/XXXXX/ring/ID/name
  /thermctl-out/XXXXX/ring/ID/ready
  /thermctl-out/XXXXX/ring/ID/program
  /thermctl-out/XXXXX/ring/ID/threshold
  /thermctl-out/XXXXX/ring/ID/mode
  /thermctl-out/XXXXX/ring/ID/status
  /thermctl-out/XXXXX/ring/ID/tds
  /thermctl-out/XXXXX/ring/ID/output
*/
void send_mqtt_ring(void)
{
  //char str_topic[64];
  //char hostname[10];
  char payload[64];
  uint8_t tdsid;
  //char tmp1[12];
  //device_get_name(hostname);
  for (uint8_t idx = 0; idx < MAX_THERMOSTAT; idx++)
    //// odeslu pouze pokud je ring pripraveny
    if (thermostat_ring_get_active(idx) != RING_FREE)
    {
      thermostat_ring_get_name(idx, payload);
      send_mqtt_message_prefix_id_topic_payload(&mqtt_client, "ring", idx, "name", payload);
      itoa(thermostat_ring_get_active(idx), payload, 10);
      send_mqtt_message_prefix_id_topic_payload(&mqtt_client, "ring", idx, "active", payload);
      itoa(thermostat_ring_get_program_id(idx), payload, 10);
      send_mqtt_message_prefix_id_topic_payload(&mqtt_client, "ring", idx, "program", payload);
      itoa(thermostat_ring_get_mezni(idx), payload, 10);
      send_mqtt_message_prefix_id_topic_payload(&mqtt_client, "ring", idx, "threshold", payload);
      itoa(thermostat_ring_get_mode(idx), payload, 10);
      send_mqtt_message_prefix_id_topic_payload(&mqtt_client, "ring", idx, "mode", payload);
      convert_mode_text(thermostat_ring_get_mode(idx), payload);
      send_mqtt_message_prefix_id_topic_payload(&mqtt_client, "ring", idx, "text_mode", payload);
      itoa(thermostat_ring_get_state(idx), payload, 10);
      send_mqtt_message_prefix_id_topic_payload(&mqtt_client, "ring", idx, "status", payload);
      tdsid = thermostat_ring_get_asociate_tds(idx);
      if (tdsid >= TDS_MEMORY_MAP_TDS && tdsid < TDS_MEMORY_MAP_RTDS)
      {
        itoa(tdsid, payload, 10);
        send_mqtt_message_prefix_id_topic_payload(&mqtt_client, "ring", idx, "tds", payload);
      }
      if (tdsid >= TDS_MEMORY_MAP_RTDS && tdsid < 127)
      {
        itoa(tdsid - TDS_MEMORY_MAP_RTDS, payload, 10);
        send_mqtt_message_prefix_id_topic_payload(&mqtt_client, "ring", idx, "rtds", payload);
      }
      itoa(thermostat_ring_get_output(idx), payload, 10);
      send_mqtt_message_prefix_id_topic_payload(&mqtt_client, "ring", idx, "output", payload);

      itoa(thermostat_ring_get_status_data(idx), payload, 10);
      send_mqtt_message_prefix_id_topic_payload(&mqtt_client, "ring", idx, "status_bites", payload);

      itoa(last_output_update[idx], payload, 10);
      send_mqtt_message_prefix_id_topic_payload(&mqtt_client, "ring", idx, "output_update", payload);
    }
}
///
///
/// funkce pro odeslani informaci o tds sensoru
//// /thermctl-out/XXXXX/tds/ID/temp
//// /thermctl-out/XXXXX/tds/ID/name
//// /thermctl-out/XXXXX/tds/ID/offset
//// /thermctl-out/XXXXX/tds/ID/online
//// /thermctl-out/XXXXX/tds/ID/rom
//// /thermctl-out/XXXXX/tds/ID/period
void send_mqtt_tds(void)
{
  struct_DDS18s20 tds;
  char payload[64];
  char tmp1[4];
  int tt;
  long avg = 0;
  for (uint8_t id = 0; id < HW_ONEWIRE_MAXROMS; id++)
    if (get_tds18s20(id, &tds) == 1)
      if (tds.used == 1) if (status_tds18s20[id].online == True)
        {
          tt = status_tds18s20[id].temp / 10;
          itoa(tt, payload, 10);
          send_mqtt_message_prefix_id_topic_payload(&mqtt_client, "tds", id, "temp", payload);
          avg = 0;
          for (uint8_t c = 0; c < MAX_AVG_TEMP; c++) avg = avg + status_tds18s20[id].average_temp[c];
          avg = avg / MAX_AVG_TEMP;
          avg = avg / 10;
          itoa(avg, payload, 10);
          send_mqtt_message_prefix_id_topic_payload(&mqtt_client, "tds", id, "temp_avg", payload);

          strcpy(payload, tds.name);
          send_mqtt_message_prefix_id_topic_payload(&mqtt_client, "tds", id, "name", payload);
          tt = tds.offset;
          itoa(tt, payload, 10);
          send_mqtt_message_prefix_id_topic_payload(&mqtt_client, "tds", id, "offset", payload);
          tt = status_tds18s20[id].online;
          itoa(tt, payload, 10);
          send_mqtt_message_prefix_id_topic_payload(&mqtt_client, "tds", id, "online", payload);
          payload[0] = 0;
          createString(payload, ':', tds.rom, 8, 16);
          send_mqtt_message_prefix_id_topic_payload(&mqtt_client, "tds", id, "rom", payload);
          tt = tds.period;
          itoa(tt, payload, 10);
          send_mqtt_message_prefix_id_topic_payload(&mqtt_client, "tds", id, "period", payload);

          tt = (uptime & 0xff) - status_tds18s20[id].period_now;
          itoa(tt, payload, 10);
          send_mqtt_message_prefix_id_topic_payload(&mqtt_client, "tds", id, "start_at", payload);
        }
}
///
///
//// thermctl-out/XXXXX/prog/IDX/name  - 8znaku
//// thermctl-out/XXXXX/prog_interval/IDX/IDCko/week  - program plati v tech dnech
//// thermctl-out/XXXXX/prog_interval/IDX/IDcko/mode - pro jednotlive casove useky ruzny rezim
//// thermctl-out/XXXXX/prog_interval/IDX/IDcko/theshold - pro jednotlive casove useky ruzne teploty
//// thermctl-out/XXXXX/prog_interval/IDX/IDcko/active - pro jednotlivy usek povoleni zakazani
//// thermctl-out/XXXXX/prog_interval/IDX/IDcko/time - nastavi cas pro jednotlive intervaly
void send_mqtt_program(void)
{
  char payload[64];
  char tmp1[6];
  uint8_t act = 0;
  uint8_t start_hour, start_min, stop_hour, stop_min, active;
  for (uint8_t idx = 0; idx < AVAILABLE_PROGRAM; idx++)
  {
    act = thermostat_program_get_active(idx);
    if ( act != 0)
    {
      thermostat_program_get_name(idx, payload);
      send_mqtt_message_prefix_id_topic_payload(&mqtt_client, "prog", idx, "name", payload);
      itoa(act, payload, 10);
      send_mqtt_message_prefix_id_topic_payload(&mqtt_client, "prog", idx, "active", payload);
      for (uint8_t progid = 0 ; progid < MAX_PROGRAM_INTERVAL; progid++)
      {
        thermostat_program_get_time(idx, progid, &start_hour, &start_min, &stop_hour, &stop_min, &active);
        if (active == 1)
        {
          itoa(start_hour, tmp1, 10);
          strcpy(payload, tmp1);
          strcat(payload, ",");
          itoa(start_min, tmp1, 10);
          strcat(payload, tmp1);
          strcat(payload, ",");
          itoa(stop_hour, tmp1, 10);
          strcat(payload, tmp1);
          strcat(payload, ",");
          itoa(stop_min, tmp1, 10);
          strcat(payload, tmp1);
          send_mqtt_message_prefix_id_idx_topic_payload(&mqtt_client, "prog_interval", idx, progid, "time", payload);
          itoa(active, tmp1, 10);
          strcpy(payload, tmp1);
          send_mqtt_message_prefix_id_idx_topic_payload(&mqtt_client, "prog_interval", idx, progid, "active", payload);
          itoa(thermostat_program_get_threshold(idx, progid), tmp1, 10);
          strcpy(payload, tmp1);
          send_mqtt_message_prefix_id_idx_topic_payload(&mqtt_client, "prog_interval", idx, progid, "threshold", payload);
          itoa(thermostat_program_get_week(idx, progid), payload, 10);
          send_mqtt_message_prefix_id_idx_topic_payload(&mqtt_client, "prog_interval", idx, progid, "week", payload);
        }
      }
    }
  }
}
///
///
/// funkce pro odeslani PID vlastnosti regulatoru
/*
   /thermctl-out/XXXXX/pid/IDX/kp
   /thermctl-out/XXXXX/pid/IDX/ki
   /thermctl-out/XXXXX/pid/IDX/kd
*/
void mqtt_send_pid_variable(uint8_t idx)
{
  char payload[32];
  dtostrf(thermostat_get_pid_p(idx), 7, 2, payload);
  send_mqtt_message_prefix_id_topic_payload(&mqtt_client, "pid", idx, "kp", payload);
  dtostrf(thermostat_get_pid_i(idx), 7, 2, payload);
  send_mqtt_message_prefix_id_topic_payload(&mqtt_client, "pid", idx, "ki", payload);
  dtostrf(thermostat_get_pid_d(idx), 7, 2, payload);
  send_mqtt_message_prefix_id_topic_payload(&mqtt_client, "pid", idx, "kd", payload);
  itoa(thermostat_get_pid_time(idx), payload, 10);
  send_mqtt_message_prefix_id_topic_payload(&mqtt_client, "pid", idx, "time", payload);
}
///
///
/*
  void send_know_device(void)
  {
  char str_topic[64];
  char payload[64];
  for (uint8_t idx = 0; idx < MAX_KNOW_MQTT; idx++)
  {
    if (know_mqtt[idx].type != TYPE_FREE)
    {
      itoa(know_mqtt[idx].type, payload, 10);
      send_mqtt_message_prefix_id_topic_payload(&mqtt_client, "know_mqtt_device", idx, "type", payload);
      itoa(know_mqtt[idx].last_update, payload, 10);
      send_mqtt_message_prefix_id_topic_payload(&mqtt_client, "know_mqtt_device", idx, "last_update", payload);
      strcpy(payload, know_mqtt[idx].device);
      send_mqtt_message_prefix_id_topic_payload(&mqtt_client, "know_mqtt_device", idx, "device", payload);
    }
  }
  }
*/
///
///
void send_mqtt_remote_tds_status(void)
{
  uint8_t active = 0;
  char payload[RTDS_DEVICE_STRING_LEN];
  for (uint8_t idx = 0; idx < MAX_RTDS; idx++)
  {
    remote_tds_get_complete(idx, &active, payload);
    /// odeslu pouze pokud je neco aktivni, jinak ne
    if (active == 1)
    {
      send_mqtt_message_prefix_id_topic_payload(&mqtt_client, "rtds", idx, "name", payload);
      itoa(active, payload, 10);
      send_mqtt_message_prefix_id_topic_payload(&mqtt_client, "rtds", idx, "active", payload);
      itoa(remote_tds_get_data(idx), payload, 10);
      send_mqtt_message_prefix_id_topic_payload(&mqtt_client, "rtds", idx, "temp", payload);
      itoa(remote_tds_get_last_update(idx), payload, 10);
      send_mqtt_message_prefix_id_topic_payload(&mqtt_client, "rtds", idx, "last_update", payload);
    }
  }
}
///
///
void mqtt_publis_output(uint8_t idx, uint8_t state)
{
  char str_topic[64];
  char payload[64];
  char str1[8];
  if (idx != 255)
  {
    strcpy_P(str_topic, termbig_header_in);
    strcat(str_topic, "power-output/");
    itoa(idx, str1, 10);
    strcat(str_topic, str1);
    strcat(str_topic, "/state");
    itoa(state, payload, 10);
    mqtt_client.publish(str_topic, payload);
  }
}
///
///
void mqtt_publis_output_pwm(uint8_t idx, uint8_t mode, uint8_t pwm)
{
  char str_topic[64];
  char payload[64];
  char str1[8];
  if (idx != 255)
  {
    strcpy_P(str_topic, termbig_header_in);
    strcat(str_topic, "power-output/");
    itoa(idx, str1, 10);
    strcat(str_topic, str1);
    if (mode == TERM_MODE_MAN_HEAT)
      strcat(str_topic, "/heat");
    if (mode == TERM_MODE_MAN_COOL)
      strcat(str_topic, "/cool");
    if (mode == TERM_MODE_FAN)
      strcat(str_topic, "/fan");
    if (mode == TERM_MODE_ERR)
      strcat(str_topic, "/err");
    strcat(str_topic, "/pwm");
    itoa(pwm, payload, 10);
    mqtt_client.publish(str_topic, payload);
  }
}
/*************************************************************************************************************************/
/////  zde resim, zpracovani zmerenych dat ///////////////////////////////////////////////////
void a2d_complete(uint8_t channel, uint16_t data)
{
  if (channel == A_PROUD) proud = data;
  if (channel == A_TRIV ) tritri = data;
  if (channel == A_PETV) petnula = data;
  if (channel == A_DVANACTV) dvanact = data;
  if (channel == A_LIGHT) light_curr = data;
  a2d_run_now = 1;
}



///////////// kontrolni funkce ///////////////////////////
/// selftest_set_0 - nastaveni chyboveho flagu
/*
   what - jaky chybovy flag
*/
void selftest_set_0(uint8_t what)
{
  sbi(selftest_data, what) ;
}
///
/// selftest_clear_0 - vymazani chyboveho flagu, problem jiz neni
/*
   what - jaky chybovy flag
*/
void selftest_clear_0(uint8_t what)
{
  cbi(selftest_data, what) ;
}
///
/// selftest_get_0 - flag je v jakem stavu
/*
   what - jaky chybovy flag

   return
     ... 0 - neni oznacen jako chybny
     ... neni 0 - chybny flag
*/
uint8_t selftest_get_0(uint8_t what)
{
  return selftest_data & (1 << what);
}
///
/// selftest - pusteni skupiny vlastnich selftestu
void selftest(void)
{
  if (!rtc.isrunning())
    sbi(selftest_data, SELFTEST_ERR_RTC);
  else
    cbi(selftest_data, SELFTEST_ERR_RTC);
}
///
/// funkce pro prepocet napeti z ADC prevodniku na skutecnou hodnotu
/*
   vstup - 10bit hodnota z prevodniku
   prevodni_pomer - magic (2.5m * pomer delice)*1000

    return
      - float cislo s vysledkem ve Voltech
*/
float prepocet_napeti(uint16_t vstup, uint8_t prevodni_pomer)
{
  float volt = (vstup * prevodni_pomer) / 1000.0;
  return volt;
}
///
/// funkce pro vypocet odebiraneho proudu zarizeni
/*
   vstup - 10bit hodnota z prevodniku

    return
      - float hodnota v Amperech
*/
float prepocet_proudu(uint16_t vstup)
{
#define proud_constanta  470
  float proud = (vstup - proud_constanta) * 5 / 185;
  return vstup;
}
///
/////////////// Casove funkce ///////////////////////////////////////////////////////////////////
/// time_get_offset - ziska casovy offset, letni/zimni cas
uint8_t time_get_offset(void)
{
  return EEPROM.read(time_offset);
}
/// time_set_offset - nastavi casovy offset letni/zimni
void time_set_offset(uint8_t offset)
{
  EEPROM.write(time_offset, offset);
}
/////////////// HW funkce ///////////////////////////////////////////////////////////////////////

/// setup_io_pin - nastaveni IO pinu
void setup_io_pin(void)
{
  pinMode(ETH_RST, OUTPUT);
  pinMode(ETH_CS, OUTPUT);
  pinMode(LED, OUTPUT);
  pinMode(PIEZO, OUTPUT);
  pinMode(STORAGE_RAM_CS, OUTPUT);
  pinMode(STORAGE_EEPROM_CS, OUTPUT);
  pinMode(JUMPER, INPUT_PULLUP);

  digitalWrite(PIEZO, LOW);
  delay(1000);
  digitalWrite(PIEZO, HIGH);



}
//////////////////////////////////////////////////////////////////////////////////////////////////
////////////////// Prevodni funkce ///////////////////////////////////////////////////////////////
/// funkce prevadi ciselnou hodnotu na skutecne pojmenovani
uint8_t convert_text_mode(char *str2)
{
  uint8_t mode = 0;
  if (strcmp(str2, "off") == 0) mode = TERM_MODE_OFF;
  if (strcmp(str2, "heat") == 0) mode = TERM_MODE_MAX;
  if (strcmp(str2, "manual") == 0) mode = TERM_MODE_MAN_HEAT;
  if (strcmp(str2, "auto") == 0) mode = TERM_MODE_PROG;
  if (strcmp(str2, "cool") == 0) mode = TERM_MODE_CLIMATE_MAX;
  if (strcmp(str2, "fan_only") == 0) mode = TERM_MODE_FAN;
  return mode;
}

void convert_mode_text(uint8_t mode, char *str)
{
  if (mode == TERM_MODE_OFF)   strcpy(str, "off");
  if (mode == TERM_MODE_MAX)   strcpy(str, "heat");
  if (mode == TERM_MODE_MAN_HEAT)   strcpy(str, "manual");
  if (mode == TERM_MODE_PROG)   strcpy(str, "auto");
  if (mode == TERM_MODE_CLIMATE_MAX)   strcpy(str, "cool");
  if (mode == TERM_MODE_FAN)   strcpy(str, "fan_only");
}

void convert_mode_text_1(uint8_t mode, char *str)
{
  if (mode == TERM_MODE_OFF)   strcpy_P(str, button_term_off);
  if (mode == TERM_MODE_MAX)   strcpy_P(str, button_term_max);
  if (mode == TERM_MODE_MAN_HEAT)   strcpy_P(str, button_term_man);
  if (mode == TERM_MODE_PROG)   strcpy_P(str, button_term_prog);
  if (mode == TERM_MODE_CLIMATE_MAX)   strcpy_P(str, button_term_man);
  if (mode == TERM_MODE_FAN)   strcpy_P(str, button_term_fan);
  if (mode == TERM_MODE_MAN)   strcpy_P(str, button_term_man);
  if (mode == TERM_MODE_MIN)   strcpy_P(str, button_term_min);
}
/*************************************************************************************************************************/

//////
void thermostat(void)
{
  uint8_t tdsid = 0;
  uint8_t tmode = 0;
  uint8_t tout = 0;
  int16_t thresh = 0;
  uint8_t pwm = 200; // start hodnota
  uint8_t te = 0;
  uint8_t prg = 0;
  uint8_t act;
  uint8_t active = 0;
  struct_DDS18s20 tds;


  for (uint8_t tix = 0; tix < MAX_THERMOSTAT; tix++)
  {
    tdsid = thermostat_ring_get_asociate_tds(tix);
    tmode = thermostat_ring_get_mode(tix);
    tout = thermostat_ring_get_output(tix);
    thresh = thermostat_ring_get_mezni(tix);
    if (tmode == TERM_MODE_PROG)
    {
      prg = thermostat_ring_get_program_id(tix);
      te = thermostat_running(prg, &thresh, now.hour(), now.minute(), now.dayOfTheWeek());
      switch (te)
      {
        case PROG_FREE:
          {
            tmode = TERM_MODE_OFF;
            /// blikam s ledkou protoze program neni aktivni, nebo program neni vybran
            //blink_led_set(LED_PROG_I, 3);
            //blink_led_set(LED_ERR_I, 1);
            thermostat_ring_set_state(tix, TERM_STAV_STOP);
            thermostat_ring_set_power(tix, 0);
          }
        case PROG_NO_INTERVAL:
          {
            tmode = TERM_MODE_OFF;
            /// blikam s ledkou protoze program neni aktivni, nebo program neni vybran
            //blink_led_set(LED_PROG_I, 1);
            thermostat_ring_set_state(tix, TERM_STAV_STOP);
            thermostat_ring_set_power(tix, 0);
            break;
          }
        case PROG_ACTIVE:
          {
            /// blikaci ledku vypnu, program aktivni
            //blink_led_off(LED_PROG_I);
            //blink_led_off(LED_ERR_I);
            /// zde si ziskam rezim vypnuto, topi, chladi, ventilator, podle toho si prenastavim ring mode, potrebuji nastavit pid regulator
            thermostat_ring_set_state(tix, TERM_STAV_BEZI);
            act = thermostat_program_get_active(prg);
            if (act == 1) tmode = TERM_MODE_MAN_HEAT;
            if (act == 2) tmode = TERM_MODE_MAN_COOL;
            if (act == 3) tmode = TERM_MODE_FAN;
            break;
          }
        default: break;
      }
    }
    if (tmode == TERM_MODE_MAN_HEAT)
    {

      thermostat_pid_setdirection_direct(tix);
    }

    if (tmode == TERM_MODE_MAN_COOL)
    {

      thermostat_pid_setdirection_reverse(tix);
    }

    if (tdsid >= TDS_MEMORY_MAP_TDS && tdsid < TDS_MEMORY_MAP_RTDS)
    {
      if (get_tds18s20(tdsid, &tds) == 1)
        if (tds.used == 1 && status_tds18s20[tdsid].online == True)
        {
          thermostat_pid_input(tix, status_tds18s20[tdsid].temp / 100);
          thermostat_pid_setpoint(tix, thresh);
          pwm = thermostat_pid_output(tix);
          thermostat_ring_set_power(tix, pwm);
        }
        else
        {
          tmode = TERM_MODE_ERR;
          pwm = 0;
          thermostat_ring_set_power(tix, pwm);
        }
    }

    if (tdsid >= TDS_MEMORY_MAP_RTDS && tdsid < 127)
    {
      act = tdsid - TDS_MEMORY_MAP_RTDS;
      remote_tds_get_active(act , &active);
      /*
        if (active == 1 && remote_tds_last_update[act] < 180)
        {
        thermostat_pid_input(tix, remote_tds[act] / 10);
        thermostat_pid_setpoint(tix, thresh);
        pwm = thermostat_pid_output(tix);
        thermostat_ring_set_power(tix, pwm);
        }
        else
        {
        tmode = TERM_MODE_ERR;
        pwm = 0;
        thermostat_ring_set_power(tix, pwm);
        }
      */
    }

    if (tmode == TERM_MODE_MAN_HEAT)
    {
      /*
        if (pwm < 25)
        //blink_led_set(LED_UP_I, 1);
        else
        blink_led_off(LED_UP_I);
      */
    }

    if (tmode == TERM_MODE_MAN_COOL)
    {
      /*
        if (pwm < 25)
        blink_led_set(LED_DOWN_I, 1);
        else
        blink_led_off(LED_DOWN_I);
      */
    }

    if (tmode == TERM_MODE_OFF)
    {
      mqtt_publis_output(tout, POWER_OUTPUT_OFF);
      thermostat_ring_set_power(tix, 0);
    }
    if (tmode == TERM_MODE_MAX)
    {
      mqtt_publis_output(tout, POWER_OUTPUT_HEAT_MAX);
      thermostat_ring_set_power(tix, 255);
    }
    if (tmode == TERM_MODE_CLIMATE_MAX)
    {
      mqtt_publis_output(tout, POWER_OUTPUT_COOL_MAX);
      thermostat_ring_set_power(tix, 255);
    }
    if (tmode == TERM_MODE_MAN_HEAT || tmode == TERM_MODE_MAN_COOL || tmode == TERM_MODE_FAN || tmode == TERM_MODE_ERR)
    {
      mqtt_publis_output_pwm(tout, tmode, pwm);
    }
  }
}
/*************************************************************************************************************************/


/// setup - zakladni nastaveni - vola se pouze 1x
void setup()
{
  char str1[64];
  char str2[64];
  char str3[64];
  char tmp1[20];
  char s_tritri[8];
  char s_petnula[8];
  char s_dvanact[8];
  char s_current[8];
  struct_DDS18s20 tds;
  uint32_t milis;

  NTPClient timeClient(udpClient);

  setup_io_pin();

  interrupts();             // enable all interrupts
  SPI.begin();
  Wire.begin();
  rtc.begin();

  mqtt_set_public_mqtt_client(&mqtt_client);
  fdevopen( &printf_via_mqtt, 0);
  printf_begin();

  swSPI.begin();

  MenuHistoryInit();

  my_lcd.Init_LCD();
  my_lcd.Set_Rotation(3);
  my_touch.TP_Set_Rotation(3);
  my_touch.TP_Init(my_lcd.Get_Rotation(), my_lcd.Get_Display_Width(), my_lcd.Get_Display_Height());
  my_lcd.Fill_Screen(WHITE);
  my_lcd.Set_Draw_color(BLUE);
  strcpy_P(str1, term_title);
  show_string(str1, 30, 10, 2, BLUE, WHITE, 0);
  ///
  for (uint8_t init = 0;  init < 16; init++)
  {
    show_string("X", 30 + (init * 15), 30, 1, RED, WHITE, 0);
    /// inicalizace vnitrniho ADC prevodniku
    if (init == 0)
    {
      strcpy_P(str1, text_vnitrni_ochrany);
      strcat(str1, ":1");
      show_string(str1, 30, 50 + (init * 10), 1, GREEN, WHITE, 0 );

      tritri = 0;
      petnula = 0;
      dvanact = 0;
      dvanact = 0;
      light_curr = 0;
      /// inicializace prevodniku
      init_a2d((1 << A_PROUD | 1 << A_TRIV | 1 << A_PETV | 1 << A_DVANACTV | 1 << A_LIGHT), 6, 3);
      a2d_handler(a2d_complete);
      ///
      /// spotreba proudu
      a2d_run_now = 0;
      a2d_start_first(A_PROUD);
      milis = millis();
      while (a2d_run_now == 0 && millis() - milis < 10);
      /// regulator 3.3V
      a2d_run_now = 0;
      a2d_start_first(A_TRIV);
      milis = millis();
      while (a2d_run_now == 0 && millis() - milis < 10);
      /// regulator 5.0V
      a2d_run_now = 0;
      a2d_start_first(A_PETV);
      milis = millis();
      while (a2d_run_now == 0 && millis() - milis < 10);
      /// vstupni napeti 12V
      a2d_run_now = 0;
      a2d_start_first(A_DVANACTV);
      milis = millis();
      while (a2d_run_now == 0 && millis() - milis < 10);
      /// aktualni intenzita svetla
      a2d_run_now = 0;
      a2d_start_first(A_LIGHT);
      milis = millis();
      while (a2d_run_now == 0 && millis() - milis < 10);
      /// zatim nepotrebuji automaticke mereni
      a2d_run_now = 0;
      /// prevedu float na string
      dtostrf(prepocet_napeti(tritri, CONST_PREVOD_TRIV), 4, 2, s_tritri);
      dtostrf(prepocet_napeti(petnula, CONST_PREVOD_PETV), 4, 2, s_petnula);
      dtostrf(prepocet_napeti(dvanact, CONST_PREVOD_DVANACTV), 4, 2, s_dvanact);
      dtostrf(prepocet_proudu(proud), 4, 2, s_current);
      sprintf(str2, "3.3V=%s; 5.0V=%s; IN=%s; I=%s", s_tritri, s_petnula, s_dvanact, s_current);
      show_string(str2, 160, 50 + (init * 10), 1, GREEN, WHITE, 0 );
    }
    ///
    /// inicializace vnitrniho teplomeru LM75B
    if (init == 1)
    {
      strcpy_P(str1, text_vnitrni_ochrany);
      strcat(str1, ":2");
      show_string(str1, 30, 50 + (init * 10), 1, GREEN, WHITE, 0 );
      dtostrf(lm75_temp.readTemperatureC(), 4, 2, s_current);
      sprintf(str2, "iT=%sC", s_current);
      show_string(str2, 160, 50 + (init * 10), 1, GREEN, WHITE, 0 );
    }
    ///
    //// nastaveni vychozich hodnot systemu
    if (init == 2)
    {
      if (EEPROM.read(set_default_values) == 255)
      {
        EEPROM.write(set_default_values, 0);
        strcpy_P(str1, text_vychozi_nastaveni);
        show_string(str1, 30, 50 + (init * 10), 1, RED, WHITE, 0 );

        set_default_show_temp(255);

        for (uint8_t idx = 0; idx < MAX_THERMOSTAT; idx++)
          thermostat_ring_clear(idx);
        ///
        for (uint8_t idx = 0; idx < MAX_RTDS; idx++)
        {
          strcpy(tmp1, "");
          remote_tds_clear(idx);
        }
        ///
        for (uint8_t idx = 0; idx < AVAILABLE_PROGRAM; idx++)
        {
          strcpy(str2, "PROG");
          thermostat_program_set_name(idx, str2);
          thermostat_program_set_active(idx, 0);
          for (uint8_t interval_id = 0; interval_id < MAX_PROGRAM_INTERVAL; interval_id++)
          {
            thermostat_program_set_time(idx, interval_id, 0, 0, 0, 0, 0);
            thermostat_program_set_threshold(idx, interval_id, 220);
            thermostat_program_set_week(idx, interval_id, 0);
          }
        }
        ///
        for (uint8_t idx = 0; idx < HW_ONEWIRE_MAXDEVICES; idx++)
        {
          get_tds18s20(idx, &tds);
          strcpy(tds.name, "FREE");
          tds.used = 0;
          tds.offset = 0;
          tds.assigned_ds2482 = 0;
          tds.period = 10;
          for (uint8_t m = 0; m < 8; m++) tds.rom[m] = 0xff;
          set_tds18s20(idx, &tds);
        }

        rtc.adjust(DateTime(2020, 12, 14, 17, 14, 0));
        time_set_offset(1);
        /// pokus o vetsi random
        device.mac[0] = 2; device.mac[1] = 1; device.mac[2] = 2; device.mac[3] = dvanact >> 1; device.mac[4] = light_curr >> 1; device.mac[5] = proud >> 1;
        device.myIP[0] = 192; device.myIP[1] = 168; device.myIP[2] = 2; device.myIP[3] = 110;
        device.myMASK[0] = 255; device.myMASK[1] = 255; device.myMASK[2] = 255; device.myMASK[3] = 0;

        device.myGW[0] = 192; device.myGW[1] = 168; device.myGW[2] = 2; device.myGW[3] = 1;
        device.myDNS[0] = 192; device.myDNS[1] = 168; device.myDNS[2] = 2; device.myDNS[3] = 1;
        device.mqtt_server[0] = 192; device.mqtt_server[1] = 168; device.mqtt_server[2] = 2; device.mqtt_server[3] = 1;
        device.ntp_server[0] = 192; device.ntp_server[1] = 168; device.ntp_server[2] = 2; device.ntp_server[3] = 1;
        device.mqtt_port = 1883;
        strcpy(device.mqtt_user, "saric");
        strcpy(device.mqtt_key, "no");
        save_setup_network();
        strcpy(str1, "TERM D1");
        device_set_name(str1);
        char hostname[10];
        device_get_name(hostname);
        default_ring = 0;
        set_default_ring(default_ring);
      }
      else
      {
        strcpy_P(str1, text_bezny_start);
        show_string(str1, 30, 50 + (init * 10), 1, GREEN, WHITE, 0 );
      }
    }
    ///
    /// test SRAM
    if (init == 3)
    {
      SRAM.begin();
      SRAM.writeByte(0, 0xAB);
      if (SRAM.readByte(0) == 0xAB)
      {
        strcpy_P(str1, text_test_ram);
        strcat(str1, " init ...");
        show_string(str1, 30, 50 + (init * 10), 1, GREEN, WHITE, 0 );
        for (uint8_t i = 0; i < 64; i++) str3[i] = 0;
        /*
          for (uint32_t p = 0; p < 2049; p++)
          {
          SRAM.writeBlock((p * 64), 64, (uint8_t)str3);
          if ((p % 256) == 0)
          {
            sprintf(str1, "%d/2048", p);
            show_string(str1, 180, 50 + (init * 10), 1, GREEN, WHITE, 0 );
          }
          }
        */
        strcpy_P(str1, text_ok);
        show_string(str1, 210, 50 + (init * 10), 1, GREEN, WHITE, 0 );
      }
      else
      {
        strcpy_P(str1, text_test_ram);
        strcat_P(str1, text_err);
        show_string(str1, 30, 50 + (init * 10), 1, RED, WHITE, 0 );
      }
    }

    //// init EEPROMKY
    if (init == 4)
    {
      SROM.begin();
      SROM.writeByte(0, 0xBA);
      if (SROM.readByte(0) == 0xBA)

      {
        strcpy_P(str1, text_test_eeprom);
        strcat_P(str1, text_ok);
        show_string(str1, 30, 50 + (init * 10), 1, GREEN, WHITE, 0 );
      }
      else
      {
        strcpy_P(str1, text_test_eeprom);
        strcat_P(str1, text_err);
        show_string(str1, 30, 50 + (init * 10), 1, RED, WHITE, 0 );
      }
    }

    ///  nacteni provoznich parametru
    if (init == 5)
    {
      strcpy_P(str1, text_nastaveni_systemu);
      show_string(str1, 30, 50 + (init * 10), 1, GREEN, WHITE, 0 );
      /// nactu nastaveni site
      load_setup_network();
      //// kvuli lepsimu nabehu pocitani nastavim vychozi hodnotu na 2000 = 20 stupnu
      for (uint8_t idx = 0; idx < HW_ONEWIRE_MAXROMS; idx++)
        for (uint8_t cnt = 0; cnt < MAX_AVG_TEMP; cnt++)
          status_tds18s20[idx].average_temp[cnt] = 20000;
      ///
      for (uint8_t idx = 0; idx < MAX_THERMOSTAT; idx++)
        last_output_update[idx] = 0;
    }
    ///
    /// zobrazeni kalibracnich informaci touchscreenu
    if (init == 6)
    {
      strcpy_P(str1, text_touchscreen);
      show_string(str1, 30, 50 + (init * 10), 1, GREEN, WHITE, 0 );
      my_touch.TP_Init(my_lcd.Get_Rotation(), my_lcd.Get_Display_Width(), my_lcd.Get_Display_Height());
    }
    ///
    /// inicializace ds2482
    if (init == 7)
    {
      strcpy_P(str1, text_onewire_rozhrani);
      show_string(str1, 30, 50 + (init * 10), 1, GREEN, WHITE, 0 );
      ds2482_address[0].i2c_addr = 0b0011000;
      ds2482_address[0].HWwirenum = 0;
      ///
      for (uint8_t idx = 0; idx < HW_ONEWIRE_MAXROMS; idx++ )
      {
        status_tds18s20[idx].wait = false;
        status_tds18s20[idx].period_now = 0;
      }
      itoa(ds2482_address[0].i2c_addr, tmp1, 10);
      if (ds2482reset(ds2482_address[0].i2c_addr) == DS2482_ERR_OK)
      {
        strcpy_P(str1, text_ok);
        show_string(str1, 160, 50 + (init * 10), 1, GREEN, WHITE, 0 );
        selftest_clear_0(SELFTEST_ERR_ONEWIRE);
      }
      else
      {
        strcpy_P(str1, text_err);
        show_string(str1, 160, 50 + (init * 10), 1, RED, WHITE, 0 );
        selftest_set_0(SELFTEST_ERR_ONEWIRE);
      }
    }
    ///
    /// hledani zarizeni na 1wire sbernici
    if (init == 8)
    {
      strcpy_P(str1, text_onewire_hledam);
      show_string(str1, 30, 50 + (init * 10), 1, GREEN, WHITE, 0 );
      Global_HWwirenum = 0;
      one_hw_search_device(0);
      tds_update_associate();
      strcpy (str1, "Nalezeno: ");
      itoa(Global_HWwirenum, str2, 10);
      strcat(str1, str2);
      show_string(str1, 160, 50 + (init * 10), 1, GREEN, WHITE, 0 );
    }
    ///
    /// inicializace wiznet w5500
    if (init == 9)
    {
      digitalWrite(ETH_RST, HIGH);
      delay(10);
      digitalWrite(ETH_RST, LOW);
      delay(10);
      digitalWrite(ETH_RST, HIGH);
      delay(100);
      Ethernet.init(ETH_CS);
      Ethernet.begin(device.mac, device.myIP, device.myDNS, device.myGW, device.myMASK);
      strcpy_P(str1, text_ethernet_rozhrani);
      show_string(str1, 30, 50 + (init * 10), 1, GREEN, WHITE, 0 );
      delay(100);
    }
    ///
    ///inicializace mqtt rozhrani
    if (init == 10)
    {
      strcpy_P(str1, text_mqtt_rozhrani);
      show_string(str1, 30, 50 + (init * 10), 1, GREEN, WHITE, 0 );
      mqtt_client.setServer(device.mqtt_server, device.mqtt_port);
      mqtt_client.setCallback(mqtt_callback);
      send_mqtt_set_header(thermctl_header_out);
      milis = millis();
      while ((millis() - milis) < 3000 )
      {
        mqtt_client.loop();
        if (mqtt_reconnect() == 0)
        {
          selftest_clear_0(SELFTEST_MQTT_LINK);
          break;
        }
        else
          selftest_set_0(SELFTEST_MQTT_LINK);
      }

      if (selftest_get_0(SELFTEST_MQTT_LINK) != 0)
      {
        strcpy_P(str1, text_err);
        show_string(str1, 160, 50 + (init * 10), 1, RED, WHITE, 0 );
      }
      else
      {
        strcpy_P(str1, text_ok);
        show_string(str1, 160, 50 + (init * 10), 1, GREEN, WHITE, 0 );
      }

    }
    ///
    /// inicializace rest api rozhrani
    if (init == 11)
    {
      strcpy_P(str1, text_rest_rozhrani);
      show_string(str1, 30, 50 + (init * 10), 1, GREEN, WHITE, 0 );
    }
    ///
    /// inicializace nrf
    if (init == 12)
    {
      strcpy_P(str1, text_nrf_rozhrani);
      show_string(str1, 30, 50 + (init * 10), 1, GREEN, WHITE, 0 );
    }
    ///
    /// inicializace termostatu
    if (init == 13)
    {
      strcpy_P(str1, text_pid_regulator);
      show_string(str1, 30, 50 + (init * 10), 1, GREEN, WHITE, 0 );
      thermostat_init_pid();
      /// nastaveni vychozich hodnot pro regulator
      for (uint8_t idx = 0; idx < MAX_THERMOSTAT; idx++)
      {
        thermostat_ring_set_state(idx, 0);
        thermostat_ring_set_power(idx, 0);
        thermostat_ring_get_pid_eeprom(idx);
        thermostat_ring_update_pid_parameters(idx);
      }
      default_ring = get_default_ring();
      default_show_temp = get_default_show_temp();
    }
    ///
    /// kontrola NTP serveru
    if (init == 14)
    {
      strcpy_P(str1, text_ntp_cas);
      show_string(str1, 30, 50 + (init * 10), 1, GREEN, WHITE, 0 );
      if (ntp_check(&timeClient) == 0)
      {
        strcpy_P(str1, text_err);
        show_string(str1, 160, 50 + (init * 10), 1, RED, WHITE, 0 );
        selftest_set_0(SELFTEST_ERR_NTP);
      }
      else
      {
        strcpy_P(str1, text_ok);
        show_string(str1, 160, 50 + (init * 10), 1, GREEN, WHITE, 0 );
        selftest_clear_0(SELFTEST_ERR_NTP);
      }
    }
    ///
    /// inicializace RTC casu
    if (init == 15)
    {
      strcpy_P(str1, text_rtc_cas);
      show_string(str1, 30, 50 + (init * 10), 1, GREEN, WHITE, 0 );
      if (rtc.isrunning())
      {
        strcpy_P(str1, text_ok);
        show_string(str1, 160, 50 + (init * 10), 1, GREEN, WHITE, 0 );
        selftest_clear_0(SELFTEST_ERR_RTC);
      }
      else
      {
        strcpy_P(str1, text_err);
        show_string(str1, 160, 50 + (init * 10), 1, RED, WHITE, 0 );
        selftest_set_0(SELFTEST_ERR_RTC);
      }
    }

  }
  delay(500);
  MenuPrepareStyle();
  draw_menu(true);
  a2d_run_now = 1;

}



/************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////
/// Hlavni programova smycka ///
void loop() {
  // put your main code here, to run repeatedly:
  char str1[16];
  unsigned long load_now;



  load_now = millis();
  if (mqtt_reconnect() == 0)
    selftest_clear_0(SELFTEST_MQTT_LINK);
  else
    selftest_set_0(SELFTEST_MQTT_LINK);

  mqtt_client.loop();


  if (draw_menu(false) == true)
    draw_menu(true);

  ////////////////////
  /// kazdych 10sec
  if ((millis() - milis_10s) > 10000)
  {
    milis_10s = millis();


    menu_redraw10s = 1;

    //device_get_name(str1);

    send_mqtt_onewire();
    send_mqtt_status(&mqtt_client);
    send_device_status();
    send_mqtt_ring();
    send_mqtt_tds();

    //send_mqtt_program();
    ///thermostat();
    //mqtt_send_pid_variable();
    send_mqtt_remote_tds_status();
    //send_network_config(&mqtt_client);
    //send_light_controler();
    //send_know_device();
    //send_mesh_status();


  }

  if ((millis() - milis_1s) > 1000)
  {
    milis_1s = millis();
    uptime++;
    mereni_hwwire(uptime);
    tds_extended_memory_store();
    remote_tds_update_last_update();
  }

  //digitalWrite(LED, LOW);
  if ((millis() - milis_05s) > 500)
  {
    milis_05s = millis();
    now = rtc.now();
    selftest();
    menu_redraw05s = 1;


    use_rtds = count_use_rtds();
    use_tds = count_use_tds();
  }






  /// kazdych 50ms
  if ((millis() - milis_005s) > 50)
  {
    milis_005s = millis();

    /// obsluha adc prevodniku
    if (a2d_run_now == 1)
    {
      a2d_start_process();
      a2d_run_now = 0;
      if (a2d_done() == 0) a2d_run_now = 1;
    }
  }

  load = millis() - load_now;
  if (load < load_min) load_min = load;
  if (load > load_max) load_max = load;

  /// automaticke nastaveni jasu displaye
  if (light_curr < light_min) light_min = light_curr;
  if (light_curr > light_max) light_max = light_curr;
  ///
  /*
    if (jas_disp == 0) // Automatika
    {
      auto_jas = (float) (light_curr - light_min) / (light_max - light_min) * 255;
      itmp = auto_jas;
      if (itmp > 240) itmp = 240;
      analogWrite(PWM_DISP, itmp);
    }
  */
}


void display_element_rectangle(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint8_t args1, uint8_t args2)
{
  my_lcd.Draw_Rectangle(x, y, x + size_x, y + size_y);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void display_element_dialog_default_ring(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint8_t args1, uint8_t args2)
{
  char name[10];
  char default_text[30];
  strcpy_P(default_text, regulator_default_text);
  if (thermostat_ring_get_active(default_ring) != 255)
  {
    thermostat_ring_get_name(default_ring, name);
    strcat(default_text, name);
  }
  else
  {
    strcpy_P(name, text_err);
    strcat(default_text, name);
  }
  show_string(default_text, x, y, 2, BLACK, WHITE, 0);
}
////
void display_element_default_ring_set_temp(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint8_t args1, uint8_t args2)
{
  display_element_dialog_set_variable(x, y, size_x, size_y, args1, args2);
}


/// funkce pro zobrazeni casu
void display_element_show_time_1(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint8_t args1, uint8_t args2)
{
  char str1[24];
  uint16_t pos = 0;
  strcpy_P(str1, current_time);
  pos = (244 - show_string_size_width(16, 1)) / 2;
  show_string(str1, x + pos + 5, y + 5, 1, BLACK, WHITE, 0);
  if (selftest_get_0(SELFTEST_ERR_RTC) == 0)
  {
    if (uptime % 2 )
      sprintf(str1, "%02d:%02d", now.hour(), now.minute());
    else
      sprintf(str1, "%02d %02d", now.hour(), now.minute());
  }
  else
  {
    strcpy(str1, "--:--");
  }
  my_lcd.Set_Draw_color(BLACK);
  show_string(str1, x + 4, y + 20, 8, BLACK, WHITE, 0);
  my_lcd.Draw_Rectangle(x, y, x + 244, y + 84);
}
///
/// funkce pro zobrazeni teploty
void display_element_show_temp_1(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint8_t args1, uint8_t args2)
{
  char str1[32];
  char str2[20];
  float te;
  uint8_t stat;
  strcpy_P(str1, current_temp);
  stat = get_global_temp(default_show_temp, str2, &te);
  strcat(str1, str2);
  show_string(str1, x + 5, y + 5, 1, BLACK, WHITE, 0);
  if (stat == 1)
  {
    dtostrf(te, 4, 1, str1);
    if (strlen(str1) <= 4)
      strcat(str1, "C");
    show_string(str1, x + 4, y + 20, 8, BLACK, WHITE, 0);
  }
  else
  {
    strcpy_P(str1, text_err);
    show_string(str1, x + 4, y + 20, 8, RED, WHITE, 0);
  }
  my_lcd.Set_Draw_color(BLACK);
  my_lcd.Draw_Rectangle(x, y, x + 244, y + 84);
}
////////////////////////////////////////////////////
void display_element_show_date_1(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint8_t args1, uint8_t args2)
{
  char str1[16];
  if (selftest_get_0(SELFTEST_ERR_RTC) == 0)
  {
    sprintf(str1, "%02d.%02d.%04d", now.day(), now.month(), now.year());
  }
  else
  {
    strcpy(str1, "--.--.----");
  }
  my_lcd.Set_Draw_color(WHITE); my_lcd.Draw_Fast_HLine(x, y, 142); my_lcd.Draw_Fast_HLine(x, y + 1, 142); show_string(str1, x, y + 2, 3, BLACK, WHITE, 0);
}
/////
void display_element_dialog_show_text(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y)
{
  show_string(dialog_text, x, y, 2, BLACK, WHITE, 0);
}
//////////////////////////////////////////////
void display_element_show_tds_info_dynamics(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint8_t args1, uint8_t args2)
{
  struct_DDS18s20 tds;
  char str1[26];
  char str2[26];
  float te;
  if (get_tds18s20(args2, &tds) == 1)
  {
    /// zobrazeni teploty
    te = status_tds18s20[args2].temp / 1000.0;
    dtostrf(te, 4, 2, str1);
    strcat(str1, "C");
    strcpy_P(str2, current_temp_short);
    strcat(str2, str1);
    show_string(str2, x + 10, y + 140 , 2, BLACK, WHITE, 0);
  }
}
////
void display_element_show_tds_info_static(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint8_t args1, uint8_t args2)
{
  struct_DDS18s20 tds;
  char str1[26];
  char str2[26];
  float te;
  my_lcd.Set_Draw_color(BLACK);
  my_lcd.Draw_Rectangle(x, y, x + size_x , y + size_y);
  if (get_tds18s20(args2, &tds) == 1)
  {
    createString(str2, ':', tds.rom, 8, 16);
    sprintf(str1, "rom: %s", str2);
    show_string(str1, x + 10, y + 85, 1, BLACK, WHITE, 0);
    ///
    strcpy_P(str2, text_nazev);
    sprintf(str1, "%s: %s", str2, tds.name);
    show_string(str1, x + 10, y + 40, 2, BLACK, WHITE, 0);
    /// zobrazeni offsetu
    te = tds.offset / 1000.0;
    dtostrf(te, 4, 2, str1);
    strcat(str1, "C");
    strcpy_P(str2, temp_offset_short);
    strcat(str2, str1);
    show_string(str2, x + 10, y + 60 , 2, BLACK, WHITE, 0);
    ///
    strcpy_P(str2, nastaveni_tds_period);
    sprintf(str1, "%s: %ds", str2, tds.period);
    show_string(str1, x + 10, y + 100 , 2, BLACK, WHITE, 0);

    ///
    strcpy_P(str2, text_online);
    sprintf(str1, "%s: %d", str2, status_tds18s20[args2].online);
    show_string(str1, x + 10, y + 120 , 2, BLACK, WHITE, 0);
  }
  else
  {
    strcpy_P(str1, text_err);
    show_string(str1, x + 25, y + 40, 4, RED, WHITE, 0);
  }
}
/////
void display_element_show_rtds_info_dynamics(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint8_t args1, uint8_t args2)
{
  char str1[32];
  char str2[26];
  char name[RTDS_DEVICE_STRING_LEN];
  float te;
  uint8_t last_update;
  uint8_t active;
  my_lcd.Set_Draw_color(BLACK);
  my_lcd.Draw_Rectangle(x, y, x + size_x , y + size_y);
  remote_tds_get_complete(args2, &active, name);
  if (active == 1)
  {
    /// nazev
    strcpy_P(str1, text_nazev);
    show_string(str1, x + 5 , y + 10 , 2, BLACK, WHITE, 0);
    strcpy(str1, name);
    show_string(str1, x + 20 , y + 30 , 2 , BLACK, WHITE, 0);
    /// teplota
    te = remote_tds_get_data(args2) / 1000.0;
    dtostrf(te, 4, 2, str1);
    strcat(str1, "C");
    strcpy_P(str2, current_temp_short);
    strcat(str2, str1);
    show_string(str2, x + 5, y + 60 , 2, BLACK, WHITE, 0);
    /// posledni aktualizace
    last_update = remote_tds_get_last_update(args2);
    strcpy_P(str2, text_last_update);
    sprintf(str1, "%s: %d", str2, last_update);
    show_string(str1, x + 5, y + 80 , 1, BLACK, WHITE, 0);
    active = 1;
    if (last_update >= 250) active = 0;
    strcpy_P(str2, text_online);
    sprintf(str1, "%s: %d", str2, active);
    show_string(str1, x + 5, y + 100 , 2, BLACK, WHITE, 0);
  }
}
////////////////////////////////////////////////////////
void display_element_dialog_set_variable(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint8_t args1, uint8_t args2)
{
  uint16_t tmp1, posx1, posy1;
  char line1[9];
  float te;

  my_lcd.Set_Draw_color(BLACK);
  for (uint8_t r = 0; r < 3; r++)
    my_lcd.Draw_Rectangle(x + r, y + r, x + size_x - r, y + size_y - r);
  my_lcd.Set_Draw_color(WHITE);
  my_lcd.Fill_Rectangle(x + 3, y + 3, x + size_x - 3, y + size_y - 3);

  if (menu_dialog_set_divider != 0)
  {
    te = menu_dialog_set_variable_now / (menu_dialog_set_divider * 1.0);
    dtostrf(te, 4, 2, line1);
  }
  else
    itoa(menu_dialog_set_variable_now, line1, 10);
  my_lcd.Set_Draw_color(BLACK);



  tmp1 = show_string_size_width(strlen(line1), 2);
  posx1 = (size_x - tmp1) / 2;
  posy1 = (size_y - show_string_size_hight(2)) / 2;
  show_string(line1, x + posx1 , y + posy1 - 1 , 2, BLACK, WHITE, 0);
}
//////////////
void display_element_dialog_set_string(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint8_t args1, uint8_t args2)
{
  uint16_t tmp1, posx1, posy1;
  my_lcd.Set_Draw_color(BLACK);
  for (uint8_t r = 0; r < 3; r++)
    my_lcd.Draw_Rectangle(x + r, y + r, x + size_x - r, y + size_y - r);
  my_lcd.Set_Draw_color(WHITE);
  my_lcd.Fill_Rectangle(x + 3, y + 3, x + size_x - 3, y + size_y - 3);

  my_lcd.Set_Draw_color(BLACK);
  tmp1 = show_string_size_width(strlen(dialog_set_string), 2);
  posx1 = (size_x - tmp1) / 2;
  posy1 = (size_y - show_string_size_hight(2)) / 2;
  show_string(dialog_set_string, x + posx1 , y + posy1 - 1 , 2, BLACK, WHITE, 0);
}
//////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
/*
   dynamicke posouvani v menu
*/
void display_element_vertical_slider(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint8_t args1, uint8_t args2)
{
  uint8_t sy;
  my_lcd.Set_Draw_color(BLACK);
  for (uint8_t r = 0; r < 3; r++)
    my_lcd.Draw_Rectangle(x + r, y + r, x + size_x - r, y + size_y - r);
  my_lcd.Set_Draw_color(WHITE);
  my_lcd.Fill_Rectangle(x + 3, y + 3, x + size_x - 3, y + size_y - 3);
  sy = (size_y / (menu_slider_data_max - menu_slider_data_max_element + 1)) * menu_slider_data_current[args1];
  my_lcd.Set_Draw_color(GRAY);
  my_lcd.Fill_Rectangle(x + 3, y + sy + 5, x + size_x - 3, y + sy + 10);
}
///
void display_function_vertical_slider_inc(uint8_t args1, uint8_t args2)
{
  if (menu_slider_data_current[args1] < (menu_slider_data_max - menu_slider_data_max_element))
    menu_slider_data_current[args1]++;
}
///
void display_function_vertical_slider_dec(uint8_t args1, uint8_t args2)
{
  if (menu_slider_data_current[args1] > menu_slider_data_min)
    menu_slider_data_current[args1]--;
}
///
void process_display_element_slider(uint8_t *start, uint8_t *stop, uint8_t args, uint8_t max_element)
{
  if (max_element > 0)
  {
    menu_slider_data_max = *stop;
    menu_slider_data_min = *start;
    menu_slider_data_max_element = max_element;
    ///
    *start = menu_slider_data_current[args];
    if (*start + max_element > menu_slider_data_max && menu_slider_data_max - menu_slider_data_min > menu_slider_data_max_element)
      *start = menu_slider_data_max - max_element;
    if (*start > menu_slider_data_max)
      *start = menu_slider_data_max;
    ///
    *stop = *start + max_element;
    if (menu_slider_data_max <= max_element)
      *stop = menu_slider_data_max;
  }
}
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/// typ tlacitka 1 ////
void button_click_1(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint8_t font_size_1, uint8_t font_size_2, char *line1, char *line2, uint8_t clean)
{
  uint16_t posx1 = 0;
  uint16_t posy1 = 0;;
  uint16_t posx2 = 0;
  uint16_t posy2 = 0;
  uint16_t tmp1 = 0;
  uint16_t tmp2 = 0;
  uint8_t num_lines_const = 0;
  my_lcd.Set_Draw_color(BLACK);
  for (uint8_t r = 0; r < 3; r++)
    my_lcd.Draw_Rectangle(x + r, y + r, x + size_x - r, y + size_y - r);
  if (clean == 1)
  {
    my_lcd.Set_Draw_color(WHITE);
    my_lcd.Fill_Rectangle(x + 3, y + 3, x + size_x - 3, y + size_y - 3);
  }
  if (strlen(line1) > 0)
  {
    num_lines_const = 2;
    tmp1 = show_string_size_width(strlen(line1), font_size_1);
    posx1 = (size_x - tmp1) / 2;
    posy1 = (size_y - show_string_size_hight(font_size_1)) / num_lines_const;
    show_string(line1, x + posx1 , y + posy1 - 1 , font_size_1, BLACK, WHITE, 0);
  }
  if (strlen(line2) > 0)
  {
    num_lines_const = num_lines_const + 2;
    tmp2 = show_string_size_width(strlen(line2), font_size_2);
    posx2 = (size_x - tmp2) / 2;
    posy2 = posy1 + ((size_y - show_string_size_hight(font_size_2)) / num_lines_const);
    show_string(line2, x + posx2 , y + posy2 - 1 , font_size_2, BLACK, WHITE, 0);
  }
}
///////////////////////
/// typ tlacitka 2 ////
void button_click_2(uint16_t x, uint16_t y, uint8_t size_x, uint8_t size_y, uint8_t font_size_1, uint8_t font_size_2, uint16_t color_active , uint16_t color_inactive, uint8_t state, char *line1, char *line2)
{

  uint16_t back_color;
  uint16_t posx1 = 0;
  uint16_t posy1 = 0;;
  uint16_t posx2 = 0;
  uint16_t posy2 = 0;
  uint16_t tmp1 = 0;
  uint16_t tmp2 = 0;
  uint8_t num_lines_const = 0;
  back_color = color_inactive;
  if (state == 1)
    back_color = color_active;
  my_lcd.Set_Draw_color(BLACK);
  for (uint8_t r = 0; r < 3; r++)
    my_lcd.Draw_Rectangle(x + r, y + r, x + size_x - r, y + size_y - r);


  my_lcd.Set_Draw_color(back_color);
  my_lcd.Fill_Rectangle(x + 3, y + 3, x + size_x - 3, y + size_y - 3);

  if (strlen(line1) > 0)
  {
    num_lines_const = 2;
    tmp1 = show_string_size_width(strlen(line1), font_size_1);
    posx1 = (size_x - tmp1) / 2;
    posy1 = (size_y - show_string_size_hight(font_size_1)) / num_lines_const;
    show_string(line1, x + posx1 , y + posy1 - 1 , font_size_1, BLACK, back_color, 0);
  }
  if (strlen(line2) > 0)
  {
    num_lines_const = num_lines_const + 2;
    tmp2 = show_string_size_width(strlen(line2), font_size_2);
    posx2 = (size_x - tmp2) / 2;
    posy2 = posy1 + ((size_y - show_string_size_hight(font_size_2)) / num_lines_const);
    show_string(line2, x + posx2 , y + posy2 - 1 , font_size_2, BLACK, back_color, 0);
  }
}
/////////////////////
/// typ button_1 ////
void button_switch_1(uint16_t x, uint16_t y, uint8_t size_x, uint8_t size_y, uint8_t font_size, uint8_t state, char *text, char *text_value)
{
  uint8_t posx , posy, tmpy, tmpx;
  my_lcd.Set_Draw_color(BLACK);
  for (uint8_t r = 0; r < 3; r++)
    my_lcd.Draw_Rectangle(x + r, y + r, x + size_x - r, y + size_y - r);
  tmpy = show_string_size_hight(font_size);
  tmpx = show_string_size_width(strlen(text), font_size);
  posx = (size_x - tmpx) / 2;
  posy = (size_y - tmpy) / 4;
  show_string(text, x + posx, y + posy, font_size, BLACK, WHITE, 0);
  tmpy = show_string_size_hight(1);
  tmpx = show_string_size_width(strlen(text_value), 1);
  posx = (size_x - tmpx) / 2;
  posy = (size_y - tmpy) / 4;
  show_string(text_value, x + posx, y + (posy * 3), 1, BLACK, WHITE, 0);
}
/////////////////////
/// typ_button_3 ////
void button_click_3(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint8_t char_size, char znak)
{
  uint8_t posx , posy, tmpy, tmpx;
  char str1[3];
  my_lcd.Set_Draw_color(BLACK);
  for (uint8_t r = 0; r < 3; r++)
    my_lcd.Draw_Rectangle(x + r, y + r, x + size_x - r, y + size_y - r);
  str1[0] = znak;
  str1[1] = 0;
  tmpy = show_string_size_hight(char_size);
  tmpx = show_string_size_width(strlen(str1), char_size);
  posx = (size_x - tmpx) / 2;
  posy = (size_y - tmpy) / 4;
  show_string(str1, x + posx, y + posy, char_size, BLACK, WHITE, 0);
}
/////
bool button_click_touch(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint16_t click_x, uint16_t click_y)
{
  bool ret = false;
  if (is_pressed(x, y, x + size_x, y + size_y, click_x, click_y) == true)
  {
    ret = true;
  }
  return ret;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
   funkce pro reseni rtds tlacitek
*/

/// funkce vraci pocet obsazenych rtds snimacu
uint8_t get_function_rtds_last_index_for_menu(uint8_t args1, uint8_t args2)
{
  return MAX_RTDS;
  //return use_rtds;
}

uint8_t get_function_rtds_active_items(uint8_t args1, uint8_t args2)
{
  return use_rtds;
}

/*
  //args1 - args
  //args2 - index polozky menu
  uint8_t get_function_active_rtds_button(uint8_t args1, uint8_t args2)
  {
  uint8_t active;
  remote_tds_get_active(args2, &active);
  return active;
  }
*/

void get_function_rtds_text_button(uint8_t args1, uint8_t args2, char *line1, char *line2)
{
  uint8_t active;
  strcpy_P(line1, text_not_used);
  //sprintf(line2, "%d %d", args1, args2);
  line2[0] = 0;
  remote_tds_get_active(args1, &active);
  if (active == 1)
  {
    remote_tds_get_complete(args1, &active, line1);
  }
}

/*
   args1 --- atributy z nastaveni tlacitka
   args2 --- index polozky z menu
*/
void click_rtds_deassociate_onewire(uint8_t args1, uint8_t args2)
{
  //printf("mazu rtds args1:%d args2:%d\n", args1, args2);
  remote_tds_clear(args2);
  MenuHistoryPrevMenu();
}
////
void click_rtds_subscribe(uint8_t args1, uint8_t idx)
{
  //printf("prihlasuji %d %d\n", args1, idx);
  remote_tds_subscibe_topic(idx);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
/*
   funkce pro definici dynamickych tlacitek onewire
*/

/*
   funkce ktera vraci stav nalezenych TDS cidel
   args1 parametr ... definice tlacitka
   args2 parametr ... definece z globalniho nastaveni menu, zde v nasem pripade je to index 1w cidla na sbernici
 * *line1 - vraci ROM adresu
 * *line2 - vraci akci associovat/nastavit
*/
void get_function_one_wire_associate_or_setting_text_button(uint8_t args1, uint8_t args2 , char *line1, char *line2)
{
  char str2[26];
  strcpy_P(line1, text_not_used);
  strcpy(line2, "-");
  if (w_rom[args1].used == 1)
  {
    createString(str2, ':', w_rom[args1].rom, 8, 16);
    sprintf(line1, "%d: %s", args1, str2);

    if (w_rom[args1].tds_idx != 255)
    {
      strcpy_P(line2, text_nastavit_tds);
      tds_get_name(w_rom[args1].tds_idx, str2);
      strcat(line2, " ");
      strcat(line2, str2);
    }
    else
      strcpy_P(line2, text_associovat_tds);
  }
}

/// funkce, ktera vraci pocet dynamickych polozek 1w
uint8_t get_function_one_wire_last_index_for_menu(uint8_t idx)
{
  uint8_t active = 0;
  for (uint8_t cnt = 0; cnt < HW_ONEWIRE_MAXROMS; cnt++)
    if (w_rom[cnt].used == 1) active++;
  return active;
}
/// funkce. ktera
void click_tds_associate_or_setting_onewire(uint8_t args1, uint8_t idx)
{
  //printf("%d %d\n", args1, idx);
  if (w_rom[idx].tds_idx == 255)
  {
    MenuHistoryNextMenu(MENU_DIALOG_YES_NO, 0);
    dialog_yes_function = &tds_associate;
    dialog_yes_args1 = idx;
    strcpy_P(dialog_text, text_associovat_tds);
  }
  else
  {
    MenuHistoryNextMenu(MENU_NASTAVENI_TDS, w_rom[idx].tds_idx);
  }
}
/// zruseni associace
void click_tds_deassociate_onewire(uint8_t args1, uint8_t idx)
{

  tds_set_clear_wrom_id(idx);
  MenuHistoryPrevMenu();
}
//////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////
//// vraci pocet pouzitych vzdalenych mqtt tds cidel
uint8_t count_use_rtds(void)
{
  uint8_t cnt = 0;
  uint8_t active = 0;
  for (uint8_t idx = 0; idx < MAX_RTDS; idx++)
  {
    remote_tds_get_active(idx, &active);
    if (active == 1) cnt++;
  }
  return cnt;
}

void click_rtds_add_sensor(uint8_t args1, uint8_t args)
{
  char rtds_topic[5];
  uint8_t idx;
  idx = remote_tds_find_free();
  if (idx != 255)
  {
    strcpy_P(rtds_topic, text_rtds_prefix);
    MenuHistoryNextMenu(MENU_DIALOG_KEYBOARD_ALFA, 0);
    display_element_set_string(rtds_topic, RTDS_DEVICE_STRING_LEN, idx);
    dialog_save_variable_function = &menu_rtds_create_name;
  }

}


void menu_rtds_create_name(uint8_t args1, uint8_t args2)
{
  char name[RTDS_DEVICE_STRING_LEN];
  uint8_t active = 1;
  uint8_t idx = display_element_get_string_args();
  display_element_get_string(name);
  //printf("create %s, %d\n", name, idx);
  if (strlen(name) > 0)
    if (remote_tds_name_exist(name) == 255)
    {
      remote_tds_set_complete(idx, active, name);
      remote_tds_subscibe_topic(idx);
    }
}

void menu_rtds_update_name(uint8_t args1, uint8_t args2)
{
  char name[RTDS_DEVICE_STRING_LEN];
  uint8_t idx = display_element_get_string_args();
  display_element_get_string(name);
  //printf("update %s, %d\n", name, idx);
  if (strlen(name) > 0)
  {
    if (remote_tds_name_exist(name) == 255)
      remote_tds_set_name(idx, name);
  }
}

/*
    args1 - parametr z globalniho nastaveni tlacitek
    loop_idx - polozka z menu
*/
void click_rtds_setting_sensor(uint8_t args1, uint8_t loop_idx)
{
  char name[RTDS_DEVICE_STRING_LEN];
  uint8_t active;
  remote_tds_get_active(loop_idx, &active);
  if (active == 1)
  {
    MenuHistoryNextMenu(MENU_NASTAVENI_RTDS_DETAIL, loop_idx);
  }
}
//////////////////////////////
void click_select_default_temp(uint8_t args1, uint8_t args2)
{

}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// funkce ktera vraci hodnoty pro zobrazeni, vrazi nazev, teplotu, navratova hodnota, zda jsou cisla aktualne platna
uint8_t get_global_temp(uint8_t device, char*name, float *temp)
{
  struct_DDS18s20 tds;
  uint8_t cri = 0;
  uint8_t ret = 0;
  uint8_t active;
  strcpy_P(name, text_err);
  *temp = 0.0;
  /// cast pro tds cidla
  for (uint8_t idx = 0; idx < HW_ONEWIRE_MAXROMS; idx++)
    if (get_tds18s20(idx, &tds) == 1)
      if (tds.used == 1)
      {
        if (cri == device)
        {
          strcpy(name, tds.name);
          if (status_tds18s20[idx].online == True)
          {
            *temp = status_tds18s20[idx].temp / 1000.0;
            ret = 1;
            goto get_global_temp_end;
          }
        }
        cri++;
      }
  /// cast pro rtds cidla
  for (uint8_t idx = 0; idx < MAX_RTDS; idx++)
  {
    remote_tds_get_active(idx, &active);
    if (active == 1)
    {
      if (cri == device)
      {
        remote_tds_get_complete(idx, &active, name);
        if (remote_tds_get_last_update(idx) < 250)
        {
          *temp = remote_tds_get_data(idx) / 1000.0;
          ret = 1;
          goto get_global_temp_end;
        }
      }
      cri++;
    }
  }
  /// goto skok
get_global_temp_end:
  return ret;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// funkce, ktera vraci zpet popisek k tlacitkum
void get_function_default_temp(uint8_t args1, uint8_t args2, char *line1, char *line2)
{
  char name[20];
  uint8_t cri = 0;
  uint8_t active;
  strcpy(line1, "");
  strcpy(line2, "");
  /// pro tds cidla
  for (uint8_t idx = 0; idx < HW_ONEWIRE_MAXROMS; idx++)
  {
    if (tds_used(idx) == 1)
    {
      if (cri == args1)
      {
        tds_get_name(idx, name);
        strcpy(line2, name);
        strcpy_P(line1, nastaveni_onewire);
        goto get_function_default_temp_end;
      }
      cri++;
    }
  }
  /// pro rtds
  for (uint8_t idx = 0; idx < MAX_RTDS; idx++)
  {
    remote_tds_get_active(idx, &active);
    if (active == 1)
    {
      if (cri == args1)
      {
        remote_tds_get_complete(idx, &active, name);
        strcpy(line2, name);
        strcpy_P(line1, nastaveni_rtds);
        goto get_function_default_temp_end;
      }
      cri++;
    }
  }
get_function_default_temp_end:
  /// hack kvuli goto
  cri = 0;
}
/////

//////////////////////////////////////////////////////////////////////////////
/// funkce, ktera nastavi promenou vychozi teplomer
/// funkce je povesena na klik tlacitka
void click_function_default_temp(uint8_t args1, uint8_t args2)
{
  default_show_temp = args2;
  set_default_show_temp(args2);
}
//////////////////////////////////////////////////////////////////////////////
/// funkce ktera vraci soucet vsech cidel v systemu
uint8_t get_function_default_temp_max_items(uint8_t args1, uint8_t args2)
{
  return use_tds + use_rtds;
}


uint8_t get_function_default_temp_active(uint8_t args1, uint8_t args2, uint8_t args3)
{
  uint8_t ret = 0;
  if (default_show_temp == args3)
    ret = 1;

  return ret;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



void get_funtion_default_ring(uint8_t args1, uint8_t args2, char *line1, char *line2)
{
  sprintf(line2, "Vychozi ring %d", default_ring);
  sprintf(line1, "Zmenit vychozi ring");
}

void get_funtion_default_ring_temp_mode(uint8_t args1, uint8_t args2, char *line1, char *line2)
{
  sprintf(line1, "Aktualni mod termostatu");
  convert_mode_text_1(thermostat_ring_get_mode(default_ring), line2);
}

/*
   funkce pro zjisteni, zda mode v args je aktivni a nebo ne
   return
     - uint8_t 1... activni, 0... neaktivni
*/
uint8_t get_function_thermostat_has_mode(uint8_t args)
{
  uint8_t ret = 0;
  if (thermostat_ring_get_active(default_ring) != 255)
    if (thermostat_ring_get_mode_1(default_ring, args) == true)
      ret = 1;
  return ret;
}
void click_thermostat_set_mode(uint8_t args)
{
  if (thermostat_ring_get_active(default_ring) != 255)
  {
    thermostat_ring_set_mode(default_ring, args);
    change_term_mode = 1;
  }
}


uint8_t get_function_thermostat_heat_or_cool(uint8_t args)
{
  uint8_t ret = 0;
  uint8_t b;
  if (thermostat_ring_get_active(default_ring) != 255)
  {
    b = thermostat_ring_get_status_bites(default_ring, STATUS_BIT_HEAT_OR_COOL);
    if (b == 0 && args == TERM_MODE_MAN_HEAT)
      ret = 1;
    if (b != 0 && args == TERM_MODE_MAN_COOL)
      ret = 1;
  }
  return ret;
}

void set_function_thermostat_heat_or_cool(uint8_t args)
{
  if (thermostat_ring_get_active(default_ring) != 255)
  {
    if (args == TERM_MODE_MAN_HEAT)
      thermostat_ring_update_bites(default_ring, STATUS_BIT_HEAT_OR_COOL, 0);
    if (args == TERM_MODE_MAN_COOL)
      thermostat_ring_update_bites(default_ring, STATUS_BIT_HEAT_OR_COOL, 1);
  }
}

uint8_t display_enable_show_term_mode_man(uint16_t args1, uint16_t args2, uint8_t args3)
{
  uint8_t mode = thermostat_ring_get_mode(default_ring);
  if (mode == TERM_MODE_MAN_HEAT || mode == TERM_MODE_MAN_COOL || mode == TERM_MODE_MAN)
    return 1;

  return 0;
}

void click_thermostat_set_mode_via_dialog(uint8_t args1, uint8_t args2)
{
  MenuHistoryNextMenu(MENU_DIALOG_SELECT_TERM_MODE, default_ring);
}

uint8_t get_function_thermostat_mode_max_items(uint8_t args1, uint8_t args2, args3)
{
  return 5;
}


/*



       te = remote_tds_get_data(args2) / 1000.0;
    dtostrf(te, 4, 2, str1);
    strcat(str1, "C");
    strcpy_P(str2, current_temp_short);
    strcat(str2, str1);
    show_string(str2, x + 5, y + 60 , 2, BLACK, WHITE, 0);
    /// posledni aktualizace
    last_update = remote_tds_get_last_update(args2);
    strcpy_P(str2, text_last_update);
    sprintf(str1, "%s: %d", str2, last_update);
    show_string(str1, x + 5, y + 80 , 1, BLACK, WHITE, 0);
    active = 1;
    if (last_update >= 250) active = 0;
    strcpy_P(str2, text_online);
    sprintf(str1, "%s: %d", str2, active);
    show_string(str1, x + 5, y + 100 , 2, BLACK, WHITE, 0);


  for (uint8_t idx = 0; idx < HW_ONEWIRE_MAXROMS; idx++)
  {
    if (tds_used(idx) == 1)
    {
      if (cri == args1)
      {
        tds_get_name(idx, name);
        strcpy(line2, name);
        strcpy_P(line1, nastaveni_onewire);
        goto get_function_default_temp_end;
      }
      cri++;
    }
  }
  ///
  for (uint8_t idx = 0; idx < MAX_RTDS; idx++)
  {
    remote_tds_get_active(idx, &active);
    if (active == 1)
    {
      if (cri == args1)
      {
        remote_tds_get_complete(idx, &active, name);
        strcpy(line2, name);
        strcpy_P(line1, nastaveni_rtds);
        goto get_function_default_temp_end;
      }
      cri++;
    }
  }
*/
