#include <WiFi.h>
#include <HTTPClient.h>
#include "LedControl.h"

const char* ssid = ""; // set WiFi network SSID
const char* password =  ""; // set Wifi password

String mac;

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

#define displayTimeout 30

// inicializace LED matice z knihovny
LedControl ledMat = LedControl(DIN, CLK, CS, mojeMat);

byte smileHappy[8] = { B00100000,
                       B01100110,
                       B01000110,
                       B10000000,
                       B10000000,
                       B01000110,
                       B01100110,
                       B00100000
                     };

byte smileMeh[8] = { B01000000,
                     B01000110,
                     B01000110,
                     B01000000,
                     B01000000,
                     B01000110,
                     B01000110,
                     B01000000
                   };

byte smileBad[8] = { B10000000,
                     B11000110,
                     B01000110,
                     B00100000,
                     B00100000,
                     B01000110,
                     B11000110,
                     B10000000
                   };

int lastBtn = 0;
float total = 0.0;
float btnPressed = 0.0;
float result = 2.0;
bool refresh = true;
int lastMicros;
int debounceDelay = 500;

// variable just for loading animation
int loadRow = 0;
int loadCol = 0;

bool debounce() {
  return (millis() - lastMicros) > debounceDelay;
}

bool displayOff() {
  if ((millis() - lastMicros) > (displayTimeout * 1000)) {
    ledMat.clearDisplay(mojeMat);
  }
}

void display(byte smile[8]) {
  for (int i = 0; i < 8; i++) {
    ledMat.setRow(mojeMat, i, smile[i]);
  }
}

void blink(byte smile[8], int speed) {
  for (int j = 0; j < 3; j++) {
    display(smile);
    delay(speed);
    ledMat.clearDisplay(mojeMat);
    delay(speed);
  }
}

void animateLoading() {
  if (loadRow > 7) {
    loadRow = 0;
    loadCol = 0;
    ledMat.clearDisplay(mojeMat);
  }

  ledMat.setLed(mojeMat, loadRow, loadCol, true);
  loadCol++;

  if (loadCol > 7) {
    loadCol = 0;
    loadRow++;
  }
}

void finishAnimateLoading() {
  while (loadRow < 8) {
    animateLoading();
    delay(10);
  }

  ledMat.clearDisplay(mojeMat);
}

void setup() {
  // setup LED matrix
  ledMat.shutdown(mojeMat, false);
  ledMat.setIntensity(mojeMat, 7);
  ledMat.clearDisplay(mojeMat);
  animateLoading();

  // set button pins
  pinMode(btnHappy, INPUT);
  pinMode(btnMeh, INPUT);
  pinMode(btnBad, INPUT);

  animateLoading();

  Serial.begin(9600);

  //Delay needed before calling the WiFi.begin
  int wait = 0;
  while (wait < 4000) {
    delay(500);
    wait = wait + 500;
    animateLoading();
  }

  Serial.println("Connecting to WiFi..");
  WiFi.begin(ssid, password);

  animateLoading();

  while (WiFi.status() != WL_CONNECTED && millis() < 60000) { //Check for the connection. Max 60seconds
    delay(100);
    Serial.print(".");
    animateLoading();
  }

  mac = WiFi.macAddress();
  animateLoading();

  Serial.println("Connected to the WiFi network");
  animateLoading();

  finishAnimateLoading();
}

void sendRequest(String rating) {
  if(WiFi.status() != WL_CONNECTED) {
    return; // do not send if not connection
  }
  
  HTTPClient http;
  String url = "http://hwi.tomasjancik.net//?rating=" + rating + "&macAddr=" + mac;
  Serial.println(rating);
  Serial.println(url);
  http.begin(url);
  http.addHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/77.0.3865.120 Safari/537.36"); //Specify content-type header
  http.addHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3");
  http.addHeader("Accept-Language", "en-US,en;q=0.9,cs;q=0.8");
  http.addHeader("Cache-Control", "max-age=0");
  http.addHeader("Connection", "keep-alive");
  http.addHeader("DNT", "1");
  http.addHeader("Host", "http://hwi.tomasjancik.net/");
  int httpResponseCode = http.GET(); //Send the actual POST request
  if (httpResponseCode > 0) {

    String response = http.getString();  //Get the response to the request

    Serial.println(httpResponseCode);   //Print return code
    Serial.println(response);           //Print request answer

  } else {

    Serial.print("Error on sending GET: ");
    Serial.println(httpResponseCode);

  }

  http.end();
}

void loop() {
  if (digitalRead(btnHappy) == HIGH && debounce()) {
    Serial.println("Pressed happy button");
    blink(smileHappy, 100);
    display(smileHappy);
    sendRequest("1");
    lastBtn = btnHappy;
    total += 1;
    btnPressed += 1;
    refresh = true;
  } else if (digitalRead(btnMeh) == HIGH && debounce()) {
    blink(smileMeh, 100);
    display(smileMeh);
    Serial.println("Pressed meh button");
    sendRequest("2");
    lastBtn = btnMeh;
    total += 2;
    btnPressed += 1;
    refresh = true;
  } else if (digitalRead(btnBad) == HIGH && debounce()) {
    Serial.println("Pressed bad button");
    blink(smileBad, 100);
    display(smileBad);
    sendRequest("3");
    lastBtn = btnBad;
    total += 3;
    btnPressed += 1;
    refresh = true;
  }

  if (refresh) {
    lastMicros = millis();

    result = round(total / btnPressed);
    Serial.println("Total:");
    Serial.println(total);
    Serial.println("Btn counmt:");
    Serial.println(btnPressed);
    Serial.println("Result:");
    Serial.println(total / btnPressed);
    Serial.println(result);
    //    ledMat.clearDisplay(mojeMat);

    //    if(result == 1) {
    //      for(int i=0; i<8; i++) {
    //        ledMat.setRow(mojeMat,i,smileHappy[i]);
    //      }
    //    } else if (result == 2) {
    //      for(int i=0; i<8; i++) {
    //        ledMat.setRow(mojeMat,i,smileMeh[i]);
    //      }
    //    } else if (result == 3) {
    //      for(int i=0; i<8; i++) {
    //        ledMat.setRow(mojeMat,i,smileBad[i]);
    //      }
    //    }
    refresh = false;
  }

  displayOff();
}
