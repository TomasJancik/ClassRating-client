// @TODO need to change LED matrix pins to work with ESP
// uncomment WiFi communication code when ready


//#include <WiFi.h>
//#include <HTTPClient.h>

#include "LedControl.h"

const char* ssid = "";
const char* password =  "";

bool sent = false;

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

//  delay(4000);   //Delay needed before calling the WiFi.begin
//
//  Serial.println("Connecting to WiFi..");
//  WiFi.begin(ssid, password); 
// 
//  while (WiFi.status() != WL_CONNECTED) { //Check for the connection
//    delay(100);
//    Serial.print(".");
//  }
//
//  Serial.println("");
//  Serial.println("Connected to the WiFi network");
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


//    if(!sent) {
//    HTTPClient http;
//    http.begin("http://hwi.tomasjancik.net/"); 
//    http.addHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/77.0.3865.120 Safari/537.36"); //Specify content-type header
//    http.addHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3");
//    http.addHeader("Accept-Language", "en-US,en;q=0.9,cs;q=0.8");
//    http.addHeader("Cache-Control", "max-age=0");
//    http.addHeader("Connection", "keep-alive");
//    http.addHeader("DNT", "1");
//    http.addHeader("Host", "hwi.tomasjancik.net");
//    int httpResponseCode = http.GET(); //Send the actual POST request
//    sent = true;
//    if(httpResponseCode>0){
//     
//        String response = http.getString();  //Get the response to the request
//     
//        Serial.println(httpResponseCode);   //Print return code
//        Serial.println(response);           //Print request answer
//     
//    }else{
//     
//        Serial.print("Error on sending GET: ");
//        Serial.println(httpResponseCode);
//     
//    }
//    
//    http.end();
  }
}
