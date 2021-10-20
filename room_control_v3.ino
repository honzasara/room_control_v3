/*
   TODO
   1. Prekreslovani elementu - zrychlit obsluhu displaye
   2. Zrychlit hlavni smycku
   3.
   4. Dialog menu vyberu - neaktivni nedostupne sede barvou - TODO slozitejsi problem
   5. Dialog klavesnice kurzor editace
   6.
   7. Dialog editace PID - zobrazit krivku vypoctu
   8.
   9.
   10.
   11.
   12.
   13. vytvorit menu seznam vsech teplomeru ukazovat hodnoty - HOTOVO
        + pridat NRF devices
   14.
   15. umet nastavit promenou seznam_server pro ucel testovani konektivity, port
   16.
   17. validace zadanych hodnot pro cas a datum, nedovolit ulozit!
   18. funkce pro validaci IP adres
   19.
   20.
   21.
   22. change_term_mode - rozdelit po bitech
   23.
   24. reload nastaveni site, reload zmena nastaveni mqtt
   25.
   26.
   27.
   28.
   29.
   30. dialog s vysledkem scanovani NRF okoli
   31.
   32.
   33.
   34.
   35. automaticke rizeni jasu, nedelat skokove

  ----
  dialogum upravit hlavicku textu

  dokoncit menu statistika regulatoru
    - zde informace, aktualni stav, vykon akcniho clenu nastaveny, zpetna vazba od akcniho clenu
    - posledni update od akcniho clenu
    - vybrany mod
    - vybrany program
    - pid krivka

  menu casove plany
    - souhrny nahled

  menu NRF

  menu vystupni cleny
    - dokoncit funkci test vystupniho clenu
    - dokoncit rucni hodnota vystupu
*/


#include "room_control_v3.h"
#include "store_define.h"

#include "MainMenu.h"
#include "SettingsMenu.h"
#include "BaseDialogMenu.h"
#include "FunctionsMenu.h"
#include "NetworkSettingsMenu.h"
#include "pidDialogMenu.h"
#include "OneWireMenu.h"

#include "SystemSettingsMenu.h"
#include "PeriferieSettingsMenu.h"
#include "DisplaySettingsMenu.h"
#include "RTDSMenu.h"
#include "MenuSettingsTime.h"

#include "MenuNastaveniMqtt.h"
#include "ThermostatMenu.h"
#include "pidDialogMenu.h"
#include "ThermostatTimeMenu.h"
#include "VirtualOutputMenu.h"

SoftSPIB swSPI(STORAGE_MOSI, STORAGE_MISO, STORAGE_CLK);


RTC_DS1307 rtc;
DateTime now;
uint16_t last_year = 0;
uint8_t last_month = 0;
uint8_t last_day = 0;
uint8_t last_time_hour = 0;
uint8_t last_time_minute = 0;
uint8_t display_update_date = 0;
uint8_t display_redraw_once = 0;
uint8_t display_update_slider = 0;
uint8_t display_redraw_default_ring = 0;

uint8_t status_send_counter = 0;

EthernetClient ethClient;
EthernetClient ethClient1;
EthernetUDP udpClient;
PubSubClient mqtt_client(ethClient);
EthernetServer http_server(80);


RF24 radio(NRF_CE, NRF_CS);
RF24Network network(radio);
RF24Mesh mesh(radio, network);

Generic_LM75_11Bit lm75_temp;


LCDWIKI_KBV my_lcd(ST7796S, 33, 35, 36, 32, 34); //model,cs,cd,wr,rd,reset
LCDWIKI_TOUCH my_touch(28, 27, 29, 30, 7, 31); //tcs,tclk,tdout,tdin,tirq, sdcd

StaticJsonDocument<256> doc;


SRAM_23LC SRAM(&swSPI, STORAGE_RAM_CS, SRAM_23LC1024);
EEPROM_CAT25 SROM(&swSPI, STORAGE_EEPROM_CS , CAT25M02);



long lastmqttconnect = 0;

uint8_t selftest_data_0 = 0;
uint8_t last_selftest_data_0 = 0;

uint8_t selftest_data_1 = 0;
uint8_t last_selftest_data_1 = 0;

uint16_t tritri = 0;
uint16_t petnula = 0;
uint16_t dvanact = 0;
uint16_t proud = 0;
uint16_t light_curr = 0;
uint8_t a2d_run_now = 0;

long uptime = 0;
long milis = 0;
long milis_005s = 0;
long milis_05s = 0;
long milis_1s = 0;
long milis_10s = 0;
long milis_1ms = 0;

uint8_t display_touch_click = 0;
uint8_t click_on_display = 0;
uint8_t click_on_display_last = 0;
uint8_t click_on_display_hold = 0;
uint8_t click_delay_display = 0;
uint8_t click_delay_enable_display = 0;

long click_time = 0;


uint8_t default_ring = 0;
uint8_t last_default_ring = 0;
uint8_t default_show_temp = 0;
uint8_t thermostat_mode_default_ring_last_state = 0;



unsigned long load = 0;
unsigned long load_max = 0;
unsigned long load_min = 0xffffffff;

uint16_t light_min = 0xffff;
uint16_t light_max = 0;
uint8_t watchdog_state = 0;

uint8_t brigthness_display_values = 0;
uint8_t brigthness_display_auto_values = 0;
uint8_t last_brigthness_display_auto_values = 0;

uint8_t brigthness_display_mode = 0;
uint8_t last_brigthness_display_mode = 0;
uint8_t display_auto_shutdown = 0;
uint8_t display_auto_shutdown_now = 0;

uint8_t use_rtds = 0;
uint8_t use_rtds_type_temp = 0;
uint8_t use_tds = 0;
uint8_t use_nrf_temp = 0;
uint8_t use_virtual_output = 0;
uint8_t use_virtual_output_persistent = 0;

uint8_t menu_slider_data_current[MAX_SLIDERS];
uint8_t menu_slider_data_max;
uint8_t menu_slider_data_min;
uint8_t menu_slider_data_max_element;

float internal_temp;

struct payload_t {
  unsigned long ms;
  unsigned long counter;
};

struct tt_menu_dialog_variable
{
  float variable_now;
  float variable_max;
  float variable_min;
  uint8_t update_now;
  uint8_t number_type;
  float variable_step;
  uint8_t args1;
  uint8_t args2;
  fptr_save_function save_function;
} menu_dialog_variable[4];
uint8_t menu_dialog_variable_change;
uint8_t menu_dialog_change;



uint8_t dialog_yes_args1;
char dialog_text[32];

char dialog_set_string[32];
uint8_t dialog_set_string_max_length;
uint8_t dialog_set_string_args1;
uint8_t dialog_set_string_args2;
uint8_t dialog_set_string_keyboard_type = 0;
fptr_args dialog_save_string_function;
ret_fptr_no_args dialog_save_valid_function;


uint8_t menu_redraw05s = 0;
uint8_t menu_redraw10s = 0;
uint8_t change_term_mode = 0;
uint8_t change_virtual_output = 0;
uint8_t change_auto_brightness = 0;


uint8_t MenuHistory[MENU_MAX_HISTORY];
uint8_t Global_menu_args1[MENU_MAX_HISTORY];
uint8_t Global_menu_args2[MENU_MAX_HISTORY];
uint8_t MenuHistoryIndex = 0;

uint8_t menu_dialog_mappings[32];


fptr_args dialog_yes_function;








const MenuAll Menu_All PROGMEM = {
  .len_menu1 = 10,
  .len_menu2 = 10,
  .len_menu3 = 10,
  .len_menu4 = 10,

  .ListMenu1 = {HlavniMenu, MenuNastaveniSite, OneWireMenu, MenuNastaveniCas, SelectMenuDefaultTemp, MenuNastaveniMQTT, Menu_Show_All_temp, New_ThermostatTimeProgramMenuDetail, New_ThermostatTimeProgramSetting, DialogSetSelectTimeProgram},
  .ListMenu2 = {DialogYESNO, DialogSetVariable, DialogKyeboardAlfa, DialogKyeboardNumber , DialogOK, MenuThermostat, VirtualOutputSettingsDevice, DialogSelectVirtualOutputForTerm, DialogShowThermostatStatistics, New_ThermostatTimeProgramMenu },
  .ListMenu3 = {TDSMenu, RTDS_Menu_Detail, List_RTDS_Menu, MenuThermostat_Setting, DialogSelectRing, MenuThermostatRingSetup, DialogSelectInputSensorsForTerm, DialogSelectTermMode, DialogSelectPIDSensor, New_ThermostatTimeMenu},
  .ListMenu4 = {SystemSettingsMenu, New_NastaveniMenu, PeriferieSettingsMenu, New_DisplaySettingMenu, New_DisplaySetting_Brigthness, AboutDeviceMenu, New_DisplaySetting_Auto_Shutdown, SetNRFMenu, VirtualOutputSettingsMenu, DialogSetManualyTemp},
};






void nullfce(uint16_t args1, uint16_t args2, uint8_t args3) {};
void nullfceargs(uint16_t args1, uint16_t args2, uint8_t args3) {};
uint8_t returnnullfceargs(uint16_t args1, uint16_t args2, uint8_t args3)
{
  return 0;
}
uint8_t display_enable_show(uint16_t args1, uint16_t args2, uint8_t args3)
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





