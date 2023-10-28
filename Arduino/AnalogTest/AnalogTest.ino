int AnalogPin = A3; // Connect pressure sensor or any other analog device to this pin

int Val = 0;  // Variable to store the value read from pin analogPin

void setup() {
  Serial.begin(9600);           //  setup serial
}

void loop() {
  Val = analogRead(AnalogPin);  // read the input pin
  Serial.println(Val);          // debug value
}