#ifndef __NODE_H__
#define __NODE_H__

class Node
{
private:
	std::vector<Node*> mNeighbours;
	int mPosX;
	int mPosY;
	double mGScore;
	double mFScore;

public:
	Node(int nPosX, int nPosY)
		: mNeighbours(), mPosX(nPosX), mPosY(nPosY), mGScore(9999), mFScore(9999)
	{
	}

	std::vector<Node*>* neighbours()
	{
		return (&this->mNeighbours);
	}

	int x()
	{
		return (this->mPosX);
	}

	int y()
	{
		return (this->mPosY);
	}

	double fscore() const {
		return mFScore;
	}

	void fscore(double fScore) {
		mFScore = fScore;
	}

	double gscore() const {
		return mGScore;
	}

	void gscore(double gScore) {
		mGScore = gScore;
	}
};

#endif
