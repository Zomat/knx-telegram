#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "KnxTelegram.h"

void printByte(char myByte) {
  bool msb = true;
  for (uint8_t i = 0; i < 8; i++) {
    if (myByte & (0x80 >> i)) {
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
  uint8_t telegram[9];

  uint8_t controlByte = knxCreateControlField(false, "auto");

  /* === Source Address === */
  uint16_t sourceAddress = knxCreateSourceAddressFieldFromString("0.0.1");

  /* === Target Address === */
  uint16_t targetAddress = knxCreateTargetGroupAddressFieldFromString("0.0.2");

  /* === Target type & Routing Counter === */
  uint8_t byte5 = 0x00;
  knxSetTargetAddressType(&byte5, false);
  knxSetRoutingCounter(&byte5, 6);  

  /* === Data === */
  uint16_t data = knxCreateDataSwitchField("value_response", "0");

  /* === Checksum === */
  uint8_t checksum = knxCalculateChecksum(telegram, sizeof(telegram)/sizeof(uint8_t));

  /* Print telegram data */
  knxPrintControl(knxDecodeControlField(controlByte));
  knxPrintSourceAddress(knxDecodeSourceAddressField(sourceAddress));
  knxPrintTargetGroupAddress(knxDecodeTargetGroupAddressField(targetAddress));
  printf("Address type: %d \n", knxGetTargetAddressType(byte5));
  printf("Routing counter: %d \n", knxGetRoutingCounter(byte5));


  /* === Fill telegram === */
  telegram[0] = controlByte;
  telegram[1] = (sourceAddress >> 8) & 0x00FF;
  telegram[2] = (sourceAddress & 0x00FF);
  telegram[3] = (targetAddress >> 8) & 0x00FF;
  telegram[4] = (targetAddress) & 0x00FF;
  telegram[5] = byte5;
  telegram[6] = (data >> 8) & 0x00FF;
  telegram[7] = data & 0x00FF;
  telegram[8] = checksum;

  printf("\n");
  for (int i = 0; i < sizeof(telegram)/sizeof(uint8_t); i++) {
    printByte(telegram[i]);
  }

  return 0;
}
