/*=============================================================================
File Name           : 
Project File Number : v 0.00b
Project Name        : 
Author              : 
Start Date          : 
Chip                : STM32f401
Copyright(c) 2022, All Rights Reserved.
-------------------------------------------------------------------------------
Description: 
-----------------------------------------------------------------------------*/
/***************************   Include Files   *******************************/
#include <Arduino.h>
#include <Wire.h>
#include "AsyncSMS.h"
#include <Adafruit_INA219.h>
/***************************   Defines   *************************************/
/***************************   Flags and bit *********************************/
bool NewSMS           =  false;
/***************************   sbit ******************************************/
/***************************   Macros   **************************************/
/***************************   Data Types   **********************************/
/***************************   Local Variables   *****************************/
u_int16_t SMScount = 1;
u_int8_t dataPCFread       = 0;
String msg;
/*****************************************************************************/
/***************************   Enum   ****************************************/
/***************************   Constants   ***********************************/
/***************************   Global Variables   ****************************/
u_int16_t Readcount = 0;
/*****************************************************************************/
//             SDA  SCL
TwoWire Wire1( PB4, PA8 ); // add by wire I2C 3
//TwoWire Wire1( PB7, PB6 ); // Onboard PCF
//                      RX   TX
HardwareSerial Serial2( PA3, PA2 );
AsyncSMS smsHelper( &Serial1, 19200 );
/***************************   Function Prototypes   *************************/
void setup(void);
void SetupSTM32(void);
void loop(void);
void ReadPCF8574(void);
void TestIndput(void);
void SendSMStxt(void);
void TestInputAndSendSMS(void);
void messageReceived(char * number, char * message);
/******************************************************************************
Function name : void setup()
         Type : PRIVATE
Description   : Run after start/reset
Notes :
******************************************************************************/
void setup() 
     {
      SetupSTM32();
      smsHelper.init();
      smsHelper.smsReceived = *messageReceived;
     } // END SETUP
/******************************************************************************
Function name : void Loop()
         Type : PRIVATE
Description   :
Notes :
******************************************************************************/
/***************************   Local Variables   *****************************/
void loop()
 {
   smsHelper.process();  // call in main loop
   TestInputAndSendSMS();
//   delay(500);
 }   // END loop 
/******************************************************************************
Function name : void SetupStm32(void)
         Type :
Description   : 
Notes :
******************************************************************************/
void SetupSTM32(void)
  {
    Serial2.begin( 115200 );
    while ( !Serial2 );
    Wire1.begin();
    pinMode( PC13, OUTPUT );    // sets the digital pin 13 as output
    analogReadResolution(12);
    delay( 100 );
    Serial2.println( "Start UP" );
  } // END SETUP
/******************************************************************************
Function name : void 
         Type :
Description   : 
Notes :
/******************************************************************************
Function name : void SendSMStxt(void)
         Type :
Description   : 
Notes :
******************************************************************************/
void SendSMStxt(void)
/***************************   Local Variables   *****************************/
  {
     SMScount++;
     digitalWrite(PC13 , LOW);
     //smsHelper.send("+4522360040", (char *)msg.c_str(), msg.length());
     Serial2.write(msg.c_str(), msg.length());
     digitalWrite(PC13 , HIGH);
  } // end 
/******************************************************************************
Function name : u_int8_t ReadPCF8574(void) 
         Type :
Description   : 
Notes :
******************************************************************************/
void ReadPCF8574(void){ 
   //u_int8_t x; 
     Wire1.beginTransmission( 0x20 );
     Wire1.endTransmission();
     Wire1.requestFrom( 0x20 , 1 );
     //return x;
}
/******************************************************************************
Function name : void messageReceived(char * number, char * message)
         Type :
Description   : 
Notes :
*******************************************************************************/
void messageReceived(char * number, char * message) 
  {
  //Do something with your message
  Serial2.println("Message received");
  Serial2.println(number);
  Serial2.println(message);
  }
