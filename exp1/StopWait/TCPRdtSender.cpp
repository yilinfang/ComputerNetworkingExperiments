#include "stdafx.h"
#include "TCPRdtSender.h"


TCPRdtSender::TCPRdtSender() :N(4), base(0), count(0), nextSeqNum(0)
{
	sndpkt = new Packet[N];
}

TCPRdtSender::TCPRdtSender(int n) : N(n), base(0), count(0), nextSeqNum(0)
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
	nextSeqNum = (nextSeqNum + 1) %  (2 * N);
	return true;
}

void TCPRdtSender::receive(Packet & ackPkt)
{
	if (pUtils->calculateCheckSum(ackPkt) == ackPkt.checksum)
	{
		if (base == ackPkt.acknum)
		{
			count++;
			if (count == 3)
			{
				count = 0;
				pUtils->printPacket("对该报文进行快速重传", sndpkt[base % N]);
				pns->sendToNetworkLayer(RECEIVER, sndpkt[base % N]);
				return;
			}
		}
		else
		{
			count = 0;
		}
		base = ackPkt.acknum;
		cout << "此时发送方滑动窗口的base值为:" << base << endl << "缓冲区中还有报文:" << endl;
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

void TCPRdtSender::timeoutHandler(int seqNum)
{
	pUtils->printPacket("接收ack超时，重新发送此报文", sndpkt[base % N]);
	pns->stopTimer(SENDER, seqNum);
	pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);
	pns->sendToNetworkLayer(RECEIVER, sndpkt[base % N]);
}

bool TCPRdtSender::getWaitingState()
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
