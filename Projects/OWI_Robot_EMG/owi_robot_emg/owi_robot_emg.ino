#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"
#include <SoftwareSerial.h> 

Adafruit_MotorShield AFMS = Adafruit_MotorShield();

Adafruit_DCMotor *motor1 = AFMS.getMotor(1);  // Pincher
Adafruit_DCMotor *motor2 = AFMS.getMotor(2);  // Elbow Motor
Adafruit_DCMotor *motor3 = AFMS.getMotor(3);  // Turntable Motor
Adafruit_DCMotor *motor4 = AFMS.getMotor(4);  // Hip Motor

uint8_t message = 0xFF;   // Message from Serial Port

int ss_rx_pin = 10;  // RX pin on Arduino, TX pin of bluetooth module [D10]
int ss_tx_pin = 11;  // TX pin of Arduino, RX pin of bluetooth module [D11]

SoftwareSerial bluetooth(ss_rx_pin, ss_tx_pin);

void setup() 
{
    Serial.begin(9600);       // Setting up serial communication to computer
    AFMS.begin();             // Setting up the Adafruit Motor Shield
    motor1->setSpeed(100);    // Setting the initial speed of the motor #1 to 100
    motor2->setSpeed(100);    // Setting the initial speed of the motor #2 to 100
    motor3->setSpeed(100);    // Setting the initial speed of the motor #3 to 100
    motor4->setSpeed(100);    // Setting the initial speed of the motor #4 to 100

//    bluetooth.begin(115200);  // The Bluetooth Mate defaults to 115200bps
//    bluetooth.print("$");  // Print three times individually
//    bluetooth.print("$");
//    bluetooth.print("$");  // Enter command mode
//    delay(100);  // Short delay, wait for the Mate to send back CMD
//    bluetooth.println("U,9600,N");  // Temporarily Change the baudrate to 9600, no parity
//    bluetooth.end();    // Stop the bluetooth data transmission
//    delay(100);
    // 115200 can be too fast at times for NewSoftSerial to relay the data reliably
    bluetooth.begin(9600);  // Start bluetooth serial at 9600
    
    delay(1000);              // Delaying 1 second to initialize functions
    Serial.println("Hello Moto");
    bluetooth.write(100);
}


void loop() 
{
  delay(100);
  if(Serial.available() || bluetooth.available())
  {
      if(Serial.available())
      {
          message = Serial.read();
          Serial.print("Serial message: ");
      }
      else if(bluetooth.available())
      {
          message = bluetooth.read();
          
          Serial.println("Bluetooth message: ");
      }
      if(message != 0x00)
      {
          _robot_handler();
          _serial_flush();
      }
  }      
}

/*
 * Small function to clear the input serial buffer for the Serial input and the Bluetooth input
 */
static void _serial_flush()
{
    while(Serial.available() || bluetooth.available())
    {
        Serial.read();
        bluetooth.read();
    }
}

