/*
NMEA2000_SocketCAN.cpp

2017 Copyright (c) Al Thomason   All rights reserved

Support the socketCAN access (ala, Linux, RPi)
See: https://github.com/thomasonw/NMEA2000_socketCAN
     https://github.com/ttlappalainen/NMEA2000


Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.


THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

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
        cerr << "Failed CAN socket" << endl;
        return (false);
        }
    
    strcpy(ifr.ifr_name, _CANport);
    if (ioctl(skt, SIOCGIFINDEX, &ifr) < 0) {
        cerr << "Failed CAN ioctl" << endl;
        return (false);
        }
    
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    if (bind(skt, (struct sockaddr *)&addr, sizeof(addr)) < 0)  {
        cerr << "Failed CAN bind" << endl;
        return (false);
        }
    
    
    //----- Set socket for non-blocking
    flags = fcntl(skt, F_GETFL, 0);
    if (flags < 0) {
        cerr << "Failed CAN flag fetch" << endl;
        return (false);
        }
    
    if (fcntl(skt, F_SETFL, flags | O_NONBLOCK) < 0) {
        cerr << "Failed CAN flag set" << endl;
        return (false);
        }

    return true; 
}


//*****************************************************************************
bool tNMEA2000_SocketCAN::CANSendFrame(unsigned long id, unsigned char len, const unsigned char *buf, bool wait_sent) {
   struct can_frame frame_wr;
   bool  results;

   frame_wr.can_id  = id | CAN_EFF_FLAG;
   frame_wr.can_dlc = len;
   memcpy(frame_wr.data, buf, 8);
 
   results = (write(skt, &frame_wr, sizeof(frame_wr)) == sizeof(frame_wr));     // Send this frame out to the socketCAN handler
  
   if (!results)
       return(false);                                                           // Passing packet to socketCAN failed ...
   
   if (wait_sent) {
       usleep(2000);                                                            // Wait 2mS to gain a level of confidence this packet has been transmitted.
   }										// Still working to improve this ---
   
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




