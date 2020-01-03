/*
  ModbusRtuMaster.h - Modbus RTU Master library for Arduino

    Copyright (C) 2019-2020 Sfera Labs S.r.l. - All rights reserved.

  This code is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  See file LICENSE.txt for further informations on licensing terms.
*/

#ifndef ModbusRtuMaster_h
#define ModbusRtuMaster_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define MB_FC_READ_COILS 0x01
#define MB_FC_READ_DISCRETE_INPUTS 0x02
#define MB_FC_READ_HOLDING_REGISTERS 0x03
#define MB_FC_READ_INPUT_REGISTERS 0x04
#define MB_FC_WRITE_SINGLE_COIL 0x05
#define MB_FC_WRITE_SINGLE_REGISTER 0x06
#define MB_FC_WRITE_MULTIPLE_COILS 0x0F
#define MB_FC_WRITE_MULTIPLE_REGISTERS 0x10

#define MB_EX_ILLEGAL_FUNCTION 0x01
#define MB_EX_ILLEGAL_DATA_ADDRESS 0x02
#define MB_EX_ILLEGAL_DATA_VALUE 0x03
#define MB_EX_SERVER_DEVICE_FAILURE 0x04
#define MB_EX_ACKNOWLEDGE 0x05
#define MB_EX_SERVER_DEVICE_BUSY 0x06
#define MB_EX_MEMORY_PARITY_ERROR 0x08
#define MB_EX_GATEWAY_PATH_UNAVAILABLE 0x0A
#define MB_EX_DEVICE_FAILED_TO_RESPOND 0x0B

#define MB_RESP_OK 0x00

#define MODBUS_BUFFER_SIZE 64

typedef byte Callback(byte unitAddr, byte function, size_t len, byte *data);

class ModbusRtuMasterClass {
  public:
    static void begin(Stream *serial, unsigned long baud, int txEnPin);
    static void setCallback(Callback *callback);
    static void process();
    static void readHoldingRegisters(byte unitAddr, word regAddr, word qty);
    static void readInputRegisters(byte unitAddr, word regAddr, word qty);

  private:
    static Stream *_port;
    static int _txEnPin;
    static uint16_t _last_available;
    static unsigned long _last_available_ts;
    static unsigned long _t35chars;
    static byte _inBuff[MODBUS_BUFFER_SIZE];
    static byte _outBuff[MODBUS_BUFFER_SIZE];
    static Callback *_callback;
    static void writeOutBuff(size_t len);
};

extern ModbusRtuMasterClass ModbusRtuMaster;

#endif
