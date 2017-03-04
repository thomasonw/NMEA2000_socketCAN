/* 
NMEA2000_socketCAN.h

2017 Copyright (c) Al Thomason 

Support the socketCAN access (ala, Linux, RPi)
See: https://github.com/thomasonw/NMEA2000_socketCAN
     https://github.com/ttlappalainen/NMEA2000

          
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-
  1301  USA
  
Inherited NMEA2000 object for socketCAN setup. See also NMEA2000 library.
*/



#ifndef NMEA2000_SOCKETCAN_H_
#define NMEA2000_SOCKETCAN_H_

#include <NMEA2000.h> 
#include <N2kMsg.h>

using namespace std;

class tNMEA2000_SocketCAN : public tNMEA2000
{
protected:
    virtual bool CANSendFrame(unsigned long id, unsigned char len, const unsigned char *buf, bool wait_sent);
    virtual bool CANOpen();
    virtual bool CANGetFrame(unsigned long &id, unsigned char &len, unsigned char *buf);
    
public:
    tNMEA2000_SocketCAN();
    virtual void SetCANPort(const char *CANport);
  
};

class tSocketStream : public N2kStream {
public:
   virtual int read();
   virtual size_t write(const uint8_t* data, size_t size);
};


void     delay(uint32_t ms);
uint32_t millis(void);



#endif /* NMEA2000_SOCKETCAN_H */

