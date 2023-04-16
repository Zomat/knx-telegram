/**
 * @file KnxTelegram.c
 * @author Mateusz Zolisz <mateusz.zolisz@gmail.com>
 * @brief 
 * @version 0.1
 * @date 2023-03-22
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

typedef struct {
  uint8_t area;
  uint8_t line;
  uint16_t device;
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

typedef struct {

} KnxTelegram;

/**
 * @brief Create 8 bits control field
 * 
 * @param retransmission 
 * @param priority 
 * @return uint8_t 
 */
uint8_t knxCreateControlField(bool retransmission, char* priority) {
  uint8_t field = 0b10110000;
  bool priorityMatch = false;

  /* Repeat bit is set to 0 if this is a retransmission */
  if (retransmission) {
    field &= ~(1 << 5);
  }

  if (strcmp(priority, "system") == 0) {
    priorityMatch = true;
  }

  if (strcmp(priority, "alarm") == 0) {
    field |= (1 << 3);
    priorityMatch = true;
  }

  if (strcmp(priority, "normal") == 0) {
    field |= (1 << 2);
    priorityMatch = true;
  }

  if (strcmp(priority, "auto") == 0) {
    field |= (1 << 2) | (1 << 3);
    priorityMatch = true;
  }

  if (! priorityMatch) {
    return -1;
  }

  return field;
}

/**
 * @brief Decode 8 bits of control field
 * Priority:
 *  -> 00 = System functions
 *  -> 01 = Alarm functions
 *  -> 10 = Normal mode, high priority
 *  -> 11 = Auto - Normal mode, low priority
 * 
 * @param field 
 * @return KnxControl 
 */
KnxControl knxDecodeControlField(uint8_t field) {
  KnxControl control;
  bool priorityMsbBit = (field & (1 << 3));
  bool priorityLsbBit = (field & (1 << 2));

  if (field & (1 << 5)) {
    control.retransmission = false;
  } else {
    control.retransmission = true;
  }

  if (! priorityMsbBit && ! priorityLsbBit) {
    control.priority = "system";
  } else if (priorityMsbBit && ! priorityLsbBit) {
    control.priority = "alarm";
  } else if (! priorityMsbBit && priorityLsbBit) {
    control.priority = "normal";
  } else {
    control.priority = "auto";
  }

  return control;
}

/**
 * @brief Print control field
 * 
 * @param control 
 */
void knxPrintControl(KnxControl control) {
  printf("Retransmission: %s, Priority: %s \n", control.retransmission ? "true" : "false", control.priority);
}

/**
 * @brief Create KnxSourceAddress struct from given string in format "area/line/device"
 * 
 * @param address 
 * @return KnxSourceAddress 
 */
KnxSourceAddress knxCreateSourceAddressFromString(char* address) {
  KnxSourceAddress sourceAddress = {0, 0, 0};
  uint8_t area, line, device;
  if (sscanf(address, "%hhu/%hhu/%hhu", &area, &line, &device) != 3) {
    return sourceAddress;
  }

  sourceAddress.area = area;
  sourceAddress.line = line;
  sourceAddress.device = device;

  return sourceAddress;
}

/**
 * @brief Create 16 bit source address field
 * 
 * @param address 
 * @return uint16_t 
 */
uint16_t knxCreateSourceAddressField(KnxSourceAddress address) {
  uint16_t field;
  
  // Set individual address
  field = (address.device & 0xFF);

  // Set line address
  field |= ((address.line & 0x0F) << (8));

  // set area address
  field |= ((address.area & 0x0F) << (12));

  return field;
}

/**
 * @brief Check if source address is valid
 * 
 * @param address 
 * @return true 
 * @return false 
 */
bool knxIsValidSourceAddress(KnxSourceAddress address) {
  if (address.device == 0) {
    return false;
  }

  return true;
}

/**
 * @brief Decode 16 bits of source address field
 * 
 * @param field 
 * @return KnxSourceAddress 
 */
KnxSourceAddress knxDecodeSourceAddressField(uint16_t field) {
    KnxSourceAddress source;
    source.area = ((field & 0xF000) >> 12);
    source.line = ((field & 0x0F00) >> 8);
    source.device = ((field & 0x00FF));

    return source;
}

/**
 * @brief Print source address
 * 
 * @param address 
 */
void knxPrintSourceAddress(KnxSourceAddress address) {
  printf("Area: %d, Line: %d, Individual: %d \n", address.area, address.line, address.device);
}

/**
 * @brief Decode 16 bits of target physical field
 * 
 * @param field 
 * @return KnxTargetPhysicalAddress 
 */
