#ifndef POSE_NODE_HPP
#define POSE_NODE_HPP

class PoseNode {
public:
	const int MODE_DEFAULT = 0;
	const int MODE_BLINKING = 0;

	PoseNode();
	PoseNode(bool active);
	PoseNode(bool active, int interval);

	void update();

	int type() const;
	int active() const;
	int interval() const;

private:
	int  mType;
	bool mActive;
	int  mInterval;
};

#endif
