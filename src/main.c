#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "KnxTelegram.c"
/**
 * KNX Telegram Description
 * 
 * Telegram Fields:
 *  -> Control Field (1 byte)
 *  -> Source Address (2 bytes)
 *  -> Target Address (2 bytes + 1 bit)
 *  -> Routing (3 bits)
 *  -> Length (4 bits)
 *  -> Data (Up to 16 bytes)
 *  -> Check (1 byte)
 * 
 *    [Previous ACK]...<t1>...[Telegram]...<t2>...[ACK]
 * 
 *    ACK (1 byte):
 *    -> 0b11000000 - BUSY (Device unable to process new information)
 *    -> 0b00001100 - NACK (Incorrect) 
 *    -> 0b11001100 - ACK (Correct)
 * 
 *   
 */


// struct knx_data data = {
//     .data_length = 8,
//     .control_field = 0x80,
//     .source_address = {0x00, 0x01},
//     .destination_address = {0x00, 0x02},
//     .data_payload = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
//     .crc = 0x00
//   };


/** 
  Wysłanie ramki KNX na magistralę jest możliwe, gdy magistrala jest wolna. 
  Magistrala KNX wykorzystuje system "kolidowania z odsłuchem" 
  (ang. CSMA/CA - Carrier Sense Multiple Access with Collision Avoidance), 
  który zapobiega kolizjom między ramkami. Przed wysłaniem ramki urządzenie musi najpierw słuchać magistrali, 
  aby upewnić się, że jest ona wolna. Jeśli magistrala jest zajęta, 
  urządzenie czeka losową ilość czasu, a następnie ponownie próbuje wysłać ramkę.

 W przypadku wysyłania ramki przez kilka urządzeń w tym samym czasie, system CSMA/CA pozwala na wykrycie kolizji 
 i ponowne wysłanie ramki przez urządzenia, które uległy kolizji.
 */
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

  uint8_t controlField = knxCreateControlField(true, "auto");
  KnxControl c = knxDecodeControlField(controlField);
  knxPrintControl(c);

  telegram[0] = controlField;

  KnxSourceAddress sa = knxCreateSourceAddressFromString("3/10/20");

  if (knxIsValidSourceAddress(sa)) {
    uint16_t sourceAddressField = knxCreateSourceAddressField(sa);
    KnxSourceAddress a = knxDecodeSourceAddressField(sourceAddressField);
    knxPrintSourceAddress(a);
  } else {
    printf("Invalid source address!\n");
  }

  telegram[1] = (knxCreateSourceAddressField(sa) >> 8) & 0x0F;
  telegram[2] = (knxCreateSourceAddressField(sa)) & 0x0F;

  KnxTargetPhysicalAddress tpa = {3, 10, 21};
  uint16_t tpa1 = knxCreateTargetPhysicalAddressField(tpa);
  tpa = knxDecodeTargetPhysicalAddressField(tpa1);
  knxPrintTargetPhysicalAddress(tpa);

  KnxTargetGroupAddress tga = {2, 3, 10};
  uint16_t tga1 = knxCreateTargetGroupAddressField(tga);
  tga = knxDecodeTargetGroupAddressField(tga1);
  knxPrintTargetGroupAddress(tga);

  telegram[3] = (tga1 >> 8) & 0x0F;
  telegram[4] = (tga1) & 0x0F;

  uint8_t byte5 = 0x00;
  knxSetTargetAddressType(&byte5, false);
  knxSetRoutingCounter(&byte5, 7);
  printf("Address type: %d \n", knxGetTargetAddressType(byte5));
  printf("Routing counter: %d \n", knxGetRoutingCounter(byte5));

  telegram[5] = byte5;

  uint16_t data = knxCreateDataSwitchField("value_response", "0");
  printByte(data);

  telegram[6] = (data >> 8) & 0x0F;
  telegram[7] = data & 0x0F;

  uint8_t checksum = knxCalculateChecksum(telegram);
  telegram[8] = checksum;

  printByte(checksum);

  return 0;
}
