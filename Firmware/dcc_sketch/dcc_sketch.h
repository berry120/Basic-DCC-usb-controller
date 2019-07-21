#ifndef __DCCSKETCH_H__
#define __DCCSKETCH_H__

#include <arduino.h>

#define NULL_ADDRESS 0xFFFF
#define LONG_ADDRESS_MASK 0x8000

#define isNull(x) ((x).address==NULL_ADDRESS)

struct SpeedInfo {
  uint16_t address;
  uint8_t speedType;
  byte speedVal;
  DCCDirection dir;
};

struct FunctionInfo {
  uint16_t address;
  uint32_t functionMask;
};


FunctionInfo FI_NULL = (const struct FunctionInfo){ NULL_ADDRESS, 0 };
SpeedInfo SI_NULL = (const struct SpeedInfo){ NULL_ADDRESS, 0 };

#endif
