/*
 * Util.h
 *
 *  Created on: 14 f�vr. 2012
 *      Author: Amadou G
 */

#ifndef UTIL_H_
#define UTIL_H_

#include "IPAddressResolver.h"
#include "TCPSocket.h"
#include <vector>

using namespace std;

//static vector<int> node_ids;
struct Node
{
	int node_id;
	//IPvXAddress address;
};
struct Ring
{

	 vector<Node> set_of_nodes;
};

struct Data
{
	int counter;
	vector<Node> buffer;
};

struct classcomp {
  bool operator() (Node n1, Node n2) const
  {return n1.node_id<n2.node_id;}
};

struct timeHop
{
  simtime_t time;
  int hop;
};
#endif /* UTIL_H_ */
