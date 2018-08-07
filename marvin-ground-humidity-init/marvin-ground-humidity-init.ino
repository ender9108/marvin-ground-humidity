String action;

void setup() {
  Serial.begin(9600);
}

void loop() {
  action = Serial.readString();
  action.toUpperCase();

  if (action == "PING-GROUND-HUMIDITY") {
    Serial.println(F("PONG-GROUND-HUMIDITY"));
  }
}
