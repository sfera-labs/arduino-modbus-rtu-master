/*  
  ModbusMaster.ino - Arduino sketch showing the use of the ModbusRtuMaster library

    Copyright (C) 2020 Sfera Labs S.r.l. - All rights reserved.
  
  This code is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  See file LICENSE.txt for further informations on licensing terms.
*/

#include <ModbusRtuMaster.h>

#define SLAVE_ADDR 10
#define REGISTER 301
#define BAUD 38400
#define PIN_TX_ENABLE 4

unsigned long now;
unsigned long lastReqTs;

void setup() {
  /**
   * Init serial port used for logging
   */
  SERIAL_PORT_MONITOR.begin(9600);
  
  /**
   * Init serial port used for Modbus communication
   * baud rate: BAUD
   * data bits: 8
   * parity: none
   * stop bits: 2
   */
  SERIAL_PORT_HARDWARE.begin(BAUD, SERIAL_8N2);

  /*
   * Init the Modbus master
   */
  ModbusRtuMaster.begin(&SERIAL_PORT_HARDWARE, BAUD, PIN_TX_ENABLE);

  /**
   * Set callback function to handle responses
   */
  ModbusRtuMaster.setCallback(&onResponse);
}

void loop() {
  // Perform a request every 3 seconds
  unsigned long now = millis();
  if (now - lastReqTs >= 3000) {
    ModbusRtuMaster.readInputRegisters(SLAVE_ADDR, REGISTER, 1);
    lastReqTs = now;
  }

  ModbusRtuMaster.process();
}

byte onResponse(byte unitAddr, byte function, size_t len, byte *data) {
  SERIAL_PORT_MONITOR.print("Response from ");
  SERIAL_PORT_MONITOR.println(unitAddr);
  SERIAL_PORT_MONITOR.print("Data: ");
  for (int i = 0; i < len; i++) {
    SERIAL_PORT_MONITOR.print(data[i]);
    SERIAL_PORT_MONITOR.print(" ");
  }
  SERIAL_PORT_MONITOR.println("\n");
}
