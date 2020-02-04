#pragma once
#include "../event.h"

namespace quesync {
namespace events {
struct file_transmission_progress_event : public event {
    /// Default constructor.
    file_transmission_progress_event() : event(event_type::file_transmission_progress_event) {}

    /**
     * Event constructor.
     *
     * @param file_id The id of the file.
     * @param bytes Amount of bytes transfered.
     */
    file_transmission_progress_event(std::string file_id, unsigned long bytes)
        : event(event_type::file_transmission_progress_event) {
        this->file_id = file_id;
        this->bytes = bytes;
    }

    virtual nlohmann::json encode() const {
        return {{"eventType", type}, {"fileId", file_id}, {"bytes", bytes}};
    }
    virtual void decode(nlohmann::json j) {
        type = j["eventType"];
        file_id = j["fileId"];
        bytes = j["bytes"];
    }

    /// The id of the file.
    std::string file_id;

    /// Amount of bytes transfered.
    unsigned long bytes;
};
};  // namespace events
};  // namespace quesync
