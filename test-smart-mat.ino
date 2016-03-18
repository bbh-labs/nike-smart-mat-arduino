#include <Adafruit_NeoPixel.h>

#define NUM_NODES (10)
#define NUM_PIXELS (60)
#define PIN (2)
#define THRESHOLD (2)

Adafruit_NeoPixel pixels[NUM_NODES] = {
	Adafruit_NeoPixel(NUM_PIXELS, PIN, NEO_RGBW + NEO_KHZ800),
	Adafruit_NeoPixel(NUM_PIXELS, PIN + 1, NEO_RGBW + NEO_KHZ800),
	Adafruit_NeoPixel(NUM_PIXELS, PIN + 2, NEO_RGBW + NEO_KHZ800),
	Adafruit_NeoPixel(NUM_PIXELS, PIN + 3, NEO_RGBW + NEO_KHZ800),
	Adafruit_NeoPixel(NUM_PIXELS, PIN + 4, NEO_RGBW + NEO_KHZ800),
	Adafruit_NeoPixel(NUM_PIXELS, PIN + 5, NEO_RGBW + NEO_KHZ800),
	Adafruit_NeoPixel(NUM_PIXELS, PIN + 6, NEO_RGBW + NEO_KHZ800),
	Adafruit_NeoPixel(NUM_PIXELS, PIN + 7, NEO_RGBW + NEO_KHZ800),
	Adafruit_NeoPixel(NUM_PIXELS, PIN + 8, NEO_RGBW + NEO_KHZ800),
	Adafruit_NeoPixel(NUM_PIXELS, PIN + 9, NEO_RGBW + NEO_KHZ800),
};

static int values[NUM_NODES] = { 0, 0 };

void setup() {
	for (int i = 0; i < NUM_NODES; i++) {
		pinMode(A0 + i, INPUT);
		pixels[i].begin();
		pixels[i].setBrightness(32);
	}
	//Serial.begin(9600);
}

void loop() {
	for (int i = 0; i < NUM_NODES; i++) {
		values[i] = (analogRead(A0 + i) + (values[i] * 7)) >> 3;
		Serial.print(values[i]);
		Serial.print(" ");
		if (values[i] >= THRESHOLD) {
			for (int j = 0; j < NUM_PIXELS; j++) {
				pixels[i].setPixelColor(j, pixels[i].Color(255, 0, 0));
			}
		} else {
			for (int j = 0; j < NUM_PIXELS; j++) {
				pixels[i].setPixelColor(j, pixels[i].Color(0, 255, 0));
			}
		}
		pixels[i].show();
	}
	//Serial.println();
}
