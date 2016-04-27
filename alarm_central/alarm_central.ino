
#include <RFremote.h>
//By default, the RF SENSOR pin is definited in RFremote.h on pin2
#define PIN_SENSOR_PIR1 3
#define PIN_SENSOR_PIR2 4
#define PIN_NEWCONTROL_BUTTON 5 
#define SIREN 8
#define RED_LED 9
#define GREEN_LED 10
int state;
enum Status {
    ALARM_OFF,
    ALARM_ON,
    ALARM_STARTED,
    NEW_CONTROL
  };

enum receivedSignal {
    CONTROL_SIGNAL,
    SENSOR_SIGNAL
  };
const char *sensor = "01010101010101010101010";
const char *controle = "0110100100110100110100100110110110100100100100100100110100100110110110100100110100110";


SignalPatternParams params;
RFrecv rfrecv;

void setup()
{
  int state = ALARM_OFF;
  initiate_pins();
  Serial.begin(9600);
  delay(500);
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

  switch (state) {
      case ALARM_OFF:
          Serial.println("Alarm Off");
          if (receivedSignal() == CONTROL_SIGNAL) {
              state = ALARM_ON;  
            }
      break;
      case ALARM_ON:
          Serial.println("Alarm On");
          if (receivedSignal() == CONTROL_SIGNAL) {
              state = ALARM_OFF;
          } else if(receivedSignal() == SENSOR_SIGNAL) {
              state = ALARM_STARTED;
          }
      break;
    }
  
  if (rfrecv.available())
  {
    if (strncmp((char*)rfrecv.cmd, sensor, CMD_SIZE) == 0)
    {
      Serial.println("Disparado!");
    } 
    else if (strncmp((char*)rfrecv.cmd, controle, CMD_SIZE) == 0) {
      Serial.println("Sinal do Controle!");
    } else {
      Serial.println("ERRO!");
    }
  }
}

void initiate_pins() {
    pinMode(PIN_SENSOR_PIR1, INPUT);
    pinMode(PIN_SENSOR_PIR2, INPUT);
    pinMode(PIN_NEWCONTROL_BUTTON, INPUT_PULLUP);
    pinMode(SIREN, OUTPUT);
    pinMode(RED_LED, OUTPUT);
    pinMode(GREEN_LED, OUTPUT);
}
int receivedSignal() {
    
  }
void sirenOff() {
  digitalWrite(SIREN, LOW);  
}
void sirenOn() {
  digitalWrite(SIREN, HIGH);  
}
