#ifndef __peerreview_history_h__
#define __peerreview_history_h__

#include "peerreview/identity.h"

/* Policy given to a SecureHistory to tell it which event types to hash when
   serializing a sequence of events. For example, such a policy might tell
   the SecureHistory to hash all SEND and SENDSIGN events, plus all CHECKPOINT
   events except for the first one. */

class HashPolicy {
public:
  HashPolicy() {};
  virtual ~HashPolicy() {};
  virtual bool hashEntry(unsigned int type, unsigned char *content, int contentLen) = 0;
};

class SecureHistory {
public:
  virtual ~SecureHistory() {};  
  virtual void appendEntry(char type, bool storeFullEntry, const void *entry, int size, const void *header = NULL, int headerSize = 0) = 0;
  virtual void appendHash(char type, const unsigned char *hash) = 0;
  virtual void getTopLevelEntry(void *nodeHash, long long *seq) = 0;
  virtual long long getLastSeq() = 0;
  virtual bool setNextSeq(long long nextSeq) = 0;
  virtual int getNumEntries() = 0;
  virtual long long getBaseSeq() = 0;
  virtual bool statEntry(int idx, long long *seq, unsigned char *type, int *sizeBytes, unsigned char *contentHash, unsigned char *nodeHash) = 0;
  virtual int getEntry(int idx, unsigned char *buffer, int buflen) = 0;
  virtual bool serializeRange(int idxFrom, int idxTo, HashPolicy *hashPolicy, FILE *outfile) = 0;
  virtual int findLastEntry(unsigned char *types, int numTypes, long long maxSeq) = 0;
  virtual int findNextEntry(unsigned char *types, int numTypes, long long minSeq) = 0;
  virtual int findSeq(long long seq) = 0;
  virtual bool upgradeHashedEntry(int idx, const void *entry, int size) = 0;
  virtual void syncWithFileOnDisk() = 0;
};

class SecureHistoryFactory {
public:
  virtual SecureHistory *createTemp(long long baseSeq, const unsigned char *baseHash, HashProvider *hashprov) = 0;
  virtual SecureHistory *create(const char *name, long long baseSeq, const unsigned char *baseHash, HashProvider *hashprov) = 0;
  virtual SecureHistory *open(const char *name, const char *mode, HashProvider *hashprov) = 0;
};

/* The following class implements PeerReview's log. A log entry consists of
   a sequence number, a type, and a string of bytes. On disk, the log is
   stored as two files: An index file and a data file. */

class SimpleSecureHistory : public SecureHistory {
protected:
  static const int HASH_LENGTH = 20;
  
  friend class SimpleSecureHistoryFactory;

  struct indexEntry {
    long long seq;
    int fileIndex;
    int sizeInFile;
    int type;
    unsigned char contentHash[HASH_LENGTH];
    unsigned char nodeHash[HASH_LENGTH];
  } __attribute__ ((packed));

  HashProvider *hashprov;
  bool pointerAtEnd;
  indexEntry topEntry;
  long long baseSeq;
  long long nextSeq;
  int numEntries;
  FILE *indexFile;
  FILE *dataFile;
  bool readonly;
  
  int findSeqOrHigher(long long seq, bool allowHigher);
  
public:
  SimpleSecureHistory(FILE *indexFile, FILE *dataFile, bool readonly, HashProvider *hashprov);
  
  virtual ~SimpleSecureHistory();  
  virtual void appendEntry(char type, bool storeFullEntry, const void *entry, int size, const void *header = NULL, int headerSize = 0);
  virtual void appendHash(char type, const unsigned char *hash);
  virtual void getTopLevelEntry(void *nodeHash, long long *seq);
  virtual long long getLastSeq() { return topEntry.seq; };
  virtual bool setNextSeq(long long nextSeq);
  virtual int getNumEntries() { return numEntries; };
  virtual long long getBaseSeq() { return baseSeq; };
  virtual bool statEntry(int idx, long long *seq, unsigned char *type, int *sizeBytes, unsigned char *contentHash, unsigned char *nodeHash);
  virtual int getEntry(int idx, unsigned char *buffer, int buflen);
  virtual bool serializeRange(int idxFrom, int idxTo, HashPolicy *hashPolicy, FILE *outfile);
  virtual int findLastEntry(unsigned char *types, int numTypes, long long maxSeq);
  virtual int findNextEntry(unsigned char *types, int numTypes, long long minSeq);
  virtual int findSeq(long long seq) { return findSeqOrHigher(seq, false); };
  virtual bool upgradeHashedEntry(int idx, const void *entry, int size);
  virtual void syncWithFileOnDisk();
};

class SimpleSecureHistoryFactory : public SecureHistoryFactory {
public:
  SimpleSecureHistoryFactory() : SecureHistoryFactory() {};
  virtual SimpleSecureHistory *createTemp(long long baseSeq, const unsigned char *baseHash, HashProvider *hashprov) { return create(NULL, baseSeq, baseHash, hashprov); };
  virtual SimpleSecureHistory *create(const char *name, long long baseSeq, const unsigned char *baseHash, HashProvider *hashprov);
  virtual SimpleSecureHistory *open(const char *name, const char *mode, HashProvider *hashprov);
};

#endif /* defined(__peerreview_history_h__) */