/******************************************************************************
Function name : void TestIndput(u_int8_t dataPCFread) 
         Type :
Description   : 
Notes :
******************************************************************************/
void TestIndput(void)
 { 
     msg = "Samsoe 4 x BWE 632\n";
     msg += "Zone 1: ";  //Zone 1
     if (bitRead( dataPCFread, 0 ) != true) // test Zone 1 for low pellets
     {
        msg += "LAV\n";
     }
     else
     {
		  msg += "OK\n";
	  }
    msg += "Zone 2: ";  //Zone 2 
     if (bitRead( dataPCFread, 1 ) != true) // test Zone 2 for low pellets
     {
        msg += "LAV\n";
     }
     else
     {
		  msg += "OK\n";
	  }
    msg += "Zone 3: ";  //Zone 3
     if (bitRead( dataPCFread, 2 ) != true) // test Zone 3 for low pellets
     {
        msg += "LAV\n";
     }
     else
     {
		  msg += "OK\n";
	   }
    msg += "Zone 4: ";  //Zone 4
     if (bitRead( dataPCFread, 3 ) != true) // test Zone 4 for low pellets
     {
        msg += "LAV\n";
     }
     else
     {
		    msg += "OK\n";
	  }
    msg += "Fase 1: ";  //Fase 1
     if (bitRead( dataPCFread, 4 ) != true) // test Fase 1 for power
     {
        msg += "OFF\n";
     }
     else
     {
		  msg += "OK\n";
	  }
    msg += "Fase 2: ";  //Fase 2   
     if (bitRead( dataPCFread, 5 ) != true) // test Fase 2 for power
     {
        msg += "OFF\n";
     }
    else{
		    msg += "OK\n";
	   }
    msg += "Fase 3: ";  //Fase 3  
     if (bitRead( dataPCFread, 6 ) != true) // test Fase 3 for power
     {
        msg += "OFF\n";
     }
     else
     {
		  msg += "OK\n";
	  }
        msg += "SMS No: ";
        msg += SMScount;
        msg += "\n";
} // end check input
/******************************************************************************
Function name : void TestInputAndSendSMS(void) 
         Type : FSM
Description   : 
Notes :
******************************************************************************/
void TestInputAndSendSMS(void)
{
/***************************   Local Variables   *****************************/
static unsigned long ReadPCFinterval = 0;
unsigned long mili_time = 0;
static u_int8_t dataPCFold = 0;
static u_int8_t LastDataSend = 0;
static int Readcount = 0; 
static int indputAndSmsState = 0;
#define DEBOUNCE_TIME 5

   switch (indputAndSmsState) {
      case 0: 
         mili_time = millis();
         if( mili_time - ReadPCFinterval >= 1000 )
         { 
            // Read interval 1000 = 1 sek.
            ReadPCF8574();
            ReadPCFinterval = mili_time;
            Serial2.print(dataPCFread);
            Serial2.print(" - ");
            Serial2.println(Readcount);            
            // Move to next state
            indputAndSmsState = 1;
         }
         break; 
      case 1: 
         if ( Wire1.available() )
         { 
            // if 1 byte received
            // Read byte from i2c PC8574  
            dataPCFread = Wire1.read();
            if ( dataPCFread != dataPCFold )
            {  
               if ( dataPCFread == LastDataSend)
               {
                  Readcount = DEBOUNCE_TIME;
               }
               else
               {
                  Readcount = 0;
               }
               dataPCFold = dataPCFread;
            }
            else if(Readcount < DEBOUNCE_TIME)
            {
                Readcount++; 
                if(Readcount == DEBOUNCE_TIME)
                {
                    LastDataSend = dataPCFread;
                    Serial2.println(dataPCFread);
                }
            }
           indputAndSmsState = 0;
         }
         break;
      default:
         break;
   }      
}    
/******************************************************************************
Function name : void 
         Type : 
Description   : 
Notes :
******************************************************************************/