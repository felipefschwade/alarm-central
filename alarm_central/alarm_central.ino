
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
    NEW_CONTROL_BUTTON_PRESSED
};

const char *sensor = "01010101010101010101010";
const char *controle = "0110100100110100110100100110110110100100100100100100110100100110110110100100110100110";
const char *controle_novo;


SignalPatternParams params;
RFrecv rfrecv;

void setup() {
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
              Serial.println();
              if (strncmp((char*)rfrecv.cmd, sensor, CMD_SIZE) == 0) {
                Serial.println("Door/Window Open Sensor Signal!");
                return SENSOR_SIGNAL;
              }
              if (strncmp((char*)rfrecv.cmd, controle, CMD_SIZE) == 0) {
                Serial.println("Control Signal");
                return CONTROL_SIGNAL; 
              }
              if (strncmp((char*)rfrecv.cmd, controle_novo, CMD_SIZE) == 0) {
                Serial.println("New Control Signal");
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
    turnOff(GREEN_LED);  
}

void setNewControllAddingState() {
    //modify state glabal variable
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
