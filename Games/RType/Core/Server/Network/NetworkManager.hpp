#pragma once

#include <memory>
#include <asio.hpp>
#include <thread>
#include <vector>
#include <atomic>
#include "UDPServer.hpp"


namespace RType::Network {

    class UdpServer;

    /**
     * @brief Main network manager that orchestrates the entire network layer
     *
     * This class manages the io_context, server instances, and provides
     * a clean interface for starting/stopping the network subsystem.
     */
    class NetworkManager {
        public:
            NetworkManager();
            ~NetworkManager();

            /**
             * @brief Initialize the network manager with specified port
             * @param port Server port to bind to
             * @return true if initialization succeeded
             */
            bool initialize(uint16_t port);

            /**
             * @brief Start the network manager and begin accepting connections
             * @param thread_count Number of worker threads for io_context
             * @return true if startup succeeded
             */
            bool start(size_t thread_count = std::thread::hardware_concurrency());

            /**
             * @brief Stop the network manager and all connections
             */
            void stop();

            /**
             * @brief Check if the network manager is currently running
             */
            bool is_running() const;

            /**
             * @brief Get the io_context for advanced usage
             */
            asio::io_context& get_io_context() { return io_context_; }

            /**
             * @brief Get the UDP server instance
             */
            std::shared_ptr<UdpServer> get_server() const { return server_; }

        private:
            void run_worker_thread();

            asio::io_context io_context_;
            std::unique_ptr<asio::io_context::work> work_guard_;
            std::shared_ptr<UdpServer> server_;
            std::vector<std::thread> worker_threads_;
            std::atomic<bool> running_;
            uint16_t port_;
    };

} // namespace RType::Network