KnxTargetPhysicalAddress knxDecodeTargetPhysicalAddressField(uint16_t field) {
    KnxTargetPhysicalAddress target;
    target.area = ((field & 0xF000) >> 12);
    target.line = ((field & 0x0F00) >> 8);
    target.device = ((field & 0x00FF));

    return target;
}

/**
 * @brief Decode 16 bits of target group field
 * 
 * @param field 
 * @return KnxTargetGroupAddress 
 */
KnxTargetGroupAddress knxDecodeTargetGroupAddressField(uint16_t field) {
    KnxTargetGroupAddress target;
    target.main = ((field & 0x7800) >> 11);
    target.middle = ((field & 0x0700) >> 8);
    target.sub = ((field & 0x00FF));

    return target;
}

uint16_t knxCreateTargetPhysicalAddressField(KnxTargetPhysicalAddress address) {
    uint16_t field;
  
    // Set individual address
    field = address.device;

    // Set line address
    field |= (address.line << 8);

    // set area address
    field |= (address.area << 12);

    return field;
}

void knxPrintTargetPhysicalAddress(KnxTargetPhysicalAddress address) {
  printf("Area: %d, Line: %d, Individual: %d \n", address.area, address.line, address.device);
}

uint16_t knxCreateTargetGroupAddressField(KnxTargetGroupAddress address) {
  uint16_t field;

  // Set sub address
  field = address.sub;

  // Set middle addrees
  field |= (address.middle << 8);

  // Set main address
  field |= (address.main << 11);

  return field;
}

void knxPrintTargetGroupAddress(KnxTargetGroupAddress address) {
  printf("Main: %d, Middle: %d, Sub: %d \n", address.main, address.middle, address.sub);
}

/**
 * @brief Set target address type
 * 6th byte
 * @param field 
 * @param type 
 *  - true: group
 *  - false: individual 
 */
void knxSetTargetAddressType(uint8_t *field, bool type) {
  *field |= (type << 7);
}

/**
 * @brief Get target address type
 * 6th byte
 * @param field
 * @return true: group 
 * @return false: individual
 */
bool knxGetTargetAddressType(uint8_t field) {
  return (field & (1 << 7)) > 0;
}

/**
 * @brief Set routing counter value
 * 7 is max and means, that filtering is disabled
 * 6th byte
 * @param field 
 * @param value 
 */
void knxSetRoutingCounter(uint8_t *field, uint8_t value) {
  // Max value is 7
  if (value > 7) {
    value = 7;
  }

  *field |= (value << 4);
}

/**
 * @brief Get value of routing counter
 * 6th byte
 * @param field 
 * @return uint8_t 
 */
uint8_t knxGetRoutingCounter(uint8_t field) {
  return ((field & 0x70) >> 4);
}

/**
 * @brief Set length of data (payload)
 * 6th byte
 * @param field 
 * @param length 
 */
void knxSetDataLength(uint8_t *field, uint8_t length) {
  *field |= (length - 1) & 0x0F;
}

/** === N_PDU === 
 * 6h byte - address type, routing, data length
 * 
 * === T_PDU ===
 * 7th byte -
 *    - 2 bits -> Type of transport layer communication
 *    - 4 bits -> sequence number (no meaning, if unnumbered)
 *    === A_PDU ===
 *      - 2 bits -> (default: 00)
 *      - OR? if UDP or NDP
 *        - next 4 bits
*/

/**
 * @brief Create 2 bytes for switch data
 * 
 * @param cmd 
 * @param state (on/off)
 * @return uint16_t 
 */
uint16_t knxCreateDataSwitchField(char* cmd, bool state) {
  uint16_t field = 0x00;

  // First 6 bits has no evaluation

  // Next 4 bits is command
  // Default: "value_read"

  if (strcmp(cmd, "value_response") == 0) {
    field |= (0b0001 << 6);
  }

  if (strcmp(cmd, "value_write") == 0) {
    field |= (0b0010 << 6);
  }

  if (strcmp(cmd, "memory_write") == 0) {
    field |= (0b1010 << 6);
  }

  // Next 6 bits is object
  // For switch is only one bit

  field |= state;

  return field;
}

uint8_t knxCalculateChecksum(uint8_t telegram[])
{
  uint8_t indexChecksum, xorSum=0;  
  indexChecksum = 6 + 1 + 1;
  for (uint8_t i = 0; i < indexChecksum ; i++) {
    xorSum ^= telegram[i]; // XOR Sum of all the databytes
  }
  
  return (uint8_t)(~xorSum); // Checksum equals 1's complement of databytes XOR sum
}