bool draw_menu(bool redraw, uint8_t click_type, uint16_t click_x, uint16_t click_y)
{
  bool ret = false;
  char str1[128];
  char str2[32];
  char str3[32];
  const Element_Button_1 *button_1;
  const Element_Button_2 *button_2;
  const Element_Function_1 *function_1;
  const Element_Switch_1 *switch_1;
  const Element_Dyn_Button_1 *dyn_button_1;
  const Element_Dyn_Symbol_1 *dyn_symbol_1;
  const Element_Symbol_1 *symbol_button_1;
  const Element_Dyn_Select_1 *dyn_select_box_1;
  ret_fptr *rfnt;
  fptr_args *fntargs;
  fptr_coordinate_xy *fnt_coordinate_xy;
  ret_string_fptr *gss;
  const Menu1 *current;
  uint8_t state;
  uint8_t loop_cnt, loop_i, loop_t, loop_r;
  uint8_t index_items;
  uint16_t new_x;
  uint16_t new_y;
  uint16_t global_x;
  uint16_t global_y;
  uint8_t rdr;
  uint8_t menu_args1;
  uint8_t redraw_class;
  uint8_t active;
  uint8_t slider_active;

  current = MenuHistoryGetMenu(&menu_args1);

  global_x = pgm_read_word(&current->x);
  global_y = pgm_read_word(&current->y);


  rfnt = (ret_fptr*)pgm_read_word(&current->redraw_class_0);
  rdr = (ret_fptr(rfnt))(pgm_read_byte(&current->idx), menu_args1, -1);
  if (rdr == 0)
    cbi(redraw_class, 0);
  else
    sbi(redraw_class, 0);
  ///
  rfnt = (ret_fptr*)pgm_read_word(&current->redraw_class_1);
  rdr = (ret_fptr(rfnt))(pgm_read_byte(&current->idx), menu_args1, -1);
  if (rdr == 0)
    cbi(redraw_class, 1);
  else
    sbi(redraw_class, 1);
  ///
  rfnt = (ret_fptr*)pgm_read_word(&current->redraw_class_2);
  rdr = (ret_fptr(rfnt))(pgm_read_byte(&current->idx), menu_args1, -1);
  if (rdr == 0)
    cbi(redraw_class, 2);
  else
    sbi(redraw_class, 2);
  ///
  rfnt = (ret_fptr*)pgm_read_word(&current->redraw_class_3);
  rdr = (ret_fptr(rfnt))(pgm_read_byte(&current->idx), menu_args1, -1);
  if (rdr == 0)
    cbi(redraw_class, 3);
  else
    sbi(redraw_class, 3);
  ///
  if (redraw == false)
    cbi(redraw_class, 4);
  else
    sbi(redraw_class, 4);
  /////
  if (display_redraw_once == 0)
  {
    display_redraw_once = 1;
    sbi(redraw_class, 5);
  }
  else
  {
    cbi(redraw_class, 5);
  }
  /////
  rdr = pgm_read_byte(&current->redraw_class);
  if (enable_redraw(rdr, redraw_class) == true)
  {
    strcpy_P(str1, (char*)pgm_read_word(&current->name));
    show_string(str1, pgm_read_word(&current->x) + 5, pgm_read_word(&current->y) + 5, 2, BLACK, WHITE, 0);
  }
  ///
  /// menu programy
  for (uint8_t idx = 0; idx < pgm_read_byte(&current->len_function_1); idx++)
  {
    function_1 = &current->function_1[idx];
    rfnt = (ret_fptr*)pgm_read_word(&function_1->enable_show);
    active = (ret_fptr(rfnt))(pgm_read_byte(&function_1->args), menu_args1, idx);
    if (active == 0) continue;
    rdr = pgm_read_byte(&function_1->redraw_class);
    if (enable_redraw(rdr, redraw_class) == true)
    {
      strcpy_P(str1, (char*)pgm_read_word(&function_1->name));
      fnt_coordinate_xy =  (fptr_coordinate_xy*)pgm_read_word(&function_1->fnt_coordinate_xy);
      ((fptr_coordinate_xy)fnt_coordinate_xy)(global_x + pgm_read_word(&function_1->x), global_y + pgm_read_word(&function_1->y), pgm_read_word(&function_1->size_x), pgm_read_word(&function_1->size_y), pgm_read_word(&function_1->args), menu_args1, str1);
    }
    if (click_x > 0 && click_y > 0)
      if (button_click_touch(global_x + pgm_read_word(&function_1->x), global_y + pgm_read_word(&function_1->y), pgm_read_word(&function_1->size_x), pgm_read_word(&function_1->size_y), click_x, click_y) == true)
      {
        fntargs =  (fptr_args*)pgm_read_word(&function_1->onclick);
        if ((fptr_args)fntargs != nullfce)
        {
          ((fptr_args)fntargs)(pgm_read_word(&function_1->args), menu_args1, idx);
          ret = true;
          printf("mrdka funkce\n");
        }
      }
  }
  if (ret == true)  goto draw_menu_end;
  ///
  /// tlacitko typ 1 se prekresluje s celym menu
  /// tlacitko typ 1
  for (uint8_t idx = 0; idx < pgm_read_byte(&current->len_button_1); idx++)
  {
    button_1 = &current->button_1[idx];
    rfnt = (ret_fptr*)pgm_read_word(&button_1->enable_show);
    active = (ret_fptr(rfnt))(pgm_read_byte(&button_1->args), menu_args1, idx);
    if (active == 0) continue;
    rdr = pgm_read_byte(&button_1->redraw_class);
    if (enable_redraw(rdr, redraw_class) == true)
    {
      strcpy_P(str2, (char*)pgm_read_word(&button_1->name));
      str1[0] = 0;
      button_click_1(global_x + pgm_read_word(&button_1->x), global_y + pgm_read_word(&button_1->y), pgm_read_word(&button_1->size_x), pgm_read_word(&button_1->size_y), pgm_read_byte(&button_1->font_size), pgm_read_byte(&button_1->font_size), str2, str1, 0);
    }
    if (click_x > 0 && click_y > 0)
      if (button_click_touch(global_x + pgm_read_word(&button_1->x), global_y + pgm_read_word(&button_1->y), pgm_read_word(&button_1->size_x), pgm_read_word(&button_1->size_y), click_x, click_y) == true)
      {
        fntargs = (fptr_args*)pgm_read_word(&button_1->onclick);
        ((fptr_args)fntargs)(pgm_read_byte(&button_1->args), menu_args1, idx);
        ret = true;
      }
  }
  if (ret == true)  goto draw_menu_end;
  ///
  /// tlacitko typ 2
  for (uint8_t idx = 0; idx < pgm_read_byte(&current->len_button_2); idx++)
  {
    button_2 = &current->button_2[idx];
    rfnt = (ret_fptr*)pgm_read_word(&button_2->enable_show);
    active = (ret_fptr(rfnt))(pgm_read_byte(&button_2->args), menu_args1, idx);
    if (active == 0) continue;
    rdr = pgm_read_byte(&button_2->redraw_class);
    if (enable_redraw(rdr, redraw_class) == true)
    {
      strcpy_P(str2, (char*)pgm_read_word(&button_2->name));
      rfnt = (ret_fptr*)pgm_read_word(&button_2->get_status_fnt);
      state = ((ret_fptr)rfnt)(pgm_read_byte(&button_2->args), menu_args1, idx);
      /// pro REDRAW_ONCE, kdyz mi vyjde stev 2, to je neni na tlacitku zadna zmena, tak vykresli jako neaktivni, bez teto podminky, by se nic neukazalo
      if ((enable_redraw(redraw_class, (1 << REDRAW_ONCE)) == true) && (state == 2)) state = 0;
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
        fntargs =  (fptr_args*)pgm_read_word(&button_2->onclick);
        ((fptr_args)fntargs)(pgm_read_byte(&button_2->args), menu_args1, idx);
        ret = true;
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
      strcpy_P(str2, (char*)pgm_read_word(&switch_1->name));
      rfnt = (ret_fptr*)pgm_read_word(&switch_1->get_status_fnt);
      gss = (ret_string_fptr*)pgm_read_word(&switch_1->get_status_string);
      state = ((ret_fptr)rfnt)(pgm_read_byte(&switch_1->args), menu_args1, idx);
      str3[0] = 0;
      ((ret_string_fptr)gss)(pgm_read_byte(&switch_1->args), menu_args1, pgm_read_byte(&switch_1->args), str1, str3);
      button_switch_1(global_x + pgm_read_word(&switch_1->x), \
                      global_y + pgm_read_word(&switch_1->y), \
                      pgm_read_word(&switch_1->size_x), \
                      pgm_read_word(&switch_1->size_y), \
                      pgm_read_word(&switch_1->font_size), state, str2, str1);

    }
    if (click_x > 0 && click_y > 0)
      if (button_click_touch(global_x + pgm_read_word(&switch_1->x), global_y + pgm_read_word(&switch_1->y), pgm_read_word(&switch_1->size_x), pgm_read_word(&switch_1->size_y), click_x, click_y) == true)
      {
        fntargs =  (fptr_args*)pgm_read_word(&switch_1->onclick);
        ((fptr_args)fntargs)(pgm_read_byte(&switch_1->args), menu_args1, idx);
        ret = true;
      }
  }
  if (ret == true)  goto draw_menu_end;
  ////
  ////
  for (uint8_t idx = 0; idx < pgm_read_byte(&current->len_symbol_button_1); idx++)
  {
    symbol_button_1 = &current->symbol_button_1[idx];
    rfnt = (ret_fptr*)pgm_read_word(&symbol_button_1->enable_show);
    active = (ret_fptr(rfnt))(pgm_read_byte(&symbol_button_1->args), menu_args1, idx);
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
        fntargs =  (fptr_args*)pgm_read_word(&symbol_button_1->onclick);
        ((fptr_args)fntargs)(pgm_read_byte(&symbol_button_1->args), menu_args1, idx);
        ret = true;
      }
  }
  if (ret == true)  goto draw_menu_end;
  ////
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
      rfnt = (ret_fptr*)pgm_read_word(&dyn_symbol_1->function_for_max_items);
      loop_cnt = ((ret_fptr)rfnt)(pgm_read_byte(&dyn_symbol_1->args), menu_args1, loop_i);
      gss = (ret_string_fptr*)pgm_read_word(&dyn_symbol_1->get_status_string);
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
        ((ret_string_fptr)gss)(loop_i, menu_args1, pgm_read_byte(&dyn_symbol_1->args), str1, str2);
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
      rfnt = (ret_fptr*)pgm_read_word(&dyn_symbol_1->function_for_max_items);
      loop_cnt = ((ret_fptr)rfnt)(pgm_read_byte(&dyn_symbol_1->args), menu_args1, loop_i);
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
          fntargs =  (fptr_args*)pgm_read_word(&dyn_symbol_1->dyn_symbol_onclick);
          ((fptr_args)fntargs)(pgm_read_byte(&dyn_symbol_1->args), menu_args1, loop_i);
          ret = true;
          break;
        }
        loop_i++;
      }
    }
  }
  if (ret == true)  goto draw_menu_end;
  ////
  ////
  for (uint8_t idx = 0; idx < pgm_read_byte(&current->len_dyn_button_1); idx++)
  {
    dyn_button_1 = &current->dyn_button[idx];
    rfnt = (ret_fptr*)pgm_read_word(&dyn_button_1->enable_show);
    active = (ret_fptr(rfnt))(pgm_read_byte(&dyn_button_1->args), menu_args1, idx);
    if (active == 0) continue;
    rdr = pgm_read_byte(&dyn_button_1->redraw_class);
    if (enable_redraw(rdr, redraw_class) == true)
    {
      loop_i = 0;
      loop_t = 0;
      rfnt = (ret_fptr*)pgm_read_word(&dyn_button_1->function_for_max_items);
      loop_cnt = ((ret_fptr)rfnt)(pgm_read_byte(&dyn_button_1->args), menu_args1, loop_i);
      gss = (ret_string_fptr*)pgm_read_word(&dyn_button_1->get_status_string);

      slider_active = pgm_read_byte(&dyn_button_1->slider_args);
      if (slider_active != MENU_SLIDER_OFF)
        process_display_element_slider(&loop_i, &loop_cnt, slider_active, pgm_read_byte(&dyn_button_1->max_items_count)*pgm_read_byte(&dyn_button_1->max_row_count));
      while (loop_i < loop_cnt)
      {
        new_x = global_x + pgm_read_word(&dyn_button_1->first_x);
        new_y = global_y + pgm_read_word(&dyn_button_1->first_y);
        if (pgm_read_byte(&dyn_button_1->direction) == VERTICAL)
          new_y = global_y + pgm_read_word(&dyn_button_1->first_y) + (pgm_read_byte(&dyn_button_1->step_y) * loop_t);
        if (pgm_read_byte(&dyn_button_1->direction) == HORIZONTAL)
          new_x = global_x + pgm_read_word(&dyn_button_1->first_x) + (pgm_read_byte(&dyn_button_1->step_x) * loop_t);
        ((ret_string_fptr)gss)(loop_i, menu_args1, pgm_read_byte(&dyn_button_1->args) , str1, str2);
        button_click_1(new_x, new_y , pgm_read_word(&dyn_button_1->size_x), pgm_read_word(&dyn_button_1->size_y), pgm_read_byte(&dyn_button_1->font_size), pgm_read_byte(&dyn_button_1->font_size), str1, str2, 1);
        loop_t++;
        loop_i++;
      }
    }
    if (click_x > 0 && click_y > 0)
    {
      loop_i = 0;
      loop_t = 0;
      rfnt = (ret_fptr*)pgm_read_word(&dyn_button_1->function_for_max_items);
      loop_cnt = ((ret_fptr)rfnt)(pgm_read_byte(&dyn_button_1->args), menu_args1, loop_i);

      slider_active = pgm_read_byte(&dyn_button_1->slider_args);
      if (slider_active != MENU_SLIDER_OFF)
        process_display_element_slider(&loop_i, &loop_cnt, slider_active, pgm_read_byte(&dyn_button_1->max_items_count)*pgm_read_byte(&dyn_button_1->max_row_count));
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
          fntargs =  (fptr_args*)pgm_read_word(&dyn_button_1->dyn_button_onclick);
          ((fptr_args)fntargs)(pgm_read_byte(&dyn_button_1->args), menu_args1, loop_i);
          ret = true;
          break;
        }
        loop_t++;
        loop_i++;
      }
    }
  }
  if (ret == true)  goto draw_menu_end;
  ////
  for (uint8_t idx = 0; idx < pgm_read_byte(&current->len_dyn_select_box_1); idx++)
  {
    dyn_select_box_1 = &current->dyn_select_box_1[idx];
    rfnt = (ret_fptr*)pgm_read_word(&dyn_select_box_1->enable_show);
    active = (ret_fptr(rfnt))(pgm_read_byte(&dyn_select_box_1->args), menu_args1, idx);
    if (active == 0) continue;
    rdr = pgm_read_byte(&dyn_select_box_1->redraw_class);
    if (enable_redraw(rdr, redraw_class) == true)
    {
      loop_i = 0;
      loop_t = 0;
      loop_r = 0;
      rfnt = (ret_fptr*)pgm_read_word(&dyn_select_box_1->function_for_max_items);
      loop_cnt = ((ret_fptr)rfnt)(pgm_read_byte(&dyn_select_box_1->args), menu_args1, loop_i);
      gss = (ret_string_fptr*)pgm_read_word(&dyn_select_box_1->get_status_string);
      slider_active = pgm_read_byte(&dyn_select_box_1->slider_args);
      if (slider_active != MENU_SLIDER_OFF)
        process_display_element_slider(&loop_i, &loop_cnt, slider_active, pgm_read_byte(&dyn_select_box_1->max_items_count)*pgm_read_byte(&dyn_select_box_1->max_row_count));
      while (loop_i < loop_cnt)
      {
        if (pgm_read_byte(&dyn_select_box_1->direction) == VERTICAL)
        {
          new_x = global_x + pgm_read_word(&dyn_select_box_1->first_x);
          new_y = global_y + pgm_read_word(&dyn_select_box_1->first_y) + (pgm_read_byte(&dyn_select_box_1->step_y) * loop_t);
          loop_t++;
        }
        if (pgm_read_byte(&dyn_select_box_1->direction) == HORIZONTAL)
        {
          new_y = global_y + pgm_read_word(&dyn_select_box_1->first_y);
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
        ((ret_string_fptr)gss)(loop_i, menu_args1, pgm_read_byte(&dyn_select_box_1->args), str1, str2);
        rfnt = (ret_fptr*)pgm_read_word(&dyn_select_box_1->get_status_fnt);
        state = ((ret_fptr)rfnt)(pgm_read_byte(&dyn_select_box_1->args), menu_args1, loop_i);
        /// pro REDRAW_ONCE, kdyz mi vyjde stev 2, to je neni na tlacitku zadna zmena, tak vykresli jako neaktivni, bez teto podminky, by se nic neukazalo
        if ((enable_redraw(redraw_class, (1 << REDRAW_ONCE)) == true) && (state == BUTTON_NO_SHOW)) state = BUTTON_NO_ACTIVE;
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
      rfnt = (ret_fptr*)pgm_read_word(&dyn_select_box_1->function_for_max_items);
      loop_cnt = ((ret_fptr)rfnt)(pgm_read_byte(&dyn_select_box_1->args), menu_args1, loop_i);
      slider_active = pgm_read_byte(&dyn_select_box_1->slider_args);
      if (slider_active != MENU_SLIDER_OFF)
        process_display_element_slider(&loop_i, &loop_cnt, slider_active, pgm_read_byte(&dyn_select_box_1->max_items_count)*pgm_read_byte(&dyn_select_box_1->max_row_count));
      while (loop_i < loop_cnt)
      {
        if (pgm_read_byte(&dyn_select_box_1->direction) == VERTICAL)
        {
          new_x = global_x + pgm_read_word(&dyn_select_box_1->first_x);
          new_y = global_y + pgm_read_word(&dyn_select_box_1->first_y) + (pgm_read_byte(&dyn_select_box_1->step_y) * loop_t);
          loop_t++;
        }
        if (pgm_read_byte(&dyn_select_box_1->direction) == HORIZONTAL)
        {
          new_y = global_y + pgm_read_word(&dyn_select_box_1->first_y);
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
          fntargs =  (fptr_args*)pgm_read_word(&dyn_select_box_1->dyn_symbol_onclick);
          ((fptr_args)fntargs)(pgm_read_byte(&dyn_select_box_1->args), menu_args1, loop_i);
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
  const Menu1 *current;
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
  rfnt = (ret_fptr*)pgm_read_word(&current->preload_function);
  (ret_fptr(rfnt))(pgm_read_byte(&current->idx), args1, -1);

  display_redraw_once = 0;
}






/////////////////////////////////////////////////////////////////////////////////////
void MenuHistoryPrevMenu(uint16_t args1, uint16_t args2, uint8_t args3)
{
  if (MenuHistoryIndex > 0)
  {
    MenuHistoryIndex--;
    MenuPrepareStyle();
  }
}
///
void MenuHistoryNextMenu(uint16_t id, uint16_t args1, uint8_t args2)
{
  if (MenuHistoryIndex < MENU_MAX_HISTORY)
  {
    MenuHistoryIndex++;
    MenuHistory[MenuHistoryIndex] = id;
    Global_menu_args1[MenuHistoryIndex] = args1;
    Global_menu_args2[MenuHistoryIndex] = args2;
    MenuPrepareStyle();
  }
}
///
void MenuHistoryUpdateArgs1(uint8_t args1)
{
  Global_menu_args1[MenuHistoryIndex] = args1;
}
void MenuHistoryUpdateArgs2(uint8_t args2)
{
  Global_menu_args2[MenuHistoryIndex] = args2;
}
///
void MenuHistoryInit(void)
{
  MenuHistory[MenuHistoryIndex] = MENU_DEFAULT_SCREEN;
}
///
const Menu1 *MenuHistoryGetMenu(uint8_t *args1)
{
  const Menu1 *menus;
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

  for (uint8_t idx = 0; idx < pgm_read_byte(&Menu_All.len_menu4); idx++)
  {
    menus = &Menu_All.ListMenu4[idx];
    if (pgm_read_byte(&menus->idx) == MenuHistory[MenuHistoryIndex])
    {
      *args1 = Global_menu_args1[MenuHistoryIndex];
      return menus;
    }
  }
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
void DisplayClean(uint16_t color)
{
  my_lcd.Fill_Screen(color);
}
//////////////////////////////////////////////////////////////////////////////////

/*
  args2 -- menu_dialog_set_variable_now, aktualni hodnota
  args1 -- menu_dialog_set_variable_args, id ringu
  args3 -- menu_dialog_set_variable_now, aktualni hodnota uint8_t format
*/
void helper_thermostat_set_mezni(uint16_t args1, uint8_t args2, float now, uint8_t args3)
{
  thermostat_ring_set_mezni(args1, (now * 10.0));
}
void helper_thermostat_set_pid_p(uint16_t args1, uint8_t args2, float now, uint8_t args3)
{
  thermostat_ring_pid_set_kp(args1, now);
}
void helper_thermostat_set_pid_i(uint16_t args1, uint8_t args2, float now, uint8_t args3)
{
  thermostat_ring_pid_set_ki(args1, now);
}
void helper_thermostat_set_pid_d(uint16_t args1, uint8_t args2, float now, uint8_t args3)
{
  thermostat_ring_pid_set_kd(args1, now);
}
void helper_thermostat_set_pid_time(uint16_t args1, uint8_t args2, float now, uint8_t args3)
{
  thermostat_ring_pid_set_time(args1, args3);
}
void button_click_ring_term_set_pid_default(uint16_t args1, uint16_t args2, uint8_t args3)
{
  thermostat_ring_pid_set_kp(args2, 1.0);
  thermostat_ring_pid_set_ki(args2, 0.5);
  thermostat_ring_pid_set_kd(args2, 1.0);
  thermostat_ring_pid_set_time(args2, 60);
  preload_pid_menu(0, args2, 0);
}


uint8_t preload_regulator_menu(uint16_t args1, uint16_t args2, uint8_t args3)
{
  display_function_set_variable(thermostat_ring_get_mezni(args2) / 10.0, 16.0, 32.0, 0.5, args2, 0, NUMBER_TYPE_FLOAT,  H_TRUE, DIALOG_SET_VARIABLE_GENERAL, &helper_thermostat_set_mezni);
}


/// TODO spravne predat id ringu
uint8_t preload_pid_menu(uint16_t args1, uint16_t args2, uint8_t args3)
{
  display_function_set_variable(thermostat_get_pid_p(args2), -10.0, 10.0, 0.1, args2, 0, NUMBER_TYPE_FLOAT,  H_TRUE, DIALOG_SET_VARIABLE_PID_P, &helper_thermostat_set_pid_p);
  display_function_set_variable(thermostat_get_pid_i(args2), -10.0, 10.0, 0.1, args2, 0, NUMBER_TYPE_FLOAT,  H_TRUE, DIALOG_SET_VARIABLE_PID_I, &helper_thermostat_set_pid_i);
  display_function_set_variable(thermostat_get_pid_d(args2), -10.0, 10.0, 0.1, args2, 0, NUMBER_TYPE_FLOAT,  H_TRUE, DIALOG_SET_VARIABLE_PID_D, &helper_thermostat_set_pid_d);
  display_function_set_variable(thermostat_get_pid_time(args2),  10, 254, 2, args2, 0, NUMBER_TYPE_INT,  H_TRUE, DIALOG_SET_VARIABLE_PID_T, &helper_thermostat_set_pid_time);
}

/*
   args2 je idprogramu z menu
*/
uint8_t preload_program_interval_menu(uint16_t args1, uint16_t args2, uint8_t args3)
{
  uint8_t program_id = Global_menu_args2[MenuHistoryIndex];
  uint8_t interval_id = args2;
  display_function_set_variable(thermostat_program_get_threshold(program_id, interval_id) / 10.0, 16.0, 32.0, 0.5, program_id, interval_id, NUMBER_TYPE_FLOAT,  H_TRUE, DIALOG_SET_VARIABLE_GENERAL, &helper_program_time_interval_temp);
}

/*
   args1 .... program id
   args2 .... interval id
   now .... float teplota
*/
void helper_program_time_interval_temp(uint16_t args1, uint8_t args2, float now, uint8_t args3)
{
  thermostat_program_set_threshold(args1, args2, now * 10);
}
/////////////////////////////////////////////////////////////////////////////////
/*****************************************************************************************************************************/
/*
   pomocne funkce pro nastavovani promenych
   idx.. je argument idx, index ridicich promenych
   kdyz je vice techto dialogu v jednom nenu
*/

uint8_t display_function_get_variable_args1(uint8_t idx)
{
  return menu_dialog_variable[idx].args1;
}
uint8_t display_function_get_variable_args2(uint8_t idx)
{
  return menu_dialog_variable[idx].args2;
}

float display_function_get_variable_float(uint8_t idx)
{
  return menu_dialog_variable[idx].variable_now ;
}

int display_function_get_variable_int(uint8_t idx)
{
  return menu_dialog_variable[idx].variable_now ;
}

void display_function_set_variable_minus(uint16_t idx, uint16_t args2, uint8_t args3)
{
  if (menu_dialog_variable[idx].variable_now - menu_dialog_variable[idx].variable_step >= menu_dialog_variable[idx].variable_min)
  {
    menu_dialog_variable_change = 1;
    menu_dialog_variable[idx].variable_now = menu_dialog_variable[idx].variable_now  - menu_dialog_variable[idx].variable_step;
    if (menu_dialog_variable[idx].update_now == H_TRUE)
    {
      menu_dialog_variable[idx].save_function(menu_dialog_variable[idx].args1, menu_dialog_variable[idx].args2, menu_dialog_variable[idx].variable_now, float_to_int(menu_dialog_variable[idx].variable_now));
    }
  }
}
void display_function_set_variable_plus(uint16_t idx, uint16_t args2, uint8_t args3)
{
  if ((menu_dialog_variable[idx].variable_now + menu_dialog_variable[idx].variable_step) <= menu_dialog_variable[idx].variable_max)
  {
    menu_dialog_variable_change = 1;
    menu_dialog_variable[idx].variable_now = menu_dialog_variable[idx].variable_now + menu_dialog_variable[idx].variable_step;
    if (menu_dialog_variable[idx].update_now == H_TRUE)
    {
      menu_dialog_variable[idx].save_function(menu_dialog_variable[idx].args1, menu_dialog_variable[idx].args2, menu_dialog_variable[idx].variable_now, float_to_int(menu_dialog_variable[idx].variable_now));
    }
  }
}
void display_function_set_variable(float now, float min, float max, float step, uint8_t args1, uint8_t args2, uint8_t number_type, uint8_t update_now, uint8_t idx, fptr_save_function save_function)
{
  menu_dialog_variable[idx].variable_now = now;
  menu_dialog_variable[idx].variable_min = min;
  menu_dialog_variable[idx].variable_max = max;
  menu_dialog_variable[idx].variable_step = step;
  menu_dialog_variable[idx].number_type = number_type;
  menu_dialog_variable[idx].args1 = args1;
  menu_dialog_variable[idx].args2 = args2;

  menu_dialog_variable[idx].update_now = update_now;
  menu_dialog_variable[idx].save_function = save_function;
}
/////
void display_element_set_string(char *str, uint8_t max_length, uint8_t args1, uint8_t args2, fptr_args save_function, ret_fptr_no_args valid_function)
{
  strcpy(dialog_set_string, str);
  dialog_set_string_max_length = max_length;
  dialog_set_string_args1 = args1;
  dialog_set_string_args2 = args2;
  dialog_save_string_function = save_function;
  dialog_save_valid_function = valid_function;
}

uint8_t display_element_get_string_args1(void)
{
  return dialog_set_string_args1;
}

uint8_t display_element_get_string_args2(void)
{
  return dialog_set_string_args2;
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

void display_element_set_string_del_char(uint16_t args1, uint16_t idx, uint8_t args3)
{
  uint8_t len = strlen(dialog_set_string);
  if (len > 0)
    dialog_set_string[len - 1] = 0;
}

/////
/*
   args1 index polozky pomocnych promenych pro nastavovaci dialog
*/
void menu_tds_save_offset(uint16_t args1, uint8_t args2, float now, uint8_t args3)
{
  tds_set_offset(display_function_get_variable_args1(args1), display_function_get_variable_float(args1) * 10);
}

void menu_tds_save_period(uint16_t args1, uint8_t args2, float now, uint8_t args3)
{
  tds_set_period(display_function_get_variable_args1(args1), display_function_get_variable_float(args1));
}
/////
void menu_tds_save_name(uint16_t args1, uint16_t args2, uint8_t args3)
{
  char name[10];
  display_element_get_string(name);
  if (strlen(name) > 0)
    tds_set_name(display_element_get_string_args1(), name);
}


//////////////////////////////////////////////////////////////////////////////////
/*
   funkce ktere se volaji z menu tlacitek
   args1 ... je parametr z definice tlacitka, pouzito jako index ukazatele pomocnych promenych pro nastavovaci dialog
   args2 ... je dynamicky parametr nastaveni menu

*/
/// funkce pro nastaveni offsetu tds
void display_menu_tds_set_offset(uint16_t args1, uint16_t args2, uint8_t args3)
{
  MenuHistoryNextMenu(MENU_DIALOG_SET_VARIABLE, 0, 0);
  display_function_set_variable(tds_get_offset(args2) / 10.0, -10, 10, 0.1, args2, 0, NUMBER_TYPE_FLOAT, H_FALSE, args1, &menu_tds_save_offset);
  //dialog_save_variable_function = ;
}

/// funkce pro nastaveni merici periody
void display_menu_tds_set_period(uint16_t args1, uint16_t args2, uint8_t args3)
{
  MenuHistoryNextMenu(MENU_DIALOG_SET_VARIABLE, 0, 0);
  display_function_set_variable((float)tds_get_period(args2), 1, 255, 1, args2, 0, NUMBER_TYPE_INT , H_FALSE, args1, &menu_tds_save_period);
  //dialog_save_variable_function = ;
}

/// funkce pro nastaveni nazvu cidla
void display_menu_tds_set_name(uint16_t args1, uint16_t args2, uint8_t args3)
{
  char name[RTDS_DEVICE_STRING_LEN];
  tds_get_name(args2, name);
  MenuHistoryNextMenu(MENU_DIALOG_KEYBOARD_ALFA, 0, 0);
  display_element_set_string(name, 8, args2, 0, &menu_tds_save_name, &valid_true);
}
/// funkce pro nastaveni nazvu rtds
void display_menu_rtds_update_name(uint16_t args1, uint16_t args2, uint8_t args3)
{
  char name[RTDS_DEVICE_STRING_LEN];
  uint8_t active;
  remote_tds_get_complete(args2, &active, name);
  MenuHistoryNextMenu(MENU_DIALOG_KEYBOARD_ALFA, 0, 0);
  display_element_set_string(name, RTDS_DEVICE_STRING_LEN, args2, 0, &menu_rtds_update_name, &valid_true);
  remote_tds_unsubscibe_topic(args2);
}
////////////////////////////////////////////////////////

/*
   funkce pro nastaveni budiku
*/
uint8_t get_function_budik_enabled(uint16_t args1, uint16_t args2, uint8_t args3)
{
  return BUTTON_ACTIVE;
}
void get_function_budik_text_state(uint8_t args1, uint8_t args2, uint8_t args3, char *line1, char *line2)
{
  strcpy(line1, "Zapnuto 06:45");
  line2[0] = 0;
}



////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
/// funkce, ktera vraci pocet polozek dynamickeho menu
uint8_t get_function_keyboard_number_max_keys(uint16_t args1, uint16_t args2, uint8_t args3)
{
  return 12;
}
/*
   args1 ... argument ze statickeho menu nastaveni
   args2 ... globalni argument menu
   args3 ... index prvku
*/
void click_keyboard_number(uint16_t args1, uint16_t args2, uint8_t args3)
{
  char znak;
  znak = '0' + args3;
  if (args3 == 10) znak = '.';
  if (args3 == 11) znak = ':';
  display_element_set_string_add_char(znak);
}
///////////////////////////
/*
   args1 - index prvku
   args2
*/
void get_function_keyboard_number_char(uint8_t args1, uint8_t args2, uint8_t args3, char *line1, char *line2)
{
  line1[0] = '0' + args1;
  if (args1 == 10) line1[0] = '.';
  if (args1 == 11) line1[0] = ':';
  line2[0] = 0;
}

/////
void get_function_keyboard_alfa_char(uint8_t args1, uint8_t args2, uint8_t args3, char *line1, char *line2)
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
   args2 ... globalni argument menu
   args3 ... index prvku
*/
void click_keyboard_alfa(uint16_t args1, uint16_t args2, uint8_t args3)
{
  char znak;
  if (dialog_set_string_keyboard_type == KEYBOARD_TYPE_UPPER)
    znak = pgm_read_byte(&keyboad_number_char_upper[args3]);
  if (dialog_set_string_keyboard_type == KEYBOARD_TYPE_LOWER)
    znak = pgm_read_byte(&keyboad_number_char_lower[args3]);
  if (dialog_set_string_keyboard_type == KEYBOARD_TYPE_SPECIAL)
    znak = pgm_read_byte(&keyboad_number_char_special[args3]);
  ///
  display_element_set_string_add_char(znak);
}
/// vraci pocet prvku pro zobrazeni
uint8_t get_function_keyboard_alfa_max_keys(uint16_t args1, uint16_t args2, uint8_t args3)
{
  if (dialog_set_string_keyboard_type == KEYBOARD_TYPE_UPPER)
    return KEYBOARD_SIZE_CHAR_UPPER;
  if (dialog_set_string_keyboard_type == KEYBOARD_TYPE_LOWER)
    return KEYBOARD_SIZE_CHAR_LOWER;
  if (dialog_set_string_keyboard_type == KEYBOARD_TYPE_SPECIAL)
    return KEYBOARD_SIZE_CHAR_SPECIAL;
}

void click_keyboard_type(uint16_t args1, uint16_t args2, uint8_t args3)
{
  dialog_set_string_keyboard_type = args1;
}

////////////////////////////////////////////////////////////
/// funkce pro obsluhu dialogu
/*
   args1 - jsou data z definice funkce
   args2 - jsou globalni data z definice menu
*/
void click_dialog_button_yes(uint16_t args1, uint16_t args2, uint8_t args3)
{
  dialog_yes_function(dialog_yes_args1, 0, 0);
  MenuHistoryPrevMenu(0, 0, 0);
}

void dialog_set_variable_button_click(uint16_t args1, uint16_t args2, uint8_t args3)
{
  menu_dialog_variable[args1].save_function(args1, 0, args2, args3);
  MenuHistoryPrevMenu(0, 0, 0);
}

void dialog_set_string_button_click(uint16_t args1, uint16_t args2, uint8_t args3)
{
  if (dialog_save_valid_function() == 1)
  {
    dialog_save_string_function(args1, args2, args3);
    MenuHistoryPrevMenu(0, 0, 0);
  }
}





uint8_t button_redraw(uint8_t args1, uint8_t args2)
{
  return 0;
}


uint8_t menu_redraw_time05s(uint16_t args1, uint16_t args2, uint8_t args3)
{
  uint8_t ret = 0;
  /*
    if (menu_redraw05s == 1)
    {
      menu_redraw05s = 0;
      return 1;
    }
    return 0;
  */


  if (selftest_data_0 != last_selftest_data_0)
  {
    last_selftest_data_0 = selftest_data_0;
    ret = 1;
  }

  if (selftest_data_1 != last_selftest_data_1)
  {
    last_selftest_data_1 = selftest_data_1;
    ret = 1;
  }

  if (last_time_hour != now.hour() || last_time_minute != now.minute())
  {
    last_time_hour = now.hour();
    last_time_minute = now.minute();
    ret = 1;
  }

  return ret;
}

uint8_t menu_redraw_change_auto_brightness(uint16_t args1, uint16_t args2, uint8_t args3)
{
  uint8_t ret = 0;
  /// refresh pouze kdyz je automaticky rezim
  if ((change_auto_brightness == 1) && ((brigthness_display_mode & (1 << DISPLAY_MODE_AUTO_BRIGHTNESS)) != 0) )
  {
    change_auto_brightness = 0;
    ret = 1;
  }
  return ret;
}

uint8_t menu_redraw_change_term_mode(uint16_t args1, uint16_t args2, uint8_t args3)
{
  if (change_term_mode == 1)
  {
    change_term_mode = 0;
    return 1;
  }
  return 0;
}

uint8_t menu_redraw_change_term_mode_cool_heat(uint16_t args1, uint16_t args2, uint8_t args3)
{

  if (change_term_mode == 2)
  {
    change_term_mode = 0;
    return 1;
  }
  return 0;
}

uint8_t menu_redraw_change_term_input(uint16_t args1, uint16_t args2, uint8_t args3)
{
  if (change_term_mode == 4)
  {
    change_term_mode = 0;
    return 1;
  }
  return 0;
}

uint8_t menu_redraw_change_term_program_active(uint16_t args1, uint16_t args2, uint8_t args3)
{
  if (change_term_mode == 8)
  {
    change_term_mode = 0;
    return 1;
  }
  return 0;
}

uint8_t menu_redraw_update_temp(uint16_t args1, uint16_t args2, uint8_t args3)
{
  if (menu_redraw10s == 1)
  {
    menu_redraw10s = 0;
    return 1;
  }
  return 0;
}


uint8_t menu_redraw_date(uint16_t args1, uint16_t args2, uint8_t args3)
{
  if (now.year() != last_year || now.month() != last_month || now.day() != last_day || display_update_date == 1)
  {
    display_update_date = 0;
    last_year = now.year();
    last_month = now.month();
    last_day = now.day();
    return 1;
  }
  return 0;
}



uint8_t menu_redraw_slider(uint16_t args1, uint16_t args2, uint8_t args3)
{
  if (display_update_slider == 1)
  {
    display_update_slider = 0;
    return 1;
  }
  return 0;
}


uint8_t menu_redraw_change_default_ring(uint16_t args1, uint16_t args2, uint8_t args3)
{
  if (display_redraw_default_ring == 1)
  {
    display_redraw_default_ring = 0;
    return 1;
  }
  return 0;
}

uint8_t menu_redraw_change_variable(uint16_t args1, uint16_t args2, uint8_t args3)
{
  if (menu_dialog_variable_change == 1)
  {
    menu_dialog_variable_change = 0;
    return 1;
  }
  return 0;
}

uint8_t menu_redraw_change_virtual_output(uint16_t args1, uint16_t args2, uint8_t args3)
{
  if (change_virtual_output == 4)
  {
    change_virtual_output = 0;
    return 1;
  }
  return 0;
}

uint8_t menu_redraw_change_display_brightness_mode(uint16_t args1, uint16_t args2, uint8_t args3)
{
  uint8_t ret = 0;
  if (last_brigthness_display_mode != brigthness_display_mode)
  {
    last_brigthness_display_mode = brigthness_display_mode;
    ret = 1;
  }
  return ret;
}

uint8_t menu_redraw_dialog_ok(uint16_t args1, uint16_t args2, uint8_t args3)
{
  uint8_t ret = 0;
  if (menu_dialog_change == 1)
  {
    ret = 1;
    menu_dialog_change = 0;
  }

  return ret;
}

uint8_t get_function_return_args_1(uint16_t args1, uint16_t args2, uint8_t args3)
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
  sprintf_P(buf, new_text_ip_format, ip[0], ip[1], ip[2], ip[3]);
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
    strcpy_P(tmp2, new_text_slash_rtds_slash); /// /rtds/
    strcat(tmp2, tmp1);
    strcat(tmp2, "/#");
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
    strcpy_P(tmp2, new_text_slash_rtds_slash);
    strcat(tmp2, tmp1);
    strcat(tmp2, "/#");
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
void remote_tds_set_type(uint8_t idx, uint8_t type)
{
  if (idx < MAX_RTDS)
  {
    SRAM.writeByte(ram_remote_tds_store_type + (ram_remote_tds_store_size * idx), type);
  }
}
uint8_t remote_tds_get_type(uint8_t idx)
{
  uint8_t type = 255;
  if (idx < MAX_RTDS)
  {
    type = SRAM.readByte(ram_remote_tds_store_type + (ram_remote_tds_store_size * idx));
  }
  return type;
}

void remote_tds_set_data(uint8_t idx, int value)
{
  if (idx < MAX_RTDS)
  {
    SRAM.writeByte(ram_remote_tds_store_data_low + (ram_remote_tds_store_size * idx), (value & 0xff));
    SRAM.writeByte(ram_remote_tds_store_data_high + (ram_remote_tds_store_size * idx), ((value >> 8) & 0xff));
    SRAM.writeByte(ram_remote_tds_store_last_update + (ram_remote_tds_store_size * idx), 0);
  }
}
int remote_tds_get_data(uint8_t idx)
{
  int value = 0;
  if (idx < MAX_RTDS)
  {
    value = SRAM.readByte(ram_remote_tds_store_data_high + (ram_remote_tds_store_size * idx)) << 8;
    value = value + SRAM.readByte(ram_remote_tds_store_data_low + (ram_remote_tds_store_size * idx));
  }
  return value;
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
    {
      last++;
      SRAM.writeByte(ram_remote_tds_store_last_update + (ram_remote_tds_store_size * idx), last);
    }
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void new_parse_at(char *input, char *out1, char *out2, char delim)
{
  uint8_t count = 0;
  uint8_t q = 0;
  out1[0] = 0;
  out2[0] = 0;

  while ( (count < strlen(input)) && (input[count] != delim) && (q < MAX_TEMP_BUFFER - 1))
  {
    out1[q] = input[count];
    out1[q + 1] = 0;
    q++;
    count++;
  }

  count++;
  q = 0;
  while ((count < strlen(input)) && (q < MAX_TEMP_BUFFER - 1) )
  {
    out2[q] = input[count];
    out2[q + 1] = 0;
    q++;
    count++;
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*


          NRF REMOTE DEVICE

*/

/*
   inicializace vychozich promenych v RAM
*/
void nrf_init_nei_store(void)
{
  for (uint8_t idx = 0; idx < ram_nrf_devices; idx++)
  {
    nrf_set_default_device(idx);
  }
}

uint8_t nrf_set_default_device(uint8_t idx)
{
  uint8_t ret = 0;
  if (idx < ram_nrf_devices)
  {
    nrf_set_nei_name(idx, "FREE");
    nrf_set_nei_last_update(idx, 250);
    nrf_set_nei_device_index(idx, 255);
    nrf_set_nei_device_used(idx, 0);
    ret = 1;
  }
  return ret;
}

/*
   zobrazeni ulozenych NRF pripojenych zarizeni
*/
uint8_t nrf_list_nei_store(uint8_t idx, char *name, uint8_t *device_index, uint8_t *last_seen)
{
  uint8_t ret = 0;
  if (nrf_get_nei_device_used(idx) == 1)
  {
    nrf_get_nei_name(idx, name);
    *device_index = nrf_get_nei_device_index(idx);
    *last_seen = nrf_get_nei_last_update(idx);
    ret = 1;
  }
  return ret;
}

uint8_t nrf_add_nei_store(uint8_t id, char *name)
{
  uint8_t ret = 0;
  uint8_t know = 0;
  for (uint8_t idx = 0; idx < ram_nrf_devices; idx++)
  {
    if (nrf_get_nei_device_used(idx) == 1 && nrf_get_nei_device_index(idx) == id)
    {
      know = 1;
      break;
    }
  }

  if (know == 0)
    for (uint8_t idx = 0; idx < ram_nrf_devices; idx++)
    {
      if (nrf_get_nei_device_used(idx) == 0)
      {
        nrf_set_nei_device_used(idx, 1);
        nrf_set_nei_name(idx, name);
        nrf_set_nei_last_update(idx, 0);
        nrf_set_nei_device_index(idx, id);
        ret = 1;
        break;
      }
    }
  return ret;
}

void nrf_set_nei_name(uint8_t idx, char *name)
{
  sram_set_name(ram_nrf_device_start + (idx * ram_nrf_device_store_size) + ram_nrf_device_name, name, ram_nrf_name_length);
}

void nrf_get_nei_name(uint8_t idx, char *name)
{
  sram_get_name(ram_nrf_device_start + (idx * ram_nrf_device_store_size) + ram_nrf_device_name, name, ram_nrf_name_length);
}

void nrf_set_nei_last_update(uint8_t idx, uint8_t last_seen)
{
  SRAM.writeByte(ram_nrf_device_start + (idx * ram_nrf_device_store_size) + ram_nrf_last_update, last_seen);
}

uint8_t nrf_get_nei_last_update(uint8_t idx)
{
  return SRAM.readByte(ram_nrf_device_start + (idx * ram_nrf_device_store_size) + ram_nrf_last_update);
}

void nrf_inc_nei_last_update(void)
{
  uint8_t cnt = 0;
  for (uint8_t idx = 0; idx < ram_nrf_name_length; idx++)
    if (nrf_get_nei_device_used(idx) == 1)
    {
      cnt = nrf_get_nei_last_update(idx);
      if (cnt < 250)
      {
        cnt++;
        nrf_set_nei_last_update(idx, cnt);
      }
    }
}

void nrf_reset_nei_last_update(uint8_t idx)
{
  uint8_t store_index;
  if (nrf_find_store_index(idx, &store_index) == 1 )
  {
    nrf_set_nei_last_update(store_index, 0);
  }
}

void nrf_set_nei_device_index(uint8_t idx, uint8_t device_index)
{
  SRAM.writeByte(ram_nrf_device_start + (idx * ram_nrf_device_store_size) + ram_nrf_device_index, device_index);
}
uint8_t nrf_get_nei_device_index(uint8_t idx)
{
  SRAM.readByte(ram_nrf_device_start + (idx * ram_nrf_device_store_size) + ram_nrf_device_index);
}



uint8_t nrf_find_store_index(uint8_t device_index, uint8_t *store_index)
{
  uint8_t ret = 0;
  for (uint8_t idx = 0; idx < ram_nrf_name_length; idx++)
  {
    if (nrf_get_nei_device_index(idx) == device_index)
    {
      *store_index = idx;
      ret = 1;
      break;
    }
  }
  return ret;
}


void nrf_set_nei_device_used(uint8_t idx, uint8_t used)
{
  SRAM.writeByte(ram_nrf_device_start + (idx * ram_nrf_device_store_size) + ram_nrf_device_used, used);
}

uint8_t nrf_get_nei_device_used(uint8_t idx)
{
  return SRAM.readByte(ram_nrf_device_start + (idx * ram_nrf_device_store_size) + ram_nrf_device_used);
}








void nrf_add_nei_store_service_info_uptime(uint8_t device_index, long uptime)
{
  uint8_t idx;
  if (nrf_find_store_index(device_index, &idx) == 1)
  {
    SRAM.writeByte(ram_nrf_device_info_start + (idx * ram_nrf_device_len) + ram_nrf_device_info_uptime, uptime >> 24);
    SRAM.writeByte(ram_nrf_device_info_start + (idx * ram_nrf_device_len) + ram_nrf_device_info_uptime + 1, uptime >> 16);
    SRAM.writeByte(ram_nrf_device_info_start + (idx * ram_nrf_device_len) + ram_nrf_device_info_uptime + 2, uptime >> 8);
    SRAM.writeByte(ram_nrf_device_info_start + (idx * ram_nrf_device_len) + ram_nrf_device_info_uptime + 3, uptime);
  }
}

long nrf_get_nei_store_service_info_uptime(uint8_t idx)
{
  long uptime = 0;
  uptime = SRAM.readByte(ram_nrf_device_info_start + (idx * ram_nrf_device_len) + ram_nrf_device_info_uptime + 0) << 24;
  uptime = uptime + SRAM.readByte(ram_nrf_device_info_start + (idx * ram_nrf_device_len) + ram_nrf_device_info_uptime + 1) << 16;
  uptime = uptime + SRAM.readByte(ram_nrf_device_info_start + (idx * ram_nrf_device_len) + ram_nrf_device_info_uptime + 2) << 8;
  uptime = uptime + SRAM.readByte(ram_nrf_device_info_start + (idx * ram_nrf_device_len) + ram_nrf_device_info_uptime + 3);
  return uptime;
}




void nrf_add_nei_store_service_info_send(uint8_t device_index, uint16_t value)
{
  uint8_t idx;
  if (nrf_find_store_index(device_index, &idx) == 1)
  {
    SRAMWrite16b(ram_nrf_device_info_start + (idx * ram_nrf_device_len) + ram_nrf_device_info_send, value);
  }
}

uint16_t nrf_get_nei_store_service_info_send(uint8_t idx)
{
  return SRAMRead16b(ram_nrf_device_info_start + (idx * ram_nrf_device_len) + ram_nrf_device_info_send);
}



void nrf_add_nei_store_service_info_recv(uint8_t device_index, uint16_t value)
{
  uint8_t idx;
  if (nrf_find_store_index(device_index, &idx) == 1)
  {
    SRAMWrite16b(ram_nrf_device_info_start + (idx * ram_nrf_device_len) + ram_nrf_device_info_recv, value);
  }
}

uint16_t nrf_get_nei_store_service_info_recv(uint8_t idx)
{
  return SRAMRead16b(ram_nrf_device_info_start + (idx * ram_nrf_device_len) + ram_nrf_device_info_recv);
}


void nrf_add_nei_store_service_info_err(uint8_t device_index, uint16_t value)
{
  uint8_t idx;
  if (nrf_find_store_index(device_index, &idx) == 1)
  {
    SRAMWrite16b(ram_nrf_device_info_start + (idx * ram_nrf_device_len) + ram_nrf_device_info_err, value);
  }
}

uint16_t nrf_get_nei_store_service_info_err(uint8_t idx)
{
  return SRAMRead16b(ram_nrf_device_info_start + (idx * ram_nrf_device_len) + ram_nrf_device_info_err);
}

void nrf_add_nei_store_service_info_renew(uint8_t device_index, uint16_t value)
{
  uint8_t idx;
  if (nrf_find_store_index(device_index, &idx) == 1)
  {
    SRAMWrite16b(ram_nrf_device_info_start + (idx * ram_nrf_device_len) + ram_nrf_device_info_renew, value);
  }
}

uint16_t nrf_get_nei_store_service_info_renew(uint8_t idx)
{
  return SRAMRead16b(ram_nrf_device_info_start + (idx * ram_nrf_device_len) + ram_nrf_device_info_renew);
}






/*
   inicializace pametoveho mista pro vzdalene NRF cidla
*/
void nrf_init_meas_store(void)
{
  for (uint8_t idx = 0; idx < ram_nrf_meas_store_count; idx++)
  {
    nrf_set_default_value(idx);
  }
}

uint8_t nrf_set_default_value(uint8_t idx)
{
  uint8_t ret = 0;
  if (idx < ram_nrf_meas_store_count)
  {
    nrf_set_meas_store_name(idx, "SENSOR");
    nrf_set_meas_store_associate(idx, 255);
    nrf_set_meas_store_used(idx, 0);
    nrf_set_meas_store_last_update(idx, 250);
    ret = 1;
  }
  return ret;
}

uint8_t nrf_delete_meas_store_by_device_id(uint8_t device_id)
{
  uint8_t cnt = 0;
  for (uint8_t idx = 0; idx < ram_nrf_meas_store_count; idx++)
  {
    if (nrf_get_meas_store_associate(idx) == device_id)
    {
      nrf_set_default_value(idx);
      cnt++;
    }
  }
  return cnt;
}

uint8_t nrf_add_update_meas_store(uint8_t device_id, uint8_t type, char *name, float value)
{
  uint8_t ret = 0;
  uint8_t know = 0;
  char tmp_name[10];
  uint8_t found_idx;
  uint8_t first_free_idx = 0;

  for (uint8_t idx = 0; idx < ram_nrf_meas_store_count; idx++)
  {
    if (nrf_get_meas_store_used(idx) == 1)
      if (nrf_get_meas_store_associate(idx) == device_id)
        if (nrf_get_meas_store_type(idx) == type)
        {
          nrf_get_meas_store_name(idx, tmp_name);
          //printf("%s == %s\n", name, tmp_name);
          if (strcmp(name, tmp_name) == 0)
          {
            //printf("znam %d %s\n", idx, tmp_name);
            know = 1;
            found_idx = idx;
            ret = 1;
            break;
          }
        }
  }

  if (know == 1)
  {
    nrf_set_meas_store_last_update(found_idx, 0);
    nrf_set_meas_store_value(found_idx, value);
  }

  if (know == 0)
  {
    for (uint8_t first_free_idx = 0; first_free_idx < ram_nrf_meas_store_count; first_free_idx++)
      if (nrf_get_meas_store_used(first_free_idx) == 0)
      {
        //printf("novy %d %s %d %d\n",first_free_idx, name, device_id,type );
        nrf_set_meas_store_associate(first_free_idx, device_id);
        nrf_set_meas_store_used(first_free_idx, 1);
        nrf_set_meas_store_name(first_free_idx, name);
        nrf_set_meas_store_last_update(first_free_idx, 0);
        nrf_set_meas_store_type(first_free_idx, type);
        nrf_set_meas_store_value(first_free_idx, value);
        ret = 1;
        break;
      }
  }
  return ret;
}

uint8_t nrf_list_meas_store(uint8_t idx, uint8_t *device_id, uint8_t *type, uint8_t *last_seen, float *value, char *name)
{
  uint8_t ret = 0;
  if (nrf_get_meas_store_used(idx) == 1 )
  {
    *device_id = nrf_get_meas_store_associate(idx);
    *type = nrf_get_meas_store_type(idx);
    *value = nrf_get_meas_store_value(idx);
    *last_seen = nrf_get_meas_store_last_update(idx);
    nrf_get_meas_store_name(idx, name);
    ret = 1;
  }
  return ret;
}


/*
   nastav/ziskej nazev NRF cidla
*/
void nrf_set_meas_store_name(uint8_t idx, char *name)
{
  sram_set_name(ram_nrf_meas_store_start + (idx * ram_nrf_meas_store_size) + ram_nrf_meas_store_name, name, ram_nrf_meas_name_length);
}
void nrf_get_meas_store_name(uint8_t idx, char *name)
{
  sram_get_name(ram_nrf_meas_store_start + (idx * ram_nrf_meas_store_size) + ram_nrf_meas_store_name, name, ram_nrf_meas_name_length);
}
/*
   zde si ulozime/ziskame informaci, ke kteremu vzdalenemu NRF dame cidlo patri
*/
void nrf_set_meas_store_associate(uint8_t idx, uint8_t device_id)
{
  SRAM.writeByte(ram_nrf_meas_store_start + (idx * ram_nrf_meas_store_size) + ram_nrf_meas_store_associate_device, device_id);
}

uint8_t nrf_get_meas_store_associate(uint8_t idx)
{
  return SRAM.readByte(ram_nrf_meas_store_start + (idx * ram_nrf_meas_store_size) + ram_nrf_meas_store_associate_device);
}
/*
   zde si ulozime/ziskame informaci, ze dany pametovy prostor je vyhrazen pro cidlo
*/
void nrf_set_meas_store_used(uint8_t idx, uint8_t used)
{
  SRAM.writeByte(ram_nrf_meas_store_start + (idx * ram_nrf_meas_store_size) + ram_nrf_meas_store_used, used);
}
uint8_t nrf_get_meas_store_used(uint8_t idx)
{
  return SRAM.readByte(ram_nrf_meas_store_start + (idx * ram_nrf_meas_store_size) + ram_nrf_meas_store_used);
}
/*
   informace o posledni aktualizaci hodnoty
*/
void nrf_set_meas_store_last_update(uint8_t idx, uint8_t last_seen)
{
  SRAM.writeByte(ram_nrf_meas_store_start + (idx * ram_nrf_meas_store_size) + ram_nrf_meas_store_last_update, last_seen);
}
uint8_t nrf_get_meas_store_last_update(uint8_t idx)
{
  return SRAM.readByte(ram_nrf_meas_store_start + (idx * ram_nrf_meas_store_size) + ram_nrf_meas_store_last_update);
}

void nrf_inc_meas_store_last_update(void)
{
  uint8_t last;
  for (uint8_t idx = 0; idx < ram_nrf_meas_store_count; idx++)
  {
    last = nrf_get_meas_store_last_update(idx);
    if (last < 250)
    {
      last++;
      nrf_set_meas_store_last_update(idx, last);
    }
  }
}

/*
   informaco o typu cidla teplota/vlhkost/tlak/byte/
*/
void nrf_set_meas_store_type(uint8_t idx, uint8_t last_seen)
{
  SRAM.writeByte(ram_nrf_meas_store_start + (idx * ram_nrf_meas_store_size) + ram_nrf_meas_store_value_type, last_seen);
}
uint8_t nrf_get_meas_store_type(uint8_t idx)
{
  return SRAM.readByte(ram_nrf_meas_store_start + (idx * ram_nrf_meas_store_size) + ram_nrf_meas_store_value_type);
}

/*
   ulozeni/ziskani hodnoty z pameti
*/
void nrf_set_meas_store_value(uint8_t idx, float value)
{
  uint8_t tmp[4];
  getInt(tmp, value);
  for (uint8_t i = 0; i < 4; i++)
    SRAM.writeByte(ram_nrf_meas_store_start + (idx * ram_nrf_meas_store_size) + ram_nrf_meas_store_value + i, tmp[i]);
}
float nrf_get_meas_store_value(uint8_t idx)
{
  float value;
  uint8_t tmp[4];
  for (uint8_t i = 0; i < 4; i++)
    tmp[i] = SRAM.readByte(ram_nrf_meas_store_start + (idx * ram_nrf_meas_store_size) + ram_nrf_meas_store_value + i);
  getFloat(tmp, &value);
  return value;
}



uint8_t count_use_nrf_temp(void)
{
  uint8_t count = 0;
  for (uint8_t idx = 0; idx < ram_nrf_meas_store_count; idx++)
    if (nrf_get_meas_store_used(idx) == 1 && nrf_get_meas_store_type(idx) == NRF_MEAS_TYPE_TEMP)
      count++;
  return count;
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
/*
 **********************************************************************************************************************
 **********************************************************************************************************************
    SKUPINA FUNKCI pro obsluhu virtualnich vystupu
*/

/// perzistentni ulozeni znamych associovanych virtualnich vystupu
/*
   inicializovani dat v eeprom, vola se pouze pri volbe set_default
*/

/*
   funkce vraci pocet pouzitych/associovanych/znamych/aktivnich virtualnich vystupu
*/
uint8_t output_virtaul_persistent_store_active(void)
{
  uint8_t cnt = 0;
  for (uint8_t idx = 0; idx < eeprom_know_output_virtual_count; idx++)
  {
    if (output_virtual_persistent_store_get_used(idx) == 1)
    {
      cnt++;
    }
  }
  return cnt;
}

/*
   nastaveni do vychozich hodnot, vsecny
*/
void output_virtual_persistent_store_init(void)
{
  for (uint8_t idx = 0; idx < eeprom_know_output_virtual_count; idx++)
    output_virtual_persistent_store_clear(idx);
}

/*
   nastaveni do vychozich hodnot, pouze index
*/
void output_virtual_persistent_store_clear(uint8_t idx)
{
  output_virtual_persistent_store_set_name(idx, "FREE");
  output_virtual_persistent_store_set_id(idx, POWER_OUTPUT_ERR);
  output_virtual_persistent_store_set_used(idx, 0);
}




/*
   funkce vraci seznam aktivnich/registrovanych virtualnich vystupu
   navratove hodnoty
   0 ... v pametovem prostoru neni nic
   1 ... v pametovem prostoru se nachazi informace o aktivni/registrovanem virtualnim vystupu

   uklada se do EEPROM store
*/
uint8_t output_virtual_persistent_store_list(uint8_t idx, uint8_t *virtual_id, char *name)
{
  uint8_t ret = 0;
  if (output_virtual_persistent_store_get_used(idx) == 1)
  {
    output_virtual_persistent_store_get_name(idx, name);
    *virtual_id = output_virtual_persistent_store_get_id(idx);
    ret = 1;
  }
  return ret;
}

/*
   funkce pro associovani noveho virtual vystupu.
   kdyz je uplne novy vystup, podle virtual_id, najde prvni neobsazene pametove misto, tak ulozi informaci, nazev, index virtual_id
   kdyz je jiz registrovany, vrati pouze index, kde se informace o virtualnim vystupu nachazi

   uklada se do EEPROM store
*/
uint8_t output_virtual_persistent_store_associate(uint8_t virtual_id, char *name)
{
  uint8_t ret_id = 255;
  uint8_t found = 0;
  char str1[10];
  for (uint8_t idx = 0; idx < eeprom_know_output_virtual_count; idx++)
  {
    if (output_virtual_persistent_store_get_used(idx) == 1)
      if (output_virtual_persistent_store_get_id(idx) == virtual_id)
      {
        found = 1;
        ret_id = idx;
        break;
      }
  }
  if (found == 0)
    for (uint8_t idx = 0; idx < eeprom_know_output_virtual_count; idx++)
    {
      if (output_virtual_persistent_store_get_used(idx) == 0)
      {
        output_virtual_persistent_store_set_id(idx, virtual_id);
        output_virtual_persistent_store_set_name(idx, name);
        output_virtual_persistent_store_set_used(idx, 1);
        ret_id = idx;
        break;
      }
    }
  return ret_id;
}

/*
   funkce, ktera zkopiruje informace o virtualnich vystupech z RAM store do EEPROM store
   vystup
   0 ... nepovedlo se
   1 ... povedlo se
*/
uint8_t output_virtual_persistent_store_associate_from_ram_store(uint8_t ram_store_id)
{
  uint8_t virtual_id;
  char name[10];
  uint8_t ret = 0;
  if (output_virtual_ram_store_get_used(ram_store_id) == 1)
  {
    output_virtual_ram_store_get_name(ram_store_id, name);
    virtual_id = output_virtual_ram_store_get_id(ram_store_id);
    if (output_virtual_persistent_store_associate(virtual_id, name) != 255)
      ret = 1;
  }
  return ret;
}


/*
   funkce, ktera na startu nacte do ram storu informace o znamych asociovanych v persistent store virtual vystupu
*/
void output_virtual_ram_store_load_from_persistent_store(void)
{
  uint8_t virtual_id;
  char name[10];
  for (uint8_t idx = 0; idx < eeprom_know_output_virtual_count; idx++)
  {
    if (output_virtual_persistent_store_get_used(idx) == 1)
    {
      output_virtual_persistent_store_get_name(idx, name);
      virtual_id = output_virtual_persistent_store_get_id(idx);
      virtual_id = output_virtual_ram_store_new(virtual_id);
      if (virtual_id != 255)
        output_virtual_ram_store_update_name(virtual_id, name);
    }
  }
}



void output_virtual_persistent_store_set_name(uint8_t idx, char *name)
{
  char c;
  for (uint8_t i = 0; i < eeprom_know_output_virtual_name_len - 1; i++)
  {
    c = name[i];
    EEPROM.write(eeprom_know_output_virtual_start + (idx * eeprom_know_output_virtual_size) + eeprom_know_output_virtual_name + i , c);
    if (c == 0) break;
  }
}
void output_virtual_persistent_store_get_name(uint8_t idx, char *name)
{
  char c;
  for (uint8_t i = 0; i < eeprom_know_output_virtual_name_len - 1; i++)
  {
    c = EEPROM.read(eeprom_know_output_virtual_start + (idx * eeprom_know_output_virtual_size) + eeprom_know_output_virtual_name + i);
    name[i] = c;
    name[i + 1] = 0;
    if (c == 0) break;
  }
}


void output_virtual_persistent_store_set_id(uint8_t idx, uint8_t virtual_id)
{
  EEPROM.write(eeprom_know_output_virtual_start + (idx * eeprom_know_output_virtual_size) + eeprom_know_output_virtual_id, virtual_id);
}

uint8_t output_virtual_persistent_store_get_id(uint8_t idx)
{
  return EEPROM.read(eeprom_know_output_virtual_start + (idx * eeprom_know_output_virtual_size) + eeprom_know_output_virtual_id);
}

void output_virtual_persistent_store_set_used(uint8_t idx, uint8_t used)
{
  EEPROM.write(eeprom_know_output_virtual_start + (idx * eeprom_know_output_virtual_size) + eeprom_know_output_virtual_used, used);
}

uint8_t output_virtual_persistent_store_get_used(uint8_t idx)
{
  return EEPROM.read(eeprom_know_output_virtual_start + (idx * eeprom_know_output_virtual_size) + eeprom_know_output_virtual_used);
}

/////////////////////////////////////





////
/*
   inicializovani vychozich parametru v ram store pro virtual-output
*/
void output_virtual_ram_store_init(void)
{
  for (uint8_t idx = 0; idx < ram_output_virtual_count; idx++)
    output_virtual_ram_store_clear(idx);
}

/*
   nastav idx pametove misto pro virtual-output ram store
*/
void output_virtual_ram_store_clear(uint8_t idx)
{
  output_virtual_ram_store_set_used(idx, 0);
  output_virtual_ram_store_set_id(idx, 255);
  output_virtual_ram_store_set_type(idx, OUTPUT_REAL_MODE_NONE);
  output_virtual_ram_store_update_name(idx, "FREE");
  output_virtual_ram_store_set_state(idx, POWER_OUTPUT_ERR);
}

/*
   funkce, zalozeni noveho cidla v ram store
   nebo hledani pametoveho mista podle virtual_id
   funkce vraci ID pametoveho mista
*/
uint8_t output_virtual_ram_store_new(uint8_t virtual_id)
{
  uint8_t ret_id = 255;
  uint8_t found = 0;
  for (uint8_t idx = 0; idx < ram_output_virtual_count; idx++)
  {
    if (output_virtual_ram_store_get_used(idx) == 1)
      if (output_virtual_ram_store_get_id(idx) == virtual_id)
      {
        ret_id = idx;
        found = 1;
        break;
      }
  }
  if (found == 0)
  {
    for (uint8_t idx = 0; idx < ram_output_virtual_count; idx++)
    {
      if (output_virtual_ram_store_get_used(idx) == 0)
      {
        output_virtual_ram_store_set_used(idx, 1);
        output_virtual_ram_store_set_id(idx, virtual_id);
        output_virtual_ram_store_set_last_update(idx, 255);
        ret_id = idx;
        break;
      }
    }
  }
  return ret_id;
}

/*
   listovani v ram store virtualnich vystupu
*/
uint8_t output_virtual_ram_store_list_store(uint8_t idx, char *name, uint8_t *state,  uint8_t *id, uint8_t *type, uint8_t *last_update)
{
  uint8_t ret = 0;
  if (output_virtual_ram_store_get_used(idx) == 1)
  {
    *state = output_virtual_ram_store_get_state(idx);
    *type = output_virtual_ram_store_get_type(idx);
    *id = output_virtual_ram_store_get_id(idx);
    *last_update = output_virtual_ram_store_get_last_update(idx);
    output_virtual_ram_store_get_name(idx, name);
    ret = 1;
  }
  return ret;
}




void output_virtual_ram_store_update_name(uint8_t idx, char *name)
{
  sram_set_name(ram_output_virtual_start_pos + (idx * ram_output_virtual_size) + ram_output_virtual_name, name, ram_output_virtual_name_len);
}

void output_virtual_ram_store_get_name(uint8_t idx, char *name)
{
  sram_get_name(ram_output_virtual_start_pos + (idx * ram_output_virtual_size) + ram_output_virtual_name, name, ram_output_virtual_name_len);
}

uint8_t output_virtual_ram_store_get_used(uint8_t idx)
{
  return SRAM.readByte(ram_output_virtual_start_pos + (idx * ram_output_virtual_size) + ram_output_virtual_used);
}
void output_virtual_ram_store_set_used(uint8_t idx, uint8_t used)
{
  SRAM.writeByte(ram_output_virtual_start_pos + (idx * ram_output_virtual_size) + ram_output_virtual_used, used);
}

uint8_t output_virtual_ram_store_get_id(uint8_t idx)
{
  return SRAM.readByte(ram_output_virtual_start_pos + (idx * ram_output_virtual_size) + ram_output_virtual_id);
}
void output_virtual_ram_store_set_id(uint8_t idx, uint8_t id)
{
  SRAM.writeByte(ram_output_virtual_start_pos + (idx * ram_output_virtual_size) + ram_output_virtual_id, id);
}

uint8_t output_virtual_ram_store_get_type(uint8_t idx)
{
  return SRAM.readByte(ram_output_virtual_start_pos + (idx * ram_output_virtual_size) + ram_output_virtual_type);
}
void output_virtual_ram_store_set_type(uint8_t idx, uint8_t type)
{
  SRAM.writeByte(ram_output_virtual_start_pos + (idx * ram_output_virtual_size) + ram_output_virtual_type, type);
}

uint8_t output_virtual_ram_store_get_state(uint8_t idx)
{
  return SRAM.readByte(ram_output_virtual_start_pos + (idx * ram_output_virtual_size) + ram_output_virtual_state);
}
void output_virtual_ram_store_set_state(uint8_t idx, uint8_t state)
{
  SRAM.writeByte(ram_output_virtual_start_pos + (idx * ram_output_virtual_size) + ram_output_virtual_state, state);
}

uint8_t output_virtual_ram_store_get_last_update(uint8_t idx)
{
  return SRAM.readByte(ram_output_virtual_start_pos + (idx * ram_output_virtual_size) + ram_output_virtual_last_update);
}
void output_virtual_ram_store_set_last_update(uint8_t idx, uint8_t last_update)
{
  SRAM.writeByte(ram_output_virtual_start_pos + (idx * ram_output_virtual_size) + ram_output_virtual_last_update, last_update);
}

/*
   funkce inkrementuje cas od posledni aktualizace virtualniho vystupu
   funkce vraci pocet aktivnich, pouzitych virtualnich vystupu
*/
uint8_t output_virtual_inc_last_update(void)
{
  uint8_t last = 0;
  uint8_t cnt = 0;
  for (uint8_t idx = 0; idx < ram_output_virtual_count; idx++)
    if (output_virtual_ram_store_get_used(idx) == 1)
    {
      cnt++;
      last = output_virtual_ram_store_get_last_update(idx);
      if (last < 250)
      {
        last++;
        output_virtual_ram_store_set_last_update(idx, last);
      }
    }
  return cnt;
}



/*
   genericke funkce pro ulozeni nazvu a int cisla
*/
void sram_set_name(uint32_t addr, char *name, uint8_t len)
{
  char c ;
  for (uint8_t i = 0; i < len - 1; i++)
  {
    c = name[i];
    SRAM.writeByte(addr + i, c);
    if (c == 0) break;
  }
}
void sram_get_name(uint32_t addr, char *name, uint8_t len)
{
  char c;
  for (uint8_t i = 0; i < len - 1; i++)
  {
    c = SRAM.readByte(addr + i);
    name[i] = c;
    name[i + 1] = 0;
    if (c == 0)
      break;
  }
}

uint16_t SRAMRead16b(uint32_t addr)
{
  uint16_t value;
  value = SRAM.readByte(addr) << 8;
  value = value + SRAM.readByte(addr + 1);
  return value;
}

void SRAMWrite16b(uint32_t addr, uint16_t value)
{
  SRAM.writeByte(addr , value >> 8);
  SRAM.writeByte(addr + 1, value);
}




/*************************************************************************************************************************/
///SEKCE MQTT ///

void mqtt_callback_prepare_topic_array(char *out_str, char *in_topic)
{
  uint8_t cnt = 0;
  for (uint8_t f = strlen(out_str); f < strlen(in_topic); f++)
  {
    out_str[cnt] = in_topic[f];
    out_str[cnt + 1] = 0;
    cnt++;
  }
}


/// mqtt_callback - zpracovani prichozich MQTT zprav
/*
   topic - ukazatel na nazev topicu
   payload - ukazatel na pole prichozich dat
   length - pocet prichozich dat
*/
void mqtt_callback(char* topic, byte * payload, unsigned int length)
{
  char str1[64];
  char tmp1[32];
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
  DateTime ted;
  for (uint8_t j = 0; j < 128; j++) my_payload[j] = 0;
  ////
  mqtt_receive_message++; /// inkrementuji promenou celkovy pocet prijatych zprav
  strncpy(my_payload, (char*) payload, length);
  ///

  /// pridam mqqt kamarada typ termbig
  strcpy_P(str1, termbig_subscribe);
  if (strcmp(str1, topic) == 0)
  {
    if (strcmp(device.nazev, my_payload) != 0) /// sam sebe ignoruj
    {
      mqtt_process_message++; /// inkrementuji promenou celkovy pocet zpracovanych zprav
      know_mqtt_create_or_update(my_payload, TYPE_TERMBIG);
    }
  }
  ///
  //// pridam mqtt kamarada typ room controler
  strcpy_P(str1, thermctl_subscribe);
  if (strcmp(str1, topic) == 0)
  {
    if (strcmp(device.nazev, my_payload) != 0) /// sam sebe ignoruj
    {
      mqtt_process_message++;
      know_mqtt_create_or_update(my_payload, TYPE_THERMCTL);
    }
  }
  ///
  //// zpracovani udalosti virtualniho vystupu
  strcpy_P(str1, termbig_virtual_output);
  if (strncmp(str1, topic, strlen(str1)) == 0)
  {
    mqtt_process_message++;
    mqtt_callback_prepare_topic_array(str1, topic);
    cnt = 0;
    pch = strtok (str1, "/");
    while (pch != NULL)
    {
      if (cnt == 0)
      {
        id = output_virtual_ram_store_new(atoi(pch));
        output_virtual_ram_store_set_last_update(id, 0);
      }
      if ((cnt == 1) && (strcmp(pch, "name") == 0)) output_virtual_ram_store_update_name(id, my_payload);
      if ((cnt == 1) && (strcmp(pch, "type") == 0)) output_virtual_ram_store_set_type(id, atoi(my_payload));
      if ((cnt == 1) && (strcmp(pch, "state") == 0)) output_virtual_ram_store_set_state(id, atoi(my_payload));
      pch = strtok (NULL, "/");
      cnt++;
    }
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
    if (ntp_update(&timeClient, &rtc, time_get_offset(), &ted) == 1)
      selftest_clear_0(SELFTEST_ERR_NTP);
    else
      selftest_set_0(SELFTEST_ERR_NTP);
  }
  //// /termbig-in/global/time/offset - nastaveni offsetu casu
  strcpy_P(str1, thermctl_header_in);
  strcat_P(str1, global_time_offset);
  if (strcmp(str1, topic) == 0)
  {
    mqtt_process_message++;
    time_set_offset(atoi(my_payload));
  }
  ///
  /// nastavovani virtualnich vystupu - associovani znameho virtualniho vystupu z ram do eeprom
  //// /thermctl-in/XXXX/virtual-output/associate
  strcpy_P(str1, thermctl_header_in);
  strcat(str1, device.nazev);
  strcat(str1, "/virtual-output/associate");
  if (strcmp(str1, topic) == 0)
  {
    mqtt_process_message++;
    change_virtual_output = 4;
    id = atoi(my_payload);
    if (output_virtual_persistent_store_associate_from_ram_store(id) == 0)
      log_error(&mqtt_client, "E");
  }
  /// nastavovani virtualnich vystupu - vymazani virtualniho vystupu z persistent store
  //// /thermctl-in/XXXX/virtual-output/clear
  strcpy_P(str1, thermctl_header_in);
  strcat(str1, device.nazev);
  strcat(str1, "/virtual-output/clear");
  if (strcmp(str1, topic) == 0)
  {
    mqtt_process_message++;
    change_virtual_output = 4;
    id = atoi(my_payload);
    thermostat_ring_clear_output_in_rings(output_virtual_persistent_store_get_id(id));
    output_virtual_persistent_store_clear(id);
  }
  ///
  //// /thermctl-in/XXXX/virtual-output/set/IDX/name
  //// /thermctl-in/XXXX/virtual-output/set/IDX/virtual_output_id
  //// /thermctl-in/XXXX/virtual-output/set/IDX/used
  strcpy_P(str1, thermctl_header_in);
  strcat(str1, device.nazev);
  strcat(str1, "/virtual-output/set");
  if (strcmp(str1, topic) == 0)
  {
    mqtt_process_message++;
    change_virtual_output = 4;
    mqtt_callback_prepare_topic_array(str1, topic);
    cnt = 0;
    pch = strtok (str1, "/");
    while (pch != NULL)
    {
      if (cnt == 0) id = atoi(pch);
      if ((cnt == 1) && (strcmp(pch, "used") == 0)) output_virtual_persistent_store_set_used(id, atoi(my_payload));
      if ((cnt == 1) && (strcmp(pch, "name") == 0)) output_virtual_persistent_store_set_name(id, my_payload);
      if ((cnt == 1) && (strcmp(pch, "virtual_output_id") == 0)) output_virtual_persistent_store_set_id(id, atoi(my_payload));
      pch = strtok (NULL, "/");
      cnt++;
    }
  }
  //////
  /// nastavovani virtualnich vystupu - vymazani virtualniho vystupu z ram store
  //// /thermctl-in/XXXX/virtual-output/clear
  strcpy_P(str1, thermctl_header_in);
  strcat(str1, device.nazev);
  strcat(str1, "/virtual-output/ram/clear");
  if (strcmp(str1, topic) == 0)
  {
    mqtt_process_message++;
    id = atoi(my_payload);
    output_virtual_ram_store_clear(id);
    change_virtual_output = 4;
    //TODO vymazat vcetne associace
  }
  /////
  ///
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
      log_error(&mqtt_client, "E");
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
    mqtt_callback_prepare_topic_array(str1, topic);
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
        log_error(&mqtt_client, "E");
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
      log_error(&mqtt_client, "E");
  }
  ////////
  ////////
  /// nastavovani vlastnosti RTDS ///
  ///
  //// thermctl-in/XXXXX/rtds-control/register - registruje nove vzdalene cidlo
  strcpy_P(str1, thermctl_header_in);
  strcat(str1, device.nazev);
  strcat(str1, "/rtds-control/register");
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
  ///
  //// thermctl-in/XXXXX/rtds/set/IDX/name - nastavi a udela prihlaseni
  strcpy_P(str1, thermctl_header_in);
  strcat(str1, device.nazev);
  strcat(str1, "/rtds-control/set/");
  if (strncmp(str1, topic, strlen(str1)) == 0)
  {
    mqtt_process_message++;
    mqtt_callback_prepare_topic_array(str1, topic);
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
  ///
  //// /thermctl-in/XXXX/rtds/clear index vymaze a odhlasi
  strcpy_P(str1, thermctl_header_in);
  strcat(str1, device.nazev);
  strcat(str1, "/rtds-control/clear");
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
  strcat(str1, "/rtds-control/get");
  if (strncmp(str1, topic, strlen(str1)) == 0)
  {
    send_mqtt_remote_tds_status();
  }
  ////
  //// rtds/NAME - hodnota, kde NAME je nazev cidla
  strcpy_P(str1, new_text_slash_rtds_slash); /// /rtds/
  if (strncmp(str1, topic, strlen(str1)) == 0)
  {
    mqtt_process_message++;
    mqtt_callback_prepare_topic_array(str1, topic);
    cnt = 0;
    pch = strtok (str1, "/");
    while (pch != NULL)
    {
      if (cnt == 0)
        strcpy(tmp1, pch);
      if (cnt == 1)
        strcpy(tmp2, pch);
      pch = strtok (NULL, "/");
      cnt++;
    }
    for (uint8_t idx = 0; idx < MAX_RTDS; idx++)
    {
      uint8_t active = 0;
      str1[0] = 0;
      remote_tds_get_complete(idx, &active, str1);
      if (active == 1 && strcmp(str1, tmp1) == 0)
      {
        if (strcmp(tmp2, "value") == 0)
          remote_tds_set_data(idx, atoi(my_payload));
        if (strcmp(tmp2, "type") == 0)
          remote_tds_set_type(idx, atoi(my_payload));
      }
    }
  }
  ///

  strcpy_P(str1, new_text_slash_rtds_control_list); /// /rtds-control/list"
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
    mqtt_callback_prepare_topic_array(str1, topic);
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
        log_error(&mqtt_client, "E");
        break;
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
      strcpy_P(tmp2, text_prog);
      thermostat_program_set_name(id, tmp2);
      thermostat_program_set_active(id, PROG_FREE);
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
    mqtt_callback_prepare_topic_array(str1, topic);
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
        log_error(&mqtt_client, "E");
        break;
      }
      pch = strtok (NULL, "/");
      cnt++;
    }
  }

  /// vymaze ulozena data z nrf vzdaleneho zarizeni
  strcpy_P(str1, thermctl_header_in);
  strcat(str1, device.nazev);
  strcat(str1, "/nrf/measurement/clear");
  if (strncmp(str1, topic, strlen(str1)) == 0)
  {
    mqtt_process_message++;
    nrf_set_default_value(atoi(my_payload));
  }

  strcpy_P(str1, thermctl_header_in);
  strcat(str1, device.nazev);
  strcat(str1, "/nrf/device/clear");
  if (strncmp(str1, topic, strlen(str1)) == 0)
  {
    mqtt_process_message++;
    id_interval = atoi(my_payload);
    id = nrf_get_nei_device_index(id_interval);
    nrf_set_default_device(id_interval);
    nrf_delete_meas_store_by_device_id(id);
  }

  ///
  //// thermctl-in/XXXXX/ring/default, nastavi vychozi ring na displaji
  strcpy_P(str1, thermctl_header_in);
  strcat(str1, device.nazev);
  strcat(str1, "/ring/default");
  if (strcmp(str1, topic) == 0)
  {
    mqtt_process_message++;
    last_default_ring = default_ring;
    default_ring = atoi(my_payload);
    display_redraw_default_ring = 1;
    set_default_ring(default_ring);
  }
  ///
  //// thermctl-in/XXXXX/ring/get/IDcko/pid
  strcpy_P(str1, thermctl_header_in);
  strcat(str1, device.nazev);
  strcat(str1, "/ring/get/");
  if (strncmp(str1, topic, strlen(str1)) == 0)
  {
    mqtt_process_message++;
    mqtt_callback_prepare_topic_array(str1, topic);
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
        log_error(&mqtt_client, "E");
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
    mqtt_process_message++;
    mqtt_callback_prepare_topic_array(str1, topic);
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
        if ((cnt == 1) && (strcmp(pch, "threshold-f") == 0))
        {
          thermostat_ring_set_mezni(id, atof(my_payload) * 10); /// thereshold hodnota presne ve floatu * 10
        }
        if ((cnt == 1) && (strcmp(pch, "threshold") == 0))
        {
          thermostat_ring_set_mezni(id, atoi(my_payload));
        }
        if ((cnt == 1) && (strcmp(pch, "text_mode") == 0))
        {
          thermostat_mode_default_ring_last_state = thermostat_ring_get_mode(id);
          active = convert_text_mode(my_payload);
          thermostat_ring_set_mode(id, active);
          if (active == TERM_MODE_MAN_HEAT)
            thermostat_ring_update_bites(id, STATUS_BIT_HEAT_OR_COOL, 0);
          if (active == TERM_MODE_MAN_COOL)
            thermostat_ring_update_bites(id, STATUS_BIT_HEAT_OR_COOL, 1);
          change_term_mode = 1;

        }
        if ((cnt == 1) && (strcmp(pch, "mode") == 0))
        {
          thermostat_mode_default_ring_last_state = thermostat_ring_get_mode(id);
          active = atoi(my_payload);
          thermostat_ring_set_mode(id, active);
          if (active == TERM_MODE_MAN_HEAT)
            thermostat_ring_update_bites(id, STATUS_BIT_HEAT_OR_COOL, 0);
          if (active == TERM_MODE_MAN_COOL)
            thermostat_ring_update_bites(id, STATUS_BIT_HEAT_OR_COOL, 1);
          change_term_mode = 1;
        }
        if ((cnt == 1) && (strcmp(pch, "input") == 0)) thermostat_ring_set_asociate_tds(id, atoi(my_payload));
        //if ((cnt == 1) && (strcmp(pch, "rtds") == 0)) thermostat_ring_set_asociate_tds(id, atoi(my_payload) + TDS_MEMORY_MAP_RTDS);
        if ((cnt == 1) && (strcmp(pch, "active") == 0)) thermostat_ring_set_active(id, atoi(my_payload));
        if ((cnt == 1) && (strcmp(pch, "output") == 0)) {
          thermostat_ring_set_output(id, atoi(my_payload));
          change_virtual_output = 4;
        }
        if ((cnt == 1) && (strcmp(pch, "pid_kp") == 0)) thermostat_ring_pid_set_kp(id, atof(my_payload));
        if ((cnt == 1) && (strcmp(pch, "pid_ki") == 0)) thermostat_ring_pid_set_ki(id, atof(my_payload));
        if ((cnt == 1) && (strcmp(pch, "pid_kd") == 0)) thermostat_ring_pid_set_kd(id, atof(my_payload));
        if ((cnt == 1) && (strcmp(pch, "pid_time") == 0)) thermostat_ring_pid_set_time(id, atoi(my_payload));
      }
      else
      {
        log_error(&mqtt_client, "E");
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
      log_error(&mqtt_client, "E");
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
    mqtt_callback_prepare_topic_array(str1, topic);
    cnt = setting_network(str1, my_payload);
    /// je zmena s ulozeni
    if (cnt == 1)
    {
      save_setup_network();
      selftest_set_0(SELFTEST_RESTART_NEEDED);
    }
    /// pouze zmena
    if (cnt == 2)
    {
      selftest_set_0(SELFTEST_RESTART_NEEDED);
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
    mqtt_callback_prepare_topic_array(str1, topic);
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
  /*
    /// zpetna vazba od vystupu
    strcpy_P(str1, termbig_header_out);
    strcat(str1, "output/");
    if (strncmp(str1, topic, strlen(str1)) == 0)
    {
    mqtt_process_message++;
    mqtt_callback_prepare_topic_array(str1, topic);
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
  */
  //// /thermctl-in/XXXX/rf/scan - 0|1 zapnuti/vypnuti scanovani rf site
  strcpy_P(str1, thermctl_header_in);
  strcat(str1, device.nazev);
  strcat(str1, "/rf/scan");
  if (strcmp(str1, topic) == 0)
  {
    mqtt_process_message++;
    if (atoi(my_payload) == 1)
    {
      start_scan_rf_network(&radio);
      selftest_set_0(SELFTEST_NRF_SCAN);
    }
    else
    {
      stop_scan_rf_network(&radio);
      scan_rf_network_public(&mqtt_client);
      selftest_clear_0(SELFTEST_NRF_SCAN);
    }
  }

  ///// //thermctl-in/XXXX/rf/stat - statisticke informace
  strcpy_P(str1, thermctl_header_in);
  strcat(str1, device.nazev);
  strcat(str1, "/rf/stat");
  if (strcmp(str1, topic) == 0)
  {
    mqtt_process_message++;
    scan_rf_net_enable = 2;
  }

  ///// //thermctl-in/XXXX/rf/channel - nastaveni komunikacniho kanalu
  strcpy_P(str1, thermctl_header_in);
  strcat(str1, device.nazev);
  strcat(str1, "/rf/channel");
  if (strcmp(str1, topic) == 0)
  {
    mqtt_process_message++;
    nrf_save_channel(atoi(my_payload));
    mesh.setChannel(atoi(my_payload));
  }

  ///// //thermctl-in/XXXX/rf/mesh/id - nastaveni komunikacniho kanalu
  strcpy_P(str1, thermctl_header_in);
  strcat(str1, device.nazev);
  strcat(str1, "/rf/mesh/id");
  if (strcmp(str1, topic) == 0)
  {
    mqtt_process_message++;
    mesh.setNodeID(atoi(my_payload));
  }

  ///// //thermctl-in/XXXX/rf/power - nastaveni komunikacniho kanalu
  strcpy_P(str1, thermctl_header_in);
  strcat(str1, device.nazev);
  strcat(str1, "/rf/power");
  if (strcmp(str1, topic) == 0)
  {
    mqtt_process_message++;
    radio.setPALevel(atoi(my_payload));
    nrf_save_power((rf24_pa_dbm_e) atoi(my_payload));
  }


  //// thermctl-in/XXXXX/brightness
  strcpy_P(str1, thermctl_header_in);
  strcat(str1, device.nazev);
  strcat(str1, "/brightness");
  if (strcmp(str1, topic) == 0)
  {
    mqtt_process_message++;
    my_touch.TP_SetBacklight(atoi(my_payload));
  }

  //// thermctl-in/XXXXX/reload
  strcpy_P(str1, thermctl_header_in);
  strcat(str1, device.nazev);
  strcat(str1, "/reload");
  if (strcmp(str1, topic) == 0)
  {
    mqtt_process_message++;
    log_error(&mqtt_client, "reload..");
    //resetFunc();
    wdt_enable(WDTO_1S);
    while (1);
  }

  //// thermctl-in/XXXXX/bootloader
  strcpy_P(str1, thermctl_header_in);
  strcat(str1, device.nazev);
  strcat(str1, "/bootloader");
  if (strcmp(str1, topic) == 0)
  {
    mqtt_process_message++;
    log_error(&mqtt_client, "bootloader");
    EEPROM.write(bootloader_tag, atoi(my_payload));
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
  long mil;
  ///  /thermctl/xxxxxxxx/#
  ///  /thermctl/global/#
  if (!mqtt_client.connected())
  {
    device_get_name(nazev);
    mil = millis();
    if (mil > lastmqttconnect)
    {
      lastmqttconnect = mil + 5000;
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
        strcpy_P(topic, termbig_virtual_output);
        strcat(topic, "#");
        mqtt_client.subscribe(topic);
      }
    }
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
    createString(payload, ':', w_rom[i].rom, 8, 16, 2);
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
  long time_now;
  if (mqtt_client.connected())
  {
    strcpy(str_topic, "status/uptime");
    //sprintf(payload, "%ld", uptime);
    ltoa(uptime, payload, 10);
    send_mqtt_general_payload(&mqtt_client, str_topic, payload);
    ///
    strcpy(str_topic, "status/brigthness");
    itoa(brigthness_display_values, payload, 10);
    send_mqtt_general_payload(&mqtt_client, str_topic, payload);
    ///
    strcpy(str_topic, "status/light");
    itoa(light_curr, payload, 10);
    send_mqtt_general_payload(&mqtt_client, str_topic, payload);
    ///
    strcpy(str_topic, "status/auto_brigthness");
    itoa(brigthness_display_auto_values, payload, 10);
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
    strcpy(str_topic, "status/selftest/0");
    itoa(selftest_data_0, payload, 10);
    send_mqtt_general_payload(&mqtt_client, str_topic, payload);
    ///
    strcpy(str_topic, "status/selftest/1");
    itoa(selftest_data_1, payload, 10);
    send_mqtt_general_payload(&mqtt_client, str_topic, payload);
    ///
    strcpy(str_topic, "status/rtds/count");
    itoa(use_rtds, payload, 10);
    send_mqtt_general_payload(&mqtt_client, str_topic, payload);
    ///
    strcpy(str_topic, "status/nrf/temp/count");
    itoa(use_nrf_temp, payload, 10);
    send_mqtt_general_payload(&mqtt_client, str_topic, payload);
    /*
      strcpy(str_topic, "status/light/count");
      itoa(use_light_curr, payload, 10);
      send_mqtt_general_payload(&mqtt_client, str_topic, payload);
    */
    itoa(time_get_offset(), payload, 10);
    send_mqtt_general_payload(&mqtt_client, "status/time/ntp_offset", payload);
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

    dtostrf(internal_temp, 4, 2, payload);
    send_mqtt_general_payload(&mqtt_client, "status/internal_temp", payload);

    time_now = DateTime(__DATE__, __TIME__).unixtime();
    sprintf(payload, "%ld", time_now);
    send_mqtt_general_payload(&mqtt_client, "status/build_time", payload);
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
  char payload[64];
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
      itoa(thermostat_ring_get_asociate_tds(idx), payload, 10);
      send_mqtt_message_prefix_id_topic_payload(&mqtt_client, "ring", idx, "input", payload);
      itoa(thermostat_ring_get_output(idx), payload, 10);
      send_mqtt_message_prefix_id_topic_payload(&mqtt_client, "ring", idx, "output", payload);

      itoa(thermostat_ring_get_status_data(idx), payload, 10);
      send_mqtt_message_prefix_id_topic_payload(&mqtt_client, "ring", idx, "status_bites", payload);

      //itoa(last_output_update[idx], payload, 10);
      //send_mqtt_message_prefix_id_topic_payload(&mqtt_client, "ring", idx, "output_update", payload);
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
  //int tt;
  //long avg = 0;
  for (uint8_t id = 0; id < HW_ONEWIRE_MAXROMS; id++)
    if (get_tds18s20(id, &tds) == 1)
      if (tds.used == 1 && status_tds18s20[id].online == True)
      {
        //tt = status_tds18s20[id].temp;
        itoa(status_tds18s20[id].temp, payload, 10);
        send_mqtt_message_prefix_id_topic_payload(&mqtt_client, "tds", id, "temp", payload);
        //avg = 0;
        //for (uint8_t c = 0; c < MAX_AVG_TEMP; c++) avg = avg + status_tds18s20[id].average_temp[c];
        //avg = avg / MAX_AVG_TEMP;
        itoa(status_tds18s20[id].average_temp_now , payload, 10);
        send_mqtt_message_prefix_id_topic_payload(&mqtt_client, "tds", id, "temp_avg", payload);
        strcpy(payload, tds.name);
        send_mqtt_message_prefix_id_topic_payload(&mqtt_client, "tds", id, "name", payload);
        //tt = tds.offset;
        itoa(tds.offset, payload, 10);
        send_mqtt_message_prefix_id_topic_payload(&mqtt_client, "tds", id, "offset", payload);
        //tt = status_tds18s20[id].online;
        itoa(status_tds18s20[id].online, payload, 10);
        send_mqtt_message_prefix_id_topic_payload(&mqtt_client, "tds", id, "online", payload);
        payload[0] = 0;
        createString(payload, ':', tds.rom, 8, 16, 2);
        send_mqtt_message_prefix_id_topic_payload(&mqtt_client, "tds", id, "rom", payload);
        //tt = tds.period;
        itoa(tds.period, payload, 10);
        send_mqtt_message_prefix_id_topic_payload(&mqtt_client, "tds", id, "period", payload);
        //tt = status_tds18s20[id].period_now;
        utoa(status_tds18s20[id].period_now, payload, 10);
        send_mqtt_message_prefix_id_topic_payload(&mqtt_client, "tds", id, "start_at", payload);
        //tt = status_tds18s20[id].crc_error;
        itoa(status_tds18s20[id].crc_error, payload, 10);
        send_mqtt_message_prefix_id_topic_payload(&mqtt_client, "tds", id, "crc_error", payload);
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
    if ( act != PROG_FREE)
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

void send_know_device(void)
{
  char str_topic[64];
  char payload[64];
  char str2[20];
  strcpy_P(str2, text_know_mqtt_device);
  for (uint8_t idx = 0; idx < MAX_KNOW_MQTT_INTERNAL_RAM; idx++)
  {
    if (know_mqtt[idx].type != TYPE_FREE)
    {
      itoa(know_mqtt[idx].type, payload, 10);
      send_mqtt_message_prefix_id_topic_payload(&mqtt_client, str2, idx, "type", payload);
      itoa(know_mqtt[idx].last_update, payload, 10);
      send_mqtt_message_prefix_id_topic_payload(&mqtt_client, str2, idx, "last_update", payload);
      strcpy(payload, know_mqtt[idx].device);
      send_mqtt_message_prefix_id_topic_payload(&mqtt_client, str2, idx, "device", payload);
    }
  }
}

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
      send_mqtt_message_prefix_id_topic_payload(&mqtt_client, "rtds", idx, "value", payload);
      itoa(remote_tds_get_type(idx), payload, 10);
      send_mqtt_message_prefix_id_topic_payload(&mqtt_client, "rtds", idx, "type", payload);
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



void send_virtual_output(void)
{
  char str1[10];
  char str2[22];
  uint8_t id, state, type, last_update;
  strcpy_P(str2, text_virtual_output);
  for (uint8_t idx = 0; idx < ram_output_virtual_count; idx++)
    if (output_virtual_ram_store_list_store(idx, str1, &state, &id, &type, &last_update) == 1)
    {
      send_mqtt_message_prefix_id_topic_payload(&mqtt_client, str2, idx, "name", str1);
      itoa(id, str1, 10);
      send_mqtt_message_prefix_id_topic_payload(&mqtt_client, str2, idx, "id", str1);
      itoa(type, str1, 10);
      send_mqtt_message_prefix_id_topic_payload(&mqtt_client, str2, idx, "type", str1);
      itoa(state, str1, 10);
      send_mqtt_message_prefix_id_topic_payload(&mqtt_client, str2, idx, "state", str1);
      itoa(last_update, str1, 10);
      send_mqtt_message_prefix_id_topic_payload(&mqtt_client, str2, idx, "last_update", str1);
    }

  strcpy_P(str2, text_know_virtual_output);
  for (uint8_t idx = 0; idx < eeprom_know_output_virtual_count; idx++)
    if (output_virtual_persistent_store_list(idx, &id, str1) == 1)
    {
      send_mqtt_message_prefix_id_topic_payload(&mqtt_client, str2, idx, "name", str1);
      itoa(id, str1, 10);
      send_mqtt_message_prefix_id_topic_payload(&mqtt_client, str2, idx, "id", str1);
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

void send_mesh_status(void)
{
  char str_topic[64];
  char str_space[16];
  char payload[128];
  uint8_t tmp1, tmp2, tmp3;
  float value;

  strcpy_P(str_topic, text_mesh_id);
  itoa(mesh.getNodeID(), payload, 10);
  send_mqtt_general_payload(&mqtt_client, str_topic, payload);


  strcpy_P(str_topic, text_mesh_neighbours);
  itoa(mesh.addrListTop, payload, 10);
  send_mqtt_general_payload(&mqtt_client, str_topic, payload);


  strcpy_P(str_topic, text_rf_channel);
  itoa(nrf_load_channel(), payload, 10);
  send_mqtt_general_payload(&mqtt_client, str_topic, payload);


  strcpy_P(str_topic, text_rf_power);
  itoa(radio.getPALevel(), payload, 10);
  send_mqtt_general_payload(&mqtt_client, str_topic, payload);


  strcpy_P(str_topic, text_mesh_neighbour);
  for (uint8_t idx = 0; idx < mesh.addrListTop; idx++)
  {
    itoa(mesh.addrList[idx].nodeID, payload, 10);
    send_mqtt_message_prefix_id_topic_payload(&mqtt_client, str_topic, idx, "id", payload);
    itoa(mesh.addrList[idx].address, payload, 10);
    send_mqtt_message_prefix_id_topic_payload(&mqtt_client, str_topic, idx, "address", payload);
  }

  strcpy_P(str_topic, text_rf_mesh_send_total);
  utoa(nrf_message_send_total, payload, 10);
  send_mqtt_general_payload(&mqtt_client, str_topic, payload);

  strcpy_P(str_topic, text_rf_mesh_receive_total);
  utoa(nrf_message_receive_total, payload, 10);
  send_mqtt_general_payload(&mqtt_client, str_topic, payload);

  strcpy_P(str_topic, text_rf_mesh_process_total);
  utoa(nrf_message_process_total, payload, 10);
  send_mqtt_general_payload(&mqtt_client, str_topic, payload);

  strcpy_P(str_topic, text_mesh_nrf_store);
  for (uint8_t idx = 0; idx < ram_nrf_devices; idx++)
  {
    if (nrf_list_nei_store(idx, payload, &tmp1, &tmp2) == 1)
    {
      strcpy_P(str_space, text_name);
      send_mqtt_message_prefix_id_topic_payload(&mqtt_client, str_topic, idx, str_space, payload);
      ///
      itoa(tmp1, payload, 10);
      send_mqtt_message_prefix_id_topic_payload(&mqtt_client, str_topic, idx, "id", payload);
      ///
      itoa(tmp2, payload, 10);
      strcpy_P(str_space, mesh_device_nrf_last_seen_x);
      send_mqtt_message_prefix_id_topic_payload(&mqtt_client, str_topic, idx, str_space, payload);

      //sprintf(payload, "%ld", nrf_get_nei_store_service_info_uptime(idx));
      ltoa(nrf_get_nei_store_service_info_uptime(idx), payload, 10);
      strcpy_P(str_space, mesh_device_nrf_uptime_x);
      send_mqtt_message_prefix_id_topic_payload(&mqtt_client, str_topic, idx, str_space, payload);

      utoa(nrf_get_nei_store_service_info_send(idx), payload, 10);
      strcpy_P(str_space, mesh_device_nrf_send_x);
      send_mqtt_message_prefix_id_topic_payload(&mqtt_client, str_topic, idx, str_space, payload);

      utoa(nrf_get_nei_store_service_info_recv(idx), payload, 10);
      strcpy_P(str_space, mesh_device_nrf_recv_x);
      send_mqtt_message_prefix_id_topic_payload(&mqtt_client, str_topic, idx, str_space, payload);

      utoa(nrf_get_nei_store_service_info_err(idx), payload, 10);
      strcpy_P(str_space, mesh_device_nrf_err_x);
      send_mqtt_message_prefix_id_topic_payload(&mqtt_client, str_topic, idx, str_space, payload);

      utoa(nrf_get_nei_store_service_info_renew(idx), payload, 10);
      strcpy_P(str_space, mesh_device_nrf_renew_x);
      send_mqtt_message_prefix_id_topic_payload(&mqtt_client, str_topic, idx, str_space, payload);
    }
  }

  ///uint8_t nrf_list_meas_store(uint8_t idx, uint8_t *device_id, uint8_t *type, uint8_t *last_seen, float *value, char *name)
  strcpy_P(str_topic, text_mesh_nrf_meas_store);
  for (uint8_t idx = 0; idx < ram_nrf_meas_store_count; idx++)
  {
    if (nrf_list_meas_store(idx, &tmp1, &tmp2, &tmp3, &value, payload) == 1)
    {
      strcpy_P(str_space, text_name);
      send_mqtt_message_prefix_id_topic_payload(&mqtt_client, str_topic, idx, str_space, payload);
      ///
      strcpy_P(str_space, text_device);
      itoa(tmp1, payload, 10);
      send_mqtt_message_prefix_id_topic_payload(&mqtt_client, str_topic, idx, str_space, payload);
      ///
      strcpy_P(str_space, text_type);
      itoa(tmp2, payload, 10);
      send_mqtt_message_prefix_id_topic_payload(&mqtt_client, str_topic, idx, str_space, payload);
      ///
      strcpy_P(str_space, mesh_device_nrf_last_seen_x);
      itoa(tmp3, payload, 10);
      send_mqtt_message_prefix_id_topic_payload(&mqtt_client, str_topic, idx, str_space, payload);
      ///
      strcpy_P(str_space, text_value);
      dtostrf(value, 4, 2, payload);
      send_mqtt_message_prefix_id_topic_payload(&mqtt_client, str_topic, idx, str_space, payload);
    }
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
  if (channel == A_LIGHT) light_curr = 1023 - data;
  a2d_run_now = 1;
}



///////////// kontrolni funkce ///////////////////////////
/// selftest_set_0 - nastaveni chyboveho flagu
/*
   what - jaky chybovy flag
*/
void selftest_set_0(uint8_t what)
{
  sbi(selftest_data_0, what) ;
}

void selftest_set_1(uint8_t what)
{
  sbi(selftest_data_1, what) ;
}
///
/// selftest_clear_0 - vymazani chyboveho flagu, problem jiz neni
/*
   what - jaky chybovy flag
*/
void selftest_clear_0(uint8_t what)
{
  cbi(selftest_data_0, what) ;
}
void selftest_clear_1(uint8_t what)
{
  cbi(selftest_data_1, what) ;
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
  return selftest_data_0 & (1 << what);
}
uint8_t selftest_get_1(uint8_t what)
{
  return selftest_data_1 & (1 << what);
}
///
/// selftest - pusteni skupiny vlastnich selftestu
void selftest(void)
{
  if (!rtc.isrunning())
    //sbi(selftest_data, SELFTEST_ERR_RTC);
    selftest_set_0(SELFTEST_ERR_RTC);
  else
    //cbi(selftest_data, SELFTEST_ERR_RTC);
    selftest_clear_0(SELFTEST_ERR_RTC);

  if (Ethernet.link() == 1)
    //cbi(selftest_data, SELFTEST_ETH_LINK);
    selftest_clear_0(SELFTEST_ETH_LINK);
  else
    //sbi(selftest_data, SELFTEST_ETH_LINK);
    selftest_set_0(SELFTEST_ETH_LINK);
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
#define proud_constanta  499
  float proud = (vstup - proud_constanta) * 14;


  return proud;
}
///
/////////////// Casove funkce ///////////////////////////////////////////////////////////////////
/// time_get_offset - ziska casovy offset, letni/zimni cas
int8_t time_get_offset(void)
{
  return EEPROM.read(time_offset);
}
/// time_set_offset - nastavi casovy offset letni/zimni
void time_set_offset(int8_t offset)
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

  digitalWrite(PIEZO, HIGH);
  delay(1000);
  digitalWrite(PIEZO, LOW);



}
//////////////////////////////////////////////////////////////////////////////////////////////////
////////////////// Prevodni funkce ///////////////////////////////////////////////////////////////
/// funkce prevadi ciselnou hodnotu na skutecne pojmenovani


uint8_t convert_text_mode(char *str2)
{
  uint8_t mode = 0;
  if (strcmp_P(str2, text_off) == 0) mode = TERM_MODE_OFF;
  if (strcmp_P(str2, text_heat) == 0) mode = TERM_MODE_HEAT_MAX;
  if (strcmp_P(str2, text_manual) == 0) mode = TERM_MODE_MAN;
  if (strcmp_P(str2, text_auto) == 0) mode = TERM_MODE_PROG;
  if (strcmp_P(str2, text_cool) == 0) mode = TERM_MODE_CLIMATE_MAX;
  if (strcmp_P(str2, text_fan_only) == 0) mode = TERM_MODE_FAN;
  return mode;
}

void convert_mode_text(uint8_t mode, char *str)
{
  if (mode == TERM_MODE_OFF)   strcpy_P(str, text_off);
  if (mode == TERM_MODE_HEAT_MAX)   strcpy_P(str, text_heat);
  if (mode == TERM_MODE_MAN_HEAT)   strcpy_P(str, text_manual);
  if (mode == TERM_MODE_PROG)   strcpy_P(str, text_auto);
  if (mode == TERM_MODE_CLIMATE_MAX)   strcpy_P(str, text_cool);
  if (mode == TERM_MODE_FAN)   strcpy_P(str, text_fan_only);
}





/*                                ^~~~
  Projekt zabírá 167418 bytů (64%)  úložného místa pro program. Maximum je 261120 bytů.
  Globální proměnné zabírají 5121

*/


/*
   prevodni funkce id modu na textovy popisek
*/
void convert_mode_text_1(uint8_t mode, char *str)
{
  if (mode == TERM_MODE_OFF)   strcpy_P(str, text_button_term_off);
  if (mode == TERM_MODE_HEAT_MAX)   strcpy_P(str, text_button_term_max);
  if (mode == TERM_MODE_MAN_HEAT)   strcpy_P(str, text_button_term_man);
  if (mode == TERM_MODE_PROG)   strcpy_P(str, text_button_term_prog);
  if (mode == TERM_MODE_CLIMATE_MAX)   strcpy_P(str, text_button_term_man);
  if (mode == TERM_MODE_FAN)   strcpy_P(str, text_button_term_fan);
  if (mode == TERM_MODE_MAN)   strcpy_P(str, text_button_term_man);
  if (mode == TERM_MODE_MIN)   strcpy_P(str, text_button_term_min);
}
/*************************************************************************************************************************/

//////
void thermostat(void)
{
  uint8_t tdsid = 0;
  uint8_t tmode = 0;
  uint8_t tout = 0;
  int16_t thresh = 0;
  uint8_t tbites = 0;
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
    tbites = thermostat_ring_get_status_data(tix);
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

    if (tmode == TERM_MODE_MAN)
    {
      if ((tbites & (1 << STATUS_BIT_HEAT_OR_COOL)) == 0)
      {
        tmode = TERM_MODE_MAN_HEAT;
      }
      else
      {
        tmode = TERM_MODE_MAN_COOL;
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

    if (tdsid == RING_NO_INPUT)
    {
      tmode = TERM_MODE_ERR;
      pwm = 0;
      thermostat_ring_set_power(tix, pwm);
    }

    if (tdsid < HW_ONEWIRE_MAXROMS )
    {
      if (get_tds18s20(tdsid, &tds) == 1)
        if (tds.used == 1 && status_tds18s20[tdsid].online == True)
        {
          thermostat_pid_input(tix, status_tds18s20[tdsid].temp);
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


    if (tdsid >= HW_ONEWIRE_MAXROMS && tdsid < HW_ONEWIRE_MAXROMS + MAX_RTDS)
    {
      act = tdsid - HW_ONEWIRE_MAXROMS;
      remote_tds_get_active(act , &active);

      if (active == 1 && remote_tds_get_last_update(act) < 180 && remote_tds_get_type(act) == RTDS_REMOTE_TYPE_TEMP)
      {
        thermostat_pid_input(tix, remote_tds_get_data(act) / 1000.0);
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
      thermostat_ring_set_power(tix, POWER_OUTPUT_OFF);
    }
    if (tmode == TERM_MODE_HEAT_MAX)
    {
      mqtt_publis_output(tout, POWER_OUTPUT_HEAT_MAX);
      thermostat_ring_set_power(tix, POWER_OUTPUT_HEAT_MAX);
    }
    if (tmode == TERM_MODE_MIN)
    {
      mqtt_publis_output(tout, POWER_OUTPUT_COOL_MAX);
      thermostat_ring_set_power(tix, POWER_OUTPUT_COOL_MAX);
    }
    if (tmode == TERM_MODE_MAN_HEAT || tmode == TERM_MODE_MAN_COOL || tmode == TERM_MODE_FAN )
    {
      mqtt_publis_output_pwm(tout, tmode, pwm);
    }

    if (tmode == TERM_MODE_ERR)
    {
      mqtt_publis_output_pwm(tout, tmode, pwm);
      mqtt_publis_output(tout, POWER_OUTPUT_ERR);
    }
  }
}
/*************************************************************************************************************************/

void nrf_mesh_reinit(void)
{
  mesh.setNodeID(0);
  mesh.begin(nrf_load_channel());
}



void nrf_callback(nrf_message_t *nrf_message)
{
  char str1[20];
  char str2[20];
  char str3[20];
  nrf_message_process_total++;
  nrf_message->nodeid;
  nrf_message->mode;
  nrf_message->data;

  if (nrf_message->mode == 'G')
  {
    new_parse_at((char*)nrf_message->data, str1, str2, ':');
    if (strcmp_P(str1, mesh_device_nrf_name) == 0)
    {
      nrf_add_nei_store(nrf_message->nodeid, str2);
      nrf_reset_nei_last_update(nrf_message->nodeid);
    }
  }

  if (nrf_message->mode == 'S')
  {
    new_parse_at((char*)nrf_message->data, str1, str2, ':');
    if (strcmp_P(str1, mesh_device_nrf_uptime) == 0)
    {
      nrf_add_nei_store_service_info_uptime(nrf_message->nodeid, atoi(str2));
    }
    if (strcmp_P(str1, mesh_device_nrf_send) == 0)
    {
      nrf_add_nei_store_service_info_send(nrf_message->nodeid, atoi(str2));
    }
    if (strcmp_P(str1, mesh_device_nrf_recv) == 0)
    {
      nrf_add_nei_store_service_info_recv(nrf_message->nodeid, atoi(str2));
    }
    if (strcmp_P(str1, mesh_device_nrf_err) == 0)
    {
      nrf_add_nei_store_service_info_err(nrf_message->nodeid, atoi(str2));
    }
    if (strcmp_P(str1, mesh_device_nrf_renew) == 0)
    {
      nrf_add_nei_store_service_info_renew(nrf_message->nodeid, atoi(str2));
    }
  }

  if (nrf_message->mode == 'D')
  {
    new_parse_at((char*)nrf_message->data, str1, str2, ':');
    if (strcmp(str1, "TEP") == 0)
    {
      new_parse_at(str2, str1, str3, ':');
      nrf_add_update_meas_store(nrf_message->nodeid, NRF_MEAS_TYPE_TEMP, str1, atof(str3));
    }
    if (strcmp(str1, "HUM") == 0)
    {
      new_parse_at(str2, str1, str3, ':');
      nrf_add_update_meas_store(nrf_message->nodeid, NRF_MEAS_TYPE_HUMID, str1, atof(str3));
    }
    if (strcmp(str1, "TL") == 0)
    {
      new_parse_at(str2, str1, str3, ':');
      //printf("----> %s --> %s\n", str1, str3);
      nrf_add_update_meas_store(nrf_message->nodeid, NRF_MEAS_TYPE_PRESS, str1, atof(str3));
    }
    if (strcmp(str1, "IN") == 0)
    {
      new_parse_at(str2, str1, str3, ':');
      nrf_add_update_meas_store(nrf_message->nodeid, NRF_MEAS_TYPE_BYTE, str1, atoi(str3));
    }
    if (strcmp(str1, "VOLT") == 0)
    {
      new_parse_at(str2, str1, str3, ':');
      nrf_add_update_meas_store(nrf_message->nodeid, NRF_MEAS_TYPE_VOLT, str1, atof(str3));
    }
  }
}







/////////////////////////////////////////////////////
/// setup - zakladni nastaveni - vola se pouze 1x
void setup()
{
  uint8_t timestamp[4];
  char str1[64];
  char str2[64];
  char str3[64];
  char tmp1[20];
  char s_tritri[8];
  char s_petnula[8];
  char s_dvanact[8];
  char s_current[8];
  struct_DDS18s20 tds;
  long milis;
  uint8_t ok = 0;

  NTPClient timeClient(udpClient);
  DateTime time_now;

  wdt_reset();
  wdt_enable(WDTO_8S);

  if (EEPROM.read(bootloader_tag) == 0xee)
    EEPROM.write(bootloader_tag, 0);

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
  my_lcd.Fill_Screen(WHITE);
  my_lcd.Set_Draw_color(BLUE);
  strcpy_P(str1, term_title);
  show_string(str1, 30, 10, 2, BLUE, WHITE, 0);
  ///
  for (uint8_t init = 0;  init < 17; init++)
  {
    wdt_reset();
    show_string("X", 30 + (init * 15), 30, 1, RED, WHITE, 0);
    /// commit version
    if (init == 0)
    {
      time_now = DateTime(__DATE__, __TIME__);
      sprintf_P(str2, text_build_version, time_now.year(), time_now.month(), time_now.day(), time_now.hour(), time_now.minute(), time_now.second());
      show_string(str2, 30, 50 + (init * 10), 1, BLUE, WHITE, 0 );
    }
    ///
    /// inicalizace vnitrniho ADC prevodniku
    if (init == 1)
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
      init_a2d((1 << A_PROUD | 1 << A_TRIV | 1 << A_PETV | 1 << A_DVANACTV | 1 << A_LIGHT), 7, 3);
      a2d_handler(a2d_complete);
      ///
      for (uint8_t cnt = 0; cnt < 5; cnt++)
      {
        /// aktualni intenzita svetla
        a2d_run_now = 0;
        a2d_start_first(A_LIGHT);
        milis = millis();
        while (a2d_run_now == 0 && millis() - milis < 20);
        /// spotreba proudu
        a2d_run_now = 0;
        a2d_start_first(A_PROUD);
        milis = millis();
        while (a2d_run_now == 0 && millis() - milis < 20);
        /// regulator 3.3V
        a2d_run_now = 0;
        a2d_start_first(A_TRIV);
        milis = millis();
        while (a2d_run_now == 0 && millis() - milis < 20);
        /// regulator 5.0V
        a2d_run_now = 0;
        a2d_start_first(A_PETV);
        milis = millis();
        while (a2d_run_now == 0 && millis() - milis < 20);
        /// vstupni napeti 12V
        a2d_run_now = 0;
        a2d_start_first(A_DVANACTV);
        milis = millis();
        while (a2d_run_now == 0 && millis() - milis < 20);
      }
      /// zatim nepotrebuji automaticke mereni
      a2d_run_now = 0;
      /// prevedu float na string
      dtostrf(prepocet_napeti(tritri, CONST_PREVOD_TRIV), 4, 2, s_tritri);
      dtostrf(prepocet_napeti(petnula, CONST_PREVOD_PETV), 4, 2, s_petnula);
      dtostrf(prepocet_napeti(dvanact, CONST_PREVOD_DVANACTV), 4, 2, s_dvanact);
      dtostrf(prepocet_proudu(proud), 4, 2, s_current);
      sprintf_P(str2, text_volt_consume_light_info, s_tritri, s_petnula, s_dvanact, s_current);
      show_string(str2, 160, 50 + (init * 10), 1, GREEN, WHITE, 0 );
      delay(1000);
    }
    ///
    /// inicializace vnitrniho teplomeru LM75B
    if (init == 2)
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
    if (init == 3)
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
          //strcpy(tmp1, "");
          remote_tds_clear(idx);
        }
        ///
        for (uint8_t idx = 0; idx < AVAILABLE_PROGRAM; idx++)
        {
          strcpy_P(str2, text_prog);
          thermostat_program_set_name(idx, str2);
          thermostat_program_set_active(idx, PROG_FREE);
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
        device.myIP[0] = 192; device.myIP[1] = 168; device.myIP[2] = 2; device.myIP[3] = 111;
        device.myMASK[0] = 255; device.myMASK[1] = 255; device.myMASK[2] = 255; device.myMASK[3] = 0;

        device.myGW[0] = 192; device.myGW[1] = 168; device.myGW[2] = 2; device.myGW[3] = 1;
        device.myDNS[0] = 192; device.myDNS[1] = 168; device.myDNS[2] = 2; device.myDNS[3] = 1;
        device.mqtt_server[0] = 192; device.mqtt_server[1] = 168; device.mqtt_server[2] = 2; device.mqtt_server[3] = 1;
        device.ntp_server[0] = 192; device.ntp_server[1] = 168; device.ntp_server[2] = 2; device.ntp_server[3] = 1;
        device.mqtt_port = 1883;
        strcpy(device.mqtt_user, "saric");
        strcpy(device.mqtt_key, "no");
        save_setup_network();
        strcpy(str1, "TERM D2");
        device_set_name(str1);
        char hostname[10];
        device_get_name(hostname);
        default_ring = NO_DEFAULT_RING;
        last_default_ring = NO_DEFAULT_RING;
        set_default_ring(default_ring);
        EEPROM.write(my_brightness_values, 50);
        EEPROM.write(my_brightness_mode, 0);
        EEPROM.write(my_display_auto_shutdown, 0);

        nrf_save_channel(97);
        nrf_save_power(RF24_PA_LOW);

        output_virtual_persistent_store_init();
      }
      else
      {
        strcpy_P(str1, text_bezny_start);
        show_string(str1, 30, 50 + (init * 10), 1, GREEN, WHITE, 0 );
      }
    }
    ///
    /// test SRAM
    if (init == 4)
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
    ///
    /// init EEPROMKY
    if (init == 5)
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
    ///
    ///  nacteni provoznich parametru
    if (init == 6)
    {
      strcpy_P(str1, text_nastaveni_systemu);
      show_string(str1, 30, 50 + (init * 10), 1, GREEN, WHITE, 0 );
      /// nactu nastaveni site
      load_setup_network();
      //// kvuli lepsimu nabehu pocitani nastavim vychozi hodnotu na 2000 = 20 stupnu
      for (uint8_t idx = 0; idx < HW_ONEWIRE_MAXROMS; idx++)
      {
        for (uint8_t cnt = 0; cnt < MAX_AVG_TEMP; cnt++)
          status_tds18s20[idx].average_temp[cnt] = 200;
        status_tds18s20[idx].average_temp_now = 200;
        status_tds18s20[idx].crc_error = 0;
        status_tds18s20[idx].avg_temp_cnt = 0;
      }
      ///
      //for (uint8_t idx = 0; idx < MAX_THERMOSTAT; idx++)
      // last_output_update[idx] = 255;

      for (uint8_t idx = 0; idx < MAX_RTDS; idx++)
      {
        remote_tds_set_data(idx, 0);
        remote_tds_set_type(idx, RTDS_REMOTE_TYPE_FREE);
        remote_tds_set_last_update(idx, 255);
      }
      output_virtual_ram_store_init();
      output_virtual_ram_store_load_from_persistent_store();
    }
    ///
    /// zobrazeni kalibracnich informaci touchscreenu
    if (init == 7)
    {
      strcpy_P(str1, text_touchscreen);
      show_string(str1, 30, 50 + (init * 10), 1, GREEN, WHITE, 0 );
      my_touch.TP_Init(my_lcd.Get_Rotation(), my_lcd.Get_Display_Width(), my_lcd.Get_Display_Height());
      my_touch.TP_Set_Rotation(3);
      my_touch.TP_SetOnOff(LED_ON);
      for (uint8_t i = 1; i < 8; i++)
      {
        digitalWrite(LED, LOW);
        my_touch.TP_SetBacklight(i * 11);
        delay(100);
        digitalWrite(LED, HIGH);
        delay(100);
      }
      display_auto_shutdown = EEPROM.read(my_display_auto_shutdown);
      brigthness_display_values = EEPROM.read(my_brightness_values);
      brigthness_display_mode = EEPROM.read(my_brightness_mode);
      last_brigthness_display_mode = brigthness_display_mode;
      my_touch.TP_SetBacklight(brigthness_display_values * 2);
      brigthness_display_auto_values = 5;
    }
    ///
    /// inicializace ds2482
    if (init == 8)
    {
      strcpy_P(str1, text_onewire_rozhrani);
      show_string(str1, 30, 50 + (init * 10), 1, GREEN, WHITE, 0 );
      ds2482_address[0].i2c_addr = 0b0011000;
      ds2482_address[0].HWwirenum = 0;
      ///
      for (uint8_t idx = 0; idx < HW_ONEWIRE_MAXROMS; idx++ )
      {
        status_tds18s20[idx].wait = false;
        status_tds18s20[idx].online = false;
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
    if (init == 9)
    {
      strcpy_P(str1, text_onewire_hledam);
      show_string(str1, 30, 50 + (init * 10), 1, GREEN, WHITE, 0 );
      Global_HWwirenum = 0;
      one_hw_search_device(0);
      tds_update_associate();
      //strcpy (str1, "Nalezeno: ");
      //itoa(Global_HWwirenum, str2, 10);
      //strcat(str1, str2);
      sprintf_P(str1, text_find_new_onewire_devices, Global_HWwirenum);
      show_string(str1, 160, 50 + (init * 10), 1, GREEN, WHITE, 0 );
    }
    ///
    /// inicializace wiznet w5500
    if (init == 10)
    {
      digitalWrite(ETH_RST, HIGH);
      delay(10);
      digitalWrite(ETH_RST, LOW);
      delay(10);
      digitalWrite(ETH_RST, HIGH);
      delay(100);
      Ethernet.init(ETH_CS);
      Ethernet.begin(device.mac, device.myIP, device.myDNS, device.myGW, device.myMASK);
      w5500.setRetransmissionCount(2);
      w5500.setRetransmissionTime(600);
      strcpy_P(str1, text_ethernet_rozhrani);
      show_string(str1, 30, 50 + (init * 10), 1, GREEN, WHITE, 0 );
      delay(100);
      http_server.begin();
    }
    ///
    ///inicializace mqtt rozhrani
    if (init == 11)
    {
      strcpy_P(str1, text_mqtt_rozhrani);
      show_string(str1, 30, 50 + (init * 10), 1, GREEN, WHITE, 0 );

      mqtt_client.setServer(device.mqtt_server, device.mqtt_port);
      mqtt_client.setCallback(mqtt_callback);
      send_mqtt_set_header(thermctl_header_out);
      milis = millis();
      while ((millis() - milis) < 3000 )
      {
        lastmqttconnect = 0;
        mqtt_client.loop();
        if (mqtt_reconnect() == 0)
        {
          selftest_clear_0(SELFTEST_MQTT_LINK);
          break;
        }
        else
          selftest_set_0(SELFTEST_MQTT_LINK);
      }
      lastmqttconnect = 0;
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
    if (init == 12)
    {
      strcpy_P(str1, text_rest_rozhrani);
      show_string(str1, 30, 50 + (init * 10), 1, GREEN, WHITE, 0 );
    }
    ///
    /// inicializace nrf
    if (init == 13)
    {
      strcpy_P(str1, text_nrf_rozhrani);
      show_string(str1, 30, 50 + (init * 10), 1, GREEN, WHITE, 0 );
      nrf_init_nei_store();
      nrf_init_meas_store();
      scan_rf_net_enable = 0;
      radio.begin();
      ok = 1;
      for (uint8_t iidx = 0; iidx < 3; iidx++)
      {
        radio.setPALevel(iidx);
        if (radio.getPALevel() != iidx) ok = 0;
      }
      /// TODO vysledek ulozit do selftest
      ///
      radio.setPALevel(nrf_load_power());
      nrf_mesh_reinit();

    }
    ///
    /// inicializace termostatu
    if (init == 14)
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
      last_default_ring = default_ring;
      default_show_temp = get_default_show_temp();
      thermostat_mode_default_ring_last_state = thermostat_ring_get_mode(default_ring);
    }
    ///
    /// kontrola NTP serveru
    if (init == 15)
    {
      strcpy_P(str1, text_ntp_cas);
      show_string(str1, 30, 50 + (init * 10), 1, GREEN, WHITE, 0 );
      if (ntp_update(&timeClient, &rtc, time_get_offset(), &time_now) == 0)
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
    if (init == 16)
    {
      strcpy_P(str1, text_rtc_cas);
      show_string(str1, 30, 50 + (init * 10), 1, GREEN, WHITE, 0 );
      if (rtc.isrunning())
      {
        now = rtc.now();
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
  draw_menu(true, 0, 0, 0);
  a2d_run_now = 1;

  milis_10s = millis();
  milis_1s = millis();
  milis_05s = millis();
  milis_005s = millis();

}



/************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////
/// Hlavni programova smycka ///
void loop() {
  // put your main code here, to run repeatedly:
  char str1[16];
  char str2[32];

  unsigned long load_now;
  long mil;
  uint16_t click_x, click_y;

  nrf_message_t nrf_message;
  uint8_t nrf_data[NRF_MESSAGE_TOTAL_LEN];


  wdt_reset();

  load_now = millis();
  if  (Ethernet.link() == 1)
  {
    if (mqtt_reconnect() == 0)
      selftest_clear_0(SELFTEST_MQTT_LINK);
    else
      selftest_set_0(SELFTEST_MQTT_LINK);

    mqtt_client.loop();


    if (ethClient1.connected())
      ethClient1.stop();


    EthernetClient http_client = http_server.available();
    if (http_client.connected())
    {
      while (http_client.available())
        http_client.read();
      http_client.write("ahoj");
    }
  }

  else
  {
    mqtt_client.disconnect();
  }


  if (scan_rf_net_enable == 0)
  {
    mesh.update();
    mesh.DHCP();
    // Check for incoming data from the sensors
    if (network.available()) {
      RF24NetworkHeader header;
      network.peek(header);

      switch (header.type) {
        case 'M':
          {
            network.read(header, &nrf_data, NRF_MESSAGE_TOTAL_LEN);
            nrf_message_receive_total++;
            convert_data_to_nrf_message(&nrf_message, nrf_data);
            nrf_callback(&nrf_message);
            break;
          }
        default: network.read(header, 0, 0); break;
      }
    }
  }

  if (scan_rf_net_enable == 1)
  {
    scan_rf_network(&radio, scan_rf_net_channel);
    /// kazdy kanal proskenuji 128x
    nrf_scan_check_cnt ++;
    if (nrf_scan_check_cnt > 127)
    {
      scan_rf_net_channel++;
      nrf_scan_check_cnt = 0;
    }
    if (scan_rf_net_channel > 127)
    {
      selftest_clear_0(SELFTEST_NRF_SCAN);
      stop_scan_rf_network(&radio);
      scan_rf_network_public(&mqtt_client);
      nrf_mesh_reinit();
    }
  }


  if (scan_rf_net_enable == 2)
  {
    scan_rf_net_enable = 0;
    //radio.printDetails();
  }

  for (uint8_t idx = 0; idx < MAX_THERMOSTAT; idx++)
    thermostat_pid_compute(idx);

  ////////////////////
  /// kazdych 10sec
  if ((millis() - milis_10s) >= 10000)
  {
    milis_10s += 10000;
    menu_redraw10s = 1;
    //device_get_name(str1);
    internal_temp = lm75_temp.readTemperatureC();
    brigthness_display_auto_values = (float) (light_curr - light_min) / (light_max - light_min) * 100;
    if (last_brigthness_display_auto_values != brigthness_display_auto_values)
    {
      last_brigthness_display_auto_values = brigthness_display_auto_values;
      change_auto_brightness = 1;
    }


    if ((brigthness_display_mode & (1 << DISPLAY_MODE_AUTO_BRIGHTNESS)) != 0) // Automatika
    {
      if (brigthness_display_auto_values > 100)
        brigthness_display_auto_values = 100;
      if (brigthness_display_auto_values < 5)
        brigthness_display_auto_values = 5;
      my_touch.TP_SetBacklight(brigthness_display_auto_values * 2);
    }
  }

  if ((millis() - milis_1s) >= 1000)
  {
    milis_1s += 1000;
    uptime++;
    if ((brigthness_display_mode & (1 << DISPLAY_MODE_AUTO_SHUTDOWN_DISPLAY)) != 0)
    {
      if (display_auto_shutdown > display_auto_shutdown_now)
        display_auto_shutdown_now++;
      else
        my_touch.TP_SetOnOff(LED_OFF);
    }

    mereni_hwwire();
    thermostat();
    tds_extended_memory_store();
    remote_tds_update_last_update();
    nrf_inc_nei_last_update();
    nrf_inc_meas_store_last_update();

    strcpy_P(str2, thermctl_subscribe);
    device_get_name(str1);
    send_mqtt_payload(&mqtt_client, str2, str1);

    update_know_mqtt_device();
    use_virtual_output = output_virtual_inc_last_update();
    use_virtual_output_persistent = output_virtaul_persistent_store_active();

    if (status_send_counter == 0)
    {
      send_mqtt_onewire();
    }
    if (status_send_counter == 1)
    {
      send_mqtt_tds();
    }
    ///
    if (status_send_counter == 2)
    {
      send_mqtt_status(&mqtt_client);
      send_mqtt_status_critical_only(&mqtt_client);
    }
    if (status_send_counter == 3)
    {
      send_device_status();
    }
    ///
    if (status_send_counter == 4)
    {
      send_mqtt_ring();
    }
    if (status_send_counter == 5)
    {
      send_mqtt_program();
      for (uint8_t idx = 0; idx < MAX_THERMOSTAT; idx++)
        if (thermostat_ring_get_active(idx) != RING_FREE)
          mqtt_send_pid_variable(idx);
    }
    ///
    if (status_send_counter == 6)
    {
      send_mqtt_remote_tds_status();
    }
    if (status_send_counter == 7)
    {
      //send_network_config(&mqtt_client);
      //send_light_controler();
      send_know_device();
    }
    if (status_send_counter == 8)
    {
      send_mesh_status();
    }
    ///
    if (status_send_counter == 9)
    {
      send_virtual_output();
    }
    ///
    status_send_counter++;
    if (status_send_counter == 10)
    {
      status_send_counter = 0;
    }

    /*
      for (int i = 0; i < mesh.addrListTop; i++)
      {
      payload_t payload = {millis(),uptime};
      if (mesh.addrList[i].nodeID == 1) {  //Searching for node one from address list
        payload = {uptime%3,uptime};
      }
      RF24NetworkHeader header(mesh.addrList[i].address, OCT); //Constructing a header
      if (network.write(header, &payload, sizeof(payload)) == 1)
        printf("mesh send ok\n");
      else
        printf("mesh send errr\n");
      }
    */
  }


  //digitalWrite(LED, LOW);
  if ((millis() - milis_05s) >= 500)
  {
    milis_05s += 500;
    now = rtc.now();
    selftest();
    menu_redraw05s = 1;
    if (click_delay_display < 250)
      click_delay_display++;

    if (click_delay_enable_display < 250)
      click_delay_enable_display++;

    use_rtds_type_temp = 0;
    use_rtds = count_use_rtds(&use_rtds_type_temp);
    use_tds = count_use_tds();
    use_nrf_temp = count_use_nrf_temp();
  }


  /// kazdych 100ms
  if ((millis() - milis_005s) >= 50 )
  {
    milis_005s += 50;

    /// obsluha adc prevodniku
    if (a2d_run_now == 1)
    {
      a2d_start_process();
      a2d_run_now = 0;
      if (a2d_done() == 0) a2d_run_now = 1;
    }
  }

  ///display_touch_click
  my_touch.TP_Scan(0);
  if (my_touch.TP_Get_State() & TP_PRES_DOWN)
  {
    click_x = my_touch.x;
    click_y = my_touch.y;
    click_on_display = 1;
    tone(PIEZO, 500, 50);
  }
  else
  {
    click_x = 0;
    click_y = 0;
    click_on_display = 0;
  }


  if (click_on_display == 1 && click_on_display_last == 0)
  {
    click_delay_display = 0;
    display_auto_shutdown_now = 0;
    if (((brigthness_display_mode & (1 << DISPLAY_MODE_AUTO_SHUTDOWN_DISPLAY)) != 0) && my_touch.TP_GetOnOff() == 0)
    {
      my_touch.TP_SetOnOff(LED_ON);
      //printf("zapinam display\n");
      click_delay_enable_display = 0;
    }
  }

  if (click_on_display == 1 && click_on_display_last == 1 && click_on_display_hold == 0 && click_delay_display > 4)
  {
    click_on_display_hold = 1;
  }

  if (click_on_display == 0 && click_on_display_last == 1 && click_on_display_hold == 1)
  {
    click_on_display_hold = 0;
  }

  if (click_on_display == 0 && click_on_display_last == 0)
  {
  }

  click_on_display_last = click_on_display;

  if (click_delay_enable_display > 2)
    if (draw_menu(false, 0, click_x, click_y) == true)
    {
      draw_menu(true, 0, 0, 0);
    }


  load = millis() - load_now;
  if (load < load_min) load_min = load;
  if (load > load_max) load_max = load;

  /// automaticke nastaveni jasu displaye
  if (light_curr < light_min) light_min = light_curr;
  if (light_curr > light_max) light_max = light_curr;
}

//-----------------------------------------------------------------------------------------------------------------------------------
void display_element_rectangle(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint16_t args1, uint8_t args2, char *text)
{
  my_lcd.Set_Draw_color(args1);
  my_lcd.Draw_Rectangle(x, y, x + size_x, y + size_y);
}

void display_element_fill_rectangle(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint16_t args1, uint8_t args2, char *text)
{
  my_lcd.Set_Draw_color(args1);
  my_lcd.Fill_Rectangle(x, y, x + size_x, y + size_y);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void display_element_dialog_default_ring(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint16_t args1, uint8_t args2, char *text)
{
  char name[10];
  char default_text[30];
  strcpy_P(default_text, new_text_regulator);
  if (thermostat_ring_get_active(args2) != RING_FREE)
  {
    thermostat_ring_get_name(args2, name);
    strcat(default_text, name);
  }
  else
  {
    strcpy_P(name, text_term_deactive);
    strcat(default_text, name);
  }
  show_string(default_text, x, y, 2, BLACK, WHITE, 0);
}

////
/*
  void display_element_default_ring_set_temp(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint8_t args1, uint8_t args2)
  {
  display_element_dialog_set_variable(x, y, size_x, size_y, args1, args2);
  }
*/


/// dekorativni funkce zobrazeni casu
void display_element_show_time_decorate_1(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint16_t args1, uint8_t args2, char *text)
{
  uint16_t pos = 0;
  char str1[24];
  strcpy_P(str1, current_time);
  pos = (244 - show_string_size_width(16, 1)) / 2;
  show_string(str1, x + pos + 5, y + 5, 1, BLACK, WHITE, 0);
  my_lcd.Draw_Rectangle(x, y, x + size_x, y + size_y);
}

/// funkce pro zobrazeni casu
void display_element_show_time_1(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint16_t args1, uint8_t args2, char *text)
{
  char str1[24];
  if (selftest_get_0(SELFTEST_ERR_RTC) == 0)
  {
    sprintf_P(str1, text_time_format, now.hour(), now.minute());
  }
  else
  {
    strcpy(str1, "--:--");
  }
  my_lcd.Set_Draw_color(BLACK);
  show_string(str1, x + 4, y + 20, 8, BLACK, WHITE, 0);
}
///

/// dekorativni funkce pro zobrazeni teploty
void display_element_show_temp_decorate_1(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint16_t args1, uint8_t args2, char *text)
{
  char str1[32];
  char str2[20];
  float te;
  uint8_t stat;
  strcpy_P(str1, current_temp);
  stat = get_global_temp(default_show_temp, str2, &te);
  strcat(str1, str2);
  show_string(str1, x + 5, y + 5, 1, BLACK, WHITE, 0);
  my_lcd.Set_Draw_color(BLACK);
  my_lcd.Draw_Rectangle(x, y, x + size_x, y + size_y);
}
/// funkce pro zobrazeni teploty
void display_element_show_temp_1(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint16_t args1, uint8_t args2, char *text)
{
  char str1[32];
  char str2[20];
  float te;
  uint8_t stat;
  stat = get_global_temp(default_show_temp, str2, &te);
  if (stat > 0)
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
}

/// funkce pro ukazani vsech namerenych hodnot
void display_element_show_all_temp(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint16_t args1, uint8_t args2, char *text)
{
  char name[32];
  char str1[8];
  float te;
  uint8_t active;
  uint8_t cri = 0;

  dtostrf(internal_temp, 4, 2, str1);
  sprintf_P(name, new_text_internal_temp_value, str1);
  show_string(name, x, y + (25 * cri), 2, BLACK, WHITE, 0);

  cri++;
  for (uint8_t idx = 0; idx < button_get_show_default_temp_max_items(INPUT_SENSOR_SHOW_ACTIVE, 0, 0); idx++)
  {
    active = get_global_temp(idx, name, &te);
    if (active > 0)
    {
      strcat(name, ": ");
      dtostrf(te, 4, 1, str1);
      strcat(str1, "C");
      strcat(name, str1);
    }
    else
    {
      strcat(name, ": ");
      strcat_P(name, text_err);
    }

    show_string(name, x, y + (25 * cri), 2, BLACK, WHITE, 0);
    cri++;
  }

}
////////////////////////////////////////////////////
void display_element_show_link_status(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint16_t args1, uint8_t args2, char *text)
{
  char str1[16];
  uint16_t color = GREEN;
  if (selftest_get_0(SELFTEST_MQTT_LINK) == 0 && selftest_get_0(SELFTEST_ETH_LINK) == 0 )
  {
    strcpy_P(str1, text_mqtt_connect);
  }
  else
  {
    strcpy_P(str1, text_mqtt_disconnect);
    color = RED;
  }
  my_lcd.Set_Draw_color(WHITE); my_lcd.Draw_Fast_HLine(x, y, 142); my_lcd.Draw_Fast_HLine(x, y + 1, 142); show_string(str1, x, y + 2, 3, color, WHITE, 0);
}

////////////////////////////////////////////////////
void display_element_show_date_1(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint16_t args1, uint8_t args2, char *text)
{
  char str1[16];
  if (selftest_get_0(SELFTEST_ERR_RTC) == 0)
  {
    sprintf_P(str1, text_date_format, now.day(), now.month(), now.year());
  }
  else
  {
    strcpy_P(str1, new_text_date_error);
  }
  my_lcd.Set_Draw_color(WHITE); my_lcd.Draw_Fast_HLine(x, y, 142); my_lcd.Draw_Fast_HLine(x, y + 1, 142); show_string(str1, x, y + 2, 3, BLACK, WHITE, 0);
}
/////
void display_element_dialog_show_text(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint16_t args1, uint8_t args2, char *text)
{
  show_string(dialog_text, x, y, 2, BLACK, WHITE, 0);
}
//////////////////////////////////////////////
/////////////////////////////////////////////
/// zobrazeni detailnich informaci o virtualnim vystupu
/// args2 ... index virtualniho vystupu
void display_element_show_virtual_output_detail(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint16_t args1, uint8_t args2, char *text)
{
  char line1[12];
  char str_tmp[26];
  uint8_t state, id, type, last_update, persistent_id;
  my_lcd.Set_Draw_color(BLACK);
  my_lcd.Draw_Rectangle(x, y, x + size_x , y + size_y);

  strcpy(str_tmp, "");
  strcpy(line1, "");
  if (output_virtual_ram_store_get_used(args2) == 1)
  {
    persistent_id = 255;
    output_virtual_ram_store_list_store(args2, line1, &state, &id, &type, &last_update);
    for (uint8_t idx = 0; idx < eeprom_know_output_virtual_count; idx++)
      if (output_virtual_persistent_store_get_id(idx) == id)
      {
        persistent_id = idx;
        strcpy(str_tmp, "* ");
        strcat(str_tmp, line1);
        strcpy(line1, str_tmp);
        break;
      }
    show_string(line1, x + 10, y + 10 , 2, BLACK, WHITE, 0);
    sprintf(str_tmp, "perzistence: %d", persistent_id);
    show_string(str_tmp, x + 10, y + 40 , 1, BLACK, WHITE, 0);
    sprintf(str_tmp, "stav: %d", state);
    show_string(str_tmp, x + 10, y + 55 , 1, BLACK, WHITE, 0);
    sprintf(str_tmp, "virtualni kanal: %d", id);
    show_string(str_tmp, x + 10, y + 70 , 1, BLACK, WHITE, 0);
    sprintf(str_tmp, "typ vystupu: %d", type);
    show_string(str_tmp, x + 10, y + 95 , 1, BLACK, WHITE, 0);
    sprintf(str_tmp, "posledni aktualizace: %d", last_update);
    show_string(str_tmp, x + 10, y + 110 , 1, BLACK, WHITE, 0);
  }
}

void display_element_show_tds_info_dynamics(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint16_t args1, uint8_t args2, char *text)
{
  struct_DDS18s20 tds;
  char str1[26];
  char str2[26];
  float te;
  if (get_tds18s20(args2, &tds) == 1)
  {
    /// zobrazeni teploty
    te = status_tds18s20[args2].temp / 10.0;
    dtostrf(te, 4, 2, str1);
    strcat(str1, "C");
    strcpy_P(str2, current_temp_short);
    strcat(str2, str1);
    show_string(str2, x + 10, y + 140 , 2, BLACK, WHITE, 0);
  }
}
////
void display_element_show_tds_info_static(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint16_t args1, uint8_t args2, char *text)
{
  struct_DDS18s20 tds;
  char str1[26];
  char str2[26];
  float te;
  my_lcd.Set_Draw_color(BLACK);
  my_lcd.Draw_Rectangle(x, y, x + size_x , y + size_y);
  if (get_tds18s20(args2, &tds) == 1)
  {
    createString(str2, ':', tds.rom, 8, 16, 2);
    sprintf(str1, "rom: %s", str2);
    show_string(str1, x + 10, y + 85, 1, BLACK, WHITE, 0);
    ///
    strcpy_P(str2, text_nazev);
    sprintf(str1, "%s: %s", str2, tds.name);
    show_string(str1, x + 10, y + 40, 2, BLACK, WHITE, 0);
    /// zobrazeni offsetu
    te = tds.offset / 10.0;
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
    ///
    strcpy_P(str2, text_crc_error);
    sprintf(str1, "%s: %d", str2, status_tds18s20[args2].crc_error);
    show_string(str1, x + 10, y + 160 , 2, RED, WHITE, 0);
  }
  else
  {
    strcpy_P(str1, text_err);
    show_string(str1, x + 25, y + 40, 4, RED, WHITE, 0);
  }
}
/////
void display_element_show_rtds_info_dynamics(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint16_t args1, uint8_t args2, char *text)
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
    if (remote_tds_get_type(args2) == RTDS_REMOTE_TYPE_TEMP)
    {
      /// teplota
      strcpy_P(str1, new_text_rtds_type_temp);
      show_string(str1, x + 5, y + 60 , 1, BLACK, WHITE, 0);

      te = remote_tds_get_data(args2) / 1000.0;
      dtostrf(te, 4, 2, str1);
      strcat(str1, "C");
      strcpy_P(str2, current_temp_short);
      strcat(str2, str1);
      show_string(str2, x + 5, y + 80 , 2, BLACK, WHITE, 0);
      /// posledni aktualizace
      last_update = remote_tds_get_last_update(args2);
      strcpy_P(str2, text_last_update);
      sprintf(str1, "%s: %d", str2, last_update);
      show_string(str1, x + 5, y + 100 , 1, BLACK, WHITE, 0);
      active = 1;
      if (last_update >= 250) active = 0;
      strcpy_P(str2, text_online);
      sprintf(str1, "%s: %d", str2, active);
      show_string(str1, x + 5, y + 120 , 2, BLACK, WHITE, 0);
    }
  }
}
////////////////////////////////////////////////////////
void display_element_dialog_set_variable(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint16_t idx, uint8_t args2, char *text)
{
  uint16_t tmp1, posx1, posy1;
  char line1[9];
  float te;

  my_lcd.Set_Draw_color(BLACK);
  for (uint8_t r = 0; r < 3; r++)
    my_lcd.Draw_Rectangle(x + r, y + r, x + size_x - r, y + size_y - r);
  my_lcd.Set_Draw_color(WHITE);
  my_lcd.Fill_Rectangle(x + 3, y + 3, x + size_x - 3, y + size_y - 3);

  if (menu_dialog_variable[idx].number_type == NUMBER_TYPE_FLOAT)
    dtostrf(menu_dialog_variable[idx].variable_now, 4, 2, line1);

  if (menu_dialog_variable[idx].number_type == NUMBER_TYPE_INT)
    itoa(menu_dialog_variable[idx].variable_now, line1, 10);

  my_lcd.Set_Draw_color(BLACK);

  tmp1 = show_string_size_width(strlen(line1), 2);
  posx1 = (size_x - tmp1) / 2;
  posy1 = (size_y - show_string_size_hight(2)) / 2;
  show_string(line1, x + posx1 , y + posy1 - 1 , 2, BLACK, WHITE, 0);

  if (strlen(text) > 0)
    show_string(text, x + 5 , y + posy1 - 1 , 2, BLACK, WHITE, 0);
}
//////////////
void display_element_dialog_set_string(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint16_t args1, uint8_t args2, char *text)
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
void display_element_vertical_slider(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint16_t args1, uint8_t args2, char *text)
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
void display_function_vertical_slider_inc(uint16_t args1, uint16_t args2, uint8_t args3)
{
  if (menu_slider_data_current[args1] < (menu_slider_data_max - menu_slider_data_max_element))
  {
    menu_slider_data_current[args1]++;
    display_update_slider = 1;
  }
}
///
void display_function_vertical_slider_dec(uint16_t args1, uint16_t args2, uint8_t args3)
{
  if (menu_slider_data_current[args1] > menu_slider_data_min)
  {
    menu_slider_data_current[args1]--;
    display_update_slider = 1;
  }
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
////////////////////////////
////////////////////////////
void display_element_show_thermostat_statistics(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint16_t args1, uint8_t args2, char *text)
{
  char str1[32];

  my_lcd.Set_Draw_color(BLACK);
  my_lcd.Draw_Rectangle(x, y, x + size_x , y + size_y);
  if (thermostat_ring_get_active(args2) == 1)
  {
    thermostat_ring_get_name(args2, str1);
    show_string(str1, x + 5, y + 30 , 2, BLACK, WHITE, 0);

    thermostat_ring_get_power(args2);

    thermostat_ring_get_output(args2);

    thermostat_ring_get_mezni(args2);

    thermostat_ring_get_asociate_tds(args2);

    thermostat_ring_get_mode(args2);

    thermostat_ring_get_program_id(args2);

    thermostat_get_pid_p(args2);
    thermostat_get_pid_i(args2);
    thermostat_get_pid_d(args2);
    thermostat_get_pid_time(args2);
  }
  else
  {
    strcpy_P(str1, text_term_deactive);
    show_string(str1, x + 5, y + 40 , 3, BLACK, WHITE, 0);
  }
}

/*
   args2 je index intervalu
   global_menu_args2 k jakemu programu je interval prizaren
*/
void display_element_show_program_interval_statistics(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint16_t args1, uint8_t args2, char *text)
{
  char str1[16];
  uint8_t program_id = Global_menu_args2[MenuHistoryIndex];
  uint8_t interval_id = args2;
  //printf("%d -> %d\n", interval_id, program_id);
  my_lcd.Set_Draw_color(BLACK);
  my_lcd.Draw_Rectangle(x, y, x + size_x , y + size_y);
  if (thermostat_program_get_active(program_id) != PROG_FREE)
  {
    thermostat_program_get_name(program_id, str1);
    show_string(str1, x + 5, y + 30 , 2, BLACK, WHITE, 0);
  }
  else
  {
    strcpy_P(str1, text_term_deactive);
    show_string(str1, x + 5, y + 40 , 3, BLACK, WHITE, 0);
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
  uint8_t prvni = 0;
  uint8_t druhy = 0;
  back_color = LIGHTGREY;
  if (state == BUTTON_NO_ACTIVE)
    back_color = color_inactive;
  if (state == BUTTON_ACTIVE)
    back_color = color_active;
  if (state ==  BUTTON_INACTIVE)
    back_color = LIGHTGREY;
  if (state == BUTTON_NO_SHOW)
  {
    goto button_click_2_end ;
  }
  my_lcd.Set_Draw_color(BLACK);
  for (uint8_t r = 0; r < 3; r++)
    my_lcd.Draw_Rectangle(x + r, y + r, x + size_x - r, y + size_y - r);
  my_lcd.Set_Draw_color(back_color);
  my_lcd.Fill_Rectangle(x + 3, y + 3, x + size_x - 3, y + size_y - 3);
  if (strlen(line1) > 0)
  {
    prvni = 1;
    num_lines_const = 2;
  }
  if (strlen(line2) > 0)
  {
    druhy = 1;
    num_lines_const = num_lines_const + 2;
  }
  if (prvni == 1)
  {
    tmp1 = show_string_size_width(strlen(line1), font_size_1);
    posx1 = (size_x - tmp1) / 2;
    posy1 = (size_y - show_string_size_hight(font_size_1)) / num_lines_const;
    show_string(line1, x + posx1 , y + posy1 - 1 , font_size_1, BLACK, back_color, 0);
  }
  if (druhy == 1)
  {
    tmp2 = show_string_size_width(strlen(line2), font_size_2);
    posx2 = (size_x - tmp2) / 2;
    posy2 = posy1 + ((size_y - show_string_size_hight(font_size_2)) * 2 / num_lines_const);
    show_string(line2, x + posx2 , y + posy2 - 1 , font_size_2, BLACK, back_color, 0);
  }
button_click_2_end:
  state = 0;
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
uint8_t get_function_rtds_last_index_for_menu(uint16_t args1, uint16_t args2, uint8_t args3)
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

void get_function_rtds_text_button(uint8_t args1, uint8_t args2, uint8_t args3, char *line1, char *line2)
{
  uint8_t active;
  strcpy_P(line1, text_not_used);
  line2[0] = 0;
  remote_tds_get_active(args1, &active);
  if (active == 1)
  {
    remote_tds_get_complete(args1, &active, line1);
    if (remote_tds_get_type(args1) == RTDS_REMOTE_TYPE_TEMP)
      strcpy_P(line2, new_text_rtds_type_temp);
  }
}

/*
   args1 --- atributy z nastaveni tlacitka
   args2 --- index polozky z menu
*/
void click_rtds_deassociate(uint16_t args1, uint16_t args2, uint8_t args3)
{
  remote_tds_unsubscibe_topic(args2);
  remote_tds_clear(args2);
  MenuHistoryPrevMenu(0, 0, 0);
}
////
void click_rtds_subscribe(uint16_t args1, uint16_t idx, uint8_t args3)
{
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
void get_function_one_wire_associate_or_setting_text_button(uint8_t args1, uint8_t args2 , uint8_t args3, char *line1, char *line2)
{
  char str2[26];
  strcpy_P(line1, text_not_used);
  strcpy(line2, "-");
  if (w_rom[args1].used == 1)
  {
    createString(str2, ':', w_rom[args1].rom, 8, 16, 2);
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
uint8_t get_function_one_wire_last_index_for_menu(uint16_t idx, uint16_t args2, uint8_t args3)
{
  uint8_t active = 0;
  for (uint8_t cnt = 0; cnt < HW_ONEWIRE_MAXROMS; cnt++)
    if (w_rom[cnt].used == 1) active++;
  return active;
}

void helper_tds_associate(uint16_t idx, uint16_t args2, uint8_t args3)
{
  tds_associate(idx);
}

/// funkce. ktera
void click_tds_associate_or_setting_onewire(uint16_t args1, uint16_t args2, uint8_t idx)
{
  //printf("%d %d\n", args1, idx);
  if (w_rom[idx].tds_idx == 255)
  {
    MenuHistoryNextMenu(MENU_DIALOG_YES_NO, 0, 0);
    dialog_yes_function = &helper_tds_associate;
    dialog_yes_args1 = idx;
    strcpy_P(dialog_text, text_associovat_tds);
  }
  else
  {
    MenuHistoryNextMenu(MENU_NASTAVENI_TDS, w_rom[idx].tds_idx, 0);
  }
}
/// zruseni associace
void click_tds_deassociate_onewire(uint16_t args1, uint16_t idx, uint8_t args3)
{
  tds_set_clear_wrom_id(idx);
  MenuHistoryPrevMenu(0, 0, 0);
}

void clik_button_onewire_scan_bus(uint16_t args1, uint16_t args2, uint8_t args3)
{
  char str1[32];
  Global_HWwirenum = 0;
  one_hw_search_device(0);
  tds_update_associate();
  MenuHistoryNextMenu(MENU_DIALOG_OK, 0, 0);
  /// TODO text
  sprintf_P(str1, text_find_new_onewire_devices, Global_HWwirenum);
  strcpy(dialog_text, str1);
}
//////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//// vraci pocet pouzitych vzdalenych mqtt tds cidel
uint8_t count_use_rtds(uint8_t *count_type_temp)
{
  uint8_t cnt = 0;
  uint8_t active = 0;
  uint8_t cc = 0;
  for (uint8_t idx = 0; idx < MAX_RTDS; idx++)
  {
    remote_tds_get_active(idx, &active);
    if (active == 1)
    {
      cnt++;
      if (remote_tds_get_type(idx) == RTDS_REMOTE_TYPE_TEMP)
      {
        cc = *count_type_temp;
        cc++;
        *count_type_temp = cc;
      }
    }
  }
  return cnt;
}


void click_rtds_add_sensor(uint16_t args1, uint16_t args2, uint8_t args3)
{
  char rtds_topic[5];
  uint8_t idx;
  idx = remote_tds_find_free();
  if (idx != 255)
  {
    strcpy_P(rtds_topic, text_rtds_prefix);
    MenuHistoryNextMenu(MENU_DIALOG_KEYBOARD_ALFA, 0, 0);
    display_element_set_string(rtds_topic, RTDS_DEVICE_STRING_LEN, idx, 0, &menu_rtds_create_name, &valid_true);
  }
}

void menu_rtds_create_name(uint16_t args1, uint16_t args2, uint8_t args3)
{
  char name[RTDS_DEVICE_STRING_LEN];
  uint8_t active = 1;
  uint8_t idx = display_element_get_string_args1();
  display_element_get_string(name);
  //printf("create %s, %d\n", name, idx);
  if (strlen(name) > 0)
    if (remote_tds_name_exist(name) == 255)
    {
      remote_tds_set_complete(idx, active, name);
      remote_tds_subscibe_topic(idx);
    }
}

void menu_rtds_update_name(uint16_t args1, uint16_t args2, uint8_t args3)
{
  char name[RTDS_DEVICE_STRING_LEN];
  uint8_t idx = display_element_get_string_args1();
  display_element_get_string(name);
  //printf("update %s, %d\n", name, idx);
  remote_tds_set_name(idx, name);
  remote_tds_subscibe_topic(idx);
}

/*
    args1 - parametr z globalniho nastaveni tlacitek
    args2 / definice menu
    loop_idx - polozka z menu
*/
void click_rtds_setting_sensor(uint16_t args1, uint16_t args2, uint8_t loop_idx)
{
  char name[RTDS_DEVICE_STRING_LEN];
  uint8_t active;
  remote_tds_get_active(loop_idx, &active);
  if (active == 1)
  {
    MenuHistoryNextMenu(MENU_NASTAVENI_RTDS_DETAIL, loop_idx, 0);
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// funkce, ktera vraci pocet vsech polozek virtualnich vystupu
uint8_t get_function_list_virtual_output_max_active_items(uint16_t idx, uint16_t args2, uint8_t args3)
{
  return ram_output_virtual_count;
}

/// funkce obsluha kliknuti na tlacitko virtualniho vystupu
/// idx ... index polozky menu = index v pameti
/// args1 predani promene args, index dalsiho menu
void click_virtual_output_associate_action(uint16_t args1, uint16_t args2, uint8_t idx)
{
  MenuHistoryNextMenu(args1, idx, 0);
}
/// funkce ktera vraci popisky pro tlacitko
/// args1 ... index polozky v menu
void  get_function_list_virtual_output_labels(uint8_t args1, uint8_t args2 , uint8_t args3, char *line1, char *line2)
{
  char str_tmp[16];
  uint8_t state, id, type, last_update, per_id;
  strcpy_P(line1, text_dashdash);
  strcpy(line2, "");
  if (output_virtual_ram_store_get_used(args1) == 1)
  {
    per_id = 255;
    output_virtual_ram_store_list_store(args1, line1, &state, &id, &type, &last_update);
    for (uint8_t idx = 0; idx < eeprom_know_output_virtual_count; idx++)
      if (output_virtual_persistent_store_get_id(idx) == id)
      {
        per_id = idx;
        strcpy(str_tmp, "* ");
        strcat(str_tmp, line1);
        strcpy(line1, str_tmp);
        break;
      }
    sprintf(line2, "S:%d,I:%d,T:%d,U:%d,P%d", state, id, type, last_update, per_id);
  }
}

void clik_button_virtual_output_manual(uint16_t args1, uint16_t args2, uint8_t args3)
{
  char text_power[10];
  uint8_t virtual_id;
  //output_virtual_ram_store_get_name(args2, text_power);
  virtual_id = output_virtual_ram_store_get_id(args2);
  ////state jako posledni nastavena hodnota na vystupu, muze byt MAX,OFF,PWM cislo
  //itoa(output_virtual_ram_store_get_state(args2), text_power, 10);
  itoa(0, text_power, 10);
  MenuHistoryNextMenu(MENU_DIALOG_KEYBOARD_NUMBER, 0, 0);
  display_element_set_string(text_power, 4, virtual_id, 0, &helper_dialog_set_value_virtual_output, &valid_true);
}

void helper_dialog_set_value_virtual_output(uint16_t args1, uint16_t args2, uint8_t args3)
{
  char value[4];
  uint8_t virtual_id;
  uint8_t val;
  display_element_get_string(value);
  virtual_id = display_element_get_string_args1();
  val = atoi(value);
  //output_virtual_ram_store_set_state(output_virtual_ram_store_new(virtual_id), val);
  mqtt_publis_output(virtual_id, val);
}

/// args1 ... index polozky v menu
void clik_button_virtual_output_test(uint16_t args1, uint16_t args2, uint8_t args3)
{
}
///
//. args2 ... index virtualniho vystupu
void clik_button_virtual_output_associate(uint16_t args1, uint16_t args2, uint8_t args3)
{
  output_virtual_persistent_store_associate_from_ram_store(args2);
}
//. args2 ... index virtualniho vystupu
void clik_button_virtual_output_unassociate(uint16_t args1, uint16_t args2, uint8_t args3)
{
  uint8_t vid;
  if (output_virtual_persistent_store_get_used(args2) == 1)
  {
    vid  = output_virtual_persistent_store_get_id(args2);
    if (vid != 255)
    {
      thermostat_ring_clear_output_in_rings(vid);
      output_virtual_persistent_store_clear(args2);
    }
  }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// funkce ktera vraci hodnoty pro zobrazeni, vrazi nazev, teplotu, navratova hodnota, zda jsou cisla aktualne platna
uint8_t get_global_temp(uint8_t device, char*name, float * temp)
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
          if (status_tds18s20[idx].online == true)
          {
            *temp = status_tds18s20[idx].temp / 10.0;
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
        if (remote_tds_get_last_update(idx) < 250 && remote_tds_get_type(idx) == RTDS_REMOTE_TYPE_TEMP)
        {
          *temp = remote_tds_get_data(idx) / 1000.0;
          ret = 2;
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
/*
   v args2 je bud vylistovat vsechny a nebo pouze aktivni, pouze neaktivni
   v args1 je index poradi prvku grafickeho menu
*/
void button_get_show_default_temp(uint8_t args1, uint8_t args2, uint8_t args3, char *line1, char *line2)
{
  char name[20];
  uint8_t cri = 0;
  uint8_t active;
  strcpy_P(line1, nastaveni_onewire);
  strcpy(line2, "");
  /// pro tds cidla
  for (uint8_t idx = 0; idx < HW_ONEWIRE_MAXROMS; idx++)
  {
    if (((tds_used(idx) == 1) && (args3 == INPUT_SENSOR_SHOW_ACTIVE)) || args3 == INPUT_SENSOR_SHOW_ALL)
    {
      if (cri == args1)
      {
        tds_get_name(idx, name);
        strcpy(line2, name);
        strcpy_P(line1, nastaveni_onewire);
        goto button_get_show_default_temp_end;
      }
      cri++;
    }
  }
  /// pro rtds
  strcpy_P(line1, nastaveni_rtds);
  strcpy(line2, "");
  for (uint8_t idx = 0; idx < MAX_RTDS; idx++)
  {
    remote_tds_get_active(idx, &active);
    if (((active == 1) && (args3 == INPUT_SENSOR_SHOW_ACTIVE) && (remote_tds_get_type(idx) == RTDS_REMOTE_TYPE_TEMP )) || args3 == INPUT_SENSOR_SHOW_ALL)
    {
      if (cri == args1)
      {
        remote_tds_get_complete(idx, &active, name);
        strcpy(line2, name);
        strcpy_P(line1, nastaveni_rtds);
        goto button_get_show_default_temp_end;
      }
      cri++;
    }
  }
button_get_show_default_temp_end:
  /// hack kvuli goto
  cri = 0;
}

void button_get_show_default_temp_1(uint8_t args1, uint8_t args2, uint8_t args3, char *line1, char *line2)
{
  button_get_show_default_temp(args1, args2, args3, line1, line2);
}
/////

//////////////////////////////////////////////////////////////////////////////
/// funkce, ktera nastavi promenou vychozi teplomer
/// funkce je povesena na klik tlacitka
/*
   args1 -- globalni argument z define prvku
   args2 -- globalni argument celeho menu
   args3 -- id polozky menu kliku
*/
void button_click_set_show_default_temp(uint16_t args1, uint16_t args2, uint8_t args3)
{
  default_show_temp = args3;
  set_default_show_temp(args3);
}
//////////////////////////////////////////////////////////////////////////////
/// funkce ktera vraci soucet vsech cidel v systemu
uint8_t button_get_show_default_temp_max_items(uint16_t args1, uint16_t args2, uint8_t args3)
{
  if (args1 == INPUT_SENSOR_SHOW_ACTIVE)
    return use_tds + use_rtds_type_temp;

  if (args1 == INPUT_SENSOR_SHOW_ALL)
    return HW_ONEWIRE_MAXROMS + MAX_RTDS;
}


/*
   funkce pro zobrazeni defaultniho cidla zobrazeni teploty
*/
uint8_t button_get_show_default_temp_active(uint16_t args1, uint16_t args2, uint8_t args3)
{
  uint8_t ret = BUTTON_NO_ACTIVE;
  if (default_show_temp == args3)
    ret = BUTTON_ACTIVE;

  // todo INPUT_SENSOR_SHOW_ACTIVE vs INPUT_SENSOR_SHOW_ALL
  return ret;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
   funkce pro zobrazeni prizareneho vstupniho cidla k ringu
   args3 ... id polozky menu
   args2 ... ring id
*/
uint8_t button_select_term_ring_input_in_dialog_status_fnt(uint16_t args1, uint16_t args2, uint8_t args3)
{
  uint8_t rtds_active = 0;
  uint8_t ret = BUTTON_NO_ACTIVE;
  if (thermostat_ring_get_asociate_tds(args2) == args3)
    ret = BUTTON_ACTIVE;

  // todo INPUT_SENSOR_SHOW_ACTIVE vs INPUT_SENSOR_SHOW_ALL
  if (args3 < HW_ONEWIRE_MAXROMS)
  {
    if (tds_used(args3) == 0) ret = BUTTON_INACTIVE;
  }
  else
  {
    remote_tds_get_active(args3 - HW_ONEWIRE_MAXROMS, &rtds_active);
    if (rtds_active == 0) ret = BUTTON_INACTIVE;
  }


  return ret;
}
///
void button_select_term_ring_input_in_dialog_onclick(uint16_t args1, uint16_t args2, uint8_t args3)
{
  thermostat_ring_set_asociate_tds(args2, args3);
  change_term_mode = 4;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
   funkce pro vymazani vazby ring a input cidlo
*/
void button_deassociate_input_sensor_for_term_onclick(uint16_t args1, uint16_t args2, uint8_t args3)
{
  thermostat_ring_set_asociate_tds(args2, RING_NO_INPUT);
  change_term_mode = 4;
}


/*
   funkce vymazani vazby ring a virtual output
   args2 ... ringid
*/
void button_deassociate_virtual_output_for_term_onclick(uint16_t args1, uint16_t args2, uint8_t args3)
{
  thermostat_ring_set_output(args2, POWER_OUTPUT_ERR);
  change_virtual_output = 4;
}
void button_get_show_virtual_output(uint8_t args1, uint8_t args2, uint8_t args3, char *line1, char *line2)
{
  strcpy_P(line1, new_text_virtual_output);
  output_virtual_persistent_store_get_name(args1, line2);
}



/*
   args2 je ring_id
    args3 je index polozky menu. Index polozky menu neznamena index vystupu, musi se to zjistit eepromky
*/
void button_select_term_ring_virtual_output_in_dialog_onclick(uint16_t args1, uint16_t args2, uint8_t args3)
{
  thermostat_ring_set_output(args2, output_virtual_persistent_store_get_id(args3));
  change_virtual_output = 4;
}

uint8_t button_get_virtual_output_max_items(uint16_t args1, uint16_t args2, uint8_t args3)
{
  return eeprom_know_output_virtual_count;
}

/// index polozky menu args3 neni cislo virtualniho vystupu musi se to zjistit z eeprmky
uint8_t button_select_term_ring_virtual_output_in_dialog_status_fnt(uint16_t args1, uint16_t args2, uint8_t args3)
{
  uint8_t rgt = thermostat_ring_get_output(args2);
  uint8_t psg = output_virtual_persistent_store_get_id(args3);
  uint8_t ret = BUTTON_NO_ACTIVE;
  if (rgt == psg && psg != 255)
    ret = BUTTON_ACTIVE;
  if (output_virtual_persistent_store_get_used(args3) == 0)
    ret = BUTTON_INACTIVE;
  return ret;
}


/////
/////

/// funkce vyberu vychozi ringu regulatoru
void button_change_default_ring_via_dialog_onclick(uint16_t args1, uint16_t args2, uint8_t args3)
{
  MenuHistoryNextMenu(MENU_NASTAVENI_SELECT_RING_SCREEN, default_ring, 0);
}

////

/****************************************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
   funkce vraci 1, kdyz se rovna id prvku s promenou default_ring. Default_ring je vybrany
   args1 ... globalni argument z definice menu
   args2 ... zde je cislo ringu
   args3 ... id polozky dynamickeho menu
*/
uint8_t button_get_term_ring_is_selected(uint16_t args1, uint16_t args2, uint8_t args3)
{
  uint8_t ret = BUTTON_NO_ACTIVE;
  uint8_t active_ring = thermostat_ring_get_active(args3);
  if ((args3 == default_ring) && (thermostat_ring_get_active(default_ring) == 1))
    ret = BUTTON_ACTIVE;
  if ((active_ring == 1) && (args3 != default_ring) && (args3 != last_default_ring))
    ret = BUTTON_NO_SHOW;
  if (active_ring == RING_FREE)
    ret = BUTTON_INACTIVE;
  return ret;
}
////
void term_ring_do_activate(uint16_t args1, uint16_t args2, uint8_t args3)
{
  thermostat_ring_set_active(args1, 1);
  last_default_ring = default_ring;
  default_ring = args1;
  set_default_ring(default_ring);
}

////
/*
   funkce pro nastaveni vychozi ringu z menu, v promene args3 mam id polozky tlacitka z menu
   ulozim rovnou do promene default ring
   ulozim do globalniho parametru menu
*/
void button_click_set_new_default_ring_in_dialog(uint16_t args1, uint16_t args2, uint8_t args3)
{
  if (thermostat_ring_get_active(args3) != RING_FREE)
  {
    last_default_ring = default_ring;
    default_ring = args3;
    set_default_ring(default_ring);
    MenuHistoryUpdateArgs1(args3);
  }
  else
  {
    MenuHistoryNextMenu(MENU_DIALOG_YES_NO, 0, 0);
    dialog_yes_function = &term_ring_do_activate;
    dialog_yes_args1 = args3;
    strcpy_P(dialog_text, text_term_do_activate);
  }
  display_redraw_default_ring = 1;
}

void button_no_default_ring_onclick(uint16_t args1, uint16_t args2, uint8_t args3)
{
  last_default_ring = default_ring;
  default_ring = RING_FREE;
  set_default_ring(default_ring);
  display_redraw_default_ring = 1;
}

/*
   funkce zobrazi zakladni informace v dialogu vyberu defaultniho ringu
   informace aktivni/neaktivni regulacni okruh
   args1 ... loop_id z menu
   args2 ... globalni parametr z menu
*/
void button_change_default_ring_labels_in_dialog(uint8_t args1, uint8_t args2, uint8_t args3, char *line1, char *line2)
{
  char name[10];
  char active[12];
  if (thermostat_ring_get_active(args1) != RING_FREE)
    strcpy_P(active, text_term_active);
  else
    strcpy_P(active, text_term_deactive);
  thermostat_ring_get_name(args1, name);
  sprintf(line2, "Reg. %d %s", args1, active);
  strcpy(line1, name);
}

/*
   funkce zobrazi vychozi popisek dynamickeho tlacitka v menu
   nazev vychoziho ringu
   index vychoziho ringu
   zadne vstupni argumenty
*/
void button_get_default_ring_labels(uint8_t args1, uint8_t args2, uint8_t args3, char *line1, char *line2)
{
  char name[10];
  strcpy_P(line1, text_change_default_ring);
  if (default_ring != NO_DEFAULT_RING)
  {
    thermostat_ring_get_name(default_ring, name);
    strcpy_P(line2, new_text_regulator);
    strcat(line2, name);
  }
  else
  {
    strcpy_P(line2, text_dashdash);
  }
}



void button_click_nastaveni_ring_screen(uint16_t args1, uint16_t args2, uint8_t args3)
{
  if (default_ring != NO_DEFAULT_RING)
    MenuHistoryNextMenu(args1, default_ring, 0);
}


/////////////////////
/// Funkce pro vyplneni popisku vyberoveho menu ringu termostatu
void dyn_button_show_all_thermostat_get_status_string(uint8_t args1, uint8_t args2, uint8_t args3, char *line1, char *line2)
{
  char name[10];
  thermostat_ring_get_name(args1, name);
  strcpy_P(line1, new_text_regulator);
  strcat(line1, name);
  if (thermostat_ring_get_active(args1) == 1)
    convert_mode_text_1(thermostat_ring_get_mode(args1), line2);
  else
    strcpy_P(line2, text_dashdash);
}

/*
   funkce pro vyber menu nastaveni regulatoru
   args3 je index dynamickeho tlacitka
*/
void dyn_button_show_all_thermostat_dyn_button_onclick(uint16_t args1, uint16_t args2, uint8_t args3)
{
  MenuHistoryNextMenu(MENU_NASTAVENI_RING_SCREEN, args3, 0);
}






/*******************************************************************************************************/
/*  funkce pro nastavovani nazvu regulacniho ringu
   pomocna funkce, ktera po stisknuti tlacitka ulozit v menu dialogu vykona akci
   args1
   args2
   args3
   promene nemaji zadny vyznam
*/

void helper_thermostat_button_set_ring_name(uint16_t args1, uint16_t args2, uint8_t args3)
{
  char name[10];
  display_element_get_string(name);
  args2 = display_element_get_string_args1();
  thermostat_ring_set_name(args2, name);
}
///////////////////////////////////
/*
   pomocna funkce pro nastaveni nazvu regulatoru
   vyplni parametry pro dialog klavesnice
   args1 - nepotrebuji
   args2 je idcko tlacitka
   args3 - nepotrebuji
   promene nemaji zadny vyznam
*/

void thermostat_button_set_ring_name_onclick(uint16_t args1, uint16_t args2, uint8_t args3)
{
  char name[10];
  thermostat_ring_get_name(args2, name);
  MenuHistoryNextMenu(MENU_DIALOG_KEYBOARD_ALFA, 0, 0);
  display_element_set_string(name, 9, args2, 0, &helper_thermostat_button_set_ring_name, &valid_true);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
   Funkce pro aktivovani/deaktivovani ringu termostatu
*/

void helper_thermostat_button_deactivate_ring_onclick(uint16_t args1, uint16_t args2, uint8_t args3)
{
  thermostat_ring_clear(args1);
}

void helper_thermostat_button_activate_ring_onclick(uint16_t args1, uint16_t args2, uint8_t args3)
{
  thermostat_ring_set_active(args1, 1);
}

/// funkce pro deaktivaci termostatu
void thermostat_button_deactivate_ring_onclick(uint16_t args1, uint16_t args2, uint8_t args3)
{
  //printf("deaktivece %d-%d-%d\n", args1, args2, args3);
  char str2[10];
  MenuHistoryNextMenu(MENU_DIALOG_YES_NO, 0, 0);
  dialog_yes_function = &helper_thermostat_button_deactivate_ring_onclick;
  dialog_yes_args1 = args2;
  strcpy_P(dialog_text, text_term_do_deactivate);
  strcat(dialog_text, ": ");
  thermostat_ring_get_name(args2, str2);
  strcat(dialog_text, str2);
}

void thermostat_button_activate_ring_onclick(uint16_t args1, uint16_t args2, uint8_t args3)
{
  //printf("aktivece %d-%d-%d\n", args1, args2, args3);
  char str2[10];
  MenuHistoryNextMenu(MENU_DIALOG_YES_NO, 0, 0);
  dialog_yes_function = &helper_thermostat_button_activate_ring_onclick;
  dialog_yes_args1 = args2;
  strcpy_P(dialog_text, text_term_do_activate1);
  strcat(dialog_text, ": ");
  thermostat_ring_get_name(args2, str2);
  strcat(dialog_text, str2);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/********************************************************/
/*
     Obsluzne funkce dynamickeho tlacitka vyberu vychoziho modu ringu

*/

/////
/*
   funkce pro zjisteni, zda mode v args1 je aktivni a nebo ne
   obsluzna funkce pro zobrazeni modu
   args1 .. parametr z definice prvku
   args2 .. parametr z definice menu
   args3 .. index prvku v menu
   return
     - uint8_t 1... activni, 0... neaktivni
*/

uint8_t button_status_default_ring_term_has_mode(uint16_t args1, uint16_t args2, uint8_t args3)
{
  uint8_t ret = BUTTON_NO_ACTIVE;
  if (thermostat_ring_get_active(default_ring) != RING_FREE)
  {
    if (thermostat_ring_get_mode_1(default_ring, args1) == true)
      ret = BUTTON_ACTIVE;
    if (thermostat_ring_get_mode(default_ring) != args1 && thermostat_mode_default_ring_last_state != args1)
      ret = BUTTON_NO_SHOW;
    //printf("last:%d, mode%d, idx%d\n", thermostat_mode_default_ring_last_state, thermostat_ring_get_mode(default_ring), args1);
  }
  //printf("ret %d\n\n", ret);
  return ret;
}
/////////////////////////////////////////////////////////
/*
   funkce pro nastaveni modu regulatoru pouze ve vychozim ringu
   args1 ... parametr z definice prvku
   args2 ... parametr z defini menu
   args3 ... index prvku v menu
*/

void button_click_default_term_set_mode(uint16_t args1, uint16_t args2, uint8_t args3)
{
  if (thermostat_ring_get_active(default_ring) != RING_FREE)
  {
    thermostat_mode_default_ring_last_state = thermostat_ring_get_mode(default_ring);

    if (args1 == TERM_MODE_PROG && thermostat_mode_default_ring_last_state == TERM_MODE_PROG)
    {
      MenuHistoryNextMenu(NEW_MENU_THERMOSTAT_SET_PROG, default_ring, 0);
    }

    if (args1 == TERM_MODE_MAN && thermostat_mode_default_ring_last_state == TERM_MODE_MAN)
    {
      MenuHistoryNextMenu(NEW_MENU_THERMOSTAT_MAN_TEMP, default_ring, 0);
    }

    thermostat_ring_set_mode(default_ring, args1);
    change_term_mode = 1;
  }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/************************************************************************************************************************/
/*

    Funkce pro nastavovani modu termostatu z menu dialog
    Obsluha tlacitek
*/



///
/*
   funkce ktera vraci 1 na aktualne vybrany mod podle indexu prvku v menu. Pozor na pevne stanovene poradi modu
   args1 ... globalni argument z definice menu
   args2 ... zde je cislo ringu
   args3 ... id polozky dynamickeho menu
*/
uint8_t button_select_term_mode_get_status_fnt(uint16_t args1, uint16_t args2, uint8_t args3)
{
  uint8_t mode;
  uint8_t ret = BUTTON_NO_SHOW;
  if (thermostat_ring_get_active(args2) != RING_FREE)
  {
    ret = BUTTON_NO_ACTIVE;
    mode = thermostat_ring_get_mode(args2);
    if (args3 == 0 && mode == TERM_MODE_OFF) ret = BUTTON_ACTIVE;
    if (args3 == 1 && mode == TERM_MODE_HEAT_MAX) ret = BUTTON_ACTIVE;
    if (args3 == 2 && mode == TERM_MODE_MIN) ret = BUTTON_ACTIVE;
    if (args3 == 3 && mode == TERM_MODE_PROG) ret = BUTTON_ACTIVE;
    if (args3 == 4 && (mode == TERM_MODE_MAN || mode == TERM_MODE_MAN_HEAT || mode == TERM_MODE_MAN_COOL)) ret = BUTTON_ACTIVE;
  }
  return ret;
}

///
/*
   ulozeni vybraneho modu, pozor na pevne stanovene poradi
   args1 ... globalni argument z definice menu
   args2 ... zde je cislo ringu,
   args3 ... id polozky menu
*/

void button_select_term_mode_onclick(uint16_t args1, uint16_t args2, uint8_t args3)
{
  if (args3 == 0) thermostat_ring_set_mode(args2, TERM_MODE_OFF);
  if (args3 == 1) thermostat_ring_set_mode(args2, TERM_MODE_HEAT_MAX);
  if (args3 == 2) thermostat_ring_set_mode(args2, TERM_MODE_MIN);
  if (args3 == 3) thermostat_ring_set_mode(args2, TERM_MODE_PROG);
  if (args3 == 4) thermostat_ring_set_mode(args2, TERM_MODE_MAN);
  change_term_mode = 1;
}

///
/*
   prevodni funkce
   id polozky menu na id modu. Je pevne stanovene poradi
   off,max,min,prog,man
   line1 ... kratke pojmenovani modu
   line2 ... komentar
*/
void button_select_term_mode_get_status_string(uint8_t args1, uint8_t args2, uint8_t args3, char *line1, char *line2)
{
  if (args1 == 0)
  {
    strcpy_P(line1, text_button_term_off);
    strcpy_P(line2, text_button_term_off_comment);
  }
  if (args1 == 1)
  {
    strcpy_P(line1, text_button_term_max);
    strcpy_P(line2, text_button_term_max_comment);
  }
  if (args1 == 2)
  {
    strcpy_P(line1, text_button_term_min);
    strcpy_P(line2, text_button_term_min_comment);
  }
  if (args1 == 3)
  {
    strcpy_P(line1, text_button_term_prog);
    strcpy_P(line2, text_button_term_prog_comment);
  }
  if (args1 == 4)
  {
    strcpy_P(line1, text_button_term_man);
    strcpy_P(line2, text_button_term_man_comment);
  }
}








///////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
/************************************************************************************************************/
/*

     funkce pro povoleni zakazani polozek z menu
     zde se resi rucni mod chlazeni/topeni

*/
/*
   funkce pro zjisteni, jestli manualni rezim je pro topeni a nebo chlazeni
   args1 ... paramentr z definice prvku
   args2 ... paramentr z define menu
   args3 ... index prvku v menu
*/



uint8_t button_get_term_heat_or_cool(uint16_t args1, uint16_t args2, uint8_t args3)
{
  uint8_t ret = BUTTON_NO_SHOW;
  uint8_t b;
  if (thermostat_ring_get_active(args2) != RING_FREE)
  {
    ret = BUTTON_NO_ACTIVE;
    b = thermostat_ring_get_status_bites(args2, STATUS_BIT_HEAT_OR_COOL);
    if (b == 0 && args1 == TERM_MODE_MAN_HEAT)
      ret = BUTTON_ACTIVE;
    if (b != 0 && args1 == TERM_MODE_MAN_COOL)
      ret = BUTTON_ACTIVE;
  }
  return ret;
}

/*
   funkce pro nastaveni manualniho rezimu topeni a nebo chlazeni
   args1 ... parametr z definice prvku
   args2 ... parametr z menu
   args3 ... index prvku v poradi menu
*/

void button_click_set_term_heat_or_cool(uint16_t args1, uint16_t args2, uint8_t args3)
{
  if (thermostat_ring_get_active(args2) != RING_FREE)
  {
    if (args1 == TERM_MODE_MAN_HEAT)
      thermostat_ring_update_bites(args2, STATUS_BIT_HEAT_OR_COOL, 0);
    if (args1 == TERM_MODE_MAN_COOL)
      thermostat_ring_update_bites(args2, STATUS_BIT_HEAT_OR_COOL, 1);
    change_term_mode = 2;
  }
}

/*
   funkce vraci 1 kdyz mame vybrany z nasledujich modu, rucni rizeni a jeho alternativy
   kdyz 0 tak se nezobrazuji polozky v menu
   args2 .. index ring
*/
uint8_t display_enable_show_term_mode_man(uint16_t args1, uint16_t args2, uint8_t args3)
{
  uint8_t mode = 0;
  if (thermostat_ring_get_active(args2) != RING_FREE)
  {
    mode = thermostat_ring_get_mode(args2);
    if (mode == TERM_MODE_MAN_HEAT || mode == TERM_MODE_MAN_COOL || mode == TERM_MODE_MAN)
      return 1;
  }
  return 0;
}


uint8_t display_enable_show_thermostat_active_ring(uint16_t args1, uint16_t args2, uint8_t args3)
{
  uint8_t ret = 0;
  if (thermostat_ring_get_active(args2) == RING_FREE)
    ret = 1;
  return ret;
}

uint8_t display_enable_show_thermostat_deactive_ring(uint16_t args1, uint16_t args2, uint8_t args3)
{
  uint8_t ret = 0;
  if (thermostat_ring_get_active(args2) != RING_FREE)
    ret = 1;
  return ret;
}

uint8_t display_enable_show_term_mode_prog(uint16_t args1, uint16_t args2, uint8_t args3)
{
  if (thermostat_ring_get_active(args2) != RING_FREE)
  {
    if (thermostat_ring_get_mode(args2) == TERM_MODE_PROG)
      return 1;
  }
  return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
    CASOVY TERMOSTAT
*/

void click_button_time_program_new(uint16_t args1, uint16_t args2, uint8_t args3)
{
  /// prvne najdu prvni volnou pametovou bunku
  uint8_t free_id = 255;
  char program_name[eeprom_thermostat_program_name_bytes_size];
  for (uint8_t idx = 0; idx < AVAILABLE_PROGRAM; idx++)
  {
    if (thermostat_program_get_active(idx) == PROG_FREE)
    {
      free_id = idx;
      break;
    }
  }
  if (free_id != 255)
  {
    MenuHistoryNextMenu(MENU_DIALOG_KEYBOARD_ALFA, 0, 0);
    strcpy(program_name, "PRG");
    display_element_set_string(program_name, eeprom_thermostat_program_name_bytes_size, free_id, 0, &helper_dialog_new_time_program, &valid_thermostat_program_name);
  }
}

void helper_dialog_new_time_program(uint16_t args1, uint16_t args2, uint8_t args3)
{
  char program_name[eeprom_thermostat_program_name_bytes_size];
  uint8_t new_id;
  display_element_get_string(program_name);
  new_id = display_element_get_string_args1();
  thermostat_program_set_name(new_id, program_name);
  thermostat_program_set_active(new_id, PROG_ACTIVE);
}


//// saric
uint8_t get_function_active_program_max_items(uint16_t idx, uint16_t args2, uint8_t args3)
{
  uint8_t active = 0;
  for (uint8_t idx = 0; idx < AVAILABLE_PROGRAM; idx++)
  {
    if ( thermostat_program_get_active(idx) != PROG_FREE)
    {
      menu_dialog_mappings[active] = idx;
      active++;
    }
  }
  return active;
}

void click_select_active_program(uint16_t args1, uint16_t args2, uint8_t idx)
{
  thermostat_program_set_active(args2, menu_dialog_mappings[idx]);
  display_update_slider = 1;
}

/*
   args2 .... index regulatoru
   args3 .... index polozky z menu
*/
uint8_t button_select_active_program_get_status_fnt(uint16_t args1, uint16_t args2, uint8_t args3)
{
  if (thermostat_program_get_active(args2) == menu_dialog_mappings[args3])
    return BUTTON_ACTIVE;
  else
    return BUTTON_NO_ACTIVE;
}

void get_function_active_program_label(uint8_t args1, uint8_t args2 , uint8_t args3, char *line1, char *line2)
{
  //strcpy(line1, "line1");
  char name[10];
  //if (thermostat_program_get_active(args) != PROG_FREE)
  thermostat_program_get_name(menu_dialog_mappings[args1], name);
  //sprintf(line1, "%d %s", args1, name, menu_dialog_mappings[args1]);
  strcpy(line1, name);
  strcpy(line2, "");
}



uint8_t get_function_list_all_program_max_items(uint16_t idx, uint16_t args2, uint8_t args3)
{
  return AVAILABLE_PROGRAM;
}

void click_list_all_program(uint16_t args1, uint16_t args2, uint8_t idx)
{
  if (thermostat_program_get_active(idx) != PROG_FREE)
    MenuHistoryNextMenu(NEW_MENU_THERMOSTAT_TIME_PROGRAM_MENU, idx, 0);
}

void get_function_list_all_program_label(uint8_t args1, uint8_t args2 , uint8_t args3, char *line1, char *line2)
{
  char str_tmp1[16];
  char str_tmp2[4];
  switch (thermostat_program_get_active(args1))
  {
    case PROG_FREE:
      strcpy_P(line2, text_term_deactive);
      break;
    case 1:
      strcpy_P(line2, text_button_mode_heat);
      break;
    case 2:
      strcpy_P(line2, text_button_mode_cool);
      break;
    case 3:
      strcpy_P(line2, text_button_term_fan);
      break;
    case PROG_ACTIVE:
      strcpy_P(line2, text_button_term_default);
      break;
    default:
      strcpy(line2, "-");
      break;
  }
  thermostat_program_get_name(args1, line1);
  itoa(args1, str_tmp2, 10);
  strcpy(str_tmp1, str_tmp2);
  strcat(str_tmp1, ". ");
  strcat(str_tmp1, line1);
  strcpy(line1, str_tmp1);
}

/////////////////////////////


void click_button_time_program_name(uint16_t args1, uint16_t args2, uint8_t idx)
{
  char program_name[16];
  thermostat_program_get_name(args2, program_name);
  MenuHistoryNextMenu(MENU_DIALOG_KEYBOARD_ALFA, 0, 0);
  display_element_set_string(program_name, eeprom_thermostat_program_name_bytes_size, args2, 0, &helper_dialog_new_time_program, &valid_thermostat_program_name);
}

void click_button_time_program_remove(uint16_t args1, uint16_t args2, uint8_t idx)
{
  thermostat_program_set_active(args2, PROG_FREE);
  thermostat_program_set_name(args2, "-");
}


void click_button_time_program_setting(uint16_t args1, uint16_t args2, uint8_t idx)
{
  MenuHistoryNextMenu(NEW_MENU_THERMOSTAT_TIME_PROGRAM_SETTING, args2, 0);
}


void click_button_time_program_interval_remove(uint16_t args1, uint16_t args2, uint8_t idx)
{
  uint8_t program_id = Global_menu_args2[MenuHistoryIndex];
  uint8_t interval_id = args2;
  thermostat_program_set_interval_active(program_id, interval_id, 0);
}

/*
   args3 ... index tlacitka z menu
   args2 ... index programu, pro ktery se ma vytvaret novy casovy interval
   args1 ... staticka definice... nepotrebuji
*/
void click_button_time_program_new_interval(uint16_t args1, uint16_t args2, uint8_t args3)
{
  char interval_string[2];
  uint8_t new_id = 255;
  for (uint8_t idx = 0; idx < MAX_PROGRAM_INTERVAL; idx++)
  {
    if (thermostat_program_get_time_active(args2, idx) == 0)
    {
      new_id = idx;
      break;
    }
  }
  if (new_id != 255)
  {
    interval_string[0] = 0;
    MenuHistoryNextMenu(MENU_DIALOG_KEYBOARD_ALFA, 0, 0);
    display_element_set_string(interval_string, 32, args2, new_id, &helper_dialog_new_program_interval, &valid_thermostat_program_time_interval);
  }
}

//saric

void helper_dialog_new_program_interval(uint16_t args1, uint16_t args2, uint8_t args3)
{
  char interval_string[thermostat_program_time_interval_string_size];
  uint8_t program_id = display_element_get_string_args1();
  uint8_t interval_id = display_element_get_string_args2();
  display_element_get_string(interval_string);
  thermostat_program_set_parse_interval(program_id, interval_id, interval_string);
}


uint8_t get_function_list_program_interval_max_items(uint16_t idx, uint16_t args2, uint8_t args3)
{
  return MAX_PROGRAM_INTERVAL;
}
/*
   args2 je index programu
   idx je index casoveho intervalu
*/
void click_list_program_interval(uint16_t args1, uint16_t args2, uint8_t idx)
{
  if (thermostat_program_get_time_active(args2, idx) == 1)
    MenuHistoryNextMenu(NEW_MENU_THERMOSTAT_TIME_PROGRAM_MENU_DETAIL, idx, args2);
}
/*
   args1 je index tlacitka
   args2 je index programu
   args3 je argument staticke definice z menu
*/
void get_function_list_program_interval_label(uint8_t args1, uint8_t args2 , uint8_t args3, char *line1, char *line2)
{
  uint8_t start_hour, start_min, stop_hour, stop_min, active, week;
  uint16_t threshold_temp;
  thermostat_program_get_time(args2, args1, &start_hour, &start_min, &stop_hour, &stop_min, &active);
  week = thermostat_program_get_week(args2, args1);
  threshold_temp = thermostat_program_get_threshold(args2, args1);
  sprintf(line1, "id:%d", args1);
  strcpy(line2, "");
  if (active)
  {
    sprintf(line1, "id:%d start:%d:%d stop:%d:%d", args1, start_hour, start_min, stop_hour, stop_min);
    sprintf(line2, "tyden:%d, teplota:%d C", week, threshold_temp / 10);
  }
}



/*
   args2 ... index programu
   args1 ... cislo active modu
*/
void click_dyn_button_program_term_active(uint16_t args1, uint16_t args2, uint8_t idx)
{
  //printf("klik %d - %d - %d\n", args1, args2, idx);
  change_term_mode = 8;
  thermostat_program_set_active(args2, args1);
}

/*
   args2 ... index programu
   args1 ... cislo active modu
*/
uint8_t button_status_program_term_has_active(uint16_t args1, uint16_t args2, uint8_t args3)
{
  uint8_t ret;
  //printf("get status %d - %d - %d\n", args1, args2, args3);
  if (thermostat_program_get_active(args2) == args1)
    ret = BUTTON_ACTIVE;
  else
    ret = BUTTON_NO_ACTIVE;

  //printf("%d - %d == %d\n", thermostat_program_get_active(args2), args1, ret);
  return ret;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*

   Display funkce

*/
/// Funkce ktera vrati popisek pro tlacitko
void button_change_brightness_display_get_status_string(uint8_t args1, uint8_t args2, uint8_t args3, char *line1, char *line2)
{
  char str2[8];
  strcpy_P(line1, new_text_jas_display);
  if ((brigthness_display_mode & (1 << DISPLAY_MODE_AUTO_BRIGHTNESS)) != 0 )
  {
    strcpy_P(line2, new_text_jas_display_automat);
    itoa(brigthness_display_auto_values , str2, 10);
    strcat(line2, " ");
    strcat(line2, str2);
    strcat(line2, " ");
    strcat(line2, "%");
  }
  if ((brigthness_display_mode & (1 << DISPLAY_MODE_AUTO_BRIGHTNESS)) == 0 )
  {
    strcpy_P(line2, new_text_jas_display_manual);
    itoa(brigthness_display_values, str2, 10);
    strcat(line2, " ");
    strcat(line2, str2);
    strcat(line2, " ");
    strcat(line2, "%");
  }
}
///
/// Funce pro nastaveni dialogu pro zobrazeni nastaveni podsvetleni
void button_change_brightness_display_dyn_button_onclick(uint16_t args1, uint16_t args2, uint8_t args3)
{
  MenuHistoryNextMenu(NEW_MENU_DISPLAY_NASTAVENI_BRIGTHNESS_SCREEN, 0, 0);
}
/////////
/// Funkce, ktera vrati jestli je zapnute automaticke rizeni podsvetleni
uint8_t switch_brightness_automode_get_status_fnt(uint16_t args1, uint16_t args2, uint8_t args3)
{
  uint8_t ret = BUTTON_NO_ACTIVE;
  if ((brigthness_display_mode & (1 << DISPLAY_MODE_AUTO_BRIGHTNESS)) != 0 ) ret = BUTTON_ACTIVE;
  return ret;
}
////////
/// Funkce, ktera vraci popisky pro dynamicke tlacitko
void switch_brightness_automode_get_status_string(uint8_t args1, uint8_t args2, uint8_t args3, char *line1, char *line2)
{
  char str2[8];
  if ((brigthness_display_mode & (1 << DISPLAY_MODE_AUTO_BRIGHTNESS)) != 0 )
  {
    strcpy_P(line1, new_text_jas_display_automat);
    itoa(brigthness_display_auto_values , str2, 10);
    strcat(line1, " ");
    strcat(line1, str2);
    strcat(line1, " ");
    strcat(line1, "%");
  }
  if ((brigthness_display_mode & (1 << DISPLAY_MODE_AUTO_BRIGHTNESS)) == 0 )
  {
    strcpy_P(line1, new_text_jas_display_manual);
  }
}
////////
/// Obsluzna funkce nastaveni rizeni podsvetleni
void switch_brightness_automode_onclick(uint16_t args1, uint16_t args2, uint8_t args3)
{
  if ((brigthness_display_mode & (1 << DISPLAY_MODE_AUTO_BRIGHTNESS)) != 0 )
  {
    cbi(brigthness_display_mode, DISPLAY_MODE_AUTO_BRIGHTNESS);
    my_touch.TP_SetBacklight(brigthness_display_values * 2);
    goto switch_brightness_automode_onclick_end;
  }
  if ((brigthness_display_mode & (1 << DISPLAY_MODE_AUTO_BRIGHTNESS)) == 0 )
  {
    sbi(brigthness_display_mode, DISPLAY_MODE_AUTO_BRIGHTNESS);
  }
switch_brightness_automode_onclick_end:
  EEPROM.write(my_brightness_mode, brigthness_display_mode);
}
////////
/// Nastaveni dialogu nastaveni promene
uint8_t preload_display_setting_brightness(uint16_t args1, uint16_t args2, uint8_t args3)
{
  display_function_set_variable(brigthness_display_values, 5, 100, 5, 0, 0, NUMBER_TYPE_INT,  H_TRUE, 0, &helper_display_set_brightness);
}

void helper_display_set_brightness(uint16_t args1, uint8_t args2, float now, uint8_t args3)
{
  brigthness_display_values = args3;
  my_touch.TP_SetBacklight(brigthness_display_values * 2);
  EEPROM.write(my_brightness_values, brigthness_display_values);
}

/////
uint8_t display_enable_show_brightness_manual_mode(uint16_t args1, uint16_t args2, uint8_t args3)
{
  uint8_t ret = 0;
  if (switch_brightness_automode_get_status_fnt(0, 0, 0) == 0)
    ret = 1;
  return ret;
}

uint8_t display_enable_show_brightness_auto_mode(uint16_t args1, uint16_t args2, uint8_t args3)
{
  return !display_enable_show_brightness_manual_mode(args1, args2, args3);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*


   Funkce pro automaticke vypinani displaye pri neaktivite


*/
void button_set_brightness_auto_shutdown_get_status_string(uint8_t args1, uint8_t args2, uint8_t args3, char *line1, char *line2)
{
  strcpy_P(line1, new_text_auto_display_shutdown);
  if ((brigthness_display_mode & (1 << DISPLAY_MODE_AUTO_SHUTDOWN_DISPLAY)) == 0)
    strcpy_P(line2, new_text_vypnuto);
  else
    sprintf_P(line2, new_text_enabled_x_sec , display_auto_shutdown);
}



void button_set_brightness_auto_shutdown_dyn_symbol_onclick(uint16_t args1, uint16_t args2, uint8_t args3)
{
  MenuHistoryNextMenu(NEW_MENU_DISPLAY_NASTAVENI_AUTO_SHUTDOWN_SCREEN, 0, 0);
}

uint8_t button_set_brightness_auto_shutdown_get_status_fnt(uint16_t args1, uint16_t args2, uint8_t args3)
{
  uint8_t ret = BUTTON_NO_ACTIVE;
  if ((brigthness_display_mode & (1 << DISPLAY_MODE_AUTO_SHUTDOWN_DISPLAY)) != 0)
    ret = BUTTON_ACTIVE;
  return ret;
}

void button_set_brightness_auto_shutdown_select_time_get_status_string(uint8_t args1, uint8_t args2, uint8_t args3, char *line1, char *line2)
{
  line2[0] = 0;
  if (args1 == 0)
    strcpy_P(line1, new_text_vypnuto);
  if (args1 == 1)
    strcpy_P(line1, new_text_30sec);
  if (args1 == 2)
    strcpy_P(line1, new_text_1min);
  if (args1 == 3)
    strcpy_P(line1, new_text_2min);
  if (args1 == 4)
    strcpy_P(line1, new_text_3min);
  if (args1 == 5)
    strcpy_P(line1, new_text_4min);
}



void button_set_brightness_auto_shutdown_select_time_dyn_symbol_onclick(uint16_t args1, uint16_t args2, uint8_t args3)
{
  uint8_t click1 = 0;
  display_auto_shutdown_now = 0;
  if (args3 == 0)
  {
    cbi(brigthness_display_mode, DISPLAY_MODE_AUTO_SHUTDOWN_DISPLAY);
    display_auto_shutdown = 0;
    click1 = 1;
  }
  if (args3 == 1)
  {
    sbi(brigthness_display_mode, DISPLAY_MODE_AUTO_SHUTDOWN_DISPLAY);
    display_auto_shutdown = 30;
    click1 = 1;
  }
  if (args3 == 2)
  {
    sbi(brigthness_display_mode, DISPLAY_MODE_AUTO_SHUTDOWN_DISPLAY);
    display_auto_shutdown = 60;
    click1 = 1;
  }
  if (args3 == 3)
  {
    sbi(brigthness_display_mode, DISPLAY_MODE_AUTO_SHUTDOWN_DISPLAY);
    display_auto_shutdown = 120;
    click1 = 1;
  }
  if (args3 == 4)
  {
    sbi(brigthness_display_mode, DISPLAY_MODE_AUTO_SHUTDOWN_DISPLAY);
    display_auto_shutdown = 180;
    click1 = 1;
  }
  if (args3 == 5)
  {
    sbi(brigthness_display_mode, DISPLAY_MODE_AUTO_SHUTDOWN_DISPLAY);
    display_auto_shutdown = 250;
    click1 = 1;
  }
  if (click1 == 1)
  {
    EEPROM.write(my_display_auto_shutdown, display_auto_shutdown);
    EEPROM.write(my_brightness_mode, brigthness_display_mode);
  }
}

uint8_t button_set_brightness_auto_shutdown_select_time_get_status_fnt(uint16_t args1, uint16_t args2, uint8_t args3)
{
  uint8_t ret = BUTTON_NO_ACTIVE;
  if (args3 == 0 && display_auto_shutdown == 0)
    ret = BUTTON_ACTIVE;
  if (args3 == 1 && display_auto_shutdown == 30)
    ret = BUTTON_ACTIVE;
  if (args3 == 2 && display_auto_shutdown == 60)
    ret = BUTTON_ACTIVE;
  if (args3 == 3 && display_auto_shutdown == 120)
    ret = BUTTON_ACTIVE;
  if (args3 == 4 && display_auto_shutdown == 180)
    ret = BUTTON_ACTIVE;
  if (args3 == 5 && display_auto_shutdown == 250)
    ret = BUTTON_ACTIVE;
  return ret;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint8_t preload_display_setting_nrf(uint16_t args1, uint16_t args2, uint8_t args3)
{
  display_function_set_variable(nrf_load_channel(), 1, 100, 1, 0, 0, NUMBER_TYPE_INT,  H_TRUE, 0, &helper_display_set_nrf_channel);
  display_function_set_variable(nrf_load_power(), 1, 3, 1, 0, 0, NUMBER_TYPE_INT,  H_TRUE, 1, &helper_display_set_nrf_power);
}



void helper_display_set_nrf_channel(uint16_t args1, uint8_t args2, float now, uint8_t args3)
{
  nrf_save_channel(display_function_get_variable_int(0));
  mesh.setChannel(display_function_get_variable_int(0));
}

void helper_display_set_nrf_power(uint16_t args1, uint8_t args2, float now, uint8_t args3)
{
  nrf_save_power((rf24_pa_dbm_e)display_function_get_variable_int(1));
  radio.setPALevel(display_function_get_variable_int(1));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*

   Funkce pro zobrazeni sitovych informaci

*/
void display_element_show_network_detail(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint16_t args1, uint8_t args2, char *text)
{
  char str1[36];
  char str2[24];
  my_lcd.Set_Draw_color(BLACK);
  my_lcd.Draw_Rectangle(x, y, x + size_x , y + size_y);
  /// mac adresa
  strcpy_P(str1, new_text_device_mac);
  strcat(str1, ": ");
  createString(str2, '.', device.mac, 6, 16, 2);
  strcat(str1, str2);
  show_string(str1, x + 5 , y + 10 , 1, BLACK, WHITE, 0);
  /// nazev
  strcpy_P(str1, new_text_device_nazev);
  strcat(str1, ": ");
  strcat(str1, device.nazev);
  show_string(str1, x + 5 , y + 25 , 1, BLACK, WHITE, 0);
  /// ip adresa
  ip2CharArray(device.myIP, str2);
  strcpy_P(str1, new_text_device_ip);
  strcat(str1, ": ");
  strcat(str1, str2);
  show_string(str1, x + 5 , y + 55 , 1, BLACK, WHITE, 0);
  /// ip maska
  ip2CharArray(device.myMASK, str2);
  strcpy_P(str1, new_text_device_mask);
  strcat(str1, ": ");
  strcat(str1, str2);
  show_string(str1, x + 5 , y + 70 , 1, BLACK, WHITE, 0);
  /// ip gw
  ip2CharArray(device.myGW, str2);
  strcpy_P(str1, new_text_device_gw);
  strcat(str1, ": ");
  strcat(str1, str2);
  show_string(str1, x + 5 , y + 85 , 1, BLACK, WHITE, 0);
  /// ip dns serveru
  ip2CharArray(device.myDNS, str2);
  strcpy_P(str1, new_text_device_dns);
  strcat(str1, ": ");
  strcat(str1, str2);
  show_string(str1, x + 5 , y + 100 , 1, BLACK, WHITE, 0);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
   Obsluha tlacitka synchronizace NTP casu
*/


void button_click_ntp_sync_time(uint16_t args1, uint16_t args2, uint8_t args3)
{
  char str2[6];
  NTPClient timeClient(udpClient);
  DateTime ted;
  if (ntp_update(&timeClient, &rtc, time_get_offset(), &ted) == 1)
  {
    selftest_clear_0(SELFTEST_ERR_NTP);
    MenuHistoryNextMenu(MENU_DIALOG_OK, 0, 0);
    sprintf_P(str2, text_time_format , ted.hour(), ted.minute());
    strcpy_P(dialog_text, new_text_ok_ntp_time);
    strcat(dialog_text, " ");
    strcat(dialog_text, str2);
  }
  else
  {
    selftest_set_0(SELFTEST_ERR_NTP);
    MenuHistoryNextMenu(MENU_DIALOG_OK, 0, 0);
    strcpy_P(dialog_text, new_text_error_ntp_time);
  }
}

/*
   funkce onclick a pomocna funkce pro nastaveni ntp serveru
*/
void button_ntp_set_server_onclick(uint16_t args1, uint16_t args2, uint8_t args3)
{
  char ip_text[16];
  MenuHistoryNextMenu(MENU_DIALOG_KEYBOARD_NUMBER, 0, 0);
  sprintf_P(ip_text, new_text_ip_format, device.ntp_server[0], device.ntp_server[1], device.ntp_server[2], device.ntp_server[3]);
  display_element_set_string(ip_text, 16, 0, 0, &helper_dialog_ntp_set_server, &valid_ipv4_address_element_string);
}

void helper_dialog_ntp_set_server(uint16_t args1, uint16_t args2, uint8_t args3)
{
  uint8_t ip[4];
  char ip_text[16];
  display_element_get_string(ip_text);
  parseBytes(ip_text, '.', device.ntp_server, 4, 10);
  save_setup_network();
  selftest_set_0(SELFTEST_RESTART_NEEDED);
}

/*
   funkce onclick a pomocna funkce pro nastaveni ip adresy zarizeni
*/
void button_set_network_ip_onclick(uint16_t args1, uint16_t args2, uint8_t args3)
{
  char ip_text[16];
  MenuHistoryNextMenu(MENU_DIALOG_KEYBOARD_NUMBER, 0, 0);
  sprintf_P(ip_text, new_text_ip_format, device.myIP[0], device.myIP[1], device.myIP[2], device.myIP[3]);
  display_element_set_string(ip_text, 16, 0, 0, &helper_dialog_network_set_ip, &valid_ipv4_address_element_string);
}
void helper_dialog_network_set_ip(uint16_t args1, uint16_t args2, uint8_t args3)
{
  uint8_t ip[4];
  char ip_text[16];
  display_element_get_string(ip_text);
  parseBytes(ip_text, '.', device.myIP, 4, 10);
  save_setup_network();
  selftest_set_0(SELFTEST_RESTART_NEEDED);
}

/*
   funkce onclick a pomocna funkce pro nastaveni masky
*/
void button_set_network_mask_onclick(uint16_t args1, uint16_t args2, uint8_t args3)
{
  char ip_text[16];
  MenuHistoryNextMenu(MENU_DIALOG_KEYBOARD_NUMBER, 0, 0);
  sprintf_P(ip_text, new_text_ip_format, device.myMASK[0], device.myMASK[1], device.myMASK[2], device.myMASK[3]);
  display_element_set_string(ip_text, 16, 0, 0, &helper_dialog_network_set_mask, &valid_ipv4_address_element_string);
}
void helper_dialog_network_set_mask(uint16_t args1, uint16_t args2, uint8_t args3)
{
  uint8_t ip[4];
  char ip_text[16];
  display_element_get_string(ip_text);
  parseBytes(ip_text, '.', device.myMASK, 4, 10);
  save_setup_network();
  selftest_set_0(SELFTEST_RESTART_NEEDED);
}

/*
   funkce onclick a pomocna funkce pro nastaveni vychozi brany
*/
void button_set_network_gw_onclick(uint16_t args1, uint16_t args2, uint8_t args3)
{
  char ip_text[16];
  MenuHistoryNextMenu(MENU_DIALOG_KEYBOARD_NUMBER, 0, 0);
  sprintf_P(ip_text, new_text_ip_format, device.myGW[0], device.myGW[1], device.myGW[2], device.myGW[3]);
  display_element_set_string(ip_text, 16, 0, 0, &helper_dialog_network_set_gw, &valid_ipv4_address_element_string);
}
void helper_dialog_network_set_gw(uint16_t args1, uint16_t args2, uint8_t args3)
{
  uint8_t ip[4];
  char ip_text[16];
  display_element_get_string(ip_text);
  parseBytes(ip_text, '.', device.myGW, 4, 10);
  save_setup_network();
  selftest_set_0(SELFTEST_RESTART_NEEDED);
}

/*
   funkce onclick a pomocna funkce pro nastaveni nazvu
*/
void button_set_network_name_onclick(uint16_t args1, uint16_t args2, uint8_t args3)
{
  char ip_text[16];
  MenuHistoryNextMenu(MENU_DIALOG_KEYBOARD_ALFA, 0, 0);
  display_element_set_string(device.nazev, 10, 0, 0, &helper_dialog_network_set_name, &valid_true);
}
void helper_dialog_network_set_name(uint16_t args1, uint16_t args2, uint8_t args3)
{
  char ip_text[11];
  display_element_get_string(ip_text);
  strcpy(device.nazev, ip_text);
  save_setup_network();
  selftest_set_0(SELFTEST_RESTART_NEEDED);
}

/*
   funkce onclick a pomocna funkce pro nastaveni dns
*/
void button_set_network_dns_onclick(uint16_t args1, uint16_t args2, uint8_t args3)
{
  char ip_text[16];
  MenuHistoryNextMenu(MENU_DIALOG_KEYBOARD_NUMBER, 0, 0);
  sprintf_P(ip_text, new_text_ip_format, device.myDNS[0], device.myDNS[1], device.myDNS[2], device.myDNS[3]);
  display_element_set_string(ip_text, 16, 0, 0, &helper_dialog_network_set_dns, &valid_ipv4_address_element_string);
}
void helper_dialog_network_set_dns(uint16_t args1, uint16_t args2, uint8_t args3)
{
  uint8_t ip[4];
  char ip_text[16];
  display_element_get_string(ip_text);
  parseBytes(ip_text, '.', device.myDNS, 4, 10);
  save_setup_network();
  selftest_set_0(SELFTEST_RESTART_NEEDED);
}
/////
/*
   funkce pro random mac adresy
*/
void button_set_network_mac_onclick(uint16_t args1, uint16_t args2, uint8_t args3)
{
  device.mac[0] = 2; device.mac[1] = 1; device.mac[2] = 2; device.mac[3] =  dvanact; device.mac[4] = proud; device.mac[5] = light_curr;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/*
   Obsluha tlacitka casovy offset
*/
void button_click_set_time_offset(uint16_t args1, uint16_t args2, uint8_t args3)
{
  MenuHistoryNextMenu(MENU_DIALOG_SET_VARIABLE, 0, 0);
  display_function_set_variable(time_get_offset(), -12, 12, 1, 0, 0, NUMBER_TYPE_INT, H_FALSE, 0, &helper_set_menu_time_offset);
}
/*
   obsluzna funkce nastaveni offsetu casu
*/
void helper_set_menu_time_offset(uint16_t args1, uint8_t args2, float now, uint8_t args3)
{
  time_set_offset((int8_t)display_function_get_variable_int(0));
}
/////////////////////////////////////////////////////////////////////////////////
/*
   Funkce pro rucni nastaveni casu a datumu
*/
void button_time_set_time_manualy_onclick(uint16_t args1, uint16_t args2, uint8_t args3)
{
  char cas_text[10];
  MenuHistoryNextMenu(MENU_DIALOG_KEYBOARD_NUMBER, 0, 0);
  sprintf_P(cas_text, text_time_second_format, now.hour(), now.minute(), now.second());
  display_element_set_string(cas_text, 9, 0, 0, &helper_set_time_manualy, &valid_true);
}
void helper_set_time_manualy(uint16_t args1, uint16_t args2, uint8_t args3)
{
  char cas_text[10];
  uint8_t tim[3];
  display_element_get_string(cas_text);
  parseBytes(cas_text, ':', tim, 3, 10);
  rtc.adjust(DateTime(now.year(), now.month(), now.day(), tim[0], tim[1], tim[2]));
}
////
void button_time_set_date_manualy_onclick(uint16_t args1, uint16_t args2, uint8_t args3)
{
  char datum_text[12];
  MenuHistoryNextMenu(MENU_DIALOG_KEYBOARD_NUMBER, 0, 0);
  sprintf(datum_text, "%02d.%02d.%04d", now.day() , now.month() , now.year());
  display_element_set_string(datum_text, 11, 0, 0, &helper_set_date_manualy, &valid_true);
}
void helper_set_date_manualy(uint16_t args1, uint16_t args2, uint8_t args3)
{
  char datum_text[12];
  uint16_t dat[3];
  display_element_get_string(datum_text);
  parseBytes(datum_text, '.', dat, 3, 10);
  rtc.adjust(DateTime(dat[2], dat[1], dat[0], now.hour(), now.minute(), now.second()));
}
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
/*

   Obsluzne funkce pro nastavovani MQTT parametru pripojeni


*/
//// funkce obsluha tlacitka nastaveni mqqt serveru
void button_set_mqtt_broker_onclick(uint16_t args1, uint16_t args2, uint8_t args3)
{
  char ip_text[16];
  MenuHistoryNextMenu(MENU_DIALOG_KEYBOARD_NUMBER, 0, 0);
  sprintf_P(ip_text, new_text_ip_format, device.mqtt_server[0], device.mqtt_server[1], device.mqtt_server[2], device.mqtt_server[3]);
  display_element_set_string(ip_text, 16, 0, 0, &helper_dialog_mqtt_set_server, &valid_ipv4_address_element_string);
}
//// funkce obsluha tlacitka nastaveni mqtt uzivatele
void button_set_mqtt_user_onclick(uint16_t args1, uint16_t args2, uint8_t args3)
{
  MenuHistoryNextMenu(MENU_DIALOG_KEYBOARD_ALFA, 0, 0);
  display_element_set_string(device.mqtt_user, 20, 0, 0, &helper_dialog_mqtt_set_user, &valid_true);
}
//// funkce obsluha tlacika nastaveni mqtt klice
void button_set_mqtt_pass_onclick(uint16_t args1, uint16_t args2, uint8_t args3)
{
  MenuHistoryNextMenu(MENU_DIALOG_KEYBOARD_ALFA, 0, 0);
  display_element_set_string(device.mqtt_key, 20, 0, 0, &helper_dialog_mqtt_set_pass, &valid_true);
}
//// funkce obsluha tlacitka check connection
void button_check_mqtt_connection_onclick(uint16_t args1, uint16_t args2, uint8_t args3)
{
  char str1[16];
  MenuHistoryNextMenu(MENU_DIALOG_OK, 0, 0);
  mqtt_ping(&mqtt_client);
  mqtt_text_status(str1);
  strcpy(dialog_text, str1);
}

/*

*/
void helper_dialog_mqtt_set_server(uint16_t args1, uint16_t args2, uint8_t args3)
{
  char ip_text[16];
  display_element_get_string(ip_text);
  parseBytes(ip_text, '.', device.mqtt_server, 4, 10);
  save_setup_network();
  selftest_set_0(SELFTEST_RESTART_NEEDED);
}
void helper_dialog_mqtt_set_user(uint16_t args1, uint16_t args2, uint8_t args3)
{
  char user[20];
  display_element_get_string(user);
  strcpy(device.mqtt_user, user);
  save_setup_network();
  selftest_set_0(SELFTEST_RESTART_NEEDED);
}
void helper_dialog_mqtt_set_pass(uint16_t args1, uint16_t args2, uint8_t args3)
{
  char key[20];
  display_element_get_string(key);
  strcpy(device.mqtt_key, key);
  save_setup_network();
  selftest_set_0(SELFTEST_RESTART_NEEDED);
}



////////////////////////////////////////////////////////////////////
/*
   Obsluha tlacitka vychozi hodnoty
   a pak restart
*/
void button_nastaveni_default_value(uint16_t args1, uint16_t args2, uint8_t args3)
{
  //char str2[10];
  MenuHistoryNextMenu(MENU_DIALOG_YES_NO, 0, 0);
  dialog_yes_function = &helper_default_value;
  dialog_yes_args1 = 0;
  strcpy_P(dialog_text, nastaveni_default);
}
void helper_default_value(uint16_t args1, uint16_t args2, uint8_t args3)
{
  EEPROM.write(set_default_values, 255);
  resetFunc();
}

/*
   funkce pro restartovani zarizeni
*/
void button_nastaveni_reload_onclick(uint16_t args1, uint16_t args2, uint8_t args3)
{
  /// TODO dialog ANO/NE
  MenuHistoryNextMenu(MENU_DIALOG_YES_NO, 0, 0);
  dialog_yes_function = &helper_reload_restart;
  dialog_yes_args1 = 0;
  strcpy_P(dialog_text, new_text_nastaveni_reload);
}
void helper_reload_restart(uint16_t args1, uint16_t args2, uint8_t args3)
{
  resetFunc();
}
///////////////////////////////////////////////////////////////////
/*
   Obsluha tlacitka kontrola konektivity
*/
void button_network_test_connection_onclick(uint16_t args1, uint16_t args2, uint8_t args3)
{
  if ( check_connectivity_connection() == 0)
    selftest_set_1(SELFTEST_N0_CONNECTIVITY);
  else
    selftest_clear_1(SELFTEST_N0_CONNECTIVITY);
}
///////////////////////////////////////////////////////////////////
/*
   Funkce ktera overi zda jde udelat SYN->ACK na vzdaleny server
   return
      1 - OK
      0 - ERR
*/
uint8_t check_connectivity_connection(void)
{
  char server[16];
  char str2[16];
  uint8_t ret = 0;
  int err;
  strcpy_P(server, seznam_server);
  strcpy(str2, "");
  if (!ethClient1.connected())
  {
    err = ethClient1.connect(server, 80);
    ethclient_text_status(err, str2);
    MenuHistoryNextMenu(MENU_DIALOG_OK, 0, 0);
    strcpy_P(dialog_text, new_text_connectivity);
    strcat(dialog_text, str2);
    if ( err == 1)
      ret = 1;
    else
      ret = 0;
  }
  return ret;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*

   Funkce ktera zobrazuje vsechny interni veliciny vhodne k diagnostice zarizeni
*/
void display_element_show_about_device(uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint16_t args1, uint8_t args2, char *text)
{
  char str1[64];
  char str2[16];
  DateTime time_now;

  time_now = rtc.now();
  sprintf_P(str1, text_time_now, time_now.year(), time_now.month(), time_now.day(), time_now.hour(), time_now.minute(), time_now.second());
  show_string(str1, x + 220 , y + 10 , 1, BLACK, WHITE, 0);

  strcpy_P(str1, new_text_input_volt);
  dtostrf(prepocet_napeti(dvanact, CONST_PREVOD_DVANACTV), 4, 2, str2);
  strcat(str1, str2);
  strcat(str1, "V");
  show_string(str1, x + 5 , y + 10 , 1, BLACK, WHITE, 0);

  strcpy_P(str1, new_text_pet_volt);
  strcat(str1, ": ");
  dtostrf(prepocet_napeti(petnula, CONST_PREVOD_PETV), 4, 2, str2);
  strcat(str1, str2);
  strcat(str1, "V");
  show_string(str1, x + 5 , y + 25 , 1, BLACK, WHITE, 0);

  strcpy_P(str1, new_text_tritri_volt);
  dtostrf(prepocet_napeti(tritri, CONST_PREVOD_TRIV), 4, 2, str2);
  strcat(str1, str2);
  strcat(str1, "V");
  show_string(str1, x + 5 , y + 40 , 1, BLACK, WHITE, 0);

  strcpy_P(str1, new_text_consume_ampere);;
  dtostrf(prepocet_proudu(proud), 4, 2, str2);
  strcat(str1, str2);
  strcat(str1, "mA");
  show_string(str1, x + 5 , y + 55 , 1, BLACK, WHITE, 0);

  strcpy_P(str1, new_text_internal_temp);
  dtostrf(internal_temp, 4, 2, str2);
  strcat(str1, str2);
  strcat(str1, "C");
  show_string(str1, x + 5 , y + 70 , 1, BLACK, WHITE, 0);

  sprintf_P(str1, new_text_selfcheck_test, 0);
  itoa(selftest_data_0, str2, 16);
  strcat(str1, str2);
  show_string(str1, x + 5 , y + 85 , 1, BLACK, WHITE, 0);

  sprintf_P(str1, new_text_selfcheck_test, 1);
  itoa(selftest_data_1, str2, 16);
  strcat(str1, str2);
  show_string(str1, x + 5 , y + 100 , 1, BLACK, WHITE, 0);

  strcpy_P(str1, new_text_mqtt_send);
  ltoa(mqtt_send_message, str2, 10);
  strcat(str1, str2);
  show_string(str1, x + 5 , y + 115 , 1, BLACK, WHITE, 0);

  strcpy_P(str1, new_text_mqtt_receive);
  ltoa(mqtt_receive_message, str2, 10);
  strcat(str1, str2);
  show_string(str1, x + 5 , y + 130 , 1, BLACK, WHITE, 0);

  strcpy_P(str1, new_text_mqtt_processed);
  ltoa(mqtt_process_message, str2, 10);
  strcat(str1, str2);
  show_string(str1, x + 5 , y + 145 , 1, BLACK, WHITE, 0);

  strcpy_P(str1, new_text_mqtt_error);
  ltoa(mqtt_error, str2, 10);
  strcat(str1, str2);
  show_string(str1, x + 5 , y + 160 , 1, BLACK, WHITE, 0);

  time_now = DateTime(__DATE__, __TIME__).unixtime();
  sprintf_P(str1, text_build_version, time_now.year(), time_now.month(), time_now.day(), time_now.hour(), time_now.minute(), time_now.second());
  show_string(str1, x + 5 , y + 175 , 1, BLUE, WHITE, 0);


  mqtt_text_status(str2);
  sprintf_P(str1, text_mqtt_last_error, str2);
  show_string(str1, x + 220 , y + 25 , 1, BLACK, WHITE, 0);

  sprintf_P(str1, new_text_uptime, uptime);
  show_string(str1, x + 220 , y + 40 , 1, BLACK, WHITE, 0);
}
////

void ethclient_text_status(int err, char *str2)
{
  switch (err)
  {
    case 1:
      strcpy_P(str2, text_connect_ok);
      break;
    case 0:
      strcpy_P(str2, text_con_refused);
      break;
    case -1:
    case -2:
    case -3:
      strcpy_P(str2, text_timeout);
      break;
  }
}

void mqtt_text_status(char *str2)
{
  switch (mqtt_client.state())
  {
    case 0:
      strcpy_P(str2, text_connect_ok);
      break;
    case -4:
      strcpy_P(str2, text_timeout);
      break;
    case -3:
      strcpy_P(str2, text_connlost);
      break;
    case -2:
      strcpy_P(str2, text_connfailed);
      break;
    case -1:
      strcpy_P(str2, text_disconnect);
      break;
    case 1:
      strcpy_P(str2, text_bad_proto);
      break;
    case 2:
      strcpy_P(str2, text_bad_client);
      break;
    case 3:
      strcpy_P(str2, text_con_refused);
      break;
    case 4:
      strcpy_P(str2, text_bad_secret);
      break;
    case 5:
      strcpy_P(str2, text_unauthorized);
      break;
  }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*

   Funkce pro validace dat zadanych pres vstupni formular
*/
uint8_t valid_ipv4_address_element_string(void)
{
  /*
    uint8_t ip[4];
    char ip_text[16];
    uint8_t ret = 0;
    display_element_get_string(ip_text);
    if (parseBytes(ip_text, '.', ip, 4, 10) == 3) ret = 1;
    printf("ret: %d\n", ret);
  */
  return 1;
}
////
uint8_t valid_true(void)
{
  return 1;
}


uint8_t valid_thermostat_program_name(void)
{
  char program_name[10];
  display_element_get_string(program_name);
  if (strlen(program_name) > 0)
    return 1;
  else
    return 0;
}

uint8_t valid_thermostat_program_time_interval(void)
{
  char program_interval_name[thermostat_program_time_interval_string_size];
  display_element_get_string(program_interval_name);
  if (strlen(program_interval_name) > 0)
    return 1;
  else
    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