static void _robot_handler()
{
    switch(message)
    {
        // Motor #1
        case 'a': // Open Pinchers
            motor1->run(BACKWARD);
            delay(1000);
            motor1->run(RELEASE);
            Serial.println(char(message));
            break;
        case 'b': // Close Pinchers
            motor1->run(FORWARD);
            delay(1000);
            motor1->run(RELEASE);
            Serial.println(char(message));
            break;
        case 'c': // Stop
            motor1->run(RELEASE);
            Serial.println(char(message));
            break;
        case 'd': // Open then Close Pinchers
            motor1->run(FORWARD);
            delay(2000);
            motor1->run(RELEASE);
            delay(500);
            motor1->run(BACKWARD);
            delay(2000);
            motor1->run(RELEASE);
            Serial.println(char(message));
            break;
            
        // Motor #2
        case 'e': // Clockwise Rotation
            motor2->run(FORWARD);
            delay(1000);
            motor2->run(RELEASE);
            Serial.println(char(message));
            break;
        case 'f': // Counter-Clockwise Rotation
            motor2->run(BACKWARD);
            delay(1000);
            motor2->run(RELEASE);
            Serial.println(char(message));
            break;
        case 'g': // Stop
            motor2->run(RELEASE);
            Serial.println(char(message));
            break;
        case 'h': //Clockwise then Counter-Clockwise Rotation
            motor2->run(FORWARD);
            delay(2000);
            motor2->run(RELEASE);
            delay(500);
            motor2->run(BACKWARD);
            delay(2000);
            motor2->run(RELEASE);
            Serial.println(char(message));
            break;         
    
        // Motor #3
        case 'i': // Clockwise rotation
            motor3->run(FORWARD);
            delay(1000);
            motor3->run(RELEASE);
            Serial.println(char(message));
            break;
        case 'j': // Counter-Clockwise Rotation
            motor3->run(BACKWARD);
            delay(1000);
            motor3->run(RELEASE);
            Serial.println(char(message));
            break;
        case 'k': // Stop
            motor3->run(RELEASE);
            Serial.println(char(message));
            break;
        case 'l': // Clockwise then Counter-Clockwise Rotation
            motor3->run(FORWARD);
            delay(2000);
            motor3->run(RELEASE);
            delay(500);
            motor3->run(BACKWARD);
            delay(2000);
            motor3->run(RELEASE);
            Serial.println(char(message));
            break;  
    
        // Motor #4
        case 'm': // Clockwise rotation
            motor4->run(FORWARD);
            delay(1000);
            motor4->run(RELEASE);
            Serial.println(char(message));
            break;
        case 'n': // Counter-Clockwise Rotation
            motor4->run(BACKWARD);
            delay(1000);
            motor4->run(RELEASE);
            Serial.println(char(message));
            break;
        case 'o': // Stop
            motor4->run(RELEASE);
            Serial.println(char(message));
            break;
        case 'p': //Clockwise then Counter-Clockwise Rotation
            motor4->run(FORWARD);
            delay(2000);
            motor4->run(RELEASE);
            delay(500);
            motor4->run(BACKWARD);
            delay(2000);
            motor4->run(RELEASE);
            Serial.println(char(message));
            break;  
    
        case ('1'):   // Flexion: Move from rest to the down position, wait, then move from down position to rest
        case (1):
            motor2->run(BACKWARD);
            delay(4500);
            motor2->run(RELEASE);
            
            delay(1000);
            
            motor2->run(FORWARD);
            delay(6000);
            motor2->run(RELEASE);
            Serial.println(char(message));
            break;
            
        case ('2'):   // Leg-up: Move from rest to the up position, wait, then move from up position to rest
        case (2):
            motor2->run(FORWARD);
            delay(5500);
            motor2->run(RELEASE);
            
            delay(1000);
            
            motor2->run(BACKWARD);
            delay(5000);
            motor2->run(RELEASE);
            
            Serial.println(char(message));
            break;
    
        case ('3'):   // Flex & Extension: Move from rest to the down position, then close grippers. Wait. Then move from down position to rest. Open grippers. 
        case (3):
            motor2->run(BACKWARD);
            delay(3500);
            motor2->run(RELEASE);
            
            motor1->run(FORWARD);
            delay(2000);
            motor1->run(RELEASE);
            
            delay(1000);
            
            motor2->run(FORWARD);
            delay(5000);
            motor2->run(RELEASE);
            
            motor1->run(BACKWARD);
            delay(2000);
            motor1->run(RELEASE);
            
            Serial.println(char(message));
            break;
        
        case ('4'):   // Leg Left: Move from rest position to the left position. Wait. Then move from left position to rest.
        case (4):
            motor3->run(BACKWARD);
            delay(4000);
            motor3->run(RELEASE);
            
            delay(1000);
            
            motor3->run(FORWARD);
            delay(4000);
            motor3->run(RELEASE);
            
            Serial.println(char(message));
            break;
    
        case ('5'):   // Move from rest to the grab position
        case(5):
            motor3->run(FORWARD);
            delay(4000);
            motor3->run(RELEASE);
            
            delay(1000);
            
            motor3->run(BACKWARD);
            delay(4000);
            motor3->run(RELEASE);
            
            Serial.println(char(message));
            break;
            
    //          case ('6'):   // Move from grab position to rest
    //          case (6):
    //              motor1->run(BACKWARD);
    //              delay(2000);
    //              motor1->run(RELEASE);
    //              Serial.println(char(message));
    //              break;
    //
    //          case ('7'):   // Move from rest to the left position
    //          case (7):
    //              motor3->run(BACKWARD);
    //              delay(4000);
    //              motor3->run(RELEASE);
    //              Serial.println(char(message));
    //              break;
    //          case '8':   // Move from left position to rest
    //              motor3->run(FORWARD);
    //              delay(4000);
    //              motor3->run(RELEASE);
    //              Serial.println(char(message));
    //              break;
    //
    //          
    //          case '9':   // Move from rest to the right position
    //              motor3->run(FORWARD);
    //              delay(4000);
    //              motor3->run(RELEASE);
    //              Serial.println(char(message));
    //              break;
    //          case '0':   // Move from right position to rest
    //              motor3->run(BACKWARD);
    //              delay(4000);
    //              motor3->run(RELEASE);
    //              Serial.println(char(message));
    //              break;
    
        case 10:  // New line feed
            break;
    
        case 13:  // Carriage Return
            break;
            
        default:
            Serial.println("Default Case:");
            Serial.print("Message:");
            Serial.println(message);
            break;
    }
}
