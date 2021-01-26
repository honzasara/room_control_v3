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


#define bootloader_tag 0
#define time_offset 1
#define set_default_values 90
#define my_default_ring 92

#define remote_tds_name0  1450
#define remote_tds_name1  1460
#define remote_tds_name2  1470
#define remote_tds_name3  1480
#define remote_tds_name4  1490



#define light_output_0 1516
#define light_output_7 1523


#define SELFTEST_ERR_RTC 0
#define SELFTEST_ERR_NTP 1
#define SELFTEST_ERR_ONEWIRE 2
#define SELFTEST_NRF_SCAN 4
#define SELFTEST_MQTT_LINK 5
#define SELFTEST_ETH_LINK 6
#define SELFTEST_RESTART_NEEDED 7

#define TDS_MEMORY_MAP_TDS 0
#define TDS_MEMORY_MAP_RTDS 16
#define MAX_RTDS 5

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

long int last_menu_redraw = 0;

uint8_t default_ring = 0;

unsigned long load = 0;
unsigned long load_max = 0;
unsigned long load_min = 0xffffffff;

uint16_t light_min = 0;
uint16_t light_max = 0;
uint8_t auto_jas = 0;
uint8_t watchdog_state = 0;

/*
  uint8_t screen_refresh_05s = 0;
  uint8_t screen_refresh_1s = 0;
  uint8_t screen_redraw_all = 0;
*/



char dialog_text[64];

///////////////





uint8_t MenuArgs1[MENU_MAX_HISTORY];
uint8_t MenuHistory[MENU_MAX_HISTORY];
uint8_t MenuHistoryIndex = 0;



void nullfce(void);
void nullfceargs(uint16_t args1, uint16_t args2);
void display_element_show_time_1(uint16_t x, uint16_t y);
void display_element_show_temp_1(uint16_t x, uint16_t y);
void display_element_show_date_1(uint16_t x, uint16_t y);
uint8_t thermostat_default_ring_has_mode(uint8_t args);
uint8_t thermostat_default_ring_set_mode(uint8_t args);
void MenuHistoryNextMenu(uint8_t id);
void MenuHistoryPrevMenu(void);
void button_switch_1(uint16_t x, uint16_t y, uint8_t size_x, uint8_t size_y, uint8_t font_size, uint8_t state, char *text, char *text_value);


uint8_t budik_enabled(uint8_t args)
{
  return 1;
}

void budik_text_state(uint8_t args, char *text)
{
  strcpy(text, "Zapnuto 06:45");
}

void one_wire_associate_text_button(uint8_t idx, char *line1, char *line2)
{
  char str2[26];
  createString(str2, ':', w_rom[idx].rom, 8, 16);
  sprintf(line1, "%d: %s", idx, str2);
  strcpy(line2, "Nastaveni TDS");
}

uint8_t one_wire_last_index_for_menu(uint8_t idx)
{
  uint8_t ret = 0;
  ret = HW_ONEWIRE_MAXROMS;
  return ret;
}


uint8_t button_redraw(uint8_t args)
{
  return 0;
}

void dialog_yes_button_click(uint8_t args)
{

}

void associate_onewire(uint8_t args1, uint8_t idx)
{

  printf("zde: %d, %d\n", args1, idx);
  MenuHistoryNextMenu(MENU_DIALOG_YES_NO);

}

const Element_Dyn_Button_1 onewire_associace_button PROGMEM = {
  .first_x = 30,
  .first_y = 35,
  .size_x = 200,
  .size_y = 55,
  .font_size = 1,
  .step_xy = 60,
  .direction = VERTICAL,
  .max_size = 220,
  .args = INDEX_DYN_MENU_ASSOCIATE_ONEWIRE,
  .get_status_string = one_wire_associate_text_button,
  .dyn_button_onclick =  associate_onewire,
  .function_for_max_items = one_wire_last_index_for_menu,
  .redraw = button_redraw,
};


const Element_Switch_1 switch_budik PROGMEM = {
  .name = budik_text,
  .x = 10,
  .y = 40,
  .size_x = 190,
  .size_y = 120,
  .font_size = 2,
  .color_active = 0,
  .color_inactive = 0,
  .args = SWITCH_BUDIK,
  .onclick = nullfce,
  .get_status_fnt = budik_enabled,
  .get_status_string = budik_text_state,
  .redraw = button_redraw,
};



const Element_Button_2 button_term_state_off PROGMEM = {
  .name = button_term_off,
  .x = 10,
  .y = 220,
  .size_x = 82,
  .size_y = 60,
  .font_size = 3,
  .color_active = GRAY,
  .color_inactive = WHITE,
  .args = TERM_MODE_OFF,
  .onclick = thermostat_default_ring_set_mode,
  .get_status_fnt = thermostat_default_ring_has_mode,
  .redraw = button_redraw,
};

const Element_Button_2 button_term_state_max PROGMEM = {
  .name = button_term_max,
  .x = 100,
  .y = 220,
  .size_x = 82,
  .size_y = 60,
  .font_size = 3,
  .color_active = RED,
  .color_inactive = WHITE,
  .args = TERM_MODE_MAX,
  .onclick = thermostat_default_ring_set_mode,
  .get_status_fnt = thermostat_default_ring_has_mode,
  .redraw = button_redraw,
};

const Element_Button_2 button_term_state_min PROGMEM = {
  .name = button_term_min,
  .x = 190,
  .y = 220,
  .size_x = 82,
  .size_y = 60,
  .font_size = 3,
  .color_active = BLUE,
  .color_inactive = WHITE,
  .args = TERM_MODE_MIN,
  .onclick = thermostat_default_ring_set_mode,
  .get_status_fnt = thermostat_default_ring_has_mode,
  .redraw = button_redraw,
};

const Element_Button_2 button_term_state_prog PROGMEM = {
  .name = button_term_prog,
  .x = 280,
  .y = 220,
  .size_x = 82,
  .size_y = 60,
  .font_size = 3,
  .color_active = YELLOW,
  .color_inactive = WHITE,
  .args = TERM_MODE_PROG,
  .onclick = thermostat_default_ring_set_mode,
  .get_status_fnt = thermostat_default_ring_has_mode,
  .redraw = button_redraw,
};

const Element_Button_2 button_term_state_man PROGMEM = {
  .name = button_term_man,
  .x = 370,
  .y = 220,
  .size_x = 82,
  .size_y = 60,
  .font_size = 3,
  .color_active = GREEN,
  .color_inactive = WHITE,
  .args = TERM_MODE_MAN,
  .onclick = thermostat_default_ring_set_mode,
  .get_status_fnt = thermostat_default_ring_has_mode,
  .redraw = button_redraw,
};

const Element_Button_1 button_nastaveni PROGMEM = {
  .name = nastaveni_text,
  .x = 290,
  .y = 30,
  .size_x = 170,
  .size_y = 40,
  .font_size = 2,
  .args = MENU_NASTAVENI_SCREEN,
  .onclick = MenuHistoryNextMenu,
  .redraw = button_redraw,
};

const Element_Button_1 button_funkce PROGMEM = {
  .name = funkce_text,
  .x = 290,
  .y = 90,
  .size_x = 170,
  .size_y = 40,
  .font_size = 2,
  .args = MENU_FUNKCE_SCREEN,
  .onclick = MenuHistoryNextMenu,
  .redraw = button_redraw,
};

const Element_Button_1 button_regulator PROGMEM = {
  .name = regulator_text,
  .x = 290,
  .y = 150,
  .size_x = 170,
  .size_y = 40,
  .font_size = 2,
  .args = 0,
  .onclick = nullfce,
  .redraw = button_redraw,
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
  .redraw = button_redraw,
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
  .redraw = button_redraw,
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
  .redraw = button_redraw,
};

const Element_Button_1 button_back PROGMEM = {
  .name = button_zpet,
  .x = 10,
  .y = 220,
  .size_x = 190,
  .size_y = 40,
  .font_size = 2,
  .args = 0,
  .onclick = MenuHistoryPrevMenu,
  .redraw = button_redraw,
};

const Element_Button_1 button_nastaveni_onewire PROGMEM = {
  .name = nastaveni_onewire,
  .x = 280,
  .y = 40,
  .size_x = 190,
  .size_y = 40,
  .font_size = 2,
  .args = MENU_NASTAVENI_ONEWIRE,
  .onclick = MenuHistoryNextMenu,
  .redraw = button_redraw,
};

const Element_Button_1 button_nastaveni_rtds PROGMEM = {
  .name = nastaveni_rtds,
  .x = 280,
  .y = 100,
  .size_x = 190,
  .size_y = 40,
  .font_size = 2,
  .args = 0,
  .onclick = nullfce,
  .redraw = button_redraw,
};
/////
const Element_Button_1 dialog_yes PROGMEM = {
  .name = text_yes,
  .x = 100,
  .y = 110,
  .size_x = 100,
  .size_y = 40,
  .font_size = 2,
  .args = 0,
  .onclick = dialog_yes_button_click,
  .redraw = button_redraw,
};

