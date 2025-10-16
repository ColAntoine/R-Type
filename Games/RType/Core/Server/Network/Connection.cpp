#include "Connection.hpp"
#include <sstream>
#include <iostream>

namespace RType::Network {

    Connection::Connection(const endpoint_type& endpoint, std::shared_ptr<asio::ip::udp::socket> socket)
        : endpoint_(endpoint)
        , socket_(socket)
        , connection_id_(generate_connection_id(endpoint))
        , last_activity_(clock_type::now())
        , active_(true) {
    }

    void Connection::send(const char* data, size_t size) {
        if (!active_) {
            return;
        }

        try {
            auto buf = std::make_shared<std::vector<char>>(data, data + size);
            socket_->async_send_to(
                asio::buffer(*buf),
                endpoint_,
                [this, buf](const std::error_code& ec, std::size_t /*bytes_sent*/) {
                    if (ec) {
                        std::cerr << "Async send error to " << connection_id_ << ": " << ec.message() << std::endl;
                        active_ = false;
                    } else {
                        update_activity();
                    }
                }
            );
        } catch (const std::exception& e) {
            std::cerr << "Failed to send data to " << connection_id_ << ": " << e.what() << std::endl;
            active_ = false;
        }
    }

    bool Connection::is_active() const {
        return active_;
    }

    void Connection::update_activity() {
        last_activity_ = clock_type::now();
    }

    std::chrono::milliseconds Connection::get_idle_time() const {
        auto now = clock_type::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(now - last_activity_);
    }

    void Connection::disconnect() {
        active_ = false;
    }

    std::string Connection::generate_connection_id(const endpoint_type& endpoint) {
        std::stringstream ss;
        ss << endpoint.address().to_string() << ":" << endpoint.port();
        return ss.str();
    }

} // namespace RType::Network