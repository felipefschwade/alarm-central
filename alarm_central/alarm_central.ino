
#include <RFremote.h>
//By default, the RF SENSOR pin is definited in RFremote.h on pin2
#define SENSOR_PIR1 3
#define SENSOR_PIR2 4
#define NEW_CONTROL_BUTTON 5 
#define SIREN 8
#define RED_LED 9
#define GREEN_LED 10
int state;
enum Status {
    ALARM_OFF,
    ALARM_ON,
    ALARM_STARTED,
    NEW_CONTROL_ADDING
  };

enum receivedSignal {
    CONTROL_SIGNAL,
    SENSOR_SIGNAL,
  };
const char *sensor = "01010101010101010101010";
const char *controle = "0110100100110100110100100110110110100100100100100100110100100110110110100100110100110";


SignalPatternParams params;
RFrecv rfrecv;

void setup()
{
  initiatePins();
  int state = ALARM_OFF;
  turnOn(GREEN_LED);
  Serial.begin(9600);
  Serial.println("INICIADO!");
  params.spaceMin = 3500;
  params.spaceMax = 17000;
  params.dotMin = 300;
  params.dotMax = 600;
  params.traceMin = 700;
  params.traceMax = 1050;
  params.skipFirst = 0;
  params.skipLast = 0;

  rfrecv = RFrecv(&params);
  rfrecv.begin();
}

void loop() {
  Serial.println(SENSOR_PIR1);
  int receivedsignal =  receivedSignal();
  Serial.println(receivedsignal);
  switch (state) {
      case ALARM_OFF:
          Serial.println("Alarm Off");
          if (receivedsignal == CONTROL_SIGNAL) {
              state = ALARM_ON;
              Serial.println(receivedsignal);
              turnOff(GREEN_LED);
              turnOn(RED_LED);
              sirenBeep(1);
            } else if (newControlButtonPressedFor5sec()) {
              state = NEW_CONTROL_ADDING;
            }
      break;
      case ALARM_ON:
          Serial.println("Alarm On");
          if (receivedSignal() == CONTROL_SIGNAL) {
              state = ALARM_OFF;
              turnOff(RED_LED);
              turnOn(GREEN_LED);
              sirenBeep(2);
          } else if(receivedSignal() == SENSOR_SIGNAL) {
              state = ALARM_STARTED;  
              turnOn(SIREN);
          }
      break;
      case ALARM_STARTED:
          Serial.println("Danger, Alarm Started");
          ledBlink(RED_LED);
          if (receivedSignal() == CONTROL_SIGNAL) {
            turnOff(SIREN);
            turnOn(RED_LED);
            sirenBeep(2);  
          }
      break;
      case NEW_CONTROL_ADDING:
      break;
    }
}

void initiatePins() {
    pinMode(SENSOR_PIR1, INPUT);
    pinMode(SENSOR_PIR2, INPUT);
    pinMode(NEW_CONTROL_BUTTON, INPUT_PULLUP);
    pinMode(SIREN, OUTPUT);
    pinMode(RED_LED, OUTPUT);
    pinMode(GREEN_LED, OUTPUT);
}
//ReceivedSignal will be improved using switch case, this version is just for testing
int receivedSignal() {
     if (rfrecv.available() && strncmp((char*)rfrecv.cmd, sensor, CMD_SIZE) == 0) {
        Serial.println("Door/Window Open Sensor Signal!");
        return SENSOR_SIGNAL;
     } else if (rfrecv.available() && strncmp((char*)rfrecv.cmd, controle, CMD_SIZE) == 0) {
        Serial.println("Control Signal");
        return CONTROL_SIGNAL;
     } else if (digitalRead(SENSOR_PIR1) == 0) {
         Serial.println("Sensor PIR1 Signal");
         return SENSOR_SIGNAL;
     } else if (digitalRead(SENSOR_PIR2) == 0) {
         Serial.println("Sensor PIR1 Signal");
         return SENSOR_SIGNAL;
     }
}
void ledBlink(int led) {
   digitalWrite(led, HIGH);
   delay(100);
   digitalWrite(led, LOW);
}
void blinkAllLeds(int times) {
    for (int i = 0; i < times; i++) {
      digitalWrite(RED_LED, HIGH);
      digitalWrite(GREEN_LED, HIGH);
      delay(200);
      digitalWrite(RED_LED, LOW);
      digitalWrite(GREEN_LED, LOW);
    }
  }
void turnOn(int led) {
  digitalWrite(led, HIGH);
}
void turnOff(int led) {
  digitalWrite(led, LOW);
}
void sirenBeep(int times) {
    for (int i = 0; i < times; i++) {
        pinMode(SIREN, HIGH);
        delay(200);
        pinMode(SIREN, LOW);
      }
  }
int newControlButtonPressedFor5sec() {
    for (int i = 0; i <= 50; i++) {
        if (NEW_CONTROL_BUTTON == 1) {
          return 0;
        } else if (i == 50){
          return 1;
        }
        delay(100);
    }
}
