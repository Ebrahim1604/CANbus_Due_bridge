// CAN Layer functions
// -----------------------------------------------------------------------------
#include "DueCANLayer.h"
extern byte canInit(byte cPort, long lBaudRate, int nTxMailboxes);
extern byte canTx(byte cPort, long lMsgID, bool bExtendedFormat, byte* cData, 
                  byte cDataLen);
extern byte canRx(byte cPort, long* lMsgID, bool* bExtendedFormat, byte* cData, 
                  byte* cDataLen);

// Timer functions
// -----------------------------------------------------------------------------
#include "TimerControl.h"
extern void TimerInit(void);
extern void TimerControl(void);
extern void TimerStart(struct Timer* pTimer, int nCount);
extern void TimerReset(struct Timer* pTimer);
extern struct Timer pTimer[];

// Internal functions
// -----------------------------------------------------------------------------
void LEDControl(void);
void ErrorControl(bool bError);

// Module variables
// -----------------------------------------------------------------------------
int TimerActivity_CAN0 = 0;
int TimerActivity_CAN1 = 0;

int LED1 = 14;
int LED2 = 15;

int nTxMailboxes = 3; // Equal portion between transmission and reception

void setup()
{
  // Declarations
  bool bError = false;

  // Set the serial interface baud rate
  Serial.begin(115200);

  // Initialzie the timer control; also resets all timers
  TimerInit();

  // Initialize the outputs for the LEDs
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);

  // Initialize both CAN controllers
  if(canInit(0, CAN_BPS_500K, nTxMailboxes) == CAN_OK) //Idrive NBT EVO to CAN0 at 500kbps
  {
    Serial.print("NBT EVO Idrive initialized at 500kbps on CAN0 port.\n\r");
  }
  else
  {
    Serial.print("NBT EVO Idrive Initialization Failed at 500kpbs on CAN0 port.\n\r");
    bError = true;
  }// end else
  
  if(canInit(1, CAN_BPS_100K, nTxMailboxes) == CAN_OK)// Audi X7 E71 to CAN1 at 100kbps
  {
    Serial.print("Audi X6 E71 initialized at 100kbps on CAN0 port.\n\r");
  }
  else
  {
    Serial.print("Audi X6 E71 Initialization Failed at 500kpbs on CAN0 port.\n\r");
    bError = true;
  }// end else

  // Do not continue, in case there was an error
  ErrorControl(bError);
  
}// end setup

void loop()
{
  // Declarations
  bool bError = false;
  long lMsgID;
  bool bExtendedFormat;
  byte cData[8];
  byte cDataLen;
  
  // Start timer for LED indicators
  TimerStart(&pTimerLEDs, TIMER_RATE_LED_BLINK);

  // Control LED status according to CAN traffic
  LEDControl();

  // Check for received CAN messages on port 0 for NBT EVO
  if(canRx(0, &lMsgID, &bExtendedFormat, &cData[0], &cDataLen) == CAN_OK)
    {
      // Repeat data frame to X6 E71 at port 1
      if(canTx(1, lMsgID, bExtendedFormat, &cData[0], cDataLen) == CAN_ERROR)
      {
          bError = true;
      }
    }
  // Check for received CAN messages on port 1
  if(canRx(1, &lMsgID, &bExtendedFormat, &cData[0], &cDataLen) == CAN_OK)
  {
      Serial.print("Response from X6 E71: Rx - MsgID:");
      Serial.print(lMsgID, HEX);
      Serial.print(" Ext:");
      Serial.print(bExtendedFormat);
      Serial.print(" Len:");
      Serial.print(cDataLen);
      Serial.print(" Data:");

      for(byte cIndex = 0; cIndex < cDataLen; cIndex++)
      {
        Serial.print(cData[cIndex], HEX);
        Serial.print(" ");
      }// end for

      Serial.print("\n\r");
      /*
      // Repeat data frame to port 0
      if(canTx(0, lMsgID, bExtendedFormat, &cData[0], cDataLen) == CAN_ERROR)
      {
          bError = true;
      }
      */
  }
  // Do not continue, in case there was an error
  ErrorControl(bError);

}// end loop

// ------------------------------------------------------------------------
// Error Control
// ------------------------------------------------------------------------
void ErrorControl(bool bError)
{
    // Check for error
    if(bError == true)
    {
        // Output the error message
        Serial.println("Error Condition, program stopped");

        // Remain in endless loop
        while(1);

    }// end if

}// end ErrorControl

// ------------------------------------------------------------------------
// LED Data Traffic
// ------------------------------------------------------------------------
// Note: CAN0 --> LED1
//       CAN1 --> LED2
//
void LEDControl(void)
{
    if(pTimerLEDs.bExpired == true)
    {
      // Restart the timer
      TimerStart(&pTimerLEDs, TIMER_RATE_LED_BLINK);

      // Check for activity on CAN0
      if(TimerActivity_CAN0 > 0)
      {
        TimerActivity_CAN0--;
        digitalWrite(LED1, HIGH);
      }// end if
      else
        digitalWrite(LED1, LOW);

      // Check for activity on CAN1
      if(TimerActivity_CAN1 > 0)
      {
        TimerActivity_CAN1--;
        digitalWrite(LED2, HIGH);
      }// end if
      else
        digitalWrite(LED2, LOW);

    }// end if

}// end LEDControl
