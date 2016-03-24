#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>

//#define USE_MANUAL_CALIBRATION

#ifndef USE_MANUAL_CALIBRATION

#define THRESHOLD (3)

#endif

#include "PoseNode.hpp"

#define DEBUG

#define NUM_NODES (10)
#define NUM_PIXELS (60)
#define NUM_POSES (7)
#define BRIGHTNESS (32)
#define PIN (2)

#ifdef USE_MANUAL_CALIBRATION

#define MODE_DEFAULT (0)
#define MODE_CALIBRATE (1)
static int mode = MODE_DEFAULT;
static bool bInitialCalibrationDone;
static int minThresholds[NUM_NODES];
static int maxThresholds[NUM_NODES];

#else

static int thresholds[NUM_NODES];

#endif

static int poseIndex = 0;
static PoseNode poses[NUM_POSES][NUM_NODES] = {
	// Debug
	{
		PoseNode(1), PoseNode(1),
		PoseNode(1), PoseNode(1),
		PoseNode(1), PoseNode(1),
		PoseNode(1), PoseNode(1),
		PoseNode(1), PoseNode(1),
	},

	// Hinge
	{
		PoseNode(0), PoseNode(0),
		PoseNode(0), PoseNode(0),
		PoseNode(0), PoseNode(0),
		PoseNode(1), PoseNode(1, 1000),
		PoseNode(0), PoseNode(0),
	},

	// Lunge
	{
		PoseNode(0), PoseNode(0),
		PoseNode(1, { 1000, 2000 }, 3000), PoseNode(0, { 1000, 2000 }, 3000),
		PoseNode(0), PoseNode(0),
		PoseNode(0, { 1000, 2000 }, 3000), PoseNode(1, { 1000, 2000 }, 3000),
		PoseNode(0), PoseNode(0),
	},

	// Pull
	{
		PoseNode(1, 800), PoseNode(0, 800),
		PoseNode(0), PoseNode(0),
		PoseNode(0), PoseNode(0),
		PoseNode(0), PoseNode(0),
		PoseNode(1), PoseNode(1),
	},

	// Push
	{
		PoseNode(1), PoseNode(1),
		PoseNode(0), PoseNode(0),
		PoseNode(0), PoseNode(0),
		PoseNode(0), PoseNode(0),
		PoseNode(1), PoseNode(1),
	},

	// Squat
	{
		PoseNode(0), PoseNode(0),
		PoseNode(0), PoseNode(0),
		PoseNode(0), PoseNode(0),
		PoseNode(1), PoseNode(1),
		PoseNode(0), PoseNode(0),
	},

	// Rotate
	{
		PoseNode(0), PoseNode(0),
		PoseNode(0), PoseNode(0),
		PoseNode(0), PoseNode(0),
		PoseNode(1, 250), PoseNode(0, 250),
		PoseNode(0), PoseNode(0),
	},
};

static unsigned long time, previousTime;

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

#ifdef USE_MANUAL_CALIBRATION
	load();
#else
	calibrate();
#endif

	setupSensors();
	setupLEDs();
	clearLEDs();
}

void loop() {
	previousTime = time;
	time = millis();

	readCommand();

#ifdef USE_MANUAL_CALIBRATION
	switch (mode) {
	case MODE_DEFAULT:
		updateLEDs();
		break;
	case MODE_CALIBRATE:
		calibrate();
		break;
	}
#else
	updateLEDs();
#endif

	delay(16);
}

void setupSensors() {
	for (int i = 0; i < NUM_NODES; i++) {
		pinMode(A0 + i, INPUT);
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
		pixels[i].show();
	}
}

void readCommand() {
	while (Serial.available()) {
		int c = Serial.read();
		switch (c) {
		case 1:
			if (--poseIndex < 0) {
				poseIndex = NUM_POSES - 1;
			}
			break;
		case 2:
			poseIndex = (poseIndex + 1) % NUM_POSES;
			break;
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
			poseIndex = c - 9;
			clearLEDs();
			break;
		}

#ifdef USE_MANUAL_CALIBRATION
		switch (c) {
		case 3:
			mode = MODE_CALIBRATE;
			break;
		default:
			mode = MODE_DEFAULT;
			if (bInitialCalibrationDone) {
				bInitialCalibrationDone = false;
				save();
			}
			break;
		}
#endif
	}
}

void updateLEDs() {
	PoseNode *poseNodes = poses[poseIndex];

	for (int i = 0; i < NUM_NODES; i++) {

#ifdef DEBUG

		analogRead(A0 + i);
		delay(10);

		int pressure = analogRead(A0 + i);
		Serial.print(pressure);
		Serial.print(" ");

#endif

		poseNodes[i].update(time - previousTime);

		if (poseNodes[i].active()) {

#ifndef DEBUG

			pressure = analogRead(A0 + i);

#endif

#ifdef USE_MANUAL_CALIBRATION

			const float range = maxThresholds[i] - minThresholds[i];
			const float value = (pressure - minThresholds[i]) / range;
			if (value >= 0.5) {

#else

			if (pressure > thresholds[i] + THRESHOLD) {

#endif
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

void calibrate() {

#ifdef USE_MANUAL_CALIBRATION

	if (!bInitialCalibrationDone) {
		for (int i = 0; i < NUM_NODES; i++) {
			for (int j = 0; j < NUM_PIXELS; j++) {
				pixels[i].setPixelColor(j, pixels[i].Color(0, 0, 255));
			}
			pixels[i].show();
		}
		for (int i = 0; i < NUM_NODES; i++) {
			minThresholds[i] = 1024;
			maxThresholds[i] = 0;
		}
	}

	for (int i = 0; i < NUM_NODES; i++) {
		int value = analogRead(A0 + i);
		minThresholds[i] = value < minThresholds[i] ? value : minThresholds[i];
		maxThresholds[i] = value > maxThresholds[i] ? value : maxThresholds[i];
	}

	bInitialCalibrationDone = true;

#else

	for (int j = 0; j < 10; j++) {
		for (int i = 0; i < NUM_NODES; i++) {
			analogRead(A0 + i);
			delay(10);

			thresholds[i] = analogRead(A0 + i);
		}
	}

#endif

}

#ifdef USE_MANUAL_CALIBRATION

void save() {
	for (int i = 0; i < NUM_NODES; i++) {
		const size_t address = i * 4;
		EEPROM.write(address + 0, minThresholds[i]);
		EEPROM.write(address + 1, minThresholds[i] >> 8);
		EEPROM.write(address + 2, maxThresholds[i]);
		EEPROM.write(address + 3, maxThresholds[i] >> 8);
	}
}

void load() {
	for (int i = 0; i < NUM_NODES; i++) {
		const size_t address = i * 4;
		minThresholds[i] = EEPROM.read(address + 0) | (EEPROM.read(address + 1) << 8);
		maxThresholds[i] = EEPROM.read(address + 2) | (EEPROM.read(address + 3) << 8);

#ifdef DEBUG

		Serial.print(minThresholds[i]);
		Serial.print(" ");
		Serial.print(maxThresholds[i]);
		Serial.print(" ");
#endif

	}

#ifdef DEBUG

	Serial.println();

#endif

}

#endif
