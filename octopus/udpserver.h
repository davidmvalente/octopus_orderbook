#include <boost/asio.hpp>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <thread>
#include <vector>

#include "handler.h"

using boost::asio::ip::udp;
using boost::system::error_code;
using boost::asio::io_context;

#define BUFFERSIZE 1024

class UDPServer {
  public:
    explicit UDPServer(uint16_t port);
    virtual ~UDPServer();

  private:
    io_context io_;
    boost::asio::executor_work_guard<io_context::executor_type> work_ {io_.get_executor()};
    udp::endpoint endpoint_;
    udp::socket socket_;
    std::array<char, BUFFERSIZE> recv_buffer_;
    std::thread thread_;

    void run();
    void start_receive();
    void handle_reply(const error_code& error, size_t transferred);
};
