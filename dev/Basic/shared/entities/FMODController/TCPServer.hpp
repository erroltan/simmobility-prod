/*
 * TCPServer.hpp
 *
 *  Created on: May 22, 2013
 *      Author: zhang
 */

#ifndef TCPSERVER_HPP_
#define TCPSERVER_HPP_
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include "TCPSession.hpp"
#include "vector"

namespace sim_mob {

namespace FMOD
{

class TCPServer {
public:
	TCPServer(boost::asio::io_service& io_service,int port);
	virtual ~TCPServer();
public:
  bool isClientConnect() { return connectionList.size()>0;}

private:

  std::vector<TCPSessionPtr> connectionList;
  int myPort;
  void start_accept();

  void handle_accept(boost::shared_ptr<TCPSession> new_connection, const boost::system::error_code& error);

  boost::asio::ip::tcp::acceptor acceptor_;
};

}

} /* namespace sim_mob */
#endif /* TCPSERVER_HPP_ */