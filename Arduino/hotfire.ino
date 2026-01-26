#include <Servo.h>

// Valve Pins
const int nitrogenPin = 10; // servo (PWM)
const int purgePin = 6; // servo (PWM)
const int mainEthanolPin = 3; // servo (PWM)
const int mainNitrousPin = 9; // servo (PWM)
const int asiEthanolPin = 7; // solenoid (relay)
const int asiOxygenPin = 4; // solenoid (relay)
const int nitrogenBleedPin = 12; // solenoid (relay)

// Valve Servos
Servo NitrogenServo;
Servo PurgeServo;
Servo MainEthanolServo;
Servo MainNitrousServo;

// Spark Plug Pins
const int sparkPin = 8; // to close the relay for the exciter box (relay)
const int rpmPin = 5; // to write the PWM wave to (PWM)

// PT Pins
const int NITROGEN_LINE_PT_PIN = A2; // analog in
const int ETHANOL_TANK_PT_PIN = A3; // analog in
const int NITROUS_LINE_PT_PIN = A4; // analog in
const int OXYGEN_LINE_PT_PIN = A1; // analog in
const int FUEL_INLET_PT_PIN = A3; // analog in
const int FUEL_OUTLET_PT_PIN = A3; // analog in
const int CHAMBER_PRESSURE_PT_PIN = A5; // analog in

// Load Cell Pin
const int LC_PIN = A0; // analog in, need to voltage divide so input is 0.0 - 5.0 volts


const int CLOSED_ANGLE = 179; // 179 degrees = CLOSED
const int OPEN_ANGLE = 91; // 91 degrees = OPEN


unsigned long timer = millis(); // setting up the timer variable

// As defined rn, struct string is I8fI
struct data {
  uint32_t header;
  float val1;
  float val2;
  float val3;
  float val4;
  float val5;
  float val6;
  float val7;
  float val8;
  uint32_t footer;
};

void setup() {
  Serial.begin(9600); // 9600 baud ?????? too low ???????
  Serial.setTimeout(50); // 50 milliseconds

  // Outputs
  //pinMode(nitrogenPin, OUTPUT);
  //pinMode(purgePin, OUTPUT);
  //pinMode(mainEthanolPin, OUTPUT);
  //pinMode(mainNitrousPin, OUTPUT);
  pinMode(asiEthanolPin, OUTPUT);
  pinMode(asiOxygenPin, OUTPUT);
  pinMode(nitrogenBleedPin, OUTPUT);
  pinMode(sparkPin, OUTPUT);
  pinMode(rpmPin, OUTPUT);

  digitalWrite(asiEthanolPin, HIGH); // want to flip theseeeeeeeeeeeeeeeeeeeeeee (LOW should be default state)
  digitalWrite(asiOxygenPin, HIGH);
  digitalWrite(nitrogenBleedPin, HIGH); // normally-open valveeeeeeee
  digitalWrite(sparkPin, HIGH);

  NitrogenServo.attach(nitrogenPin);
  PurgeServo.attach(purgePin);
  MainEthanolServo.attach(mainEthanolPin);
  MainNitrousServo.attach(mainNitrousPin);

  NitrogenServo.write(CLOSED_ANGLE); // 179 degrees = CLOSED
  PurgeServo.write(CLOSED_ANGLE);
  MainEthanolServo.write(CLOSED_ANGLE);
  MainNitrousServo.write(CLOSED_ANGLE);

  analogWrite(rpmPin, 0); // 0% duty cycle PWM wave


  // Inputs
  pinMode(NITROGEN_LINE_PT_PIN, INPUT);
  pinMode(ETHANOL_TANK_PT_PIN, INPUT);
  pinMode(NITROUS_LINE_PT_PIN, INPUT);
  pinMode(OXYGEN_LINE_PT_PIN, INPUT);
  pinMode(FUEL_INLET_PT_PIN, INPUT);
  pinMode(FUEL_OUTLET_PT_PIN, INPUT);
  pinMode(CHAMBER_PRESSURE_PT_PIN, INPUT);
  pinMode(LC_PIN, INPUT);
}

