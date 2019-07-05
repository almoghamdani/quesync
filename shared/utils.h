#pragma once
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

#ifdef QUESYNC_SERVER
#define MAX_TAG 9999
#endif

#ifdef QUESYNC_CLIENT
#include <napi.h>
#include <uv.h>
#endif

#include "errors.h"
#include "packets/packet.h"
#include "packets/packet_types.h"
#include "header.h"

class Utils
{
  public:
    static void CopyString(const std::string &input, char *dst);
    static std::vector<std::string> Split(const std::string &s, char delimiter);

    static std::string SHA256(const std::string str);

    static Packet *ParsePacket(std::string packet);
    static PacketType GetPacketType(std::string packet);

	static std::string EncodeHeader(Header& header);
	static Header DecodeHeader(const char *buf);

    static int RandomNumber(int min, int max);

    static std::string QuesyncErrorToString(QuesyncError error);

#ifdef QUESYNC_SERVER
    static int GenerateTag(std::string nickname, sql::Table &users_table);
#endif

#ifdef QUESYNC_CLIENT
    static Napi::Object JsonToObject(Napi::Env env, nlohmann::json json);
#endif
};