
#include <RFremote.h>
//By default, the RF SENSOR pin is definited in RFremote.h on pin2
#define SENSOR_PIR1 3
#define SENSOR_PIR2 4
#define NEW_CONTROL_BUTTON 5 
#define SIREN 8
#define RED_LED 10
#define GREEN_LED 9
#define INDEFINIDO -1
unsigned long previousMillis = 0;
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
  state = ALARM_OFF;
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
  int signalReceived = receivedSignal();
  switch (state) {
      case ALARM_OFF:
          ledBlink(GREEN_LED, 700);
          if (signalReceived == CONTROL_SIGNAL) {
              turnOff(GREEN_LED);
              Serial.println("Alarm On");
              state = ALARM_ON;
              Serial.println(state);
              turnOn(SIREN);
              delay(1000);
              turnOff(SIREN);
            }
            //} else if (newControlButtonPressedFor5sec()) {
            //  state = NEW_CONTROL_ADDING;
            //}
      break;
      case ALARM_ON:
          ledBlink(RED_LED, 700);
          if (signalReceived == CONTROL_SIGNAL) {
              Serial.println("Alarm Off");
              state = ALARM_OFF;
              Serial.println(state);
              turnOff(RED_LED);
              turnOn(SIREN);
              delay(1000);
              turnOff(SIREN);
          } else if(signalReceived == SENSOR_SIGNAL) {
              state = ALARM_STARTED;  
              Serial.println(state);
              Serial.println("Alarm STARTED");
              turnOn(SIREN);
          }
      break;
      case ALARM_STARTED:
          ledBlink(RED_LED, 200);
          if (signalReceived == CONTROL_SIGNAL) {
                turnOff(SIREN);
                state = ALARM_OFF;
                Serial.println("Alarm Off");
                Serial.println(state);
                turnOff(RED_LED);
                turnOn(SIREN);
                delay(1000);
                turnOff(SIREN); 
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
//Return the signal to the controller
int receivedSignal() {
       if (rfrecv.available()) {
              Serial.println((char*)rfrecv.cmd);
              if (strncmp((char*)rfrecv.cmd, sensor, CMD_SIZE) == 0) {
                Serial.println("Door/Window Open Sensor Signal!");
                return SENSOR_SIGNAL;
              }
              if (strncmp((char*)rfrecv.cmd, controle, CMD_SIZE) == 0) {
                Serial.println("Control Signal");
                return CONTROL_SIGNAL; 
              }
          }
     if (digitalRead(SENSOR_PIR1) == 0) {
           return SENSOR_SIGNAL; 
        }
//      if (digitalRead(SENSOR_PIR2) == 0) {        
//        Serial.println("Sensor PIR2 Signal");        
//        return SENSOR_SIGNAL;      
//      }
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
int newControlButtonPressedFor5sec() {
    for (int i = 0; i <= 50; i++) {
        if (digitalRead(NEW_CONTROL_BUTTON == 0)) {
          return 0;
        } else if (i == 50){
          return 1;
        }
        delay(100);
    }
}
