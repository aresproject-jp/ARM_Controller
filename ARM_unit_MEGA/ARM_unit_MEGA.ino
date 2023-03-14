// ARM unit for Arduino Mega
// kazuki fujita @771_8bit

#include <TimerThree.h>
bool timer_100Hz = false;

#include <mcp_can.h>
#include <SPI.h>
MCP_CAN CAN(53); // Set CS to pin 53
#define CAN_INT 48
typedef union
{
  uint32_t uint32_data;
  uint8_t uint8_data[4];
} msg_union;
msg_union msg = {0};

#include <Servo.h>
Servo servo;
#define SERVO_PWM 13
#define SERVO_default 90

#include "CytronMotorDriver.h"
//CytronMD motor(PWM_DIR, PWM, DIR);
CytronMD motor1(PWM_DIR, 12, 10);
CytronMD motor2(PWM_DIR, 11, 9);
CytronMD motor3(PWM_DIR, 8, 5);
CytronMD motor4(PWM_DIR, 7, 4);
CytronMD motor5(PWM_DIR, 6, 1);

#include <Encoder.h>
Encoder encoder1(3, 2);
Encoder encoder2(0, 14);
Encoder encoder3(15, 16);
Encoder encoder4(18, 19);
Encoder encoder5(21, 20);
//https://www.pjrc.com/teensy/td_libs_Encoder.html

#define SW1 A0
#define SW2 A1
#define SW3 A2
#define SW4 A3
#define SW5 A4

void send_CAN(uint32_t id, uint32_t data) {
  msg.uint32_data = data;
  byte sndStat = CAN.sendMsgBuf(id, 0, 4, msg.uint8_data);
  /*
    if (sndStat == CAN_OK) {
    //Serial.println("Message Sent Successfully!");
    } else {
    //Serial.println("Error Sending Message...");
    }
  */
  delayMicroseconds(100);
}

void ISR_100Hz() {
  if (timer_100Hz) {
    //Serial.println("100Hz overrun");
  } else {
    timer_100Hz = true;
  }
}

void setup() {
  //Serial.begin(115200);

  pinMode(CAN_INT, INPUT);
  if (CAN.begin(MCP_ANY, CAN_1000KBPS, MCP_16MHZ) == CAN_OK) {
    //Serial.println("MCP2515 Initialized Successfully!");
  }
  else {
    //Serial.println("Error Initializing MCP2515...");
  }
  CAN.setMode(MCP_NORMAL);

  servo.attach(SERVO_PWM);
  servo.write(SERVO_default);

  pinMode(SW1, INPUT_PULLUP);
  pinMode(SW2, INPUT_PULLUP);
  pinMode(SW3, INPUT_PULLUP);
  pinMode(SW4, INPUT_PULLUP);
  pinMode(SW5, INPUT_PULLUP);

  Timer3.initialize(10000);
  Timer3.attachInterrupt(ISR_100Hz);
}

void loop() {
  if (timer_100Hz) {
    timer_100Hz = false;
    
    //Serial.println("100Hz");
    send_CAN(0x201, abs(encoder1.read()));
    send_CAN(0x202, abs(encoder2.read()));
    send_CAN(0x203, abs(encoder3.read()));
    send_CAN(0x204, abs(encoder4.read()));
    send_CAN(0x205, abs(encoder5.read()));
    if (digitalRead(SW1) == LOW) {
      encoder1.write(0);
    }
    if (digitalRead(SW2) == LOW) {
      encoder2.write(0);
    }
    if (digitalRead(SW3) == LOW) {
      encoder3.write(0);
    }
    if (digitalRead(SW4) == LOW) {
      encoder4.write(0);
    }
    if (digitalRead(SW5) == LOW) {
      encoder5.write(0);
    }
  }

  if (!digitalRead(CAN_INT))
  {
    long unsigned int rxId;
    unsigned char len = 0;
    CAN.readMsgBuf(&rxId, &len, msg.uint8_data);

    //Serial.print(rxId, HEX);
    //Serial.print(",");
    //Serial.println(msg.uint32_data);

    switch (rxId) {
      case 0x100:
        servo.write(msg.uint32_data);
        break;
      case 0x101:
        motor1.setSpeed((int)msg.uint32_data - 256);
        break;
      case 0x102:
        motor2.setSpeed((int)msg.uint32_data - 256);
        break;
      case 0x103:
        motor3.setSpeed((int)msg.uint32_data - 256);
        break;
      case 0x104:
        motor4.setSpeed((int)msg.uint32_data - 256);
        break;
      case 0x105:
        motor5.setSpeed((int)msg.uint32_data - 256);
        break;
      default:
        break;
    }
  }
  encoder2.read();
  encoder3.read();
}
