#include <Adafruit_NeoPixel.h>

#include "PoseNode.hpp"

#define DEBUG

#define BRIGHTNESS (2)

#define NUM_NODES (10)
#define NUM_PIXELS (60)
#define NUM_POSES (7)
#define PIN (2)

#define THRESHOLD (5)
static int THRESHOLDS[NUM_NODES] = { 0 };

static int values[NUM_NODES];
static PoseNode poses[NUM_POSES][NUM_NODES] = {
	{
		PoseNode(1), PoseNode(1),
		PoseNode(1), PoseNode(1),
		PoseNode(1), PoseNode(1),
		PoseNode(1), PoseNode(1),
		PoseNode(1), PoseNode(1),
	},
	{
		PoseNode(0), PoseNode(0),
		PoseNode(0), PoseNode(0),
		PoseNode(0), PoseNode(0),
		PoseNode(0), PoseNode(1),
		PoseNode(0), PoseNode(0),
	},
	{
		PoseNode(0), PoseNode(0),
		PoseNode(1), PoseNode(1),
		PoseNode(0), PoseNode(0),
		PoseNode(1), PoseNode(1),
		PoseNode(0), PoseNode(0),
	},
	{
		PoseNode(1, 500), PoseNode(1, 500),
		PoseNode(0), PoseNode(0),
		PoseNode(0), PoseNode(0),
		PoseNode(0), PoseNode(0),
		PoseNode(1), PoseNode(1),
	},
	{
		PoseNode(1), PoseNode(1),
		PoseNode(0), PoseNode(0),
		PoseNode(0), PoseNode(0),
		PoseNode(0), PoseNode(0),
		PoseNode(1), PoseNode(1),
	},
	{
		PoseNode(0), PoseNode(0),
		PoseNode(0), PoseNode(0),
		PoseNode(0), PoseNode(0),
		PoseNode(1), PoseNode(1),
		PoseNode(0), PoseNode(0),
	},
	{
		PoseNode(0), PoseNode(0),
		PoseNode(0), PoseNode(0),
		PoseNode(0), PoseNode(0),
		PoseNode(1, 500), PoseNode(1, 500),
		PoseNode(0), PoseNode(0),
	},
};
static int poseIndex;

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

	calibrateSensors();
	setupLEDs();
	clearLEDs();
}

void loop() {
	updateTargets();
	updateLEDs();
	delay(16);
}

void calibrateSensors() {
	for (int j = 0; j < 10; j++) {
		for (int i = 0; i < NUM_NODES; i++) {
			pinMode(A0 + i, INPUT);
			THRESHOLDS[i] = analogRead(A0 + i) + THRESHOLD;
		}
		delay(10);
	}
}

void setupLEDs() {
	for (int i = 0; i < NUM_NODES; i++) {
		pixels[i].begin();
		pixels[i].setBrightness(BRIGHTNESS);
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
			poseIndex = (poseIndex + 1) % NUM_POSES;
		} else if (c == 1) {
			if (--poseIndex < 0) {
				poseIndex = NUM_POSES - 1;
			}
		}
	}
}

void updateLEDs() {
	PoseNode *poseNodes = poses[poseIndex];

	for (int i = 0; i < NUM_NODES; i++) {

#ifdef DEBUG
		values[i] = analogRead(A0 + i);
		Serial.print(values[i]);
		Serial.print(" ");
#endif

		if (poseNodes[i].active()) {

#ifndef DEBUG
			values[i] = analogRead(A0 + i);
#endif

			if (values[i] >= THRESHOLDS[i]) {
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
