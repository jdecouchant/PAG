#ifndef BUFFERMAP_H_
#define BUFFERMAP_H_

#include <vector>

using namespace std;

class Buffermap {

private:
	vector<int> updatesId;
	vector<int> receptionRounds;

	bool isPerempted(int roundId, int UpdateId);

public:
	Buffermap();

	int getBMSize();
	int getUpdatesId(int i);

	int isFromRound(int updateId);

	void insertUpdates(int roundId, vector<int> receivedUpdates);
	long actualize(int roundId);

	bool isInserted(int updateId);
	vector<int> getUpdatesUntil(int roundId, int nbRounds);
};

#endif /* BUFFERMAP_H_ */
