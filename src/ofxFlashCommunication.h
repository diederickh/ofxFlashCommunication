#ifndef OFXFLASHCOMMUNICATIONH
#define OFXFLASHCOMMUNICATIONH

#undef check
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <string>
#include <deque>

#include <vector>

struct ofxFlashPolicy {
	ofxFlashPolicy(
		 std::string sDomain
		,std::string sPort
	)
		:domain(sDomain)
		,port(sPort)
	{
	}

	std::string getXML() {
		std::string xml = "<allow-access-from " \
								"domain=\"" +domain +"\" " \
								+"secure=\"false\" " \
								+"to-ports=\"" +port +"\" />";
		return xml;
	}
	
	std::string domain;
	std::string port;
};


class ofxFlashConnection;
typedef boost::shared_ptr<ofxFlashConnection> flash_connection_ptr;
	  
class ofxFlashCommunication : public boost::enable_shared_from_this<ofxFlashCommunication> {
public:
	typedef boost::shared_ptr<ofxFlashCommunication> pointer;
	static pointer create(int iPort);
	~ofxFlashCommunication();
	void writeToClients(std::string sMessage);
	void removeConnection(flash_connection_ptr pConnection);
	void addPolicy(std::string sDomain, std::string sPort);
	std::string getPolicies();
	bool hasPolicies();
	void start();
private:
	ofxFlashCommunication(int iPort);
	
	
	void run();
	void handleAccept(
			flash_connection_ptr pConnection
			,const boost::system::error_code& rErr
	);

	int port_;
	
	boost::shared_ptr<boost::thread> thread_ptr_;
	boost::asio::io_service io_service_;
	boost::asio::ip::tcp::endpoint endpoint_;
	boost::asio::ip::tcp::acceptor acceptor_;
	boost::mutex mutex_;
	std::deque<flash_connection_ptr> connections;
	flash_connection_ptr connection_;
	std::vector<ofxFlashPolicy> policies;
};
#endif