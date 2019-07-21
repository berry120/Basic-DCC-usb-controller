//Depends on: https://github.com/fergul/DCCCommander

#define INPUT_BUFF_SIZE 128
#define BUFF_SIZE 60

#include <DCCPacket.h>
#include <DCCCommandStation.h>
#include <DCCHardware.h>
#include "dcc_sketch.h"

SpeedInfo si[BUFF_SIZE];
FunctionInfo fi[BUFF_SIZE];

boolean addSpeedInfo(SpeedInfo info) {
  removeSpeedInfo(info.address);
  int origIndex = info.address % BUFF_SIZE;
  int index = origIndex;
  while (!isNull(si[index])) {
    index = (index + 1) % BUFF_SIZE;
    if (index == origIndex) return false;
  }
  si[index] = info;
  return true;
}

boolean removeSpeedInfo(uint16_t address) {
  int origIndex = address % BUFF_SIZE;
  int index = origIndex;
  while (si[index].address != address) {
    index = (index + 1) % BUFF_SIZE;
    if (index == origIndex) return false;
  }
  si[index] = SI_NULL;
  return true;
}

boolean addFunctionInfo(FunctionInfo info) {
  removeFunctionInfo(info.address);
  int origIndex = info.address % BUFF_SIZE;
  int index = origIndex;
  while (!isNull(fi[index])) {
    index = (index + 1) % BUFF_SIZE;
    if (index == origIndex) return false;
  }
  fi[index] = info;
  return true;
}

boolean removeFunctionInfo(uint16_t address) {
  int origIndex = address % BUFF_SIZE;
  int index = origIndex;
  while (fi[index].address != address) {
    index = (index + 1) % BUFF_SIZE;
    if (index == origIndex) return false;
  }
  fi[index] = FI_NULL;
  return true;
}


char input[INPUT_BUFF_SIZE] = {0};
unsigned int strpos = 0;
DCCCommandStation dps;

void setup() {
  
  for (int i = 0; i < BUFF_SIZE; i++)  {
    si[i] = SI_NULL; fi[i] = FI_NULL;
  }

  Serial.begin(9600);
  dps.setup();
}

uint8_t i = 0;

uint32_t j = 0;

void loop() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == ';') {
      input[strpos] = '\0';
      execute(input);
      strpos = 0;
    }
    else {
      input[strpos++] = c;
    }
  }
  //dps.setSpeed28(3, DCC_ADDR_SHORT, 20, DCC_REVERSE);

  if (!isNull(si[i])) {
    switch (si[i].speedType) {
      case 14: dps.setSpeed14(si[i].address & 0x3FFF, si[i].address & LONG_ADDRESS_MASK ? DCC_ADDR_LONG : DCC_ADDR_SHORT, si[i].speedVal, si[i].dir); break;
      case 28: dps.setSpeed28(si[i].address & 0x3FFF, si[i].address & LONG_ADDRESS_MASK ? DCC_ADDR_LONG : DCC_ADDR_SHORT, si[i].speedVal, si[i].dir); break;
      case 128: dps.setSpeed128(si[i].address & 0x3FFF, si[i].address & LONG_ADDRESS_MASK ? DCC_ADDR_LONG : DCC_ADDR_SHORT, si[i].speedVal, si[i].dir); break;
    }
    delay(10);
  }
  if (!isNull(fi[i])) {
    dps.setFunctions(fi[i].address & 0x3FFF, fi[i].address & LONG_ADDRESS_MASK ? DCC_ADDR_LONG : DCC_ADDR_SHORT, fi[i].functionMask);
    delay(50);
  }

  i = (i + 1) % BUFF_SIZE;
}

