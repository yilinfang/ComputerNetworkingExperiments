#include "stdafx.h"
#include "GBNRdtSender.h"


GBNRdtSender::GBNRdtSender() :N(4), base(0), nextSeqNum(0)
{
	sndpkt = new Packet[N];
}

GBNRdtSender::GBNRdtSender(int n) : N(n), base(0), nextSeqNum(0)
{
	sndpkt = new Packet[N];
}


GBNRdtSender::~GBNRdtSender()
{
	delete[] sndpkt;
}

bool GBNRdtSender::send(Message & message)
{
	if (getWaitingState())
	{
		return false;
	}
	sndpkt[nextSeqNum % N].acknum = -1;
	sndpkt[nextSeqNum % N].seqnum = nextSeqNum;
	sndpkt[nextSeqNum % N].checksum = 0;
	memcpy(sndpkt[nextSeqNum % N].payload, message.data, sizeof(message.data));
	sndpkt[nextSeqNum % N].checksum = pUtils->calculateCheckSum(sndpkt[nextSeqNum % N]);
	pns->sendToNetworkLayer(RECEIVER, sndpkt[nextSeqNum % N]);
	pUtils->printPacket("发送方发送报文", sndpkt[nextSeqNum % N]);
	if (base == nextSeqNum)
	{
		pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
	}
	nextSeqNum = (nextSeqNum + 1) % (2 * N);
	return true;
}

void GBNRdtSender::receive(Packet & ackPkt)
{
	if (pUtils->calculateCheckSum(ackPkt) == ackPkt.checksum)
	{
		base = (ackPkt.acknum + 1) % (2 * N);
		cout << "此时滑动窗口的base值为:" << base << endl << "缓冲区中还有报文:" << endl;
		int i = base;
		while (i != nextSeqNum)
		{
			pUtils->printPacket("    ", sndpkt[i % N]);
			i = (i + 1) % (2 * N);
		}
		if (base == nextSeqNum)
		{
			pUtils->printPacket("发送方收到正确确认", ackPkt);
			pns->stopTimer(SENDER, 0);
		}
		else
		{
			pns->stopTimer(SENDER, 0);
			pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
		}
	}
}

void GBNRdtSender::timeoutHandler(int seqNum)
{
	pUtils->printPacket("接收ack超时，重新发送此报文之后的报文", sndpkt[base % N]);
	pns->stopTimer(SENDER, seqNum);
	pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);
	int i = base;
	while (i != nextSeqNum)
	{
		pns->sendToNetworkLayer(RECEIVER, sndpkt[i % N]);
		i = (i + 1) % (2 * N);
	}
}

bool GBNRdtSender::getWaitingState()
{
	if ((nextSeqNum - base + 2 * N) % (2 * N) < N)
	{
		return false;
	}
	else
	{
		return N;
	}
}
