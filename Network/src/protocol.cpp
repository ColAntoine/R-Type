#include "protocol.hpp"
#include <algorithm>
#include <stdexcept>

namespace RType::Protocol {

// ============================================================================
// PacketBuilder Implementation
// ============================================================================

PacketBuilder::PacketBuilder() : header_offset_(0) {
    buffer_.reserve(MAX_PACKET_SIZE);
}

void PacketBuilder::begin_packet(uint8_t message_type, PacketFlags flags) {
    clear();

    // Reserve space for header
    buffer_.resize(HEADER_SIZE);
    header_offset_ = 0;

    // Fill header (payload_size will be set in finalize())
    PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer_.data());
    header->message_type = message_type;
    header->flags = static_cast<uint8_t>(flags);
    header->payload_size = 0;  // Will be updated in finalize()
}

void PacketBuilder::add_data(const void* data, size_t size) {
    if (buffer_.size() + size > MAX_PACKET_SIZE) {
        throw std::runtime_error("Packet size would exceed maximum");
    }

    const uint8_t* byte_data = static_cast<const uint8_t*>(data);
    buffer_.insert(buffer_.end(), byte_data, byte_data + size);
}

std::vector<uint8_t> PacketBuilder::finalize() {
    // Update payload size in header
    size_t payload_size = buffer_.size() - HEADER_SIZE;
    if (payload_size > UINT16_MAX) {
        throw std::runtime_error("Payload size exceeds maximum");
    }

    PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer_.data());
    header->payload_size = static_cast<uint16_t>(payload_size);

    return std::move(buffer_);
}

void PacketBuilder::clear() {
    buffer_.clear();
    header_offset_ = 0;
}

// ============================================================================
// PacketParser Implementation
// ============================================================================

PacketParser::PacketParser(const char* data, size_t size)
    : payload_(nullptr), valid_(false) {

    if (!data || size < HEADER_SIZE) {
        return;
    }

    // Copy header
    std::memcpy(&header_, data, HEADER_SIZE);

    // Validate packet
    if (size != HEADER_SIZE + header_.payload_size) {
        return;
    }

    // Set payload pointer
    payload_ = data + HEADER_SIZE;
    valid_ = true;
}

// ============================================================================
// Utility Functions Implementation
// ============================================================================

std::vector<uint8_t> create_simple_packet(uint8_t message_type, PacketFlags flags) {
    PacketBuilder builder;
    builder.begin_packet(message_type, flags);
    return builder.finalize();
}

bool validate_packet(const char* data, size_t size) {
    if (!data || size < HEADER_SIZE) {
        return false;
    }

    const PacketHeader* header = reinterpret_cast<const PacketHeader*>(data);
    return size == HEADER_SIZE + header->payload_size;
}

} // namespace RType::Protocol