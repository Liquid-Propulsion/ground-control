#include <HX711.h>

const int nitrogenPin = 13;
const int oxygenPin = 10;
const int ethanolPin = 12;
const int bleedPin = 11;
const int sparkPin = 9;
const int rpmPin = 6;

const int N2_PT_Pin = A0;
const int O2_PT_Pin = A1;
const int Ign_PT_Pin = A2;

const int LC_Clock_Pin = 2; // Clock
const int LC_Out_Pin = 3; // Data

HX711 scale;

// RECOMMEND CALIBRATING THESE BEFORE A HOTFIRE
long offset = 1102767;
float calibration_factor = 358.024108;

unsigned long timer = millis(); // setting up the timer variable

// As defined rn, struct string is I4fI
struct data {
  uint32_t header;
  float val1;
  float val2;
  float val3;
  float val4;
  uint32_t footer;
};

void setup() {
  Serial.begin(9600);

  pinMode(nitrogenPin, OUTPUT);
  pinMode(oxygenPin, OUTPUT);
  pinMode(ethanolPin, OUTPUT);
  pinMode(bleedPin, OUTPUT);
  pinMode(sparkPin, OUTPUT);
  pinMode(rpmPin, OUTPUT);

  digitalWrite(nitrogenPin, HIGH);
  digitalWrite(oxygenPin, HIGH);
  digitalWrite(ethanolPin, HIGH);
  digitalWrite(bleedPin, LOW);
  digitalWrite(sparkPin, HIGH);

  analogWrite(rpmPin, 0);

  pinMode(N2_PT_Pin, INPUT);
  pinMode(O2_PT_Pin, INPUT);
  pinMode(Ign_PT_Pin, INPUT);

  scale.begin(LC_Out_Pin, LC_Clock_Pin);
  scale.set_offset(offset);
  scale.set_scale(calibration_factor);
}

void loop() {
  SendData();
  CheckForCommand();

  timer = millis(); // constantly update the timer while the main loop is running, but it will NOT update while one of the hotfire commands is running
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

float ReadLoadCell(int clock, int data) {
  return scale.get_units(); // returns GRAMS
}

void SendData() {
  uint32_t header = 0xDEADBEEF;
  uint32_t footer = 0xCAFEFADE;
  
  float n2_pt = ReadSensor(N2_PT_Pin);
  float o2_pt = ReadSensor(O2_PT_Pin);
  float ign_pt = ReadSensor(Ign_PT_Pin);
  float load_cell = ReadLoadCell(LC_Clock_Pin, LC_Out_Pin);

  struct data mydata = {header, n2_pt, o2_pt, ign_pt, load_cell, footer};
  
  uint8_t * packet = (uint8_t *) &mydata;
  
  // Convert string to binary data
  uint8_t dataSize = sizeof(data);

  // Send packet over serial
  Serial.write((char *)packet, dataSize);
}

void CheckForCommand() {
  if (Serial.available() > 0) {
    // Read the incoming byte
    String input = Serial.readStringUntil('\n');

    if (input == "VALVE: nitrogen open") {
      digitalWrite(nitrogenPin, LOW); // Set LED to HIGH
      // Serial.println("Nitrogen open!"); // Send a confirmation message over serial
      String msg = "Nitrogen open!";
      // SendString(msg);
    } else if (input == "VALVE: nitrogen close") {
      digitalWrite(nitrogenPin, HIGH);
      String msg = "Nitrogen close!";
      // SendString(msg);
    }
    else if (input == "VALVE: ethanol open") {
      digitalWrite(ethanolPin, LOW);
      String msg = "Ethanol open!";
      // SendString(msg);
    } else if (input == "VALVE: ethanol close") {
      digitalWrite(ethanolPin, HIGH);
      String msg = "Ethanol close!";
      // SendString(msg);
    }
    else if (input == "VALVE: bleed open") {
      digitalWrite(bleedPin, HIGH);
      String msg = "Bleed open!";
      // SendString(msg);
    } else if (input == "VALVE: bleed close") {
      digitalWrite(bleedPin, LOW);
      String msg = "Bleed close!";
      // SendString(msg);
    }
    else if (input == "VALVE: oxygen open") {
      digitalWrite(oxygenPin, LOW);
      String msg = "Oxygen open!";
      // SendString(msg);
    } else if (input == "VALVE: oxygen close") {
      digitalWrite(oxygenPin, HIGH);
      String msg = "Oxygen close!";
      // SendString(msg);
    }
    
    if (input == "s1") {
      digitalWrite(sparkPin, LOW);
      analogWrite(rpmPin, 5);
      String msg = "Spark on!";
      // SendString(msg);
    } else if (input == "s2") {
      digitalWrite(sparkPin, HIGH);
      analogWrite(rpmPin, 0);
      String msg = "Spark off!";
      // SendString(msg);
    }

    if (input == "dump prep") {
      DumpPrep();
    } else if (input == "hotfire 1") {
      HotFire1();
    } else if (input == "hotfire 2") {
      HotFire2();
    } else if (input == "depress") {
      Depress();
    } else if (input == "reset") {
      ResetAll();
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

void DumpPrep() {
  digitalWrite(bleedPin, LOW);
  digitalWrite(ethanolPin, HIGH);
  digitalWrite(nitrogenPin, HIGH);
  //delay(500);
  while((millis() - timer) < 500) {
    SendData();
  }
  digitalWrite(nitrogenPin, LOW);
}

void HotFire1() {
  digitalWrite(ethanolPin, HIGH);
  digitalWrite(oxygenPin, HIGH);
  digitalWrite(bleedPin, LOW);
  digitalWrite(nitrogenPin, LOW);
}

void HotFire2() {
  digitalWrite(oxygenPin, LOW);
  digitalWrite(sparkPin, LOW);
  analogWrite(rpmPin, 5);
  //delay(300);
  while((millis() - timer) < 300) {
    SendData();
  }
  digitalWrite(ethanolPin, LOW);
  //delay(2000);
  while((millis() - timer) < 2300) {
    SendData();
  }
  digitalWrite(ethanolPin, HIGH);
  //delay(300);
  while((millis() - timer) < 2600) {
    SendData();
  }
  digitalWrite(sparkPin, HIGH);
  analogWrite(rpmPin, 0);
  digitalWrite(oxygenPin, HIGH);
}

void Depress() {
  digitalWrite(nitrogenPin, HIGH);
  //delay(100);
  while((millis() - timer) < 100) {
    SendData();
  }
  digitalWrite(bleedPin, HIGH);
  digitalWrite(ethanolPin, LOW);
  //delay(1000);
  while((millis() - timer) < 1100) {
    SendData();
  }
  digitalWrite(ethanolPin, HIGH);
}

void ResetAll() {
  digitalWrite(nitrogenPin, HIGH);
  digitalWrite(ethanolPin, HIGH);
  digitalWrite(oxygenPin, HIGH);
  digitalWrite(bleedPin, HIGH);
}
