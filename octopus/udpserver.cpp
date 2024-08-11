#include "udpserver.h"

UDPServer::UDPServer(uint16_t port)
        : endpoint_(udp::endpoint(udp::v4(), port)),
          socket_(io_, endpoint_), 
          thread_(&UDPServer::run, this) {
    
    /* start initializing order book */
    handler::init();
    start_receive();
}

UDPServer::~UDPServer() { 
    handler::destroy(); 
    work_.reset(); // allow service to run out of work
    thread_.join();
}

void UDPServer::start_receive() {
    socket_.async_receive_from(boost::asio::buffer(recv_buffer_), endpoint_,
            [this](error_code ec, size_t xfer) { handle_reply(ec, xfer); });
}

void UDPServer::handle_reply(const error_code& error, size_t transferred) {
    if (!error) {
        try {
            handler::process(recv_buffer_.data(), transferred);

            std::string s(recv_buffer_.data(), transferred);
            std::cerr << "udpserver::handle_reply - processed message: " << s << std::endl;

        } catch (std::exception const& ex) {
            std::cerr << "udpserver::handle_reply: Error parsing incoming message:"
                      << ex.what() << "\n";
        } catch (...) {
            std::cerr
                << "udpserver::handle_reply: Unknown error while parsing incoming message\n";
        }

        start_receive();
    } else {
        std::cerr << "udpserver::handle_reply: error: " << error.message() << "\n";
    }
}

void UDPServer::run() {
    while (true) {
        try {
            if (io_.run() == 0u) {
                break;
            }
        } catch (const std::exception& e) {
            std::cerr << "udpserver::run: Server network exception: " << e.what() << "\n";
        } catch (...) {
            std::cerr << "udpserver::run: Unknown exception in server network thread\n";
        }
    }
    std::cerr << "udpserver::run: Server network thread stopped\n";
}

