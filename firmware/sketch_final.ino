#include <SoftwareSerial.h>

const int ENA = 5;
const int IN1 = 4;
const int IN2 = 3;

const int ENB = 6;
const int IN3 = 7;
const int IN4 = 8;

SoftwareSerial bt(10, 11);

int leftSpeed  = 0;
int rightSpeed = 0;

int leftDir  = 0;
int rightDir = 0;

char currentChannel = 0;
int  currentValue   = 0;
bool readingNumber  = false;

void applyMotorState() {
  int spL = constrain(leftSpeed, 0, 255);
  if (leftDir == 0 || spL == 0) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, 0);
  } else if (leftDir > 0) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, spL);
  } else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    analogWrite(ENA, spL);
  }

  int spR = constrain(rightSpeed, 0, 255);
  if (rightDir == 0 || spR == 0) {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    analogWrite(ENB, 0);
  } else if (rightDir > 0) {
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(ENB, spR);
  } else {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    analogWrite(ENB, spR);
  }
}

void stopAll() {
  leftSpeed = rightSpeed = 0;
  leftDir   = rightDir   = 0;
  applyMotorState();
}

void applySpeed(char channel, int value) {
  value = constrain(value, 0, 255);

  if (channel == 'A') {
    leftSpeed = value;
  } else if (channel == 'B') {
    rightSpeed = value;
  }

  Serial.print("Speed ");
  Serial.print(channel);
  Serial.print(" = ");
  Serial.println(value);

  applyMotorState();
}

void handleChar(char c) {
  Serial.print("Got: ");
  Serial.println(c);

  if (c == 'A' || c == 'B') {
    if (readingNumber && currentChannel != 0) {
      applySpeed(currentChannel, currentValue);
    }
    currentChannel = c;
    currentValue   = 0;
    readingNumber  = true;
    return;
  }

  if (c >= '0' && c <= '9' && readingNumber) {
    currentValue = currentValue * 10 + (c - '0');
    if (currentValue > 255) currentValue = 255;
    return;
  }

  if (readingNumber && currentChannel != 0) {
    applySpeed(currentChannel, currentValue);
    readingNumber  = false;
    currentChannel = 0;
    currentValue   = 0;
  }

  switch (c) {
    case 'L':
      leftDir = 1;
      break;

    case 'l':
      leftDir = -1;
      break;

    case 'R':
      rightDir = 1;
      break;

    case 'r':
      rightDir = -1;
      break;

    case 'S':
      stopAll();
      return;

    default:
      break;
  }

  applyMotorState();
}

void setup() {
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  stopAll();

  Serial.begin(9600);
  bt.begin(9600);

  Serial.println("Screw rover ready over BT!");
}

void loop() {
  while (bt.available()) {
    char c = bt.read();
    handleChar(c);
  }

  while (Serial.available()) {
    char c = Serial.read();
    handleChar(c);
  }
}
