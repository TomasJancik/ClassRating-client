#include <WiFi.h>
#include <HTTPClient.h>

#include "LedControl.h"

const char* ssid = ""; // set WiFi network SSID
const char* password =  ""; // set Wifi password

bool sent = false;

// nastavení propojovacích pinů
#define DIN 13
#define CS  12
#define CLK 14

#define btnHappy  32
#define btnMeh    35
#define btnBad    34

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
  // setup LED matrix
  ledMat.shutdown(mojeMat,false);
  ledMat.setIntensity(mojeMat,7);
  ledMat.clearDisplay(mojeMat);

  // set button pins
  pinMode(btnHappy, INPUT);
  pinMode(btnMeh, INPUT);
  pinMode(btnBad, INPUT);

  Serial.begin(9600);

  delay(4000);   //Delay needed before calling the WiFi.begin

  Serial.println("Connecting to WiFi..");
  WiFi.begin(ssid, password); 
 
  while (WiFi.status() != WL_CONNECTED) { //Check for the connection
    delay(100);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Connected to the WiFi network");
}

void sendRequest(String rating) {
    HTTPClient http;
    String url = "http://example.com/?rating=" + rating;
    Serial.println(rating);
    Serial.println(url);
    http.begin(url); 
    http.addHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/77.0.3865.120 Safari/537.36"); //Specify content-type header
    http.addHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3");
    http.addHeader("Accept-Language", "en-US,en;q=0.9,cs;q=0.8");
    http.addHeader("Cache-Control", "max-age=0");
    http.addHeader("Connection", "keep-alive");
    http.addHeader("DNT", "1");
    http.addHeader("Host", "emaple.com");
    int httpResponseCode = http.GET(); //Send the actual POST request
    if(httpResponseCode>0){
     
        String response = http.getString();  //Get the response to the request
     
        Serial.println(httpResponseCode);   //Print return code
        Serial.println(response);           //Print request answer
     
    }else{
     
        Serial.print("Error on sending GET: ");
        Serial.println(httpResponseCode);
     
    }
    
    http.end();
}

void loop() {
  if(digitalRead(btnHappy) == HIGH && debounce()) {
    Serial.println("Pressed happy button");
    sendRequest("1");
    lastBtn = btnHappy;
    total += 1;
    btnPressed +=1;
    refresh = true;
    blink(smileHappy, 100);
  } else if(digitalRead(btnMeh) == HIGH && debounce()) {
    Serial.println("Pressed meh button");
    sendRequest("2");
    lastBtn = btnMeh;
    total += 2;
    btnPressed +=1;
    refresh = true;
    blink(smileMeh, 100);
  } else if(digitalRead(btnBad) == HIGH && debounce()) {
    Serial.println("Pressed bad button");
    sendRequest("3");
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
