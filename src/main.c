#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "KnxTelegram.h"

void print_byte(char my_byte) {
  bool msb = true;
  for (uint8_t i = 0; i < 8; i++) {
    if (my_byte & (0x80 >> i)) {
      printf("1");
      msb = true;
    } else if (msb) {
      printf("0");
    } else {
      printf("_");
    }
  }
  puts("");
}

int main(int argc, char **argv) {
  uint8_t telegram[10];

  uint8_t control_byte = knx_create_control_field(false, "auto");

  /* === Source Address === */
  uint16_t source_address = knx_create_source_address_field_from_string("15.15.20");

  /* === Target Address === */
  uint16_t target_address = knx_create_target_group_address_field_from_string("0.0.3");

  /* === Target type & Routing Counter === */
  uint8_t byte_5 = 0x00;
  knx_set_target_address_type(&byte_5, true);
  knx_set_routing_counter(&byte_5, 6);  
  knx_set_data_length(&byte_5, 2);

  /* === Data === */
  uint32_t data = knx_create_data_dimming_field(KNX_CMD_VALUE_WRITE, 153);

  /* Print telegram data */
  knx_print_control(knx_decode_control_field(control_byte));
  knx_print_source_address(knx_decode_source_address_field(source_address));
  knx_print_target_group_address(knx_decode_target_group_address_field(target_address));
  printf("Address type: %d \n", knx_get_target_address_type(byte_5));
  printf("Routing counter: %d \n", knx_get_routing_counter(byte_5));
  printf("Data length counter: %d \n", knx_get_data_length(byte_5));

  /* === Fill telegram === */
  telegram[0] = control_byte;
  telegram[1] = (source_address >> 8) & 0x00FF;
  telegram[2] = source_address & 0x00FF;
  telegram[3] = (target_address >> 8) & 0x00FF;
  telegram[4] = target_address & 0x00FF;
  telegram[5] = byte_5;
  telegram[6] = (data >> 16) & 0x00FF;
  telegram[7] = (data >> 8) & 0x00FF;
  telegram[8] = data & 0x00FF;
  telegram[9] = knx_calculate_checksum(telegram, sizeof(telegram)/sizeof(uint8_t));

  printf("\n");
  for (int i = 0; i < sizeof(telegram)/sizeof(uint8_t); i++) {
    print_byte(telegram[i]);
  }

  return 0;
}
