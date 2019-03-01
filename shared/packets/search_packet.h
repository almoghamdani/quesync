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

        std::string nickname = _data["nickname"];

        sql::Table users_table = session->server()->db().getTable("users");
        std::vector<sql::Row> res;

        // Insert % before and after the string to match any string containing the nickname
        nickname.insert(0, "%");
        nickname.append("%");

        // If a tag was entered, select with it as a requirement
        if ((int)_data["tag"] != -1)
        {
            // Get all users matching the searched nickname and tag
            res = users_table.select("id", "nickname", "tag")
                .where("nickname LIKE :nickname AND tag = :tag")
                .bind("nickname", nickname).bind("tag", (int)_data["tag"]).execute().fetchAll();
        } else {
            // Get all users matching the searched nickname
            res = users_table.select("id", "nickname", "tag")
                .where("nickname LIKE :nickname")
                .bind("nickname", nickname).execute().fetchAll();
        }

        // Format the results in the json type
        for (int i = 0; i < res.size(); i++)
        {
            results[std::string(res[i][0])] = nlohmann::json({ { "nickname", res[i][1] }, { "tag", std::to_string((int)res[i][2]) } });
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