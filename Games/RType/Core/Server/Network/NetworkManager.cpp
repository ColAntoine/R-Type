#include "NetworkManager.hpp"
#include <iostream>


namespace RType::Network {

NetworkManager::NetworkManager()
    : running_(false), port_(0) {
}

NetworkManager::~NetworkManager() {
    stop();
}

bool NetworkManager::initialize(uint16_t port) {
    if (running_) {
        std::cerr << "NetworkManager is already running" << std::endl;
        return false;
    }

    port_ = port;

    try {
        // Create work guard to keep io_context alive
        work_guard_ = std::make_unique<asio::io_context::work>(io_context_);

        // Create UDP server
        server_ = std::make_shared<UdpServer>(io_context_, port_);

        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to initialize NetworkManager: " << e.what() << std::endl;
        return false;
    }
}

bool NetworkManager::start(size_t thread_count) {
    if (running_) {
        std::cerr << "NetworkManager is already running" << std::endl;
        return false;
    }

    if (!server_) {
        std::cerr << "NetworkManager not initialized. Call initialize() first." << std::endl;
        return false;
    }

    try {
        // Start the UDP server
        if (!server_->start()) {
            std::cerr << "Failed to start UDP server" << std::endl;
            return false;
        }

        running_ = true;

        // Start worker threads
        worker_threads_.reserve(thread_count);
        for (size_t i = 0; i < thread_count; ++i) {
            worker_threads_.emplace_back([this]() { run_worker_thread(); });
        }

        std::cout << "NetworkManager started with " << thread_count << " worker threads on port " << port_ << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to start NetworkManager: " << e.what() << std::endl;
        running_ = false;
        return false;
    }
}

void NetworkManager::stop() {
    if (!running_) {
        return;
    }

    std::cout << "Stopping NetworkManager..." << std::endl;

    running_ = false;

    // Stop the server
    if (server_) {
        server_->stop();
    }

    // Remove work guard to allow io_context to exit
    work_guard_.reset();

    // Stop io_context
    io_context_.stop();

    // Join all worker threads
    for (auto& thread : worker_threads_) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    worker_threads_.clear();

    std::cout << "NetworkManager stopped" << std::endl;
}

bool NetworkManager::is_running() const {
    return running_;
}

void NetworkManager::run_worker_thread() {
    try {
        io_context_.run();
    } catch (const std::exception& e) {
        std::cerr << "Worker thread exception: " << e.what() << std::endl;
    }
}

} // namespace RType::Network
