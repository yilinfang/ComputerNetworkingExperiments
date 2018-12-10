#include "stdafx.h"
#include "TCPRdtSender.h"


TCPRdtSender::TCPRdtSender() :N(4), base(0), count(0), nextSeqNum(0), preAck(-1)
{
	sndpkt = new Packet[N];
}

TCPRdtSender::TCPRdtSender(int n) : N(n), base(0), count(0), nextSeqNum(0), preAck(-1)
{
	sndpkt = new Packet[N];
}


TCPRdtSender::~TCPRdtSender()
{
	delete[] sndpkt;
}

bool TCPRdtSender::send(Message & message)
{
	if (getWaitingState())
	{
		return false;
	}
	sndpkt[nextSeqNum].acknum = -1;
	sndpkt[nextSeqNum].seqnum = nextSeqNum;
	sndpkt[nextSeqNum].checksum = 0;
	memcpy(sndpkt[nextSeqNum].payload, message.data, sizeof(message.data));
	sndpkt[nextSeqNum].checksum = pUtils->calculateCheckSum(sndpkt[nextSeqNum]);
	pns->sendToNetworkLayer(RECEIVER, sndpkt[nextSeqNum]);
	pUtils->printPacket("发送方发送报文", sndpkt[nextSeqNum]);
	if (base == nextSeqNum)
	{
		pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
	}
	nextSeqNum = (nextSeqNum + 1) % N;
	return true;
}

void TCPRdtSender::receive(Packet & ackPkt)
{
	int checkSum = pUtils->calculateCheckSum(ackPkt);
	if (checkSum == ackPkt.checksum)
	{
		if (preAck == ackPkt.acknum)
		{
			count++;
			if (count == 3)
			{
				count = 0;
				pUtils->printPacket("对该报文进行快速重传", sndpkt[(base + preAck) % N]);
				pns->sendToNetworkLayer(RECEIVER, sndpkt[(base + preAck) % N]);
				return;
			}
		}
		else
		{
			count = 0;
			preAck = ackPkt.acknum;
		}
		base = (ackPkt.acknum + 1) % N;
		cout << "此时滑动窗口的base值为:" << base << endl << "缓冲区中还有报文:" << endl;
		int i = base;
		while (i != nextSeqNum)
		{
			pUtils->printPacket("    ", sndpkt[i]);
			i = (i + 1) % N;
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

void TCPRdtSender::timeoutHandler(int seqNum)
{
	pUtils->printPacket("接收ack超时，重新发送此报文之后的报文", sndpkt[base]);
	pns->stopTimer(SENDER, seqNum);
	pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);
	int i = base;
	while (i != nextSeqNum)
	{
		pns->sendToNetworkLayer(RECEIVER, sndpkt[i]);
		i = (i + 1) % N;
	}
}

bool TCPRdtSender::getWaitingState()
{
	if ((nextSeqNum + 1) % N == base)
	{
		return true;
	}
	else
	{
		return false;
	}
}
