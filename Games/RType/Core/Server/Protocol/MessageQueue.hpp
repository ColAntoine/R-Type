#pragma once

#include <string>
#include <vector>
#include <mutex>
#include <queue>
#include <memory>

namespace RType::Network {

    // Simple container for received packets pushed by UdpServer
    struct ReceivedPacket {
        std::string session_id; // connection id (address:port)
        uint16_t server_port = 0; // local server port that received this packet
        std::vector<char> data; // raw dispatcher data (message_type + payload)
    };

    // Thread-safe queue for messages from network to server ECS
    class MessageQueue {
        public:
            void push(ReceivedPacket&& pkt) {
                std::lock_guard<std::mutex> lk(mutex_);
                queue_.push(std::move(pkt));
            }

            // Pop one packet if available. Returns true if a packet was popped.
            bool try_pop(ReceivedPacket& out) {
                std::lock_guard<std::mutex> lk(mutex_);
                if (queue_.empty()) return false;
                out = std::move(queue_.front());
                queue_.pop();
                return true;
            }

            // Drain all packets into the provided vector
            void drain(std::vector<ReceivedPacket>& out) {
                std::lock_guard<std::mutex> lk(mutex_);
                while (!queue_.empty()) {
                    out.push_back(std::move(queue_.front()));
                    queue_.pop();
                }
            }

            size_t size() const {
                std::lock_guard<std::mutex> lk(mutex_);
                return queue_.size();
            }

        private:
            mutable std::mutex mutex_;
            std::queue<ReceivedPacket> queue_;
    };

}
