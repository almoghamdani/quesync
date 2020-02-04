#pragma once
#include <memory>
#include <string>

// Only include quesync server if built with server
#ifdef QUESYNC_SERVER
#include "../server/src/session.h"
#endif

#include "packet_type.h"

#define PACKET_IDENTIFIER "QUESYNC"
#define PACKET_DELIMETER '|'
#define PACKET_TYPE_LEN 3

namespace quesync {
class packet {
   public:
    /// Default constructor.
    packet(){};

    /**
     * Packet constructor.
     *
     * @param type The type of the packet.
     */
    packet(packet_type type) : _type(type){};

    virtual ~packet(){};

    /**
     * Encode the packet.
     *
     * @return The packet encoded.
     */
    virtual std::string encode() = 0;

    /**
     * Decode the packet.
     *
     * @param packet The packet's encoded data.
     * @return True if the packet was decoded successfully or false otherwise.
     */
    virtual bool decode(std::string packet) = 0;

// A handle function for the server
#ifdef QUESYNC_SERVER
    /**
     * Handle the packet when received. (Server-side)
     *
     * @param session A shared pointer to the session object of the user.
     * @return Response packet encoded.
     */
    virtual std::string handle(std::shared_ptr<server::session> session) = 0;
#endif

    /**
     * Get the type of the packet.
     *
     * @return The type of the packet.
     */
    packet_type type() const { return _type; };

   protected:
    packet_type _type;
};
};  // namespace quesync
