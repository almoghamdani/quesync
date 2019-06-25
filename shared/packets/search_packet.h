#pragma once
#include "serialized_packet.h"

#include <iostream>
#include <vector>

#include "response_packet.h"
#include "error_packet.h"
#include "../quesync_exception.h"

class SearchPacket : public SerializedPacket
{
public:
    SearchPacket() : SearchPacket("", -1){};

    SearchPacket(std::string nickname, int tag) : SerializedPacket(SEARCH_PACKET)
    {
        _data["nickname"] = nickname;
        _data["tag"] = tag;
    };

    virtual bool verify() const
    {
        return (exists("nickname") &&
                exists("tag"));
    };

// A handle function for the server
#ifdef QUESYNC_SERVER
    virtual std::string handle(Session *session)
    {
        nlohmann::json results;

        std::string nickname = _data["nickname"];
        int tag = _data["tag"];

        try
        {
            // Try to search the wanted nickname and tag
            results = session->server()->userManager()->search(session->getShared(), nickname, tag);
        }
        catch (...)
        {
            return ErrorPacket(UNKNOWN_ERROR).encode();
        }

        try
        {
            // Return the search results found
            return ResponsePacket(SEARCH_RESULTS_PACKET, results.dump()).encode();
        }
        catch (QuesyncException &ex)
        {
            // Return the error code
            return ErrorPacket(ex.getErrorCode()).encode();
        }
        catch (...)
        {
            return ErrorPacket(UNKNOWN_ERROR).encode();
        }
    };
#endif
};