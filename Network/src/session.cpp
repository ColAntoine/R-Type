#include "session.hpp"
#include <sstream>
#include <random>

namespace RType::Network {

std::atomic<int> Session::next_player_id_{1};

Session::Session(std::shared_ptr<Connection> connection)
    : connection_(connection)
    , session_id_(generate_session_id())
    , player_id_(-1)  // -1 indicates no player ID assigned yet
    , authenticated_(false)
    , last_activity_(clock_type::now()) {
}

void Session::update_activity() {
    last_activity_ = clock_type::now();
    if (connection_) {
        connection_->update_activity();
    }
}

std::chrono::milliseconds Session::get_idle_time() const {
    auto now = clock_type::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now - last_activity_);
}

bool Session::is_timed_out(std::chrono::milliseconds timeout_duration) const {
    return get_idle_time() > timeout_duration;
}

void Session::send(const char* data, size_t size) {
    if (connection_) {
        connection_->send(data, size);
    }
}

void Session::disconnect() {
    if (connection_) {
        connection_->disconnect();
    }
}

bool Session::is_connected() const {
    return connection_ && connection_->is_active();
}

std::string Session::generate_session_id() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);

    std::stringstream ss;
    ss << "session_";
    for (int i = 0; i < 8; ++i) {
        ss << std::hex << dis(gen);
    }
    return ss.str();
}

} // namespace RType::Network