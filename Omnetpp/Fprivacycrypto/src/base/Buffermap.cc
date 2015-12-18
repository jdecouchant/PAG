#include "Buffermap.h"

#include "base.h"
#include "Source.h"
#include <cexception.h>

Buffermap::Buffermap() {}

int Buffermap::getBMSize() {
    return updatesId.size();
}

int Buffermap::getUpdatesId(int i) {
    return updatesId[i];
}

inline bool Buffermap::isPerempted(int roundId, int updateId) {
    return (updateId < (roundId - RTE) * Source::updNbPerRound);
}

int Buffermap::isFromRound(int updateId) {
    return updateId / Source::updNbPerRound;
}

void Buffermap::insertUpdates(int roundId, vector<int> receivedUpdates) {
    for (unsigned i = 0; i < receivedUpdates.size(); i++) {
        int updateId = receivedUpdates[i];
        // If update is valid, insert it in updatesId
        if (not isPerempted(roundId, updateId)) {
            // If the update was already received, change its reception round
            bool alreadyPresent = false;
            for (unsigned i = 0; i < updatesId.size(); ++i) {
                if (updatesId[i] == updateId) {
                    receptionRounds[i] = roundId;
                    alreadyPresent = true;
                    break;
                }
            }
            if (!alreadyPresent) {
                updatesId.push_back(updateId);
                receptionRounds.push_back(roundId);
            }
        }
    }
}

long Buffermap::actualize(int roundId) {
    long nbDeletedUpdates = 0;
    int bmIndex = (int) updatesId.size() - 1;
    while (bmIndex >= 0) {
        if (isPerempted(roundId, updatesId[bmIndex])) {
            ++nbDeletedUpdates;
            updatesId.erase(updatesId.begin() + bmIndex);
            receptionRounds.erase(receptionRounds.begin() + bmIndex);
        }
        --bmIndex;
    }
    return nbDeletedUpdates;
}

vector<int> Buffermap::getUpdatesUntil(int roundId, int nbRounds) {
    vector<int> res;

    for (unsigned i = 0; i < updatesId.size(); ++i) {
        if (receptionRounds[i] >= roundId - nbRounds && receptionRounds[i] < roundId)
            res.push_back(updatesId[i]);
    }
    return res;
}

bool Buffermap::isInserted(int updateId) {
    for (unsigned i = 0; i < updatesId.size(); ++i) {
        if (updatesId[i] == updateId) {
            return true;
        }
    }
    return false;
}




