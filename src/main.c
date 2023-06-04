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
  uint8_t telegram[10];

  uint8_t controlByte = knxCreateControlField(false, "auto");

  /* === Source Address === */
  uint16_t sourceAddress = knxCreateSourceAddressFieldFromString("15.15.20");

  /* === Target Address === */
  uint16_t targetAddress = knxCreateTargetGroupAddressFieldFromString("0.0.3");

  /* === Target type & Routing Counter === */
  uint8_t byte5 = 0x00;
  knxSetTargetAddressType(&byte5, true);
  knxSetRoutingCounter(&byte5, 6);  
  knxSetDataLength(&byte5, 2);

  /* === Data === */
  uint16_t data = knxCreateDataDimmingField(KNX_CMD_VALUE_WRITE, 153);

  /* Print telegram data */
  knxPrintControl(knxDecodeControlField(controlByte));
  knxPrintSourceAddress(knxDecodeSourceAddressField(sourceAddress));
  knxPrintTargetGroupAddress(knxDecodeTargetGroupAddressField(targetAddress));
  printf("Address type: %d \n", knxGetTargetAddressType(byte5));
  printf("Routing counter: %d \n", knxGetRoutingCounter(byte5));
  printf("Data length counter: %d \n", knxGetDataLength(byte5));


  /* === Fill telegram === */
  telegram[0] = controlByte;
  telegram[1] = (sourceAddress >> 8) & 0x00FF;
  telegram[2] = (sourceAddress & 0x00FF);
  telegram[3] = (targetAddress >> 8) & 0x00FF;
  telegram[4] = (targetAddress) & 0x00FF;
  telegram[5] = byte5;
  telegram[6] = (data >> 16) & 0x00FF;
  telegram[7] = (data >> 8) & 0x00FF;
  telegram[8] = data & 0x00FF;
  telegram[9] = knxCalculateChecksum(telegram, sizeof(telegram)/sizeof(uint8_t));

  printf("\n");
  for (int i = 0; i < sizeof(telegram)/sizeof(uint8_t); i++) {
    printByte(telegram[i]);
  }
   printf("CHECK: \n");
   printByte(telegram[8]);


  return 0;
}
