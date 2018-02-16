# NMEA2000_socketCAN

This library provides support for linux  socketCAN environments - including the RPi.  It allows access to the [NMEA2000](https://github.com/ttlappalainen/NMEA2000) library using socketCAN Linux CAN devices.

See https://github.com/thomasonw/NMEA2000_socketCAN

Though any socketCAN implementation should work, testing is primarily on:
-  Raspberry Pi 3
-  PiCAN2 - Duo
 
 <br> <br>



## Usage

This library is in support of ttlappalainen's NMEA2000 library.  See the [NMEA2000](https://github.com/ttlappalainen/NMEA2000) for more examples.  Include this library in your build files. 
Here is a very simple example on RPi:


```
/* 
 * File:   main.cpp
 * Author: al
 *
 * Testing for CAN and RPI 
 * 
 * See: https://github.com/thomasonw/NMEA2000_socketCAN
 *
 * Created on February 12, 2017, 2:37 PM
 */

#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include "NMEA2000_CAN.h"

using namespace std;

int main(void)
{
    cout << "Starting CAN watching" << endl;

    setvbuf (stdout, NULL, _IONBF, 0);                                          // No buffering on stdout, just send chars as they come.
 
    NMEA2000.SetForwardStream(&serStream);                                      // Connect bridge function for streaming output.
    NMEA2000.SetForwardType(tNMEA2000::fwdt_Text);                              // Show in clear text (for now)
       
    if (!NMEA2000.Open()) {
       cout << "Failed to open CAN port" << endl;
       return 1;
   }
    
    cout  << endl << "CAN started, going to watch it now" << endl;

     while(1) {
         NMEA2000.ParseMessages();                                               // Will send out CAN messages in open text 
    }
    
    return 0;
}

```


<br><br>
## Selecting CAN stream source 

Linux streams allows a wide variety of sources to be selected for the CAN.  Typically `can0` or `can1` for hardware CAN ports, but also virtual ports and even captured log files.   NMEA2000_soceketCAN defaults to connecting to `can0`.  Other ports may be selected through the passing of initialization parameters.  The following two code snippets shows how the stream source  may be selected either statically at compile time, or dynamically during runtime.


#### Static (Compile time) socket name example:

```
/* 
 * Example of static CAN port selection
 */

#include <cstdlib>
#include <stdio.h>
#include <iostream>

#define SOCKET_CAN_PORT "can1"					//Compile-time selection of CAN1 socket name.   Place this BEFORE #include NMEA2000_CAN.h
#include "NMEA2000_CAN.h"

using namespace std;

int main(void)
{

  . . . . . . . . . . .
  

```

<br><br>

#### Dynamic (run-time) socket name example:

```
/* 
 * Example of Dynamic CAN port selection
 */

#include <cstdlib>
#include <stdio.h>
#include <iostream>

char socketName[50];	
#define SOCKET_CAN_PORT socketName
#include "NMEA2000_CAN.h"

using namespace std;

int main(void)
{


  . . . . . . . . . . 

     
    
    strcpy(socketName,"can1");								// Populate string 'socketName' BEFORE calling  NMEA2000.Open();
    if (!NMEA2000.Open()) {
       cout << "Failed to open CAN port: " << socketName << endl;
       return 1;
   }
  
  
  
  
  . . . . . . . . . . .
  

```



<br><br>
## Dependencies
socketCAN must be installed and working on your system - refer to the adapters users guide.  And make sure to 'start up' the CAN port, example:
```
$ sudo /sbin/ip link set can0 up type can bitrate 250000
```

You can test out your socketCAN install by doing the following (Skip the install if the tools are already present)

```
# Skip this step if can-utils are already installed
$ sudo apt-get install can-utils


 $ candump can0
  can0  19FFFD80   [8]  01 78 0D 01 AB 91 35 77
  can0  19FFFC80   [7]  01 78 FF 7F FF FF FF
  can0  19FEC880   [8]  01 78 B0 34 00 00 FF FF
  can0  19FFFD80   [8]  01 78 0D 01 B2 8E 35 77
  can0  19FFFC80   [7]  01 78 FF 7F FF FF FF
  can0  19FEC880   [8]  01 78 B6 34 00 00 FF FF
  can0  19F21280   [8]  00 09 49 01 00 FF FF FF
  can0  19F21280   [8]  01 FF FF FF 00 00 00 00
  can0  19F21480   [8]  01 46 05 F9 FF 00 00 49
  can0  19F21980   [8]  01 D0 01 DC 05 00 FF 00
  can0  19FFFD80   [8]  01 78 0E 01 5D 93 35 77
  can0  19FFFC80   [7]  01 78 FF 7F FF FF FF
  can0  19FEC880   [8]  01 78 BE 34 00 00 FF FF
  can0  19FFFD80   [8]  01 78 0E 01 73 8A 35 77
  can0  19FFFC80   [7]  01 78 FF 7F FF FF FF
  can0  19FEC880   [8]  01 78 BE 34 00 00 FF FF

```
<br><br>
 

## Hardware

Any hardware which enabled socketCAN's should work.  Refer to hardware user’s guide for installation
