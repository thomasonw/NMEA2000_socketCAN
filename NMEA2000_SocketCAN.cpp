/*
NMEA2000_SocketCAN.cpp

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

Inherited NMEA2000 object for socketCAN
See also NMEA2000 library.
*/


#include "NMEA2000_SocketCAN.h"

#include <stdio.h>
#include <iostream>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>


int skt;
char _CANport[50] = "can0";                                                     // Default to can0 if user does not set it.



//*****************************************************************************
tNMEA2000_SocketCAN::tNMEA2000_SocketCAN() : tNMEA2000()
{
}


//*****************************************************************************
bool tNMEA2000_SocketCAN::CANOpen() {
    struct ifreq ifr;
    struct sockaddr_can addr;
    int flags;

    //----  Open a socket to the CAN port 
    skt = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if(skt < 0) {    
        return (false);
        }
    
    strcpy(ifr.ifr_name, _CANport);
    if (ioctl(skt, SIOCGIFINDEX, &ifr) < 0) {
        return (false);
        }
    
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    if (bind(skt, (struct sockaddr *)&addr, sizeof(addr)) < 0)  {
        return (false);
        }
    
    
    //----- Set socket for non-blocking
    flags = fcntl(skt, F_GETFL, 0);
    if (flags < 0) {
        return (false);
        }
    
    if (fcntl(skt, F_SETFL, flags | O_NONBLOCK) < 0) {
        return (false);
        }

    return true; 
}
    
 
bool tNMEA2000_SocketCAN::CANSendFrame(unsigned long id, unsigned char len, const unsigned char *buf, bool wait_sent) {
   struct can_frame frame_wr;
   int tryCount = 0;
   bool  results;

   frame_wr.can_id  = id;
   frame_wr.can_dlc = len;
   memcpy(frame_wr.data, buf, 8);

    do {
        if (tryCount>0) delay(3);
   	results = (write(skt, &frame_wr, sizeof(frame_wr)) == sizeof(frame_wr));
        tryCount++;
    } while (tryCount<2 && !results);

    return results;
}


//*****************************************************************************
bool tNMEA2000_SocketCAN::CANGetFrame(unsigned long &id, unsigned char &len, unsigned char *buf) {
    struct can_frame frame_rd;
    struct timeval tv = {0, 0};                                                 // Non-blocking timout when checking
    fd_set fds;
    
    FD_ZERO(&fds);
    FD_SET(skt, &fds);

    if (select((skt + 1), &fds, NULL, NULL, &tv) < 0)                           // Is there a FD with something to read out there?
        return false;
            
    if (FD_ISSET(skt, &fds)) {                                                  // Was it our CAN file-descriptor that is ready?   
        if (read(skt, &frame_rd, sizeof(frame_rd)) > 0) {
            memcpy(buf, frame_rd.data, 8);
            len = frame_rd.can_dlc;
            id  = frame_rd.can_id;
            return true;
            }       
        }
    return false; 

}




/********************************************************************
*	Other 'Bridge' functions and classes
*
*
*
**********************************************************************/
void tNMEA2000_SocketCAN::SetCANPort(const char *CANport){
    
    strncpy(_CANport, CANport, (sizeof(_CANport) -1));                          // Copy passed string, but make sure not to overrun
    _CANport[sizeof(_CANport)] = '\0';                                          //  (And make sure to null terminate)
      
};




int tSocketStream:: read() {                                                    // Serial stream bridge -- Returns first byte if incoming data, or -1 on no available data.
    struct timeval tv = { 0L, 0L };
    fd_set fds;
    
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    if (select(1, &fds, NULL, NULL, &tv) < 0)                                   // Nothing is waiting for us.
        return -1;
    
    return (getchar());                                                         // Something is there, go get one char of it.

}
   


size_t tSocketStream:: write(const uint8_t* data, size_t size) {                // Serial Stream bridge -- Write data to stream.
    
    cout << data;                                                               // Simple - note am ignoring the size parameter.
    return sizeof(data);
}


// std::this_thread::sleep_for(std::chrono::milliseconds(x));  
// http://stackoverflow.com/questions/4184468/sleep-for-milliseconds

void delay(const uint32_t ms)
{
    usleep(ms*1000);
};


uint32_t millis(void)
{
    return(clock() / (CLOCKS_PER_SEC / 1000));
};


