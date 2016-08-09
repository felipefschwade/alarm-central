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
/*
  @TODO a personal .h file for the library
*/

//Defining the functions used
void initiatePins();
int receivedSignal();
void setAlarmOff();
void ledBlink(int led, int speed_milis);
void turnOn(int pin);
void turnOff(int pin);
void sirenBeep(int times);
void setAlarmOn();
void setNewControllAddingState();
void setAlarmOff();
void startAlarm();
void SDOpenFileFailed();
void SDReadFailed();
void loadData();

//By default, the RF SENSOR pin is definited in RCswitch library on pin2
/*
  @TODO
  A function that defines the pins in a easier way
*/
#define SENSOR_PIR1 3
#define SDCARD 4
#define NEW_CONTROL_BUTTON 5 
#define SIREN 8
#define RED_LED 6
#define GREEN_LED 7
#define INDEFINIDO -1

//Defining all global scope variables
//The new control variable
long int new_control;
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
long int controls [21];

//Defining the RF433Mhz decoder library
RCSwitch mySwitch = RCSwitch();

void setup() {
  Serial.begin(9600);
  Serial.println("INICIADO!");
  initiatePins();
  mySwitch.enableReceive(0);
  //If the card isn't located the software will get into sleep mode.
  if (!SD.begin(SDCARD)) {
    SDReadFailed();
  }
  loadData();
  state = ALARM_OFF;
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
      //Reset your arduino after adding a new control.
      case NEW_CONTROL_ADDING:
        boolean flag = 0;
        if (mySwitch.available()) {
         new_control = mySwitch.getReceivedValue();
         Serial.println(new_control);
         mySwitch.resetAvailable();
         myFile = SD.open("codes.txt", FILE_WRITE);
        // if the file opened okay, write to it
        if (myFile) {
          Serial.print("Writing the new code into the codes.txt...");
          myFile.println(new_control);
          myFile.close();
          Serial.println("Control Code save with success.");
          loadData();
          //Make a loop to indicate using led blink that the control were successfull saved
          for (int i=0; i <= 2; i++) {
            //Proposital delay for avoid a accindetal Alarm Set while adding a control
            turnOn(GREEN_LED);
            delay(300);
            turnOff(GREEN_LED);
            delay(200);
            flag = 1;
           }
        } else {
          //Lock the file again if something went wrong
          SDOpenFileFailed(); 
         }
       } else if (signalReceived == NEW_CONTROL_BUTTON_PRESSED) {
          //Delay for the user don't accidetaly get again into this state
          delay(1000);
          flag = 1;
       }
      if (flag == 1) {
        Serial.println("Alarm Off");
        state = ALARM_OFF;
        break;
      }
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
              for (int i=0; i < 21; i++) {
                  Serial.println(i);
                  Serial.println(controls[i]);
                  if (controls[i] != NULL && controls[i] == mySwitch.getReceivedValue()) {
                    Serial.println("Control Signal");
                    mySwitch.resetAvailable();
                    //Delay to slow down the RFsignal reading
                    delay(500);
                    return CONTROL_SIGNAL; 
                  }
              }
              mySwitch.resetAvailable();
          }
        if (digitalRead(NEW_CONTROL_BUTTON) == 0) {
            return NEW_CONTROL_BUTTON_PRESSED;
        }
       if (digitalRead(SENSOR_PIR1) == 0) {
             return SENSOR_SIGNAL; 
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
    mySwitch.resetAvailable();
    //Setting a delay to avoid turning the alarm off accidentally
    delay(300);
    sirenBeep(1);
    Serial.println(state);
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
    mySwitch.resetAvailable();
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

//Lock the code execution and turn on the green led for show a file oppening error.
void SDOpenFileFailed() {
  // if the file didn't open, print an error and stay:
    Serial.println("Error opening codes.txt, please review your SD Card");
    turnOn(GREEN_LED);
    delay(999999);
}

//Lock the code execution and turn on the red led for show a SDCard oppening error.
void SDReadFailed() {
    Serial.println("Initialization Failed! Please verify your SD Card and try Again");
    digitalWrite(RED_LED, HIGH);
    delay(999999);
}

//Load all the data from de SD card and put it into the Arduino RAM
void loadData() {
  myFile = SD.open("codes.txt", FILE_WRITE);
  // Open the file for reading:
  myFile = SD.open("codes.txt");
  if (myFile) {
    // read from the file until there's nothing else in it:
    int i = 0;
    while (myFile.available()) {
     Serial.println("Lendo o arquivo");
     controls[i] = myFile.parseInt();
     Serial.println(controls[i]);
     i++;
    }
    // close the file:
    myFile.close();
  } else {
    SDOpenFileFailed();
  }
}
