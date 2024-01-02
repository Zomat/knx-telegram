/**
 * @file KnxTelegram.h
 * @author Mateusz Zolisz <mateusz.zolisz@gmail.com>
 * @brief 
 * @version 0.3
 * @date 2023-06-20
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef KNX_TELEGRAM_H
#define KNX_TELEGRAM_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

/* 4 MSB bits have no evaluation */
#define KNX_CMD_VALUE_RESPONSE 0b00000001
#define KNX_CMD_VALUE_READ 0b00000000
#define KNX_CMD_VALUE_WRITE 0b00000010
#define KNX_CMD_MEMORY_WRITE 0b00001010

/* Used for communication with TPUART chip */
#define TPUART_DATA_START_CONTINUE 0B10000000
#define TPUART_DATA_END 0B01000000

typedef struct {
  uint8_t area;
  uint8_t line;
  uint8_t device;
} KnxSourceAddress;

typedef struct {
  uint8_t area;
  uint8_t line;
  uint16_t device;
} KnxTargetPhysicalAddress;

typedef struct {
  uint8_t main;
  uint8_t middle;
  uint16_t sub;
} KnxTargetGroupAddress;

typedef struct {
  bool retransmission;
  char* priority;
} KnxControl;

/** === Control Field === */

uint8_t knx_create_control_field(bool retransmission, char* priority);
KnxControl knx_decode_control_field(uint8_t field);
void knx_print_control(KnxControl control);

/** === Source address === */

KnxSourceAddress knx_create_source_address_struct_from_string(char* address);
uint16_t knx_source_address_from_struct_to_field(KnxSourceAddress address);
uint16_t knx_create_source_address_field_from_string(char* address);
bool knx_is_valid_source_address(KnxSourceAddress address);
KnxSourceAddress knx_decode_source_address_field(uint16_t field);
void knx_print_source_address(KnxSourceAddress address);

/** === Target Physical (Individual) address  === */

KnxTargetPhysicalAddress knx_decode_target_physical_address_field(uint16_t field);
KnxTargetGroupAddress knx_decode_target_group_address_field(uint16_t field);
uint16_t knx_target_physical_address_struct_to_field(KnxTargetPhysicalAddress address);
KnxTargetPhysicalAddress knx_create_target_physical_address_struct_from_string(char* address);
void knx_print_target_physical_address(KnxTargetPhysicalAddress address);

/** === Target Group address  === */

uint16_t knx_create_target_physical_address_field_from_string(char* address);
KnxTargetGroupAddress knx_create_target_group_address_struct_from_string(char* address);
uint16_t knx_target_group_address_struct_to_field(KnxTargetGroupAddress address);
uint16_t knx_create_target_group_address_field_from_string(char* address);
void knx_print_target_group_address(KnxTargetGroupAddress address);

/** === Target address type*/

void knx_set_target_address_type(uint8_t *field, bool type);
bool knx_get_target_address_type(uint8_t field);

/** === Routing counter === */

void knx_set_routing_counter(uint8_t *field, uint8_t value);
uint8_t knx_get_routing_counter(uint8_t field);

/** === Data Field === */

void knx_set_data_length(uint8_t *field, uint8_t length);
uint8_t knx_get_data_length(uint8_t field);

// DPT - Switch (1.001)

uint16_t knx_create_data_switch_field(uint8_t cmd, bool state);

// DPT - Dimming

uint32_t knx_create_data_dimming_field(uint8_t cmd, uint8_t value);

/** There will be more DPTs soon ... */

/** === Checksum === */

uint8_t knx_calculate_checksum(uint8_t telegram[], uint8_t size);

#endif // KNX_TELEGRAM_H
