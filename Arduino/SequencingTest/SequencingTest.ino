const int nitrogenPin = 13;
const int oxygenPin = 12;
const int ethanolPin = 8;
const int bleedPin = 7;

struct TelemFrame {
  uint32_t header;

  int sensor1;
  int sensor2;
  float sensor3;
  float sensor4;

  uint32_t footer;
};

int sensor1() {
  return 1;
}
int sensor2() {
  return random(0,50);
}
float sensor3() {
  int num = random(0,101);
  return num/100.0;
}
float sensor4() {
  int num = random(0,101);
  return num*3.0/100.0;
}

void Sequence1() {
  Serial.println("Nitrogen Valve Open");
  digitalWrite(nitrogenPin, HIGH);
  delay(1000);
  Serial.println("Oxygen Main Valve Open");
  digitalWrite(oxygenPin, HIGH);
  delay(75);
  Serial.println("Ethanol Main Valve Open");
  digitalWrite(ethanolPin, HIGH);
  delay(2000);
  Serial.println("Oxygen Main Valve Close");
  digitalWrite(oxygenPin, LOW);
  Serial.println("Ethanol Main Valve Close");
  digitalWrite(ethanolPin, LOW);
  delay(500);
  Serial.println("Nitrogen Valve Close");
  digitalWrite(nitrogenPin, LOW);
  Serial.println("Bleed Valve Open");
  digitalWrite(bleedPin, HIGH);
}

void setup() {
  Serial.begin(9600); // Initialize serial communication at 9600 baud rate
  randomSeed(analogRead(0)); // Seed the random number generator with an analog reading
  
  pinMode(nitrogenPin, OUTPUT); // Set the LED pin as an output
  pinMode(oxygenPin, OUTPUT); // Set the LED pin as an output
  pinMode(ethanolPin, OUTPUT); // Set the LED pin as an output
  pinMode(bleedPin, OUTPUT); // Set the LED pin as an output
}

// void loop() {
//   int randomNumber = random(0, 100); // Generate a random number between 0 and 99
  
//   // Send fake sensor data over serial
//   // int header = 0xDEADBEEF;
//   // int footer = 3405707998;
//   // TelemFrame frame;
//   // frame.header = header;
//   // frame.sensor1 = sensor1();
//   // frame.sensor2 = sensor2();
//   // frame.sensor3 = sensor3();
//   // frame.sensor4 = sensor4();
//   // frame.footer = footer;
//   // Serial.println(String(frame));
//   Serial.println(String(sensor1()) + ", " +
//                  String(sensor2()) + ", " +
//                  String(sensor3()) + ", " +
//                  String(sensor4()));
  
//   if (Serial.available() > 0) {
//     String input = Serial.readStringUntil('\n'); // Read the input from serial until newline character
    
//     if (input == "n") {
//       digitalWrite(nitrogenPin, !digitalRead(nitrogenPin)); // Toggle the LED state
//       Serial.println("Nitrogen toggled!"); // Send a confirmation message over serial
//     }
//     else if (input == "e") {
//       digitalWrite(ethanolPin, !digitalRead(ethanolPin));
//       Serial.println("Ethanol toggled!");
//     }
//     else if (input == "b") {
//       digitalWrite(bleedPin, !digitalRead(bleedPin));
//       Serial.println("Bleed toggled!");
//     }
//     else if (input == "o") {
//       digitalWrite(oxygenPin, !digitalRead(oxygenPin));
//       Serial.println("Oxygen toggled!");
//     }

//     if (input == "seq 1") {
//       Sequence1();
//     }
//   }
// }

bool CommandRunning = false;
double CommandStartTime = 0.0;
String command = "";

void loop() {
  // Listen for a command
  // If there is a command, update the state and start the command function
  if (!CommandRunning) {
    command = "";
    if (Serial.available() > 0) {
      command = Serial.readStringUntil('\n');
      CommandRunning = true;
      CommandStartTime = millis();
    }
  }
  if (command == "press") {
      Command1(CommandRunning, CommandStartTime);
  } else if (command == "test1") {
    ;
  }
  // Run telemetry
  RunTelemetry();
}

void RunTelemetry() {
  // Concatenate all sensor values and write over serial
  int randomNumber = random(0, 100); // Generate a random number between 0 and 99
  // Send fake sensor data over serial
  // int header = 0xDEADBEEF;
  // int footer = 3405707998;
  // TelemFrame frame;
  // frame.header = header;
  // frame.sensor1 = sensor1();
  // frame.sensor2 = sensor2();
  // frame.sensor3 = sensor3();
  // frame.sensor4 = sensor4();
  // frame.footer = footer;
  // Serial.println(String(frame));
  Serial.println(String(sensor1()) + ", " +
                 String(sensor2()) + ", " +
                 String(sensor3()) + ", " +
                 String(sensor4()));
}

void Command1(bool command, double startTime) {
  // Handle what needs to happen based on global time and state
  double dt = millis() - startTime;
  if (command) {
    if (dt >= 0 && dt < 1000) {
      Serial.println("Nitrogen Valve Open");
      digitalWrite(nitrogenPin, HIGH);
      return;
    }
    // delay(1000);
    if (dt >= 1000 && dt < 1075) {
      Serial.println("Oxygen Main Valve Open");
      digitalWrite(oxygenPin, HIGH);
      return;
    }
    // delay(75);
    if (dt >= 1075 && dt < 3075) {
      Serial.println("Ethanol Main Valve Open");
      digitalWrite(ethanolPin, HIGH);
      return;
    }
    // delay(2000);
    if (dt >= 3075 && dt < 3575) {
      Serial.println("Oxygen Main Valve Close");
      digitalWrite(oxygenPin, LOW);
      Serial.println("Ethanol Main Valve Close");
      digitalWrite(ethanolPin, LOW);
      return;
    }
    // delay(500);
    if (dt >= 3575) {
      Serial.println("Nitrogen Valve Close");
      digitalWrite(nitrogenPin, LOW);
      Serial.println("Bleed Valve Open");
      digitalWrite(bleedPin, HIGH);
      command = false;
      return;
    }
  }
}

