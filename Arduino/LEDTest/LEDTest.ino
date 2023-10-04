const int OutputPin = 8;

void setup() {
  // Declare output pin as an output
  pinMode(OutputPin, OUTPUT);

  // Allow serial debugging and access
  // Use 9600 baud as a sane default
  Serial.begin(9600);
}


void loop() {
  // Wait for serial availability if it is lost
  while (Serial.available() == 0) {
  }

  // Send a high signal to the output pin when any key is pressed
  digitalWrite(OutputPin, HIGH);

  // Read the incoming character from the serial port,
  char IncomingByte = Serial.read();
    
  // and print it to serial.
  Serial.print("Received: ");
  Serial.println(IncomingByte);

  // Wait for a short duration (adjust as needed)
  delay(10);

  // Set the output pin LOW after a delay to control the duration
  digitalWrite(OutputPin, LOW);
}