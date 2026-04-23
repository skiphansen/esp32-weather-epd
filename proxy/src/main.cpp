#include <Arduino.h>

#include "CobsFraming.h"
#include "proxy_msgs.h"
#include "logging.h"

#define COBS_BUF_LEN      127
#define CMD_RESP  0x80

typedef union {
   int IntValue;
   unsigned int UIntValue;
   uint32_t Uint32Value;
   int32_t Int32Value;
   uint16_t Adr;
   uint32_t Adr32;
   uint8_t *pXdata; 
   uint8_t Bytes[4];
   uint8_t Uint8Value;
} CastUnion;

int gRxMsgLen;
int gMaxMsgLen;
uint8_t gRxBuf[COBS_BUF_LEN];
uint8_t gTxBuf[COBS_BUF_LEN];

void HandleMsg(void);

void setup()
{
   Serial.begin(115200);
   while(!Serial);
   delay(1000); // Wait for 1 second
   ALOG("Seeed Studio XIAO ePaper Display Board EE04 v0.1 compiled " __DATE__ " " __TIME__ "\n");
   gMaxMsgLen = SerialFrameIO_Init(gRxBuf,sizeof(gRxBuf));
}

void loop()
{
   int BytesRx = 0;
   int MsgsRx = 0;
   size_t bytes_read;
   char TempBuf[16];

   while((BytesRx = Serial.available()) > 0) {
      BytesRx = BytesRx < sizeof(TempBuf) ? BytesRx : sizeof(TempBuf);
      Serial.readBytes(TempBuf,BytesRx);

      for(int i = 0; i < BytesRx; i++) {
         gRxMsgLen = SerialFrameIO_ParseByte(TempBuf[i]);
         if(gRxMsgLen > 0) {
            MsgsRx++;
            HandleMsg();
         }
      }
   }
}

// commands <CmdByte> <command data>
// respones <CmdByte | 0x80> <Rcode> <response data>
void HandleMsg()
{
   int MsgLen = 2;
   CastUnion uCast0;
   CastUnion uCast1;
   CastUnion *pResponse = (CastUnion *) &gTxBuf[2];

// Assume that the first two arguments are 16 bits
   uCast0.Bytes[0] = gRxBuf[1];
   uCast0.Bytes[1] = gRxBuf[2];
   uCast0.Bytes[2] = uCast0.Bytes[3] = 0;
   uCast1.Bytes[0] = gRxBuf[3];
   uCast1.Bytes[1] = gRxBuf[4];
   uCast1.Bytes[2] = uCast1.Bytes[3] = 0;

   gTxBuf[0] = gRxBuf[0] | CMD_RESP;
   gTxBuf[1] = CMD_ERR_NONE;

   switch(gRxBuf[0] & ~CMD_RESP) {
      case CMD_PING:
         LOG("Got ping\n");
         break;

      default:
         LOG("Unknown command 0x%x ignored\n",gRxBuf[0]);
         gTxBuf[1] = CMD_ERR_UNKNOWN_CMD;
         break;
   }

   if(MsgLen != 0) {
   // Send reply in gTxBuf
      SerialFrameIO_SendMsg(gTxBuf,MsgLen);
   }
}

void SerialFrameIO_SendByte(uint8_t Byte)
{
   Serial.write(Byte);
}

