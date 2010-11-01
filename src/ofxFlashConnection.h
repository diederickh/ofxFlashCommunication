#ifndef OFXFLASHCONNECTIONH
#define OFXFLASHCONNECTIONH

#undef check
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <string>

class ofxFlashCommunication;
typedef boost::shared_ptr<ofxFlashCommunication> flash_communcation_ptr;


class ofxFlashConnection : public boost::enable_shared_from_this<ofxFlashConnection> {
public:
	~ofxFlashConnection();
	
	typedef boost::shared_ptr<ofxFlashConnection> pointer;
	
	static pointer create(
		 flash_communcation_ptr pCommunication
		,boost::asio::io_service& rIOService
	);
	
	void start();
	
	boost::asio::ip::tcp::socket& socket();
	
	void write(std::string);
	
	void stop();
	
	ofxFlashConnection(
		 flash_communcation_ptr pComm
		,boost::asio::io_service& rIOService
	);
	
private:
	void sendPolicyFile();
	void handleRead(const boost::system::error_code& rErr, size_t nBytesTransferred);
	void handleWrite(const boost::system::error_code& rErr);
	boost::asio::ip::tcp::socket socket_;
	boost::asio::streambuf buf_;
	flash_communcation_ptr flash_comm;
	
};
#endif