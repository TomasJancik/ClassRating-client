#include "LedControl.h"

// nastavení propojovacích pinů
#define DIN 10
#define CS  11
#define CLK 12

#define btnHappy  2
#define btnMeh    4
#define btnBad    6

// tato proměnná údává počet matic,
// počítání začíná od nuly
#define mojeMat 0

// inicializace LED matice z knihovny
LedControl ledMat=LedControl(DIN, CLK, CS, mojeMat);

byte smileHappy[8]={ B00000000,
                   B01100110,
                   B01100110,
                   B00000000,
                   B00000000,
                   B11000011,
                   B01100110,
                   B00011000};

byte smileMeh[8]={ B00000000,
                   B01100110,
                   B01100110,
                   B00000000,
                   B00000000,
                   B00000000,
                   B11111111,
                   B00000000};

byte smileBad[8]={ B00000000,
                   B01100110,
                   B01100110,
                   B00000000,
                   B00000000,
                   B00011000,
                   B01100110,
                   B11000011};

int lastBtn = 0;
float total = 0.0;
float btnPressed = 0.0;
float result = 2.0;
bool refresh = true;
int lastMicros;
int debounceDelay = 500;

bool debounce() {
  return (millis() - lastMicros) > debounceDelay;
}

void blink(byte smile[8], int speed) {
  for(int j=0; j<3; j++) {
    for(int i=0; i<8; i++) {
      ledMat.setRow(mojeMat,i,smile[i]);
    }
    delay(speed);
    ledMat.clearDisplay(mojeMat);
    delay(speed);
  }
}

void setup() {
  // probuzení matice pro zahájení komunikace
  ledMat.shutdown(mojeMat,false);
  // nastavení LED matice na střední svítivost (0-15)
  ledMat.setIntensity(mojeMat,7);
  // vypnutí všech LED diod na matici
  ledMat.clearDisplay(mojeMat);

  pinMode(btnHappy, INPUT);
  pinMode(btnMeh, INPUT);
  pinMode(btnBad, INPUT);

  Serial.begin(9600);
}

void loop() {
  if(digitalRead(btnHappy) == HIGH && debounce()) {
    Serial.println("Pressed happy button");
    lastBtn = btnHappy;
    total += 1;
    btnPressed +=1;
    refresh = true;
    blink(smileHappy, 100);
  } else if(digitalRead(btnMeh) == HIGH && debounce()) {
    Serial.println("Pressed meh button");
    lastBtn = btnMeh;
    total += 2;
    btnPressed +=1;
    refresh = true;
    blink(smileMeh, 100);
  } else if(digitalRead(btnBad) == HIGH && debounce()) {
    Serial.println("Pressed bad button");
    lastBtn = btnBad;
    total += 3;
    btnPressed +=1;
    refresh = true;
    blink(smileBad, 100);
  }
 
  if(refresh) {
    lastMicros = millis();
    result = round(total / btnPressed);
    Serial.println("Total:");
    Serial.println(total);
    Serial.println("Btn counmt:");
    Serial.println(btnPressed);
    Serial.println("Result:");
    Serial.println(total / btnPressed);
    Serial.println(result);
    ledMat.clearDisplay(mojeMat);

    if(result == 1) {
      for(int i=0; i<8; i++) {
        ledMat.setRow(mojeMat,i,smileHappy[i]);
      }
    } else if (result == 2) {
      for(int i=0; i<8; i++) {
        ledMat.setRow(mojeMat,i,smileMeh[i]);
      }
    } else if (result == 3) {
      for(int i=0; i<8; i++) {
        ledMat.setRow(mojeMat,i,smileBad[i]);
      }
    }

    refresh = false;
  }
}