const Element_Button_1 dialog_no PROGMEM = {
  .name = text_no,
  .x = 240,
  .y = 110,
  .size_x = 100,
  .size_y = 40,
  .font_size = 2,
  .args = 0,
  .onclick = MenuHistoryPrevMenu,
  .redraw = button_redraw,
};
/////
/////
const Element_Function_1 f_show_time PROGMEM = {
  .x = 10,
  .y = 30,
  .args = 0,
  .fnt = display_element_show_time_1,
  .redraw = button_redraw,
};

const Element_Function_1 f_show_temp PROGMEM = {
  .x = 10,
  .y = 130,
  .args = 0,
  .fnt = display_element_show_temp_1,
  .redraw = button_redraw,
};

const Element_Function_1 f_show_date PROGMEM = {
  .x = 280,
  .y = 290,
  .args = 0,
  .fnt = display_element_show_date_1,
  .redraw = button_redraw,
};


const Element_Symbol_1 menu_plus PROGMEM =  {
  .znak = '+',
  .x = 350,
  .y = 50,
  .size_x = 40,
  .size_y = 40,
  .args = 0,
  .onclick = nullfce,
  .redraw = button_redraw,
};

const Element_Symbol_1 menu_minus PROGMEM =  {
  .znak = '-',
  .x = 350,
  .y = 250,
  .size_x = 40,
  .size_y = 40,
  .args = 0,
  .onclick = nullfce,
  .redraw = button_redraw,
};

const Menu1 HlavniMenu PROGMEM = {
  .name = term_title,
  .button_1 = {button_regulator, button_funkce, button_nastaveni},
  .button_2 = {button_term_state_off, button_term_state_max, button_term_state_min, button_term_state_prog, button_term_state_man},
  .function_1 = {f_show_temp, f_show_time, f_show_date},
  .switch_1 = {NULL},
  .dyn_button = {NULL},
  .symbol_button_1 = {NULL},
  .len_button_1 = 3,
  .len_button_2 = 5,
  .len_function_1 = 3,
  .len_switch_1 = 0,
  .len_dyn_button_1 = 0,
  .len_symbol_button_1 = 0,
  .idx = MENU_DEFAULT_SCREEN,
  .x = 0,
  .y = 0,
  .size_x = 480,
  .size_y = 320,
  .atributes = (1 << MENU_ATTRIBUTES_CLEAN_DISPLAY),
  .color_background = WHITE,
  .redraw = button_redraw,
};

const Menu1 NastaveniMenu PROGMEM = {
  .name = nastaveni_text,
  .button_1 = {button_nastaveni_site, button_nastaveni_nrf, button_nastaveni_default, button_back, button_nastaveni_rtds, button_nastaveni_onewire},
  .button_2 = {NULL},
  .function_1 = {f_show_date},
  .switch_1 = {NULL},
  .dyn_button = {NULL},
  .symbol_button_1 = {NULL},
  .len_button_1 = 6,
  .len_button_2 = 0,
  .len_function_1 = 1,
  .len_switch_1 = 0,
  .len_dyn_button_1 = 0,
  .len_symbol_button_1 = 0,
  .idx = MENU_NASTAVENI_SCREEN,
  .x = 0,
  .y = 0,
  .size_x = 480,
  .size_y = 320,
  .atributes = (1 << MENU_ATTRIBUTES_CLEAN_DISPLAY),
  .color_background = WHITE,
  .redraw = button_redraw,
};

const Menu1 FunkceMenu PROGMEM = {
  .name = funkce_text,
  .button_1 = {button_back},
  .button_2 = {NULL},
  .function_1 = {f_show_date},
  .switch_1 = {switch_budik},
  .dyn_button = {NULL},
  .symbol_button_1 = {NULL},
  .len_button_1 = 1,
  .len_button_2 = 0,
  .len_function_1 = 1,
  .len_switch_1 = 1,
  .len_dyn_button_1 = 0,
  .len_symbol_button_1 = 0,
  .idx = MENU_FUNKCE_SCREEN,
  .x = 0,
  .y = 0,
  .size_x = 480,
  .size_y = 320,
  .atributes = (1 << MENU_ATTRIBUTES_CLEAN_DISPLAY),
  .color_background = WHITE,
  .redraw = button_redraw,
};

const Menu1 OneWireMenu PROGMEM = {
  .name = nastaveni_onewire,
  .button_1 = {button_back},
  .button_2 = {NULL},
  .function_1 = {f_show_date},
  .switch_1 = {NULL},
  .dyn_button = {onewire_associace_button},
  .symbol_button_1 = {menu_plus, menu_minus},
  .len_button_1 = 1,
  .len_button_2 = 0,
  .len_function_1 = 1,
  .len_switch_1 = 0,
  .len_dyn_button_1 = 1,
  .len_symbol_button_1 = 2,
  .idx = MENU_NASTAVENI_ONEWIRE,
  .x = 0,
  .y = 0,
  .size_x = 480,
  .size_y = 320,
  .atributes = (1 << MENU_ATTRIBUTES_CLEAN_DISPLAY),
  .color_background = WHITE,
  .redraw = button_redraw,
};











const Menu1 DialogYESNO PROGMEM = {
  .name = text_upozorneni,
  .button_1 = {dialog_yes, dialog_no},
  .button_2 = {NULL},
  .function_1 = {NULL},
  .switch_1 = {NULL},
  .dyn_button = {NULL},
  .symbol_button_1 = {NULL},
  .len_button_1 = 2,
  .len_button_2 = 0,
  .len_function_1 = 0,
  .len_switch_1 = 0,
  .len_dyn_button_1 = 0,
  .len_symbol_button_1 = 0,
  .idx = MENU_DIALOG_YES_NO,
  .x = 50,
  .y = 50,
  .size_x = 380,
  .size_y = 140,
  .atributes = (1 << MENU_ATTRIBUTES_FILL_COLOR_RECTANGLE | 1 << MENU_ATTRIBUTES_DECORATE_MENU),
  .color_background = YELLOW,
  .redraw = button_redraw,
};

const MenuAll Menu_All PROGMEM = {
  .len_menu1 = 5,
  .ListMenu1 = {HlavniMenu, NastaveniMenu, FunkceMenu, OneWireMenu, DialogYESNO},
};













void nullfce(void) {};

