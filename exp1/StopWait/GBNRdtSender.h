#pragma once
#include "RdtSender.h"
#include "Global.h"



using namespace std;


class GBNRdtSender :
	public RdtSender
{
private:
	int N;
	int base;
	int nextSeqNum;
	Packet* sndpkt;
public:
	GBNRdtSender();
	GBNRdtSender(int n);
	~GBNRdtSender();
	bool send(Message &message);
	void receive(Packet &ackPkt);
	void timeoutHandler(int seqNum);
	bool getWaitingState();
};