void loop() {
  //Serial.flush();
  
  SendData();
  //delay(10);
  //Serial.flush();

  CheckForCommand();
  //delay(10);
  //Serial.flush();

  //timer = millis(); // constantly update the timer while the main loop is running, but it will NOT update while one of the hotfire commands is running
}

float ReadSensor(int pin) {
  int rawVal = analogRead(pin); // read the input pin
  float rawVolt = (float)rawVal / 204.6; // convert to a voltage value 0.0 to 5.0 volts
  // 0.5V = 0.0psi
  // 4.5V = 200.0psi
  if(rawVolt<0.5) {
    rawVolt = 0.5;
  }
  else if(rawVolt>4.5) {
    rawVolt = 4.5;
  }
  float normalized = (rawVolt - 0.5) / (4.5 - 0.5); // normalize from 0.0 to 1.0 --> 0.0 = 0psig, 1.0 = 200psig
  float psi = normalized * 200; // convert to final psig value

  return psi;
}

float ReadSensor2(int pin) {
  int rawVal = analogRead(pin); // read the input pin
  float rawVolt = (float)rawVal / 204.6; // convert to a voltage value 0.0 to 5.0 volts
  // 0.5V = 0.0psi
  // 4.5V = 1000.0psi
  if(rawVolt<0.5) {
    rawVolt = 0.5;
  }
  else if(rawVolt>4.5) {
    rawVolt = 4.5;
  }
  float normalized = (rawVolt - 0.5) / (4.5 - 0.5); // normalize from 0.0 to 1.0 --> 0.0 = 0psig, 1.0 = 1000psig
  float psi = normalized * 1000; // convert to final psig value

  return psi;
}

float ReadLoadCell(int pin) {
  int rawVal = analogRead(pin); // read the input pin
  float rawVolt = (float)rawVal / 204.6; // convert to a voltage value 0.0 to 5.0 volts
  // 0.0V = 0kg
  // 5.0V = 1000kg
  float normalized = rawVolt / 5; // normalize from 0.0 to 1.0 --> 0.0 = 0kg, 1.0 = 1000kg
  float kg = normalized * 1000; // convert to kg value
  float lbs = kg * 2.20462; // convert to lbs
  
  return lbs;
}

void SendData() {
  uint32_t header = 0xDEADBEEF;
  uint32_t footer = 0xCAFEFADE;
  
  float one = ReadSensor2(NITROGEN_LINE_PT_PIN);
  float two = ReadSensor2(ETHANOL_TANK_PT_PIN);
  float three = ReadSensor2(NITROUS_LINE_PT_PIN);
  float four = ReadSensor(OXYGEN_LINE_PT_PIN);
  float five = ReadSensor2(FUEL_INLET_PT_PIN);
  float six = ReadSensor2(FUEL_OUTLET_PT_PIN);
  float seven = ReadSensor2(CHAMBER_PRESSURE_PT_PIN);
  float eight = ReadLoadCell(LC_PIN);

  struct data mydata = {header, one, two, three, four, five, six, seven, eight, footer};
  
  uint8_t * packet = (uint8_t *) &mydata;
  
  // Convert string to binary data
  uint8_t dataSize = sizeof(data);

  // Send packet over serial
  Serial.write((char *)packet, dataSize);
  //Serial.flush();
}

