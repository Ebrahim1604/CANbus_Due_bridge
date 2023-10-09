# include "DueCANLayer.h"

// CAN Layer functions
extern byte canInit(byte cPort, long lBaudRate);
extern byte canTx(byte cPort, long lMsgID, bool bExtendedFormat, byte* cData, byte cDataLen);
extern byte canRx(byte cPort, long* lMsgID, bool* bExtendedFormat, byte* cData, byte* cDataLen);

void setup()
{
  // Set the serial interface baud rate
  Serial.begin(115200);
  
  // Initialize both CAN controllers
  if(canInit(1, CAN_BPS_100K) == CAN_OK)
    Serial.print("BMW x6 e71 initialized at 100kbps on CAN1 port.\n\r");
  else
    Serial.print("Failed to Initialize BMW x6 e71 at 100kpbs on CAN1 port.\n\r");
  
}// end setup

void loop()
{
    delay(1);
    
    // Check for received message
    long lMsgID;
    bool bExtendedFormat;
    byte cRxData[8];
    byte cDataLen;
    if(canRx(1, &lMsgID, &bExtendedFormat, &cRxData[0], &cDataLen) == CAN_OK)
    {
      Serial.print("BMW x6 e71: Rx - MsgID:");
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
