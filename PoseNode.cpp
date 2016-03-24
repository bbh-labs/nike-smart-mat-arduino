#include "PoseNode.hpp"

#include <Arduino.h>

#define MODE_DEFAULT (0)
#define MODE_BLINKING (1)
#define MODE_BLINKING_IRREGULAR (2)

PoseNode::PoseNode() :
	mType(MODE_DEFAULT),
	mInitialActiveState(false),
	mActive(false),
	mInterval(0)
{
}

PoseNode::PoseNode(bool active) :
	mType(MODE_DEFAULT),
	mInitialActiveState(active),
	mActive(active),
	mInterval(0)
{
}

PoseNode::PoseNode(bool active, int interval) :
	mType(MODE_BLINKING),
	mInitialActiveState(active),
	mActive(active),
	mInterval(interval)
{
}

PoseNode::PoseNode(bool active, std::vector<int> timings, int duration) :
	mType(MODE_BLINKING_IRREGULAR),
	mInitialActiveState(active),
	mActive(active),
	mTimings(timings),
	mDuration(duration),
	mTimingIndex(0)
{
}

void PoseNode::update(unsigned long delta) {
	unsigned long previousTime = mTime;

	mTime = (mTime + delta) % mInterval;

	switch (mType) {
	case MODE_BLINKING:
		if (previousTime > mTime) {
			mActive = !mActive;
		}
		break;
	case MODE_BLINKING_IRREGULAR:
		if (mTime >= mTimings[mTimingIndex]) {
			mActive = !mActive;
			mTimingIndex++;
		}
		if (mTime >= mDuration) {
			mTime = 0;
			mTimingIndex = 0;
			mActive = mInitialActiveState;
		}
		break;
	default:
		break;
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
