#include "PoseNode.hpp"

#include <Arduino.h>

PoseNode::PoseNode() :
	mType(MODE_DEFAULT),
	mActive(false),
	mInterval(0)
{
}

PoseNode::PoseNode(bool active) :
	mType(MODE_DEFAULT),
	mActive(active),
	mInterval(0)
{
}

PoseNode::PoseNode(bool active, int interval) :
	mType(MODE_BLINKING),
	mActive(active),
	mInterval(interval)
{
}

void PoseNode::update() {
	unsigned long t = millis() % (mInterval * 2);
	if (t > mInterval) {
		mActive = true;
	} else {
		mActive = false;
	}
}

int PoseNode::type() const {
	return mType;
}

int PoseNode::active() const {
	return mActive;
}

int PoseNode::interval() const {
	return mInterval;
}
