void setup() {
  pinMode(8, INPUT);
  Serial.begin(9600);
}

void loop() {
  int estado = digitalRead(8);
  Serial.println(estado);
}
