const int OutputPin = 8;

void fSequence(int OutputPin) {
  int x = 1;
  // spend about 6 seconds repeating between high and low, first really fast, then slowing down
  while (x < 18) {
    digitalWrite(OutputPin, HIGH);
    delay(x*10);
    digitalWrite(OutputPin, LOW);
    delay((x+1)*10);
    x++;
  }
}

void setup() {
  // Declare output pin as an output
  pinMode(OutputPin, OUTPUT);

  // Allow serial debugging and access
  // Use 9600 baud as a sane default
  Serial.begin(9600);
}


void loop() {
  // INPUTS //
  // Wait for serial availability if it is lost
  while (Serial.available() == 0) {
  }

  // Read the incoming character from the serial port,
  char IncomingByte = Serial.read();

  // and print it to serial.
  Serial.print("Received: ");
  Serial.println(IncomingByte);


  
  // OUTPUTS //
  if (IncomingByte == 'f') {
    fSequence(OutputPin);
  } else {
    // Send a basic high signal to the output pin when any key is pressed
    digitalWrite(OutputPin, HIGH);
  }

  // Wait for a short duration before continuing (adjust as needed)
  delay(10);

  // Set the output pin LOW after a delay to control the duration
  digitalWrite(OutputPin, LOW);
}