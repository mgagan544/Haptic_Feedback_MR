#include <SoftwareSerial.h>

// ---------- Serial link to ESP ----------
SoftwareSerial espSerial(2, 3);  // RX, TX

// ---------- Solenoid control ----------
const int MOSFET_PIN = 5;

// ---------- Lub-Dub timing (ms) ----------
int LUB_MS  = 150;
int GAP_MS  = 100;
int DUB_MS  = 120;
int REST_MS = 600;

// ---------- Intensities ----------
int LUB_PWM = 255;
int DUB_PWM = 200;

// ---------- State ----------
bool isEnabled = false;     // controlled via Unity/ESP
uint8_t phase = 0;
unsigned long phaseStart = 0;

void setup() {
  pinMode(MOSFET_PIN, OUTPUT);
  analogWrite(MOSFET_PIN, 0);
  Serial.begin(9600);
  espSerial.begin(9600);
  
  // Add small delay for serial stability
  delay(100);
  
  Serial.println(F("[ARD] Ready. Commands: S=Start, X=Stop, B=BPM"));
}

void loop() {
  // --- Commands from ESP ---
  if (espSerial.available()) {
    String cmd = espSerial.readStringUntil('\n');
    cmd.trim();
    if (cmd.length()) handleCommand(cmd);
  }
  
  // --- Manual debug ---
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    if (cmd.length()) handleCommand(cmd);
  }
  
  // --- Lub-dub pattern ---
  unsigned long now = millis();
  switch (phase) {
    case 0:  // LUB start
      if (isEnabled) analogWrite(MOSFET_PIN, LUB_PWM);
      phaseStart = now;
      phase = 1;
      break;
    case 1:  // LUB on
      if (now - phaseStart >= (unsigned long)LUB_MS) {
        analogWrite(MOSFET_PIN, 0);
        phaseStart = now;
        phase = 2;
      }
      break;
    case 2:  // gap
      if (now - phaseStart >= (unsigned long)GAP_MS) {
        if (isEnabled) analogWrite(MOSFET_PIN, DUB_PWM);
        phaseStart = now;
        phase = 3;
      }
      break;
    case 3:  // DUB on
      if (now - phaseStart >= (unsigned long)DUB_MS) {
        analogWrite(MOSFET_PIN, 0);
        phaseStart = now;
        phase = 4;
      }
      break;
    case 4:  // rest
      if (now - phaseStart >= (unsigned long)REST_MS) {
        phase = 0;
      }
      break;
  }
}

// ------------------- Command Handler -------------------
void handleCommand(String cmd) {
  cmd.trim();
  cmd.toUpperCase();
  
  Serial.print(F("[ARD] CMD: "));
  Serial.println(cmd);
  
  // Single character commands are much more reliable
  if (cmd == "S" || cmd == "1") {
    startPulse();
    return;
  }
  
  if (cmd == "X" || cmd == "0") {
    stopPulse();
    return;
  }
  
  // BPM command format: B120 or BPM:120
  if (cmd.startsWith("B")) {
    int bpm;
    if (cmd.startsWith("BPM:")) {
      bpm = cmd.substring(4).toInt();
    } else {
      bpm = cmd.substring(1).toInt();
    }
    setBPM(bpm);
    return;
  }
  
  // Backward compatibility with longer commands
  if (cmd == "ENABLE" || cmd.startsWith("ENABL")) {
    startPulse();
    return;
  }
  
  if (cmd == "DISABLE" || cmd.startsWith("DISABL")) {
    stopPulse();
    return;
  }
  
  Serial.print(F("[ARD] Unknown command: '"));
  Serial.print(cmd);
  Serial.println(F("'"));
}

// ------------------- Pulse Control -------------------
void startPulse() {
  isEnabled = true;
  phase = 0;                  // reset cycle
  phaseStart = millis();
  Serial.println(F("[ARD] Pulse ENABLED."));
}

void stopPulse() {
  isEnabled = false;
  analogWrite(MOSFET_PIN, 0); // ensure solenoid OFF
  Serial.println(F("[ARD] Pulse DISABLED."));
}

// ------------------- BPM Control -------------------
void setBPM(int bpm) {
  if (bpm < 30) bpm = 30;
  if (bpm > 180) bpm = 180;
  
  float period = 60000.0f / bpm;
  float used   = (float)LUB_MS + GAP_MS + DUB_MS;
  REST_MS      = (period > used) ? (int)(period - used) : 0;
  
  Serial.print(F("[ARD] BPM=")); 
  Serial.println(bpm);
}