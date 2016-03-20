#include <Adafruit_NeoPixel.h>

#define DEBUG

#define NUM_NODES (10)
#define NUM_PIXELS (60)
#define NUM_POSES (7)
#define PIN (2)
#define THRESHOLD (5)

static const int THRESHOLDS[NUM_NODES] = { 13, 8, 18, 14, 21, 25, 12, 18, 44, 44 };

static int values[NUM_NODES];
static bool poses[NUM_POSES][NUM_NODES] = {
	{
		1, 1,
		1, 1,
		1, 1,
		1, 1,
		1, 1,
	},
	{
		0, 0,
		0, 0,
		0, 0,
		0, 1,
		0, 0,
	},
	{
		0, 0,
		1, 1,
		0, 0,
		1, 1,
		0, 0,
	},
	{
		1, 1,
		0, 0,
		0, 0,
		0, 0,
		1, 1,
	},
	{
		1, 1,
		0, 0,
		0, 0,
		0, 0,
		1, 1,
	},
	{
		0, 0,
		0, 0,
		0, 0,
		1, 1,
		0, 0,
	},
	{
		0, 0,
		0, 0,
		0, 0,
		1, 1,
		0, 0,
	},
};
static int currentPose;

Adafruit_NeoPixel pixels[NUM_NODES] = {
	Adafruit_NeoPixel(NUM_PIXELS, PIN + 0, NEO_RGBW + NEO_KHZ800),
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

void setup() {
	Serial.begin(9600);

	setupLEDs();
	clearLEDs();
}

void loop() {
	updateTargets();
	updateLEDs();
	delay(16);
}

void setupLEDs() {
	for (int i = 0; i < NUM_NODES; i++) {
		pinMode(A0 + i, INPUT);
		pixels[i].begin();
		pixels[i].setBrightness(48);
	}
}

void clearLEDs() {
	for (int i = 0; i < NUM_NODES; i++) {
		for (int j = 0; j < NUM_PIXELS; j++) {
			pixels[i].setPixelColor(j, pixels[i].Color(0, 0, 0));
		}
	}
}

void updateTargets() {
	while (Serial.available()) {
		int c = Serial.read();
		if (c == 2) {
			currentPose = (currentPose + 1) % NUM_POSES;
		} else if (c == 1) {
			if (--currentPose < 0) {
				currentPose = NUM_POSES - 1;
			}
		}
	}
}

void updateLEDs() {
	bool *targets = poses[currentPose];

	for (int i = 0; i < NUM_NODES; i++) {

#ifdef DEBUG
		analogRead(A0 + i);
		delay(10);
		values[i] = (analogRead(A0 + i) + (values[i] * 3)) >> 2;
		//values[i] = (analogRead(A0 + i) + (values[i] * 2)) >> 1;
		Serial.print(values[i]);
		Serial.print(" ");
#endif

		if (targets[i]) {

#ifndef DEBUG
			values[i] = (analogRead(A0 + i) + (values[i] * 7)) >> 3;
#endif

			if (values[i] < THRESHOLDS[i]) {
				for (int j = 0; j < NUM_PIXELS; j++) {
					pixels[i].setPixelColor(j, pixels[i].Color(255, 0, 0));
				}
			} else {
				for (int j = 0; j < NUM_PIXELS; j++) {
					pixels[i].setPixelColor(j, pixels[i].Color(0, 255, 0));
				}
			}
		} else {
			for (int j = 0; j < NUM_PIXELS; j++) {
					pixels[i].setPixelColor(j, pixels[i].Color(0, 0, 0));
			}
		}

		pixels[i].show();
	}

#ifdef DEBUG
	Serial.println();
#endif
}