void CheckForCommand() {
  if (Serial.available() > 0) {
    // Read the incoming byte
    String input = Serial.readStringUntil('\n');

    if (input == "VALVE: nitrogen open") {
      NitrogenServo.write(OPEN_ANGLE); // 91 degrees = OPEN
      //String msg = "Nitrogen open!";
      //SendString(msg);
    } else if (input == "VALVE: nitrogen close") {
      NitrogenServo.write(CLOSED_ANGLE); // 179 degrees = CLOSED
      //String msg = "Nitrogen close!";
      //SendString(msg);
    }

    else if (input == "VALVE: purge open") {
      PurgeServo.write(OPEN_ANGLE); // 91 degrees = OPEN
      //String msg = "Purge open!";
      //SendString(msg);
    } else if (input == "VALVE: purge close") {
      PurgeServo.write(CLOSED_ANGLE); // 179 degrees = CLOSED
      //String msg = "Purge close!";
      //SendString(msg);
    }

    else if (input == "VALVE: main ethanol open") {
      MainEthanolServo.write(OPEN_ANGLE); // 91 degrees = OPEN
      //String msg = "Main ethanol open!";
      //SendString(msg);
    } else if (input == "VALVE: main ethanol close") {
      MainEthanolServo.write(CLOSED_ANGLE); // 179 degrees = CLOSED
      //String msg = "Main ethanol close!";
      //SendString(msg);
    }

    else if (input == "VALVE: main nitrous open") {
      MainNitrousServo.write(OPEN_ANGLE); // 91 degrees = OPEN
      //String msg = "Main nitrous open!";
      //SendString(msg);
    } else if (input == "VALVE: main nitrous close") {
      MainNitrousServo.write(CLOSED_ANGLE); // 179 degrees = CLOSED
      //String msg = "Main nitrous close!";
      //SendString(msg);
    }

    else if (input == "VALVE: ASI ethanol open") {
      digitalWrite(asiEthanolPin, LOW); // want to flip thisssssssssssssssssssssssssssssssssssssssssssssssssssssssss
      //String msg = "ASI ethanol open!";
      //SendString(msg);
    } else if (input == "VALVE: ASI ethanol close") {
      digitalWrite(asiEthanolPin, HIGH);
      //String msg = "ASI ethanol close!";
      //SendString(msg);
    }

    else if (input == "VALVE: ASI oxygen open") {
      digitalWrite(asiOxygenPin, LOW);
      //String msg = "ASI oxygen open!";
      //SendString(msg);
    } else if (input == "VALVE: ASI oxygen close") {
      digitalWrite(asiOxygenPin, HIGH);
      //String msg = "ASI oxygen close!";
      //SendString(msg);
    }

    else if (input == "VALVE: nitrogen bleed open") {
      digitalWrite(nitrogenBleedPin, HIGH); // normally CLOSED valve
      //String msg = "Nitrogen bleed open!";
      //SendString(msg);
    } else if (input == "VALVE: nitrogen bleed close") {
      digitalWrite(nitrogenBleedPin, LOW);
      //String msg = "Nitrogen bleed close!";
      //SendString(msg);
    }

    else if (input == "SPARK: on") {
      digitalWrite(sparkPin, LOW);
      analogWrite(rpmPin, 5); // 2% duty cycle PWM wave
      //String msg = "Spark on!";
      //SendString(msg);
    } else if (input == "SPARK: off") {
      digitalWrite(sparkPin, HIGH);
      analogWrite(rpmPin, 0);
      //String msg = "Spark off!";
      //SendString(msg);
    }


    // HOTFIRE SEQUENCES:

    if (input == "asitest") {
      asiTest();
    }
    else if (input == "waterflow") {
      waterFlow();
    }
    else if (input == "3second") {
      threeSecondHotFire();
    }

  }
}

