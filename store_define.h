#ifndef __PROJECT_STORE_DEFINE_H
#define __PROJECT_STORE_DEFINE_H
#include "room_control_v3.h"
#include "UserSettings.h"







#define ram_output_virtual_count 50
#define ram_output_virtual_start_pos  3000
#define ram_output_virtual_size  17
#define ram_output_virtual_name_len 10
#define ram_output_virtual_used 0
#define ram_output_virtual_id 1
#define ram_output_virtual_type 2
#define ram_output_virtual_enable_mode 3
#define ram_output_virtual_state 4
#define ram_output_virtual_last_update 5
#define ram_output_virtual_name 6


#define eeprom_know_output_virtual_start 2000
#define eeprom_know_output_virtual_count 50
#define eeprom_know_output_virtual_size 12
#define eeprom_know_output_virtual_id 0
#define eeprom_know_output_virtual_name 1
#define eeprom_know_output_virtual_used 11
#define eeprom_know_output_virtual_name_len 10


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
#define ram_nrf_meas_store_size 19
#define ram_nrf_meas_store_count 20
#define ram_nrf_meas_name_length 10
#define ram_nrf_meas_store_associate_device 0
#define ram_nrf_meas_store_persistent 1
#define ram_nrf_meas_store_name 2
#define ram_nrf_meas_store_value 12 /// toto je float, proto 4 byty
#define ram_nrf_meas_store_used 16
#define ram_nrf_meas_store_value_type 17
#define ram_nrf_meas_store_last_update 18
///#define ram_nrf_meas_ram_store_start_last 1050


#define eeprom_nrf_meas_store_start 3000
#define eeprom_nrf_meas_store_size 12
#define eeprom_nrf_meas_store_count 20
#define eeprom_nrf_meas_name_length 10
#define eeprom_nrf_meas_store_associate_device 0
#define eeprom_nrf_meas_store_name 1
#define eeprom_nrf_meas_store_value_type 11
#define eeprom_nrf_meas_store_used 12
///#define eeprom_nrf_meas_ram_store_last 3240


#endif
