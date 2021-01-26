#include <Arduino.h>
#include <Bounce.h>
#include <FastLED.h>

#define LED_PIN     7 
#define SIGNAL_PIN 13
#define NUM_LEDS    80
#define CAP0_PIN 19
#define CAP1_PIN 20
#define CAP2_PIN 21
#define CAP3_PIN 22
#define CAP4_PIN 23
Bounce cap0 = Bounce(CAP0_PIN, 10);
Bounce cap1 = Bounce(CAP1_PIN, 10);
Bounce cap2 = Bounce(CAP2_PIN, 10);
Bounce cap3 = Bounce(CAP3_PIN, 10);
Bounce cap4 = Bounce(CAP4_PIN, 10);
Bounce caps[5] = {cap0, cap1, cap2, cap3, cap4};

/*
#define RED    0xFF0000
#define GREEN  0x00FF00
#define BLUE   0x0000FF
#define YELLOW 0xFFFF00
#define PINK   0xFF1088
#define ORANGE 0xE05800
#define WHITE  0xFFFFFF
*/

#define RED    0x160000
#define GREEN  0x001600
#define BLUE   0x000016
#define YELLOW 0x101400
#define PINK   0x120009
#define ORANGE 0x100400
#define WHITE  0x101010

#define NUM_BUTTONS 5
#define NUM_ROWS 5
#define LED_PER_ROW 16
uint16_t rowData[NUM_ROWS] = {0, 0, 0, 0, 0};

void setUp() {
  rowData[4] = 0b0010000000000100;
  rowData[3] = 0b0010000000000100;
  rowData[2] = 0b0001000000001000;
  rowData[1] = 0b0000100000010000;
  rowData[0] = 0b0000011111100000;
}

void setDown() {
  rowData[0] = 0b0010000000000100;
  rowData[1] = 0b0010000000000100;
  rowData[2] = 0b0001000000001000;
  rowData[3] = 0b0000100000010000;
  rowData[4] = 0b0000011111100000;
}

void setCenter() {
  rowData[0] = 0b0000011111100000;
  rowData[1] = 0b0000100000010000;
  rowData[2] = 0b0001000000001000;
  rowData[3] = 0b0000100000010000;
  rowData[4] = 0b0000011111100000;
}

void setRight() {
  rowData[4] = 0b0010001110010010; //0100100111000100;
  rowData[3] = 0b0100100010000100;
  rowData[2] = 0b0010000100011110; //0111100010000100;
  rowData[1] = 0b0100100010000000;
  rowData[0] = 0b0010001110010010; //0100100111000100;
}

void setLeft() {
  rowData[4] = 0b0000110000000010;
  rowData[3] = 0b0010000001001000; //0001001000000100;
  rowData[2] = 0b0010000100001000;
  rowData[1] = 0b0000100100000010; //0100100010010000;
  rowData[0] = 0b1000000001100000;
}

CRGB leds[NUM_LEDS];
CRGB BLACK = CRGB(0, 0, 0);
bool buttons[NUM_BUTTONS] = {false, false, false, false, false};
void (*buttonFunctions[NUM_BUTTONS])();
float currentHue = 0.0;
float currentHues[NUM_LEDS];

void setup() {
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(50);
  
  pinMode(SIGNAL_PIN, OUTPUT);
  pinMode(CAP0_PIN, INPUT);
  pinMode(CAP1_PIN, INPUT);
  pinMode(CAP2_PIN, INPUT);
  pinMode(CAP3_PIN, INPUT);
  pinMode(CAP4_PIN, INPUT);
  Serial.begin(9600);
  for (int i = 0; i < NUM_LEDS; i++) {
    currentHues[i] = random8();
  }
  buttonFunctions[0] = setDown; //setLeft;
  buttonFunctions[1] = setRight;
  buttonFunctions[2] = setCenter; //setUp;
  buttonFunctions[3] = setUp;
  buttonFunctions[4] = setLeft;
  setLeft();
}

void wipe(CRGB color) {
  for (int i = 0; i < NUM_ROWS; i++) {
    for (int j = 0; j < LED_PER_ROW; j++) {
      if (rowData[i] & (1 << j)) {
        leds[j + i * LED_PER_ROW] = color;
      } else leds[j + i * LED_PER_ROW] = CRGB(0, 0, 0);
    }
  }
  delay(100);
  FastLED.show();
}

void sparkle() {
  for (int i = 0; i < NUM_LEDS; i++) {
    currentHues[i] += 0.2;
    leds[i] = CHSV(int(currentHues[i]) % 255, 255, 255);
  }
  FastLED.show();
}

void loop() {
  bool pressed = false;
  for (int i = 0; i < NUM_BUTTONS; i++) {
    caps[i].update();
    if (caps[i].read()) {
    } else {
      Serial.println(i);
      (buttonFunctions[i])();
      wipe(CRGB(100, 0, 0));
      pressed = true;
    }
  }
  if (pressed) digitalWrite(SIGNAL_PIN, HIGH);
  else {
    sparkle();
    digitalWrite(SIGNAL_PIN, LOW);
  }
}