void SendString(String strData) {
  // Example data
  // String strData = "Hello from the arduino";
  
  // Convert string to binary data
  uint8_t dataSize = strData.length();
  uint8_t dataBytes[dataSize];
  strData.getBytes(dataBytes, dataSize + 1); // +1 to include the null terminator
  
  // Define header and footer
  // Data header 0xDEADBEEF
  // String header 0xBABAFACE
  uint32_t header = 0xBABAFACE;
  uint32_t footer = 0xCAFEFADE;
  uint8_t * ending_ptr = (uint8_t *) &footer;
  uint8_t * header_ptr = (uint8_t *) &header;
  
  // Calculate total packet size
  uint8_t totalSize = 4 + 1 + dataSize + 4; // header + length + data + footer
  
  // Create buffer for packet
  uint8_t packetBuffer[totalSize];

  unsigned int packet_index = 0;
  //add header to packet
  for(unsigned int i = 0; i < sizeof(uint32_t); i++)
  {
      packetBuffer[packet_index] = header_ptr[i];
      packet_index++;
  }
  //add size byte to packet
  packetBuffer[packet_index] = dataSize;
  packet_index++;
  //add string to packet
  for(unsigned int i = 0; i < dataSize; i++)
  {
      packetBuffer[packet_index] = dataBytes[i];
      packet_index++;
  }
  //add footer to packet
  for(unsigned int i = 0; i < sizeof(uint32_t); i++)
  {
      packetBuffer[packet_index] = ending_ptr[i];
      packet_index++;
  }
  
  // Send packet over serial
  Serial.write(packetBuffer, totalSize);
}

void delayAndSendData(unsigned long delay) {
  timer = millis(); // update the timer

  while (millis() - timer < delay) {
    SendData();
  }
}

// HOTFIRE SEQUENCES:

// ASI TEST SEQUENCE
void asiTest() {
  digitalWrite(asiOxygenPin, LOW);
  digitalWrite(sparkPin, LOW);
  analogWrite(rpmPin, 5);
  delayAndSendData(300);
  digitalWrite(asiEthanolPin, LOW);
  delayAndSendData(2000);
  digitalWrite(asiEthanolPin, HIGH);
  digitalWrite(sparkPin, HIGH);
  analogWrite(rpmPin, 0);
  delayAndSendData(300);
  digitalWrite(asiOxygenPin, HIGH);
}

// PINTLE WATER FLOW SEQUENCE
void waterFlow() {
  MainNitrousServo.write(OPEN_ANGLE);
  delayAndSendData(1000);
  MainEthanolServo.write(OPEN_ANGLE);
  delayAndSendData(2000);
  MainNitrousServo.write(CLOSED_ANGLE);
  MainEthanolServo.write(CLOSED_ANGLE);
}

// 3 SECOND HOTFIRE SEQUENCE
void threeSecondHotFire() {
  // Pre Purge
  PurgeServo.write(OPEN_ANGLE);
  delayAndSendData(1000); // purge 1000 ms
  PurgeServo.write(CLOSED_ANGLE);

  // ASI Start
  digitalWrite(asiOxygenPin, LOW); // asi gox open
  digitalWrite(sparkPin, LOW); // spark on
  analogWrite(rpmPin, 5); // spark on
  delayAndSendData(300); // wait 300 ms
  digitalWrite(asiEthanolPin, LOW); // asi ethanol open -- STAYS OPEN WHOLE FIRE DURATION
  delayAndSendData(500); // 500 ms of ASI running

  // Main Valves
  MainNitrousServo.write(OPEN_ANGLE); // main nitrous open
  delayAndSendData(200); // wait 200 ms
  MainEthanolServo.write(OPEN_ANGLE); // main ethanol open

  delayAndSendData(1300); // 1300 ms of ASI running WITH main valves open

  // ASI Shutoff - asi ethanol stays OPEN
  digitalWrite(sparkPin, HIGH);
  analogWrite(rpmPin, 0);
  digitalWrite(asiOxygenPin, HIGH);

  // Main burn w/ ASI OFF
  delayAndSendData(1200); // 1200 ms of main valves open

  // SHUTDOWN
  digitalWrite(asiEthanolPin, HIGH); // asi ethanol CLOSE
  MainEthanolServo.write(CLOSED_ANGLE); // main ethanol CLOSE
  delayAndSendData(200); // wait 200 ms
  MainNitrousServo.write(CLOSED_ANGLE); // main nitrous CLOSE

  // Post Purge
  delayAndSendData(500); // wait 500 ms BEFORE starting the purge
  PurgeServo.write(OPEN_ANGLE);
  delayAndSendData(1500); // purge 1500 ms
  PurgeServo.write(CLOSED_ANGLE);
}
