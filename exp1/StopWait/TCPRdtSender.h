#pragma once
#include "RdtSender.h"
#include "Global.h"
class TCPRdtSender :
	public RdtSender
{
private:
	int N;
	int base;
	int count;
	int nextSeqNum;
	Packet* sndpkt;
public:
	TCPRdtSender();
	TCPRdtSender(int n);
	~TCPRdtSender();
	bool send(Message &message);
	void receive(Packet &ackPkt);
	void timeoutHandler(int seqNum);
	bool getWaitingState();
};

