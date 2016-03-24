#ifndef POSE_NODE_HPP
#define POSE_NODE_HPP

#include <StandardCplusplus.h>
#include <vector>

class PoseNode {
public:
	PoseNode();
	PoseNode(bool active);
	PoseNode(bool active, int interval);
	PoseNode(bool active, std::vector<int> timings, int duration);

	void update(unsigned long delta);

	int type() const;
	int active() const;
	int interval() const;

private:
	int              mType;
	bool             mInitialActiveState;
	bool             mActive;
	int              mInterval;
	std::vector<int> mTimings;
	unsigned long    mTime;
	unsigned long    mDuration;
	int              mTimingIndex;
};

#endif
