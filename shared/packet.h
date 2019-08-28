#pragma once
#include <string>
#include <memory>

// Only include quesync server if built with server
#ifdef QUESYNC_SERVER
#include "../server/session.h"
#endif

#include "packet_type.h"

#define PACKET_IDENTIFIER "QUESYNC"
#define PACKET_DELIMETER '|'
#define PACKET_TYPE_LEN 3

namespace quesync {
class packet {
   public:
    packet(){};

    packet(packet_type type) : _type(type){};

    virtual ~packet(){

    };

    virtual std::string encode() = 0;
    virtual bool decode(std::string packet) = 0;

// A handle function for the server
#ifdef QUESYNC_SERVER
    virtual std::string handle(std::shared_ptr<server::session> session) = 0;
#endif

    packet_type type() const { return _type; };

   protected:
    packet_type _type;
};
};  // namespace quesync
