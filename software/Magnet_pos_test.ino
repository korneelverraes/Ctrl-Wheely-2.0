int sensorPin[] = { A0, A1, A2, A3, A4 };
int sensorValue = 0;

void setup() {
  Serial.begin(115200);
}

void loop() {
  for (int i = 0; i < 5; i++) {
    sensorValue = analogRead(sensorPin[i]);
    Serial.print(sensorValue);
    Serial.print(" | ");
  }
  Serial.println();
}
