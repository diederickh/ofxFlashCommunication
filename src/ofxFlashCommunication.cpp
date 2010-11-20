#include "ofxFlashCommunication.h"
#include <iostream>
#include "ofxFlashConnection.h"
#include "ofxFlashListener.h"

ofxFlashCommunication::ofxFlashCommunication(int iPort) 
	:port_(iPort)
	,endpoint_(boost::asio::ip::tcp::v4(), iPort)
	,acceptor_(io_service_, endpoint_)
{
	
}

ofxFlashCommunication::~ofxFlashCommunication() {
	std::cout << "~~~~ ofxFlashCommunication" << std::endl;
}

void ofxFlashCommunication::start() {
	thread_ptr_ = boost::shared_ptr<boost::thread>(new boost::thread(
		boost::bind(&ofxFlashCommunication::run, shared_from_this())
	));
}

ofxFlashCommunication::pointer ofxFlashCommunication::create(int iPort) {
	pointer com = boost::shared_ptr<ofxFlashCommunication>(new ofxFlashCommunication(iPort));
	return com;
}

void ofxFlashCommunication::run() {
	connection_ = ofxFlashConnection::create(shared_from_this(), io_service_);
	acceptor_.async_accept(
			connection_->socket()
			,boost::bind(
					&ofxFlashCommunication::handleAccept
					,shared_from_this()
					,connection_
					,boost::asio::placeholders::error
			)
	);

	io_service_.run();
}

void ofxFlashCommunication::addPolicy(std::string sDomain, std::string sPort) {
	policies.push_back(ofxFlashPolicy(sDomain, sPort));
}

void ofxFlashCommunication::handleAccept(
	 flash_connection_ptr pConnection
	,const boost::system::error_code& rErr
)
{
	if(!rErr) {
		
		connection_->start();
		connections.push_back(connection_);
		connection_.reset(new ofxFlashConnection(
			shared_from_this()
			,io_service_
		));

		acceptor_.async_accept(
				connection_->socket()
				,boost::bind(
						&ofxFlashCommunication::handleAccept
						,shared_from_this()
						,connection_
						,boost::asio::placeholders::error
				)
		);
	}
	else {
		std::cout << rErr.message() << std::endl;
	}
}

void ofxFlashCommunication::removeConnection(flash_connection_ptr pConnection) {
	std::deque<flash_connection_ptr>::iterator it = connections.begin();
	while(it != connections.end()) {
		if((*it) == pConnection) {
			(*it)->stop();
			it = connections.erase(it);
		}
		else
			++it;
	}
}

void ofxFlashCommunication::writeToClients(std::string sMessage) {
	boost::mutex::scoped_lock l(mutex_);
	sMessage += "\n"; // add the line end.
	std::deque<flash_connection_ptr>::iterator it = connections.begin();
	while(it != connections.end()) {
		(*it)->write(sMessage);
		++it;
	}
}

std::string ofxFlashCommunication::getPolicies() {
	std::vector<ofxFlashPolicy>::iterator it = policies.begin();
	std::string policy = "<cross-domain-policy>";
	while(it != policies.end()) {	
		policy += (*it).getXML();
		++it;
	}
	policy += "</cross-domain-policy>";
	policy += '\0';
	return policy;
}

void ofxFlashCommunication::onDataReceived(std::string sData, flash_connection_ptr pConn) {
	std::vector<ofxFlashListener*>::iterator it = listeners.begin();
	while(it != listeners.end()) {
		(*it)->onData(sData, pConn.get());
		++it;
	}
}

bool ofxFlashCommunication::hasPolicies() {
	return policies.size() > 0;
}

void ofxFlashCommunication::addListener(ofxFlashListener* pListener) {
	listeners.push_back(pListener);
}