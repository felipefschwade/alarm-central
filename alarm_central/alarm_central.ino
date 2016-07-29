//Adding the external libraries that are needed
/*
* SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4
*/
#include <SD.h>
#include <SPI.h>
#include <RCSwitch.h>
//By default, the RF SENSOR pin is definited in RFremote.h on pin2
/*
  @TODO
  A function that defines manually the pins
*/
#define SENSOR_PIR1 3
#define SDCARD 4
#define NEW_CONTROL_BUTTON 5 
#define SIREN 8
#define RED_LED 10
#define GREEN_LED 9
#define INDEFINIDO -1

//Defining all global scope variables

//The timer to use on Blinks.
unsigned long previousMillis = 0;
//The state control variable
int state;
//The possible status enum
enum Status {
    ALARM_OFF,
    ALARM_ON,
    ALARM_STARTED,
    NEW_CONTROL_ADDING
  };
  
//All the possible received signals
enum receivedSignal {
    CONTROL_SIGNAL,
    SENSOR_SIGNAL,
    NEW_CONTROL_BUTTON_PRESSED
  };

//Define the file containing all control codes
File myFile;

//TODO Auto control codes generation
//Here you put the quantity of controls that you want in you 
long int controles [21];

//Defining the RF433Mhz decoder library
RCSwitch mySwitch = RCSwitch();

void setup() {
  
  initiatePins();
  state = ALARM_OFF;
  Serial.begin(9600);
  Serial.println("INICIADO!");
  //If the card isn't located the software will get into sleep mode.
  if (!SD.begin(SDCARD)) {
    Serial.println("Initialization Failed! Please verify your SD Card and try Again");
    digitalWrite(RED_LED, HIGH);
    return;
  }
  mySwitch.enableReceive(0);
  myFile = SD.open("codes.txt", FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to test.txt...");
    myFile.println("testing 1, 2, 3.");
    // close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }

  // re-open the file for reading:
  myFile = SD.open("codes.txt");
  if (myFile) {
    Serial.println("codes.txt:");

    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
}

void loop() {
  int signalReceived = receivedSignal();
  switch (state) {
      case ALARM_OFF:
          if (signalReceived == CONTROL_SIGNAL) {
              setAlarmOn();
              break;
            } else if (signalReceived == NEW_CONTROL_BUTTON_PRESSED) {
              setNewControllAddingState();
              break;
            }
            ledBlink(GREEN_LED, 700);
      break;
      case ALARM_ON:
          if (signalReceived == CONTROL_SIGNAL) {
              setAlarmOff();
              break;
          } else if (signalReceived == SENSOR_SIGNAL) {
              startAlarm();
              break;
          }
          ledBlink(RED_LED, 700);
      break;
      case ALARM_STARTED:
          if (signalReceived == CONTROL_SIGNAL) {
                setAlarmOff();
                break;
              }
           ledBlink(RED_LED, 200);
      break;
      case NEW_CONTROL_ADDING:
//      if (rfrecv.available()) {
//       controle_novo = (char*)rfrecv.cmd;
//       Serial.println(controle_novo);
//       state = ALARM_OFF;
//       for (int i=0; i <= 2; i++) {
//          Serial.println(i);
//          turnOn(GREEN_LED);
//          delay(300);
//          turnOff(GREEN_LED);
//          delay(200);
  //        }
       if (signalReceived == NEW_CONTROL_BUTTON_PRESSED) {
          state = ALARM_OFF;
          Serial.println("Alarm Off");
          //Delay for the user don't accidetaly get again into this state
          delay(1000);
       }
      break;
    }
}

void initiatePins() {
    pinMode(SENSOR_PIR1, INPUT);
    pinMode(NEW_CONTROL_BUTTON, INPUT_PULLUP);
    pinMode(SIREN, OUTPUT);
    pinMode(RED_LED, OUTPUT);
    pinMode(GREEN_LED, OUTPUT);
}
//Return the signal to the controller
int receivedSignal() {
       if (mySwitch.available()) {
              Serial.println(mySwitch.getReceivedValue());
              Serial.println();
              for (int i=0; i < sizeof(controles); i++) {
                  if (controles[i] == mySwitch.getReceivedValue()) {
                  Serial.println("Control Signal");
                  mySwitch.resetAvailable();
                  return CONTROL_SIGNAL; 
                }
              }
              mySwitch.resetAvailable();
          }
//     if (digitalRead(SENSOR_PIR1) == 0) {
//           return SENSOR_SIGNAL; 
//        }
    if (digitalRead(NEW_CONTROL_BUTTON) == 0) {
        return NEW_CONTROL_BUTTON_PRESSED;
    }
    return INDEFINIDO;
}

void ledBlink(int led, int speed_milis) {
   int state = digitalRead(led);
   const long interval = speed_milis; 
   unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      digitalWrite(led, !state);
    }
}

void turnOn(int pin) {
  digitalWrite(pin, HIGH);
}

void turnOff(int pin) {
  digitalWrite(pin, LOW);
}

void sirenBeep(int times) {
      turnOn(SIREN);
      delay(times * 300);
      turnOff(SIREN);  
}

void setAlarmOn() {
    turnOff(GREEN_LED);
    Serial.println("Alarm On");
    state = ALARM_ON;
    sirenBeep(1);
    Serial.println(state);
    delay(300);
    turnOff(GREEN_LED);  
}

void setNewControllAddingState() {
      state = NEW_CONTROL_ADDING;
      Serial.println("New Control Adding");
      for (int i=0; i <= 2; i++) {
        Serial.println(i);
        turnOn(GREEN_LED);
        turnOn(RED_LED);
        delay(300);
        turnOff(GREEN_LED);
        turnOff(RED_LED);
        delay(200);
      }
}
/** 
*
*
*
**/
void setAlarmOff() {
    Serial.println("Alarm Off");
    turnOff(SIREN); 
    //Delay to avoid an accidental alarm activitation while the control button is pressed
    delay(300);
    state = ALARM_OFF;
    turnOff(RED_LED);
    sirenBeep(2);
    turnOff(RED_LED);
}

void startAlarm() {
    state = ALARM_STARTED;  
    Serial.println(state);
    Serial.println("Alarm STARTED");
    turnOn(SIREN);
}