void nullfceargs(uint16_t args1, uint16_t args2) {};



















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
  Element_Symbol_1 symbol_button_1;
  fptr *fnt;
  ret_fptr *rfnt;
  fptr_args *fntargs;
  fptr_xy *fnt_xy;
  ret_string_fptr *gss;
  Menu1 *current;
  uint16_t click_x, click_y;
  bool state;
  uint8_t loop_cnt, loop_i;
  uint8_t index_items;
  uint16_t new_x;
  uint16_t new_y;
  uint8_t rdr = 0;
  uint8_t args;

  current = MenuHistoryGetMenu();

  click_x = 0;
  click_y = 0;

  my_touch.TP_Scan(0);
  if (my_touch.TP_Get_State()&TP_PRES_DOWN && redraw == false)
  {
    click_x = my_touch.x;
    click_y = my_touch.y;
  }

  rfnt = pgm_read_word(&current->redraw);
  rdr = (ret_fptr(rfnt))(pgm_read_word(&current->idx));
  if ( rdr == 1 || redraw == true )
  {
    strcpy_P(str1, pgm_read_word(&current->name));
    show_string(str1, pgm_read_word(&current->x) + 5, pgm_read_word(&current->y) + 5, 2, BLACK, WHITE, 0);
  }

  /// tlacitko typ 1 se prekresluje s celym menu
  /// tlacitko typ 1
  for (uint8_t idx = 0; idx < pgm_read_byte(&current->len_button_1); idx++)
  {
    button_1 = &current->button_1[idx];
    rfnt = pgm_read_word(&button_1->redraw);
    rdr = (ret_fptr(rfnt))(pgm_read_word(&button_1->args));
    if ( rdr == 1 || redraw == true )
    {
      strcpy_P(str2, pgm_read_word(&button_1->name));
      str1[0] = 0;
      button_click_1(pgm_read_word(&button_1->x), pgm_read_word(&button_1->y), pgm_read_word(&button_1->size_x), pgm_read_word(&button_1->size_y), pgm_read_byte(&button_1->font_size), pgm_read_byte(&dyn_button_1->font_size), str2, str1 );
    }
    if (click_x > 0 && click_y > 0)
      if (button_click_touch(pgm_read_word(&button_1->x), pgm_read_word(&button_1->y), pgm_read_word(&button_1->size_x), pgm_read_word(&button_1->size_y), click_x, click_y) == true)
      {
        fnt =  pgm_read_word(&button_1->onclick);
        ((fptr)fnt)(pgm_read_byte(&button_1->args));
        ret = true;
      }
  }

  if (ret == true)  goto draw_menu_end;
  /// tlacitko typ 2
  for (uint8_t idx = 0; idx < pgm_read_byte(&current->len_button_2); idx++)
  {
    button_2 = &current->button_2[idx];
    rfnt = pgm_read_word(&button_2->redraw);
    rdr = (ret_fptr(rfnt))(pgm_read_word(&button_2->args));
    if ( rdr == 1 || redraw == true)
    {
      strcpy_P(str2, pgm_read_word(&button_2->name));
      rfnt = pgm_read_word(&button_2->get_status_fnt);
      state = ((ret_fptr)rfnt)(pgm_read_byte(&button_2->args));
      button_click_2(pgm_read_word(&button_2->x), \
                     pgm_read_word(&button_2->y), pgm_read_word(&button_2->size_x), \
                     pgm_read_word(&button_2->size_y), pgm_read_byte(&button_2->font_size), \
                     pgm_read_word(&button_2->color_active), \
                     pgm_read_word(&button_2->color_inactive), state,  str2);
    }
    if (click_x > 0 && click_y > 0)
      if (button_click_touch(pgm_read_word(&button_2->x), pgm_read_word(&button_2->y), pgm_read_word(&button_2->size_x), pgm_read_word(&button_2->size_y), click_x, click_y) == true)
      {
        fnt =  pgm_read_word(&button_2->onclick);
        ((fptr)fnt)(pgm_read_byte(&button_2->args));
        ret = true;
      }
  }
  if (ret == true)  goto draw_menu_end;
  /// menu programy
  for (uint8_t idx = 0; idx < pgm_read_byte(&current->len_function_1); idx++)
  {
    function_1 = &current->function_1[idx];
    rfnt = pgm_read_word(&function_1->redraw);
    rdr = (ret_fptr(rfnt))(pgm_read_word(&function_1->args));
    if (rdr == 1 || redraw == true)
    {
      fnt_xy =  pgm_read_dword(&function_1->fnt);
      ((fptr_xy)fnt_xy)(pgm_read_word(&function_1->x), pgm_read_word(&function_1->y));
    }
  }

  if (ret == true)  goto draw_menu_end;
  /// menu switch_1
  for (uint8_t idx = 0; idx < pgm_read_byte(&current->len_switch_1); idx++)
  {
    switch_1 = &current->switch_1[idx];
    rfnt = pgm_read_word(&switch_1->redraw);
    rdr = (ret_fptr(rfnt))(pgm_read_word(&switch_1->args));
    if (rdr == 1 || redraw == true)
    {
      strcpy_P(str2, pgm_read_word(&switch_1->name));
      rfnt = pgm_read_word(&switch_1->get_status_fnt);
      gss = pgm_read_word(&switch_1->get_status_string);
      state = ((ret_fptr)rfnt)(pgm_read_byte(&switch_1->args));
      str3[0] = 0;
      ((ret_string_fptr)gss)(pgm_read_byte(&switch_1->args), str1, str3);
      button_switch_1(pgm_read_word(&switch_1->x), \
                      pgm_read_word(&switch_1->y), \
                      pgm_read_word(&switch_1->size_x), \
                      pgm_read_word(&switch_1->size_y), \
                      pgm_read_word(&switch_1->font_size), state, str2, str1);

    }
    if (click_x > 0 && click_y > 0)
      if (button_click_touch(pgm_read_word(&switch_1->x), pgm_read_word(&switch_1->y), pgm_read_word(&switch_1->size_x), pgm_read_word(&switch_1->size_y), click_x, click_y) == true)
      {
        fnt =  pgm_read_word(&switch_1->onclick);
        ((fptr)fnt)(pgm_read_byte(&switch_1->args));
        ret = true;
      }
  }
  if (ret == true)  goto draw_menu_end;
  for (uint8_t idx = 0; idx < pgm_read_byte(&current->len_symbol_button_1); idx++)
  {
  }
  
  for (uint8_t idx = 0; idx < pgm_read_byte(&current->len_dyn_button_1); idx++)
  {
    dyn_button_1 = &current->dyn_button[idx];
    rfnt = pgm_read_word(&dyn_button_1->redraw);
    rdr = (ret_fptr(rfnt))(pgm_read_word(&dyn_button_1->args));
    if (rdr == 1 || redraw == true)
    {
      loop_i = 0;
      rfnt =  pgm_read_word(&dyn_button_1->function_for_max_items);
      loop_cnt = ((ret_fptr)rfnt)(pgm_read_word(&dyn_button_1->args));
      gss = pgm_read_word(&dyn_button_1->get_status_string);

      roller_function(&loop_i, &loop_cnt);
      while (loop_i < loop_cnt)
      {
        new_x = pgm_read_word(&dyn_button_1->first_x);
        new_y = pgm_read_word(&dyn_button_1->first_y);
        if (pgm_read_byte(&dyn_button_1->direction) == VERTICAL)
          new_y = pgm_read_word(&dyn_button_1->first_y) + (pgm_read_byte(&dyn_button_1->step_xy) * loop_i);
        if (pgm_read_byte(&dyn_button_1->direction) == HORIZONTAL)
          new_x = pgm_read_word(&dyn_button_1->first_x) + (pgm_read_byte(&dyn_button_1->step_xy) * loop_i);

        ((ret_string_fptr)gss)(loop_i, str1, str2);
        button_click_1(new_x, new_y , pgm_read_word(&dyn_button_1->size_x), pgm_read_word(&dyn_button_1->size_y), pgm_read_byte(&dyn_button_1->font_size), pgm_read_byte(&dyn_button_1->font_size), str1, str2);
        loop_i++;
      }
    }

    if (click_x > 0 && click_y > 0)
    {
      loop_i = 0;
      rfnt =  pgm_read_word(&dyn_button_1->function_for_max_items);
      loop_cnt = ((ret_fptr)rfnt)(pgm_read_word(&dyn_button_1->args));

      roller_function(&loop_i, &loop_cnt);
      while (loop_i < loop_cnt)
      {
        new_x = pgm_read_word(&dyn_button_1->first_x);
        new_y = pgm_read_word(&dyn_button_1->first_y);
        if (pgm_read_byte(&dyn_button_1->direction) == VERTICAL)
          new_y = pgm_read_word(&dyn_button_1->first_y) + (pgm_read_byte(&dyn_button_1->step_xy) * loop_i);
        if (pgm_read_byte(&dyn_button_1->direction) == HORIZONTAL)
          new_x = pgm_read_word(&dyn_button_1->first_x) + (pgm_read_byte(&dyn_button_1->step_xy) * loop_i);
        if (button_click_touch(new_x, new_y, pgm_read_word(&dyn_button_1->size_x), pgm_read_word(&dyn_button_1->size_y), click_x, click_y) == true)
        {
          fntargs =  pgm_read_word(&dyn_button_1->dyn_button_onclick);
          ((fptr_args)fntargs)(pgm_read_byte(&dyn_button_1->args), loop_i);
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



void roller_function(uint8_t loop_i, uint8_t loop_cnt)
{

}





void MenuPrepareStyle(void)
{
  uint8_t attr;
  uint16_t x;
  uint16_t y;
  uint16_t size_x;
  uint16_t size_y;
  uint16_t color_background;
  uint8_t args;
  Menu1 *current;

  current = MenuHistoryGetMenu();
  attr = pgm_read_byte(&current->atributes);
  x = pgm_read_word(&current->x);
  y = pgm_read_word(&current->y);
  size_x = pgm_read_word(&current->size_x);
  size_y = pgm_read_word(&current->size_y);
  color_background = pgm_read_word(&current->color_background);

  printf("attr %d\n", attr);
  if ((attr & (1 << MENU_ATTRIBUTES_CLEAN_DISPLAY)) != 0)
  {
    DisplayClean(color_background);
    printf("mazu disply\n");
  }

  if ((attr & (1 << MENU_ATTRIBUTES_FILL_COLOR_RECTANGLE)) != 0)
  {
    my_lcd.Set_Draw_color(color_background);
    my_lcd.Fill_Rectangle(x, y, x + size_x, y + size_y);
    printf("vybarvuji ctverec\n");
  }

  if ((attr & (1 << MENU_ATTRIBUTES_DECORATE_MENU)) != 0)
  {
    my_lcd.Set_Draw_color(BLACK);
    for (uint8_t i = 0; i < 4; i++)
    {
      my_lcd.Draw_Rectangle(x + i, y + i, x + size_x - i, y + size_y - i);
      my_lcd.Draw_Fast_HLine(x, y + 20 + i, size_x - i);
    }
    printf("delam dekoraci\n");
  }

}







void MenuHistoryPrevMenu(void)
{

  if (MenuHistoryIndex > 0)
  {
    MenuHistoryIndex--;
    MenuPrepareStyle();

  }
}


void MenuHistoryNextMenu(uint8_t id)
{
  if (MenuHistoryIndex < MENU_MAX_HISTORY)
  {
    MenuHistoryIndex++;
    MenuHistory[MenuHistoryIndex] = id;
    MenuPrepareStyle();
  }
}


void MenuHistoryInit(void)
{
  MenuHistory[MenuHistoryIndex] = MENU_DEFAULT_SCREEN;
}


Menu1 *MenuHistoryGetMenu(void)
{
  Menu1 *menus;
  for (uint8_t idx = 0; idx < pgm_read_byte(&Menu_All.len_menu1); idx++)
  {
    menus = &Menu_All.ListMenu1[idx];
    if (pgm_read_byte(&menus->idx) == MenuHistory[MenuHistoryIndex])
    {
      return menus;
    }
  }

}

/*

  #define MENU_MAX_ITEMS_MENU 10

  typedef struct ItemMenu
  {
  char *name;
  function on_show;
  uint8_t type;
  uint8_t first;
  uint8_t last;
  };

  typedef struct RootMenu
  {
  char *name;
  ItemMenu *Item[MENU_MAX_ITEMS_MENU];
  uint8_t items;
  uint8_t current_item;
  uint8_t args1;
  uint8_t args2;
  uint8_t args3;
  };

  typedef struct Struct_RootHistory
  {
  RootMenu *history[MENU_MAX_HISTORY];
  uint8_t menu_max;
  };
*/


/*
  void test1(void)
  {
  show_string("test1", 200, 200, 3, BLACK, WHITE, 0);
  }
  void test2(void)
  {
  show_string("test2", 200, 240, 3, BLACK, WHITE, 0);
  }
  const fptr tttt[] PROGMEM = {test1, test2};
*/

/*
  #define MENUDIALOG_ARGS_COUNT 8
  struct Struct_MenuDialog
  {
  char text[32];
  char text_ok[8];
  char text_yes[8];
  char text_no[8];
  char text_close[8];
  fptr action_yes;
  fptr action_no;
  uint8_t action_args[MENUDIALOG_ARGS_COUNT];
  } MenuDialog;

*/
/*
  struct Struct_MenuHistory
  {
  uint8_t menu[MENU_MAX_HISTORY];
  uint8_t args1[MENU_MAX_HISTORY];
  uint8_t menu_now;
  } MenuHistory;
*/




/***************************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////////

/*
  void TDS_Associate_And_Switch_Menu_Yes(void)
  {
  uint8_t tmp1[2];
  uint8_t tds_idx = 255;
  MenuDialogGetArgs(tmp1, 2);
  tds_idx = tds_associate(tmp1[0]);
  MenuHistoryUpdateMenu(tmp1[1]);
  MenuHistoryUpdateArgs1(tds_idx);
  DisplayClean();
  }
  void TDS_Associate_And_Switch_Menu_No(void)
  {
  MenuHistorySetPrev();
  DisplayClean();
  }
  ///
  ///
  void TDS_DeAssociate_And_Switch_Menu_Yes(void)
  {
  uint8_t tmp1[2];
  uint8_t tds_idx = 255;
  MenuDialogGetArgs(tmp1, 2);
  tds_set_clear(tmp1[0]);
  MenuHistorySetPrev();
  MenuHistorySetPrev();
  DisplayClean();
  }
  void TDS_DeAssociate_And_Switch_Menu_No(void)
  {
  MenuHistorySetPrev();
  DisplayClean();
  }
*/
/**************************************************************************************************/



////////////////////////////////////////////////////////////////////////////////////////////////////
/*
  bool MenuDialogSetArgs(uint8_t *args, uint8_t len)
  {
  bool ret = false;
  if (len < MENUDIALOG_ARGS_COUNT)
  {
    for (uint8_t idx = 0; idx < len; idx++)
      MenuDialog.action_args[idx] = args[idx];
    ret = true;
  }
  return ret;
  }
  ///
  bool MenuDialogGetArgs(uint8_t *args, uint8_t len)
  {
  bool ret = false;
  if (len < MENUDIALOG_ARGS_COUNT)
  {
    for (uint8_t idx = 0; idx < len; idx++)
      args[idx] = MenuDialog.action_args[idx];
    ret = true;
  }
  return ret;
  }
  ///
  void MenuDialogSetText(char *text)
  {
  strcpy(MenuDialog.text, text);
  }
  void MenuDialogGetText(char *text)
  {
  strcpy(text, MenuDialog.text);
  }
  ///
  void MenuDialogSetTextYes(char *text)
  {
  strcpy(MenuDialog.text_yes, text);
  }
  void MenuDialogGetTextYes(char *text)
  {
  strcpy(text, MenuDialog.text_yes);
  }
  ///
  void MenuDialogSetTextNo(char *text)
  {
  strcpy(MenuDialog.text_no, text);
  }
  void MenuDialogGetTextNo(char *text)
  {
  strcpy(text, MenuDialog.text_no);
  }
  ///
  void MenuDialogSetActionNo(void *action)
  {
  MenuDialog.action_no = action;
  }
  void MenuDialogGetActionNo(void *action)
  {
  action =  MenuDialog.action_no;
  }
  ///
  void MenuDialogSetActionYes(void *action)
  {
  MenuDialog.action_yes = action;
  }
  void MenuDialogGetActionYes(void *action)
  {
  action =  MenuDialog.action_yes;
  }
  ///
  void MenuDialogActionNo()
  {
  MenuDialog.action_no();
  }
  void MenuDialogActionYes()
  {
  MenuDialog.action_yes();
  }
  ///
  ///
  ///
*/
/*****************************************************************************************************/
///////////////////////////////////////////////////////////////////////////////////////////////////////
/*
  void MenuHistoryInit(void)
  {
  for (uint8_t idx = 0; idx < MENU_MAX_HISTORY; idx++)
  {
    MenuHistory.menu[idx] = 255;
    MenuHistory.args1[idx] = 255;
  }
  MenuHistory.menu_now = 0;
  MenuHistory.menu[0] = 0;
  }
  ///
  bool MenuHistorySetNext(int8_t menu_id, uint8_t args1)
  {
  bool ret = False;
  if (MenuHistory.menu_now < MENU_MAX_HISTORY)
  {
    MenuHistory.menu_now++;
    MenuHistory.menu[MenuHistory.menu_now] = menu_id;
    MenuHistory.args1[MenuHistory.menu_now] = args1;
    ret = True;
  }
  return ret;
  }
  bool MenuHistorySetNext(int8_t menu_id)
  {
  return MenuHistorySetNext(menu_id, 255);
  }
  ///
  void MenuHistoryUpdateArgs1(uint8_t args1)
  {
  MenuHistory.args1[MenuHistory.menu_now] = args1;
  }
  void MenuHistoryUpdateMenu(uint8_t menu_id)
  {
  MenuHistory.menu[MenuHistory.menu_now] = menu_id;
  }
  ///
  ///
  ///
  bool MenuHistorySetPrev(void)
  {
  bool ret = false;
  if (MenuHistory.menu_now > 0)
  {
    MenuHistory.menu_now--;
    ret = true;
  }
  return ret;
  }
  bool MenuHistorySetPrev(uint8_t cnt)
  {
  bool ret = false;
  for (uint8_t x = 0; x < cnt; x++)
    ret = ret && MenuHistorySetPrev();
  return ret;
  }
  ///
  ///
  ///
  uint8_t MenuHistoryGetMenu(uint8_t *args1)
  {
   args1 = MenuHistory.args1[MenuHistory.menu_now];
  return MenuHistory.menu[MenuHistory.menu_now];
  }
  uint8_t MenuHistoryGetMenu(void)
  {
  return MenuHistory.menu[MenuHistory.menu_now];
  }
  ///
*/

void DisplayClean(uint16_t color)
{
  my_lcd.Fill_Screen(color);
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
void remote_tds_get_name(uint8_t idx, uint8_t *active, char *name)
{
  char t;
  for (uint8_t i = 0; i < 9; i++)
  {
    t = EEPROM.read(remote_tds_name0 + (10 * idx) + i);
    name[i] = t;
    if (t == 0) break;
  }
  *active = EEPROM.read(remote_tds_name0 + (10 * idx) + 9);
}
///
/// nastavi topic
void remote_tds_set_name(uint8_t idx, uint8_t active , char *name)
{
  char tmp2[64];
  char t;
  for (uint8_t i = 0; i < 9; i++)
  {
    t = name[i];
    EEPROM.write(remote_tds_name0 + (10 * idx) + i, t);
    if (t == 0) break;
  }
  EEPROM.write(remote_tds_name0 + (10 * idx) + 9, active);
}
///
/// je aktivni
void remote_tds_get_active(uint8_t idx, uint8_t *active)
{
  *active = EEPROM.read(remote_tds_name0 + (10 * idx) + 9);
}
///
/// funkce pro nastaveni odebirani topicu vzdalenych cidel
/*
  idx - index nazvu topicu, ktery si chci subscribnout/unsubscribnout
*/
void remote_tds_subscibe_topic(uint8_t idx)
{
  char tmp1[64];
  char tmp2[64];
  uint8_t active = 0;
  remote_tds_get_name(idx, &active, tmp1);
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
  remote_tds_get_name(idx, &active, tmp1);
  if (active == 1)
  {
    strcpy(tmp2, "/rtds/");
    strcat(tmp2, tmp1);
    mqtt_client.unsubscribe(tmp2);
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
/*
   funkce pro zjisteni, zda mode v args je aktivni a nebo ne
   return
     - uint8_t 1... activni, 0... neaktivni
*/
uint8_t thermostat_default_ring_has_mode(uint8_t args)
{
  uint8_t ret = 0;
  if (thermostat_ring_get_mode_1(default_ring, args) == true)
    ret = 1;
  return ret;
}
uint8_t thermostat_default_ring_set_mode(uint8_t args)
{
  thermostat_ring_set_mode(default_ring, args);
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
void mqtt_callback(char* topic, byte* payload, unsigned int length)
{
  char str1[64];
  const char tmp1[16];
  char tmp2[16];
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
  //// thermctl-in/XXXXX/rtds/set/IDX/name - 8 znaku nastavi a udela prihlaseni
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
      if (id < MAX_RTDS)
      {
        if ((cnt == 1) && (strcmp(pch, "name") == 0))
        {
          remote_tds_get_active(id, &active);
          if (active == 0)
          {
            remote_tds_set_name(id, 1, my_payload);
            remote_tds_subscibe_topic(id);
          }
        }
      }
      else
      {
        log_error(&mqtt_client, "rtds/set bad id");
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
    if (id < MAX_RTDS)
    {
      remote_tds_unsubscibe_topic(id);
      remote_tds_set_name(id, 0, "");
    }
    else
    {
      log_error(&mqtt_client, "rtds/clear bad id");
    }
  }
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
      remote_tds_get_name(idx, &active, tmp1);
      if (active == 1 && strcmp(tmp1, str1) == 0)
      {
        //remote_tds[idx] = atoi(my_payload);
        //remote_tds_last_update[idx] = 0;
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
      /*
        //// /rtds/xxxxx
        for (uint8_t idx = 0; idx < 5; idx++)
        remote_tds_subscibe_topic(idx);
      */
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
    /*
      strcpy(str_topic, "status/rtds/count");
      itoa(use_rtds, payload, 10);
      send_mqtt_general_payload(&mqtt_client, str_topic, payload);

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
  char payload[10];
  for (uint8_t idx = 0; idx < MAX_RTDS; idx++)
  {
    remote_tds_get_name(idx, &active, payload);
    /// odeslu pouze pokud je neco aktivni, jinak ne
    if (active == 1)
    {
      send_mqtt_message_prefix_id_topic_payload(&mqtt_client, "rtds", idx, "name", payload);
      itoa(active, payload, 10);
      send_mqtt_message_prefix_id_topic_payload(&mqtt_client, "rtds", idx, "active", payload);
      /// itoa(remote_tds[idx], payload, 10);
      send_mqtt_message_prefix_id_topic_payload(&mqtt_client, "rtds", idx, "temp", payload);
      /// itoa(remote_tds_last_update[idx], payload, 10);
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

        for (uint8_t idx = 0; idx < MAX_THERMOSTAT; idx++)
          thermostat_ring_clear(idx);
        ///
        for (uint8_t idx = 0; idx < MAX_RTDS; idx++)
        {
          strcpy(tmp1, "");
          remote_tds_set_name(idx, 0 , tmp1);
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
        show_string("OK", 210, 50 + (init * 10), 1, GREEN, WHITE, 0 );
      }
      else
      {
        strcpy_P(str1, text_test_ram);
        strcat(str1, " ERR");
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
        strcat(str1, " OK");
        show_string(str1, 30, 50 + (init * 10), 1, GREEN, WHITE, 0 );
      }
      else
      {
        strcpy_P(str1, text_test_eeprom);
        strcat(str1, " ERR");
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
        strcpy(str1, "OK");
        show_string(str1, 160, 50 + (init * 10), 1, GREEN, WHITE, 0 );
        selftest_clear_0(SELFTEST_ERR_ONEWIRE);
      }
      else
      {
        strcpy(str1, "ERROR");
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
        strcpy(str1, "ERROR");
        show_string(str1, 160, 50 + (init * 10), 1, RED, WHITE, 0 );
      }
      else
      {
        strcpy(str1, "OK");
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
    }
    ///
    /// kontrola NTP serveru
    if (init == 14)
    {
      strcpy_P(str1, text_ntp_cas);
      show_string(str1, 30, 50 + (init * 10), 1, GREEN, WHITE, 0 );
      if (ntp_check(&timeClient) == 0)
      {
        strcpy(str1, "ERROR");
        show_string(str1, 160, 50 + (init * 10), 1, RED, WHITE, 0 );
        selftest_set_0(SELFTEST_ERR_NTP);
      }
      else
      {
        strcpy(str1, "OK");
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
        strcpy(str1, "OK");
        show_string(str1, 160, 50 + (init * 10), 1, GREEN, WHITE, 0 );
        selftest_clear_0(SELFTEST_ERR_RTC);
      }
      else
      {
        strcpy(str1, "ERROR");
        show_string(str1, 160, 50 + (init * 10), 1, RED, WHITE, 0 );
        selftest_set_0(SELFTEST_ERR_RTC);
      }
    }

  }
  delay(500);
  MenuPrepareStyle();

  a2d_run_now = 1;

}

////////////////////////
/*
  1. fix sync NTP cas - fixnuto - magicka konstanta


*/

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

  /*
    my_touch.TP_Scan(0);
    if (my_touch.TP_Get_State()&TP_PRES_DOWN)
    {
    //if (default_screen_touch(&my_touch) == true)
    //screen_redraw_all = 1;
    //sprintf(str1, "x=%d y=%d", my_touch.x, my_touch.y);
    //show_string(str1, 0, 270, 1, BLACK, LIGHTGREY, 0);
    }
  */

  if (draw_menu(false) == true)
    draw_menu(true);

  ////////////////////
  /// kazdych 10sec
  if ((millis() - milis_10s) > 10000)
  {
    milis_10s = millis();
    //device_get_name(str1);

    send_mqtt_onewire();
    send_mqtt_status(&mqtt_client);
    send_device_status();
    send_mqtt_ring();
    send_mqtt_tds();

    //send_mqtt_program();
    ///thermostat();
    //mqtt_send_pid_variable();
    //send_mqtt_remote_tds_status();
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
  }

  //digitalWrite(LED, LOW);
  if ((millis() - milis_05s) > 500)
  {
    milis_05s = millis();
    now = rtc.now();
    selftest();
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// funkce pro zobrazeni casu
void display_element_show_time_1(uint16_t x, uint16_t y)
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

/// funkce pro zobrazeni teploty
void display_element_show_temp_1(uint16_t x, uint16_t y)
{
  struct_DDS18s20 tds;
  uint8_t idx;
  char str1[20];
  char str2[8];
  int  tt, te, td, tu = 0;
  uint16_t pos;

  idx = thermostat_ring_get_asociate_tds(default_ring);
  strcpy_P(str1, current_temp);
  if (get_tds18s20(idx, &tds) == 1)
  {
    strcat(str1, tds.name);
  }
  else
  {
    strcat(str1, "err");
  }
  show_string(str1, x + pos + 5, y + 5, 1, BLACK, WHITE, 0);
  if (status_tds18s20[idx].online == True)
  {
    tt = status_tds18s20[idx].temp;
    te = (tt / 1000);
    tu = te * 1000;
    td = (tt - tu);
    itoa(te, str1, 10);
    strcat(str1, ".");
    itoa(td, str2, 10);
    strncat(str1, str2, 1);
    strcat(str1, "C");
    show_string(str1, x + 4, y + 20, 8, BLACK, WHITE, 0);
  }
  else
  {
    strcpy(str1, "ERR");
    show_string(str1, x + 4, y + 20, 8, RED, WHITE, 0);
  }
  my_lcd.Set_Draw_color(BLACK);
  my_lcd.Draw_Rectangle(x, y, x + 244, y + 84);
}
////////////////////////////////////////////////////
void display_element_show_date_1(uint16_t x, uint16_t y)
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
  my_lcd.Set_Draw_color(WHITE);
  my_lcd.Draw_Fast_HLine(x, y, 142);
  my_lcd.Draw_Fast_HLine(x, y + 1, 142);
  show_string(str1, x, y + 2, 3, BLACK, WHITE, 0);
}
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/*
  void display_element_show_table_onewire(uint16_t x, uint16_t y)
  {
  //struct_DDS18s20 tds;
  char str1[32];
  char str2[24];
  my_lcd.Set_Draw_color(BLACK);
  my_lcd.Draw_Rectangle(x, y, 460, 210);
  for (uint8_t idx = 0; idx < HW_ONEWIRE_MAXDEVICES; idx++)
  {
    createString(str2, ':', w_rom[idx].rom, 8, 16);
    sprintf(str1, "idx:%d, tds_idx:%d", idx, w_rom[idx].tds_idx);
    show_string(str1, x + 20, y + 10 + (idx * 30), 1, BLACK, WHITE, 0);
    sprintf(str1, "rom: %s", str2);
    show_string(str1, x + 25, y + 20 + (idx * 30), 1, BLACK, WHITE, 0);
    //if (w_rom[idx].used == 1)
    //{
    //button_click_1(x + 300, y + 26 + (idx * 30), 128, 28, 2 , "Nastav TDS");
    //}
  }
  }
*/
//////////////////////////////////////////////////////////
/// typ tlacitka 1 ////
void button_click_1(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint8_t font_size_1, uint8_t font_size_2, char *line1, char *line2)
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

/// typ tlacitka 2 ////
void button_click_2(uint16_t x, uint16_t y, uint8_t size_x, uint8_t size_y, uint8_t font_size, uint16_t color_active , uint16_t color_inactive, uint8_t state, char *text)
{
  uint8_t posx, posy, tmpy, tmpx;
  uint16_t back_color;

  back_color = color_inactive;
  if (state == 1)
    back_color = color_active;

  my_lcd.Set_Draw_color(BLACK);
  my_lcd.Draw_Rectangle(x, y, x + size_x, y + size_y);
  my_lcd.Draw_Rectangle(x + 1, y + 1, x + size_x - 1, y + size_y - 1);
  my_lcd.Draw_Rectangle(x + 2, y + 2, x + size_x - 2, y + size_y - 2);
  tmpy = show_string_size_hight(font_size);
  tmpx = show_string_size_width(strlen(text), font_size);
  posx = (size_x - tmpx) / 2;
  posy = (size_y - tmpy) / 2;
  /// horni vybarveni
  my_lcd.Fill_Rect(x + 3, y + 3, size_x - 5, posy - 3, back_color );
  /// spodni vybarveni
  my_lcd.Fill_Rect(x + 3, y + tmpy + posy + 6, size_x - 5, size_y - tmpy - posy - 8, back_color );
  /// leve vybarveni
  my_lcd.Fill_Rect(x + 3, y + posy, posx - 3, tmpy + 6, back_color);
  /// prave vybarveni
  my_lcd.Fill_Rect(x + posx + tmpx, y + posy, size_x - posx - tmpx - 2, tmpy + 6, back_color );
  show_string(text, x + posx, y + posy, font_size, BLACK, back_color, 0);
}


void button_switch_1(uint16_t x, uint16_t y, uint8_t size_x, uint8_t size_y, uint8_t font_size, uint8_t state, char *text, char *text_value)
{
  uint8_t posx , posy, tmpy, tmpx;
  my_lcd.Set_Draw_color(BLACK);
  my_lcd.Draw_Rectangle(x, y, x + size_x + 2, y + size_y);
  my_lcd.Draw_Rectangle(x + 1, y + 1, x + size_x + 1, y + size_y - 1);
  my_lcd.Draw_Rectangle(x + 2, y + 2, x + size_x, y + size_y - 2);
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


bool button_click_touch(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint16_t click_x, uint16_t click_y)
{
  bool ret = false;
  if (is_pressed(x, y, x + size_x, y + size_y, click_x, click_y) == true)
  {
    ret = true;
  }
  return ret;
}

/*************************************************************************************************************************/
/*************************************************************************************************************************/
/*
  void display_show_dialog_yes_no(uint16_t x, uint16_t y,  uint16_t size_x, uint16_t size_y, char *text, char *text_yes, char *text_no)
  {
  char str1[16];
  uint16_t posx, posy, tmp1;
  tmp1 = show_string_size_width(strlen(text), 2);
  posx = (size_x - tmp1) / 2;
  my_lcd.Set_Draw_color(YELLOW);
  my_lcd.Fill_Rectangle(x + 3, y + 3, x + size_x - 3, y + size_y - 3);
  my_lcd.Set_Draw_color(BLACK);
  my_lcd.Draw_Rectangle(x, y, x + size_x, y + size_y);
  my_lcd.Draw_Rectangle(x + 1, y + 1, x + size_x - 1, y + size_y - 1);
  my_lcd.Draw_Rectangle(x + 2, y + 2, x + size_x - 2, y + size_y - 2);
  strcpy(str1, "Upozorneni");
  show_string(str1, x + 10, y + 5, 1, RED, WHITE, 0);
  my_lcd.Draw_Fast_HLine(x, y + 15, size_x);
  show_string(text, x + posx, y + 25, 2, RED, WHITE, 0);
  button_click_1(x + 10, y + 50, 100, 30, 2, text_yes);
  button_click_1(x + size_x - 110, y + 50, 100, 30, 2, text_no);
  }
  bool display_show_dialog_yes_no_touch(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint16_t click_x, uint16_t click_y)
  {
  bool ret = false;
  /// yes
  if (button_click_touch(x + 10, y + 50, 100, 30, click_x, click_y) == true)
  {
    MenuDialogActionYes();
    ret = true;
  }
  /// no
  if (button_click_touch(x + size_x - 110, y + 50, 100, 30, click_x, click_y) == true)
  {
    MenuDialogActionNo();
    ret = true;
  }
  return ret;
  }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////////////
  void display_show_keyboard(uint16_t x, uint16_t y,  uint16_t size_x, uint16_t size_y, char *text)
  {
  char str1[16];
  uint16_t posx, posy, tmp1;
  my_lcd.Set_Draw_color(YELLOW);
  my_lcd.Fill_Rectangle(x + 3, y + 3, x + size_x - 3, y + size_y - 3);
  my_lcd.Set_Draw_color(BLACK);
  my_lcd.Draw_Rectangle(x, y, x + size_x, y + size_y);
  my_lcd.Draw_Rectangle(x + 1, y + 1, x + size_x - 1, y + size_y - 1);
  my_lcd.Draw_Rectangle(x + 2, y + 2, x + size_x - 2, y + size_y - 2);
  strcpy(str1, "Klavesnice");
  show_string(str1, x + 10, y + 5, 1, RED, WHITE, 0);
  my_lcd.Draw_Fast_HLine(x, y + 15, size_x);

  tmp1 = show_string_size_width(10, 2);
  my_lcd.Draw_Rectangle(x + 20, y + 40, x + tmp1, y + 30);

  show_string(text, x + 20, y + 40, 2, RED, WHITE, 0);
  posx = 0;
  for (uint8_t number = '0'; number <= '9'; number++)
  {
    sprintf(str1, "%c", number);
    button_click_1(x + 10 + (posx * 35), y + 60, 30, 30, 2, str1);
    posx++;
  }
  }


  bool display_show_keyboard_touch(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint16_t click_x, uint16_t click_y)
  {

  }

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  void display_element_status_H_link(uint16_t y)
  {
  my_lcd.Set_Draw_color(RED);
  my_lcd.Draw_Fast_HLine(0, y, 480);
  my_lcd.Draw_Fast_HLine(0, y + 1, 480);
  }
  ///
  /// funkce pro zobrazeni statusu mqtt pripojeni
  void display_element_mqtt_connect(uint16_t x, uint16_t y)
  {
  char str1[12];
  if (selftest_get_0(SELFTEST_MQTT_LINK) == 0)
  {
    strcpy(str1, "MQTT: OK");
    show_string(str1, x, y + 2, 3, GREEN, WHITE, 0);
  }
  else
  {
    strcpy(str1, "MQTT:ERR");
    show_string(str1, x, y + 2, 3, RED, WHITE, 0);
  }
  }
  ///






  bool display_element_show_temp_1_touch(uint16_t x, uint16_t y, uint16_t px, uint16_t py)
  {

  }
  ////////////////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////////////////

  bool button_click_touch(uint16_t x, uint16_t y, uint8_t size_x, uint8_t size_y, uint16_t click_x, uint16_t click_y)
  {
  bool ret = false;
  if (is_pressed(x, y, x + size_x, y + size_y, click_x, click_y) == true)
  {
    ret = true;
  }
  return ret;
  }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////
  /// typ tlacitka 1 ////

  /////////////////////////////////////////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////////////////////////////////////





  void display_button_hlavni(uint16_t x, uint16_t y)
  {
  char str1[16];

  my_lcd.Set_Draw_color(BLACK);
  my_lcd.Draw_Rectangle(x, y, x + 200, y + 175);

  str1[0] = 0;
  strcpy_P(str1, nastaveni_button_text);
  button_click_1(x + 20, y + 10, 160, 45, 2, str1);

  str1[0] = 0;
  strcpy_P(str1, funkce_button_text);
  button_click_1(x + 20, y + 65, 160, 45, 2, str1);

  str1[0] = 0;
  strcpy_P(str1, funkce_button_regulator);
  button_click_1(x + 20, y + 120, 160, 45, 2, str1);

  }


  bool display_button_hlavni_touch(uint16_t x, uint16_t y,  uint16_t click_x, uint16_t click_y)
  {
  /// tlacitko nastaveni
  if (button_click_touch(x + 20, y + 20, 160, 60, click_x, click_y) == true)
  {
    my_lcd.Fill_Screen(WHITE);
    MenuHistorySetNext(MENU_NASTAVENI_SCREEN);

  }
  /// tlacitko funkce
  if (button_click_touch(x + 20, y + 75, 160, 60, click_x, click_y) == true)
  {
    my_lcd.Fill_Screen(WHITE);
    MenuHistorySetNext(MENU_FUNKCE_SCREEN);
  }
  if (button_click_touch(x + 20, y + 130, 160, 60, click_x, click_y) == true)
  {
    my_lcd.Fill_Screen(WHITE);
  }
  }
  ////
  //////////////////////////////////////////////////////////////////////////////////////////////
  void display_button_funkce(uint16_t x, uint16_t y)
  {
  char str1[16];
  button_switch_1(x + 10, y + 10, 200, 70, 3, "Budik", "Zapnuto 06:22");
  //button_switch_1(x+220, y+10, 200, 70, 3, "Budik", "Vypnuto");

  strcpy_P(str1, button_zpet);
  button_click_1(x + 10, y + 200, 200, 40, 2, str1);
  }
  bool display_button_funkce_touch(uint16_t x, uint16_t y,  uint16_t click_x, uint16_t click_y)
  {
  bool ret = false;
  /// tlacitko zpet
  if (button_click_touch(x + 10, y + 200, 200, 40, click_x, click_y) == true)
  {
    my_lcd.Fill_Screen(WHITE);
    MenuHistorySetPrev();
    ret = true;
  }
  return ret;
  }


  ///////
  ////////////////////////////////
  void display_button_nastaveni(uint16_t x, uint16_t y)
  {
  char str1[32];
  strcpy_P(str1, nastaveni_site);
  button_click_1(x + 10, y + 10, 210, 40, 2, str1);

  strcpy_P(str1, nastaveni_nrf);
  button_click_1(x + 10, y + 60, 210, 40, 2, str1);

  strcpy_P(str1, nastaveni_default);
  button_click_1(x + 10, y + 110, 210, 40, 2, str1);

  strcpy_P(str1, nastaveni_onewire);
  button_click_1(x + 250, y + 10, 210, 40, 2, str1);

  strcpy_P(str1, nastaveni_rtds);
  button_click_1(x + 250, y + 60, 210, 40, 2, str1);

  strcpy_P(str1, button_zpet);
  button_click_1(x + 10, y + 200, 210, 40, 2, str1);
  }

  bool display_button_nastaveni_touch(uint16_t x, uint16_t y,  uint16_t click_x, uint16_t click_y)
  {
  bool ret = false;
  /// tlacitko nastaveni tds
  if (button_click_touch(x + 250, y + 10, 210, 40, click_x, click_y) == true)
  {
    my_lcd.Fill_Screen(WHITE);
    MenuHistorySetNext(MENU_NASTAVENI_ONEWIRE);
    ret = true;
  }

  /// tlacitko zpet
  if (button_click_touch(x + 10, y + 200, 210, 40, click_x, click_y) == true)
  {
    my_lcd.Fill_Screen(WHITE);
    MenuHistorySetPrev();
    ret = true;
  }
  return ret;
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////


  void display_nastaveni_onewire(uint16_t x, uint16_t y)
  {
  //struct_DDS18s20 tds;
  char str1[26];
  char str2[16];
  strcpy_P(str1, text_onewire_list);
  my_lcd.Set_Draw_color(BLACK);
  my_lcd.Draw_Rectangle(x, y, 460, 220);
  show_string(str1, x + 10, y + 10, 1, BLACK, WHITE, 0);
  for (uint8_t idx = 0; idx < HW_ONEWIRE_MAXDEVICES; idx++)
  {
    createString(str2, ':', w_rom[idx].rom, 8, 16);
    sprintf(str1, "idx:%d, tds_idx:%d", idx, w_rom[idx].tds_idx);
    show_string(str1, x + 20, y + 30 + (idx * 30), 1, BLACK, WHITE, 0);
    sprintf(str1, "rom: %s", str2);
    show_string(str1, x + 25, y + 40 + (idx * 30), 1, BLACK, WHITE, 0);
    if (w_rom[idx].used == 1)
    {
      button_click_1(x + 300, y + 26 + (idx * 30), 128, 28, 2 , "Nastav TDS");
    }
  }
  strcpy_P(str1, button_zpet);
  button_click_1(x + 10, y + 220, 210, 40, 2, str1);
  }
  ////
  bool display_nastaveni_onewire_touch(uint16_t x, uint16_t y,  uint16_t click_x, uint16_t click_y)
  {
  bool ret = false;
  char str1[16];
  uint8_t tmp1[2];
  /// tlacitko tds pridej tds
  for (uint8_t idx = 0; idx < HW_ONEWIRE_MAXDEVICES; idx++)
  {
    if (button_click_touch(x + 300, y + 26 + (idx * 30), 128, 28, click_x, click_y))
      if (w_rom[idx].used == 1)
      {
        if (tds_get_idx_from_wrom(idx) == 255)
        {
          strcpy_P(str1, text_onewire_asociace);
          MenuDialogSetText(str1);
          MenuDialogSetTextNo("Ne");
          MenuDialogSetTextYes("Ano");
          tmp1[0] = idx;
          tmp1[1] = MENU_NASTAVENI_TDS;
          MenuDialogSetArgs(tmp1, 2);
          MenuDialogSetActionYes(TDS_Associate_And_Switch_Menu_Yes);
          MenuDialogSetActionNo(TDS_Associate_And_Switch_Menu_No);
          MenuHistorySetNext(MENU_DIALOG_YES_NO);
          ret = true;
        }
        else
        {
          DisplayClean();
          MenuHistorySetNext(MENU_NASTAVENI_TDS);
          MenuHistoryUpdateArgs1(tds_get_idx_from_wrom(idx));
          ret = true;
        }
      }
  }
  /// tlacitko zpet
  if (button_click_touch(x + 10, y + 220, 210, 40, click_x, click_y) == true)
  {
    DisplayClean();
    MenuHistorySetPrev();
    ret = true;
  }
  return ret;
  }
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  void display_nastaveni_tds(uint16_t x, uint16_t y, uint8_t idx)
  {
  struct_DDS18s20 tds;
  char str1[26];
  char str2[26];
  strcpy_P(str1, text_tds_sensors);
  my_lcd.Set_Draw_color(BLACK);
  my_lcd.Draw_Rectangle(x, y, 460, 220);
  if (get_tds18s20(idx, &tds) == 1)
    strcpy(str2, tds.name);
  else
    itoa(idx, str2, 10);
  strcat(str1, str2);
  show_string(str1, x + 10, y + 10, 2, BLACK, WHITE, 0);

  createString(str2, ':', tds.rom, 8, 16);
  sprintf(str1, "rom: %s", str2);
  show_string(str1, x + 25, y + 40, 2, BLACK, WHITE, 0);

  sprintf(str1, "nazev: %s", tds.name);
  show_string(str1, x + 25, y + 60, 2, BLACK, WHITE, 0);

  sprintf(str1, "offset: %dC", tds.offset);
  show_string(str1, x + 25, y + 80 , 2, BLACK, WHITE, 0);

  sprintf(str1, "merici perioda: %ds", tds.period);
  show_string(str1, x + 25, y + 100 , 2, BLACK, WHITE, 0);

  button_click_1(x + 10, y + 130 , 210, 30, 2 , "Zrusit TDS");
  button_click_1(x + 10, y + 175 , 210, 30, 2 , "Nastavit nazev");
  button_click_1(x + 230, y + 130 , 210, 30, 2 , "Nastavit offset");
  button_click_1(x + 230, y + 175 , 210, 30, 2 , "Nastavit periodu");

  strcpy_P(str1, button_zpet);
  button_click_1(x + 10, y + 220, 210, 40, 2, str1);
  }

  bool display_nastaveni_tds_touch(uint16_t x, uint16_t y,  uint16_t click_x, uint16_t click_y, uint8_t args1)
  {
  char str1[16];
  bool ret = false;
  uint8_t tmp1[2];
  /// tlacitko zpet
  if (button_click_touch(x + 10, y + 220, 210, 40, click_x, click_y) == true)
  {
    my_lcd.Fill_Screen(WHITE);
    MenuHistorySetPrev();
    ret = true;
  }

  /// tlacitko zrusit associaci
  if (button_click_touch(x + 10, y + 130 , 210, 30, click_x, click_y) == true)
  {
    strcpy_P(str1, text_onewire_deasociace);
    MenuDialogSetText(str1);
    MenuDialogSetTextNo("Ne");
    MenuDialogSetTextYes("Ano");
    tmp1[0] = args1;
    tmp1[1] = MENU_NASTAVENI_ONEWIRE;
    MenuDialogSetArgs(tmp1, 2);
    MenuDialogSetActionYes(TDS_DeAssociate_And_Switch_Menu_Yes);
    MenuDialogSetActionNo(TDS_DeAssociate_And_Switch_Menu_No);
    MenuHistorySetNext(MENU_DIALOG_YES_NO);
    ret = true;
  }

  /// tlacitko zmenit nazev
  if (button_click_touch(x + 10, y + 175 , 210, 30, click_x, click_y) == true)
  {
    strcpy_P(str1, text_onewire_deasociace);
    tmp1[0] = args1;
    tmp1[1] = MENU_NASTAVENI_TDS;
    MenuDialogSetArgs(tmp1, 2);
    MenuDialogSetText(str1);
    MenuDialogSetActionYes(fnull);
    MenuHistorySetNext(MENU_DIALOG_KEYBOARD);
    ret = true;
  }

  return ret;
  }
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  void display_button_regulator(uint16_t x, uint16_t y)
  {
  /// vychozi ring
  }
  void display_button_regulator_touch(uint16_t x, uint16_t y,  uint16_t click_x, uint16_t click_y)
  {
  }


  ///////////////////////////////////////////////////////////////////////////////////////////////////
  void display_thermostat_button(uint16_t x, uint16_t y, uint8_t active_mode)
  {
  char str1[8];
  for (uint8_t b = 0; b < 5; b++)
  {
    if (b == 0)
    {
      strcpy(str1, "OFF");
      if (active_mode == TERM_MODE_OFF)
        button_click_2(x + (b * 90), y, 82, 62, 3, LIGHTGREY , str1);
      else
        button_click_2(x + (b * 90), y, 82, 62, 3, WHITE, str1);
    }
    if (b == 1)
    {
      strcpy(str1, "MAX");
      if (active_mode == TERM_MODE_MAX)
        button_click_2(x + (b * 90), y, 82, 62, 3, RED, str1);
      else
        button_click_2(x + (b * 90), y, 82, 62, 3, WHITE, str1);
    }
    if (b == 2)
    {
      strcpy(str1, "MIN");
      if (active_mode == TERM_MODE_CLIMATE_MAX)
        button_click_2(x + (b * 90), y, 82, 62, 3, BLUE, str1);
      else
        button_click_2(x + (b * 90), y, 82, 62, 3, WHITE, str1);
    }
    if (b == 3)
    {
      strcpy(str1, "PROG");
      if (active_mode == TERM_MODE_PROG)
        button_click_2(x + (b * 90), y, 82, 62, 3, YELLOW, str1);
      else
        button_click_2(x + (b * 90), y, 82, 62, 3, WHITE, str1);
    }
    if (b == 4)
    {
      strcpy(str1, "MAN");
      if (active_mode == TERM_MODE_MAN_HEAT || active_mode == TERM_MODE_MAN_COOL)
        button_click_2(x + (b * 90), y, 82, 62, 3, GREEN, str1);
      else
        button_click_2(x + (b * 90), y, 82, 62, 3, WHITE, str1);
    }
  }
  }
  ///
  bool display_thermostat_button_touch(uint16_t x, uint16_t y,  uint16_t click_x, uint16_t click_y)
  {
  uint8_t active_mode;
  uint8_t clicki = false;

  for (uint8_t b = 0; b < 5; b++)
  {
    if (b == 0)
    {
      if (button_click_touch(x + (b * 90), y, 80, 60, click_x, click_y) == true)
      {
        active_mode = TERM_MODE_OFF;
        thermostat_ring_set_mode(default_ring, active_mode);
        clicki = true;
      }
    }
    if (b == 1)
    {
      if (button_click_touch(x + (b * 90), y, 80, 60, click_x, click_y) == true)
      {
        active_mode = TERM_MODE_MAX;
        thermostat_ring_set_mode(default_ring, active_mode);
        clicki = true;
      }
    }
    if (b == 2)
    {
      if (button_click_touch(x + (b * 90), y, 80, 60, click_x, click_y) == true)
      {
        active_mode = TERM_MODE_CLIMATE_MAX;
        thermostat_ring_set_mode(default_ring, active_mode);
        clicki = true;
      }
    }
    if (b == 3)
    {
      if (button_click_touch(x + (b * 90), y, 80, 60, click_x, click_y) == true)
      {
        active_mode = TERM_MODE_PROG;
        thermostat_ring_set_mode(default_ring, active_mode);
        clicki = true;
      }
    }
    if (b == 4)
    {
      if (button_click_touch(x + (b * 90), y, 80, 60, click_x, click_y) == true)
      {
        active_mode = TERM_MODE_MAN_HEAT;
        thermostat_ring_set_mode(default_ring, active_mode);
        clicki = true;
      }
    }
  }
  return clicki;
  }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////
  bool default_screen_touch(LCDWIKI_TOUCH * my_touch)
  {
  uint8_t menu_index;
  uint8_t args1;

  menu_index = MenuHistoryGetMenu(&args1);
  bool ret = false;
  if (menu_index == MENU_DEFAULT_SCREEN)
  {
    ret = ret || display_thermostat_button_touch(10, 200,  my_touch->x, my_touch->y);
    ret = ret || display_element_show_temp_1_touch(10, 100, my_touch->x, my_touch->y);
    ret = ret || display_button_hlavni_touch(260, 10, my_touch->x, my_touch->y);
  }
  if (menu_index == MENU_FUNKCE_SCREEN)
  {
    ret = ret || display_button_funkce_touch(10, 10, my_touch->x, my_touch->y);
  }

  if (menu_index == MENU_NASTAVENI_SCREEN)
  {
    ret = ret || display_button_nastaveni_touch(10, 10, my_touch->x, my_touch->y);
  }

  if (menu_index == MENU_NASTAVENI_ONEWIRE)
  {
    ret = ret || display_nastaveni_onewire_touch(10, 10, my_touch->x, my_touch->y);
  }

  if (menu_index == MENU_NASTAVENI_TDS)
  {
    ret = ret || display_nastaveni_tds_touch(10, 10, my_touch->x, my_touch->y, args1);
  }

  if (menu_index == MENU_DIALOG_YES_NO)
  {
    ret = ret || display_show_dialog_yes_no_touch(50, 80, 300, 100, my_touch->x, my_touch->y);

  }

  if (menu_index == MENU_DIALOG_KEYBOARD)
  {
    ret = ret || display_show_keyboard_touch(50, 50, 350, 200, my_touch->x, my_touch->y);

  }

  return ret;
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  void default_screen_redraw_all(void)
  {
  uint8_t args1;
  char str1[32];
  char str2[8];
  char str3[8];

  uint8_t menu_index = MenuHistoryGetMenu(&args1);
  if ( menu_index == MENU_DEFAULT_SCREEN)
  {
    display_element_status_H_link(294);
    display_element_mqtt_connect(0, 296);
    display_element_show_date(338, 296);
    display_thermostat_button(10, 200, thermostat_ring_get_mode(default_ring));
    display_element_show_temp_1(10, 100);
    display_button_hlavni(260, 10);
  }

  if (menu_index == MENU_FUNKCE_SCREEN)
  {
    display_element_status_H_link(294);
    display_element_mqtt_connect(0, 296);
    display_element_show_date(338, 296);
    display_button_funkce(10, 10);
  }


  if (menu_index == MENU_NASTAVENI_SCREEN)
  {
    display_element_status_H_link(294);
    display_element_mqtt_connect(0, 296);
    display_element_show_date(338, 296);
    display_button_nastaveni(10, 10);
  }


  if (menu_index == MENU_NASTAVENI_ONEWIRE)
  {
    display_element_status_H_link(294);
    display_element_mqtt_connect(0, 296);
    display_element_show_date(338, 296);
    display_nastaveni_onewire(10, 10);
  }

  if (menu_index == MENU_NASTAVENI_TDS)
  {
    display_element_status_H_link(294);
    display_element_mqtt_connect(0, 296);
    display_element_show_date(338, 296);
    display_nastaveni_tds(10, 10, args1);
  }

  if (menu_index == MENU_DIALOG_YES_NO)
  {
    MenuDialogGetText(str1);
    MenuDialogGetTextNo(str3);
    MenuDialogGetTextYes(str2);
    display_show_dialog_yes_no(50, 80, 300, 100, str1, str2, str3);
  }

  if (menu_index == MENU_DIALOG_KEYBOARD)
  {
    MenuDialogGetText(str1);
    display_show_keyboard(50, 50, 350, 200, str1);
  }
  }


  void default_screen_1s(void)
  {
  uint8_t menu_index = MenuHistoryGetMenu();
  if ( menu_index == MENU_DEFAULT_SCREEN)
  {
    display_element_mqtt_connect(0, 296);
    display_element_show_date(338, 296);
    display_element_show_temp_1(10, 100);
  }

  if (menu_index == MENU_FUNKCE_SCREEN)
  {
    display_element_mqtt_connect(0, 296);
    display_element_show_date(338, 296);
  }

  if (menu_index == MENU_NASTAVENI_SCREEN)
  {
    display_element_mqtt_connect(0, 296);
    display_element_show_date(338, 296);
  }

  if (menu_index == MENU_NASTAVENI_ONEWIRE)
  {
    display_element_mqtt_connect(0, 296);
    display_element_show_date(338, 296);
  }

  if (menu_index == MENU_NASTAVENI_TDS)
  {
    display_element_mqtt_connect(0, 296);
    display_element_show_date(338, 296);
  }
  }


  void default_screen_05s(void)
  {
  uint8_t menu_index = MenuHistoryGetMenu();
  if (MenuHistoryGetMenu() == MENU_DEFAULT_SCREEN)
  {
    display_element_show_time_1(10, 10);
  }
  ////
  if (MenuHistoryGetMenu() == MENU_FUNKCE_SCREEN)
  {
  }



  }


*/
