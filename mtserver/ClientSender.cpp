#include "ClientSender.h"

ClientSender::ClientSender(TCPStream* s) : stream(s) {}

void* ClientSender::run() {
	//should never get here
	return NULL;
}

/*
	Gets the TCPStream* that represents the connection
*/
TCPStream* ClientSender::getStream() {
	return stream;
}

/*
	Destructor
*/
ClientSender::~ClientSender() {
	delete stream;
}