void execute(char command[]) {
  char* res;
  char* type = strtok(command, ",");

  if (!strcmp(type, "spd")) {
    char* typeStr = strtok(NULL, ",");
    char* addressStr = strtok(NULL, ",");
    char* addressTypeStr = strtok(NULL, ",");
    char* speedStr = strtok(NULL, ",");

    long speedType = strtol(typeStr, &res, 10);
    if (res == addressStr || *res != '\0') {
      return;
    }

    long add = strtol(addressStr, &res, 10);
    if (res == addressStr || *res != '\0') {
      return;
    }

    long addressType = strtol(addressTypeStr, &res, 10);
    if (res == addressTypeStr || *res != '\0') {
      return;
    }

    boolean stop = !strcmp(speedStr, "stop");
    boolean estop = !strcmp(speedStr, "estop");
    long vel = 0;
    if (!stop && !estop) {
      vel = strtol(speedStr, &res, 10);
      if (res == speedStr || *res != '\0') {
        return;
      }
    }

    DCCDirection dir;
    if (vel > 0) {
      dir = DCC_REVERSE;
    }
    else if (vel < 0) {
      dir = DCC_FORWARD;
    }
    if (vel < 0) vel *= -1;

    if (addressType) {
      add |= LONG_ADDRESS_MASK;
    }

    if (speedType == 28) {
      vel += 3;
    }
    SpeedInfo info;
    info.address = add;
    info.speedType = speedType;
    info.speedVal = vel;
    if (estop) {
      info.dir = DCC_ESTOP;
    }
    else if (stop) {
      info.dir = DCC_STOP;
    }
    else {
      info.dir = dir;
    }

    addSpeedInfo(info);
  }

  if (!strcmp(type, "prog")) {
    char* addressStr = strtok(NULL, ",");
    char* addressTypeStr = strtok(NULL, ",");
    char* cvStr = strtok(NULL, ",");
    char* cvDataStr = strtok(NULL, ",");

    long address = strtol(addressStr, &res, 10);
    if (res == addressStr || *res != '\0') {
      return;
    }

    long addressType = strtol(addressTypeStr, &res, 10);
    if (res == addressTypeStr || *res != '\0') {
      return;
    }

    long cv = strtol(cvStr, &res, 10);
    if (res == cvStr || *res != '\0') {
      return;
    }

    long cvData = strtol(cvDataStr, &res, 10);
    if (res == cvDataStr || *res != '\0') {
      return;
    }

    dps.opsProgramCV(address, addressType ? DCC_ADDR_LONG : DCC_ADDR_SHORT, cv, cvData);
    delay(20);
  }

  if (!strcmp(type, "func")) {
    char* addressStr = strtok(NULL, ",");
    char* addressTypeStr = strtok(NULL, ",");
    char* funcStr = strtok(NULL, ",");

    long address = strtol(addressStr, &res, 10);
    if (res == addressStr || *res != '\0') {
      return;
    }

    long addressType = strtol(addressTypeStr, &res, 10);
    if (res == addressTypeStr || *res != '\0') {
      return;
    }

    long func = strtol(funcStr, &res, 10);
    if (res == funcStr || *res != '\0') {
      return;
    }

    if (addressType) {
      address |= LONG_ADDRESS_MASK;
    }

    FunctionInfo info;
    info.address = address;
    info.functionMask = func;
    addFunctionInfo(info);
  }

  if (!strcmp(type, "reset")) {
    char* addressStr = strtok(NULL, ",");
    char* addressTypeStr = strtok(NULL, ",");

    long address = strtol(addressStr, &res, 10);
    if (res == addressStr || *res != '\0') {
      return;
    }

    long addressType = strtol(addressTypeStr, &res, 10);
    if (res == addressTypeStr || *res != '\0') {
      return;
    }

    long addressMasked = address;
    if (addressType) {
      addressMasked |= LONG_ADDRESS_MASK;
    }
    removeSpeedInfo(addressMasked);
    removeFunctionInfo(addressMasked);

    dps.reset(address, addressType ? DCC_ADDR_LONG : DCC_ADDR_SHORT);
    delay(200);
  }
}
