/*
  ModbusRtuMaster.cpp - Modbus RTU Slave library for Arduino

    Copyright (C) 2019-2020 Sfera Labs S.r.l. - All rights reserved.

  This code is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  See file LICENSE.txt for further informations on licensing terms.
*/

#include "ModbusRtuMaster.h"
#include "CRC.h"

Stream *ModbusRtuMasterClass::_port;
int ModbusRtuMasterClass::_txEnPin;
uint16_t ModbusRtuMasterClass::_last_available;
unsigned long ModbusRtuMasterClass::_last_available_ts;
unsigned long ModbusRtuMasterClass::_t35chars;
byte ModbusRtuMasterClass::_inBuff[MODBUS_BUFFER_SIZE];
byte ModbusRtuMasterClass::_outBuff[MODBUS_BUFFER_SIZE];
Callback *ModbusRtuMasterClass::_callback;

void ModbusRtuMasterClass::begin(Stream *serial, unsigned long baud, int txEnPin) {
  _port = serial;
  _txEnPin = txEnPin;

  if (_txEnPin > 0) {
      pinMode(_txEnPin, OUTPUT);
  }

  _port->setTimeout(0);

  if (baud <= 19200) {
    _t35chars = 3500000 * 11 / baud;
  } else {
    _t35chars = 1750;
  }

  _last_available = 0;
}

void ModbusRtuMasterClass::setCallback(Callback *callback) {
  _callback = callback;
}

void ModbusRtuMasterClass::process() {
  uint16_t available;
  unsigned long now;
  size_t inLen = 0;

  available = _port->available();
  if (available > 0) {
    now = micros();
    if (available != _last_available) {
      _last_available = available;
      _last_available_ts = now;

    } else if (now - _last_available_ts >= _t35chars) {
      inLen = _port->readBytes(_inBuff, available);
      _last_available = 0;
    }
  }

  if (inLen < 5) {
    return;
  }

  byte crc[2];
  CRC.crc16(_inBuff, inLen - 2, crc);
  if (_inBuff[inLen - 2] != crc[0] || _inBuff[inLen - 1] != crc[1]) {
    return;
  }

  byte unitAddr = _inBuff[0];
  byte function = _inBuff[1];
  byte len;
  byte *data;

  if (function >= 0x80) {
    len = 1;
    data = _inBuff + 2;
  } else {
    switch (function) {
      case MB_FC_READ_COILS:
      case MB_FC_READ_DISCRETE_INPUTS:
      case MB_FC_READ_HOLDING_REGISTERS:
      case MB_FC_READ_INPUT_REGISTERS:
        len = _inBuff[2];
        data = _inBuff + 3;
        break;

      case MB_FC_WRITE_SINGLE_COIL:
      case MB_FC_WRITE_SINGLE_REGISTER:
      case MB_FC_WRITE_MULTIPLE_COILS:
      case MB_FC_WRITE_MULTIPLE_REGISTERS:
        len = 4;
        data = _inBuff + 2;
        break;

      default:
        len = inLen - 4;
        data = _inBuff + 2;
    }
  }

  _callback(unitAddr, function, len & 0xff, data);
}

void ModbusRtuMasterClass::writeOutBuff(size_t len) {
  CRC.crc16(_outBuff, len, _outBuff + len);

  if (_txEnPin > 0) {
    digitalWrite(_txEnPin, HIGH);
  }

  _port->write(_outBuff, len + 2);
  _port->flush();

  if (_txEnPin > 0) {
    digitalWrite(_txEnPin, LOW);
  }
}

void ModbusRtuMasterClass::readHoldingRegisters(byte unitAddr, word regAddr, word qty) {
  _outBuff[0] = unitAddr;
  _outBuff[1] = MB_FC_READ_HOLDING_REGISTERS;
  _outBuff[2] = highByte(regAddr);
  _outBuff[3] = lowByte(regAddr);
  _outBuff[4] = highByte(qty);
  _outBuff[5] = lowByte(qty);
  writeOutBuff(6);
}

void ModbusRtuMasterClass::readInputRegisters(byte unitAddr, word regAddr, word qty) {
  _outBuff[0] = unitAddr;
  _outBuff[1] = MB_FC_READ_INPUT_REGISTERS;
  _outBuff[2] = highByte(regAddr);
  _outBuff[3] = lowByte(regAddr);
  _outBuff[4] = highByte(qty);
  _outBuff[5] = lowByte(qty);
  writeOutBuff(6);
}
