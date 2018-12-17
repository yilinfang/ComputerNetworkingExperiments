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
	pUtils->printPacket("���ͷ����ͱ���", sndpkt[nextSeqNum % N]);
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
		cout << "��ʱ�������ڵ�baseֵΪ:" << base << endl << "�������л��б���:" << endl;
		int i = base;
		while (i != nextSeqNum)
		{
			pUtils->printPacket("    ", sndpkt[i % N]);
			i = (i + 1) % (2 * N);
		}
		if (base == nextSeqNum)
		{
			pUtils->printPacket("���ͷ��յ���ȷȷ��", ackPkt);
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
	pUtils->printPacket("����ack��ʱ�����·��ʹ˱���֮��ı���", sndpkt[base % N]);
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
