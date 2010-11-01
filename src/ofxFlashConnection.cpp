#include "ofxFlashConnection.h"
#include "ofxFlashCommunication.h"
#include <iostream>


ofxFlashConnection::ofxFlashConnection(
	flash_communcation_ptr pCommunication
	,boost::asio::io_service& rIOService
)
:socket_(rIOService)
,flash_comm(pCommunication)
{
	std::cout << "ofxFlashConnection::ofxFlashConnection()" << std::endl;
}

ofxFlashConnection::~ofxFlashConnection() {
	std::cout << "~~~~ ofxFlashConnection()" << std::endl;
}
ofxFlashConnection::pointer ofxFlashConnection::create(
	 flash_communcation_ptr pCommunication
	,boost::asio::io_service& rIOService)
{
	boost::shared_ptr<ofxFlashConnection> con(new ofxFlashConnection(
		pCommunication
		,rIOService
	));
	return con;
}


boost::asio::ip::tcp::socket& ofxFlashConnection::socket() {
	return socket_;
}

void ofxFlashConnection::start() {
	boost::asio::async_read_until(
		socket_, 
		buf_
		,"\n"
		,boost::bind(
			&ofxFlashConnection::handleRead
			,shared_from_this()
			,boost::asio::placeholders::error
			,boost::asio::placeholders::bytes_transferred
		)
	);
	
}


void ofxFlashConnection::handleRead(
		const boost::system::error_code& rErr
		,size_t nBytesTransferred
)
{
	if (!rErr) {
	
		// read the received line.
		std::istream response_stream(&buf_);
		char* received_line = new char[nBytesTransferred];
		response_stream.getline(received_line, nBytesTransferred,'\n');
		std::cout << "<< " << received_line << std::endl;
		std::string line = received_line;
		delete received_line;
		
		// notify FlashCommunication we received some data.
	
		// continue reading new data.
		boost::asio::async_read_until(
			socket_, 
			buf_
			,"\n"
			,boost::bind(
				&ofxFlashConnection::handleRead
				,shared_from_this(),
				boost::asio::placeholders::error
				,boost::asio::placeholders::bytes_transferred
			)
		);
	}
	else {
		std::cout << "ofxFlashConnection::handleRead() error:" << rErr.message() << std::endl;
		flash_comm->removeConnection(shared_from_this());
	}
}

void ofxFlashConnection::write(std::string sData) {
	
	boost::asio::async_write(
		socket_
		,boost::asio::buffer(sData) 
		,boost::bind(
			&ofxFlashConnection::handleWrite
			,shared_from_this()
			,boost::asio::placeholders::error
		)
	);
}

void ofxFlashConnection::stop() {
	socket_.close();
}

void ofxFlashConnection::handleWrite(const boost::system::error_code& rErr) {
	if(!rErr) {
		std::cout << "ofxFlashConnection::handlWrite(). SUCCESS!" << std::endl;
	}
	else {
		std::cout << "ofxFlashConnection::handleWrite: Error: " << rErr.message() << std::endl;
	}
}