#include "ClientReceiver.h"

ClientReceiver::ClientReceiver(TCPStream* s) : stream(s) {}

/*

*/
void* ClientReceiver::run() {

	//should never get here
	return NULL;
}

/*
	Destructor
*/
ClientReceiver::~ClientReceiver() {
	delete stream;
}