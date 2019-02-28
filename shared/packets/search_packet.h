#pragma once
#include "serialized_packet.h"

#include <vector>
#include <sstream>

#ifdef QUESYNC_SERVER
#include <sqlitepp.h>
#endif

#include "response_packet.h"
#include "error_packet.h"
#include "../quesync_exception.h"

class SearchPacket : public SerializedPacket
{
public:
    SearchPacket() : SearchPacket("", -1) {};

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
    virtual std::string handle (Session *session)
    {
        nlohmann::json results;

        sqlitepp::query search_query(*session->server()->db(), "SELECT id, nickname, tag FROM users WHERE nickname LIKE ?");
        sqlitepp::result res;

        int tag = _data["tag"];
        std::string nickname = _data["nickname"], tag_str = std::to_string(tag);

        // Add % and % before and after the nickname to make the search of the sqlite search for all possible matches
        nickname.insert(0, "%");
        nickname.append("%");

        // Set the nickname
        search_query.bind(1, nickname);

        // If a tag was entered, set it as a requirement
        if (tag != -1)
        {
            // Set the tag requirement
            search_query << " AND tag=?" << tag_str;
        }

        // Search for the nicknames in the database
        res = search_query.store();

        // Format the results in the json type
        for (int i = 0; i < res.size(); i++)
        {
            results[res[i]["id"]] = nlohmann::json({ { "nickname", res[i]["nickname"] }, { "tag", res[i]["tag"] } });
        }
        
        try {
            // Return autheticated packet with the user's info
            return ResponsePacket(AUTHENTICATED_PACKET, results.dump()).encode();
        } catch (QuesyncException& ex) {
            // Return the error code
            return ErrorPacket(ex.getErrorCode()).encode();
        }  
    };
    #endif
};