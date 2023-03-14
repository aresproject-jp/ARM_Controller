// CAN_USB for Seeeduino XIAO
// kazuki fujita @771_8bit

#include <mcp_can.h>
#include <SPI.h>

MCP_CAN CAN(6); // Set CS to pin 6
#define CAN_INT 7

void setup()
{
  pinMode(CAN_INT, INPUT);
  Serial.begin(115200);

  if (CAN.begin(MCP_ANY, CAN_1000KBPS, MCP_16MHZ) == CAN_OK) {
    //Serial.println("MCP2515 Initialized Successfully!");
  }
  else {
    //Serial.println("Error Initializing MCP2515...");
  }
  CAN.setMode(MCP_NORMAL);   // Change to normal mode to allow messages to be transmitted
}


typedef union
{
  uint32_t uint32_data;
  uint8_t uint8_data[4];
} msg_union;
msg_union msg = {0};


int i = 0;
char buff[256];
uint32_t UART_data[16];
char *p;

void loop()
{
  if (!digitalRead(CAN_INT))                        // If CAN_INT pin is low, read receive buffer
  {
    long unsigned int rxId;
    unsigned char len = 0;
    CAN.readMsgBuf(&rxId, &len, msg.uint8_data);      // Read data: len = data length, buf = data byte(s)

    Serial.print(rxId, HEX);
    Serial.print(",");
    Serial.println(msg.uint32_data);
  }

  uint32_t canid = 0;
  int i_UART_data = 0;
  while (Serial.available() > 0) {
    buff[i] = (char)Serial.read();
    if (buff[i] == '\n') {
      buff[i] == '\0';
      canid = strtol(strtok(buff, ","), NULL, 16);

      for (i_UART_data = 0; true; i_UART_data++) {
        p = strtok(NULL, ",");
        if (p != NULL) {
          UART_data[i_UART_data] = atoi(p);
        } else {
          break;
        }
      }
      i = 0;
      break;
    } else {
      i += 1;
    }
  }
  if (i_UART_data > 0) {
    msg.uint32_data = UART_data[0];
    //Serial.println(canid);
    byte sndStat = CAN.sendMsgBuf(canid, 0, 4, msg.uint8_data);
    /*
        if (sndStat == CAN_OK) {
          Serial.println("Message Sent Successfully!");
        } else {
          Serial.println("Error Sending Message...");
        }
    */
    delayMicroseconds(500);
  }
}
