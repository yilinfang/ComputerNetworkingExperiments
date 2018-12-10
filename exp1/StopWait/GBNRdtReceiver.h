#pragma once
#include "RdtReceiver.h"
#include "Global.h"
class GBNRdtReceiver :
	public RdtReceiver
{
private:
	int N;
	int expectedSeqNum;
	Packet sndpkt;
public:
	GBNRdtReceiver();
	GBNRdtReceiver(int n);
	~GBNRdtReceiver();
	void receive(Packet &packet);
};

