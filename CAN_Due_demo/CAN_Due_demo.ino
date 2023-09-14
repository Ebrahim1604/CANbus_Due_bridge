#include "DueCANLayer.h"

// CAN Layer functions
extern byte canInit(byte cPort, long lBaudRate);
extern byte canTx(byte cPort, long lMsgID, bool bExtendedFormat, byte* cData, byte cDataLen);
extern byte canRx(byte cPort, long* lMsgID, bool* bExtendedFormat, byte* cData, byte* cDataLen);

void setup()
{
  // Set the serial interface baud rate
  Serial.begin(115200);
  
  // Initialize both CAN controllers
  if(canInit(0, CAN_BPS_500K) == CAN_OK)
    Serial.print("NBT EVO Idrive initialized at 500kbps on CAN0 port.\n\r");
  else
    Serial.print("NBT EVO Idrive Initialization Failed at 500kpbs on CAN0 port.\n\r");
  
}// end setup

void loop()
{
    delay(1);
    
    // Check for received message
    long lMsgID;
    bool bExtendedFormat;
    byte cRxData[8];
    byte cDataLen;
    if(canRx(0, &lMsgID, &bExtendedFormat, &cRxData[0], &cDataLen) == CAN_OK)
    {
      Serial.print("CAN0-NBT EVO: Rx - MsgID:");
      Serial.print(lMsgID, HEX);
      Serial.print(" Ext:");
      Serial.print(bExtendedFormat);
      Serial.print(" Len:");
      Serial.print(cDataLen);
      Serial.print(" Data:");

      for(byte cIndex = 0; cIndex < cDataLen; cIndex++)
      {
        Serial.print(cRxData[cIndex], HEX);
        Serial.print(" ");
      }// end for

      Serial.print("\n\r");
      
    }// end if

}// end loop
