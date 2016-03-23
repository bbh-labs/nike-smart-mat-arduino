#include "PoseNode.hpp"

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

int PoseNode::type() const {
	return mType;
}

int PoseNode::active() const {
	return mActive;
}

int PoseNode::interval() const {
	return mInterval;
}
