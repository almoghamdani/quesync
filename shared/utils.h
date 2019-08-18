#pragma once

#include <string>
#include <vector>
#include <memory>
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

#define AES_KEY_SIZE (256 / 8)
#define HMAC_KEY_SIZE (160 / 8)

#define PKBDF2_SALT_SIZE (128 / 8)
#define PKBDF2_ITERATIONS 15000
#define PKBDF2_HASH_SIZE (512 / 8)

class Utils
{
public:
	static std::vector<std::string> Split(const std::string &s, char delimiter);

	static std::string SHA256(const std::string str);

	static Packet *ParsePacket(std::string packet);
	static PacketType GetPacketType(std::string packet);

	static std::string EncodeHeader(Header &header);
	static Header DecodeHeader(const char *buf);

	static std::shared_ptr<unsigned char> RandBytes(unsigned int amount);

	static std::string PBKDF2_SHA512(std::string password, unsigned char *salt);
	static bool PBKDF2_SHA512Compare(std::string password, std::string hash);

	static std::string AES256Encrypt(std::string data, unsigned char *key, unsigned char *iv);
	static std::string AES256Decrypt(std::string data, unsigned char *key, unsigned char *iv);

	static std::string HMAC(std::string data, unsigned char *key);

	static std::string Base64Encode(std::string data);
	static std::string Base64Decode(std::string data);

	template <typename T>
	static std::string EncryptVoicePacket(T *packet, unsigned char *aes_key, unsigned char *hmac_key);

	template <typename T>
	static std::shared_ptr<T> DecryptVoicePacket(std::string data, unsigned char *aes_key, unsigned char *hmac_key);

	template <typename T>
	static std::shared_ptr<T> ConvertToBuffer(std::string data);

	static int RandomNumber(int min, int max);

	static std::string QuesyncErrorToString(QuesyncError error);

#ifdef QUESYNC_SERVER
	static int GenerateTag(std::string nickname, sql::Table &users_table);
#endif

#ifdef QUESYNC_CLIENT
	static Napi::Object JsonToObject(Napi::Env env, nlohmann::json json);

	template <typename T, typename R>
	static std::vector<T> ArrayToNative(Napi::Array arr)
	{
		std::vector<T> items;

		for (int i = 0; i < arr.Length(); i++)
		{
			items.push_back(arr.Get(i).As<R>());
		}

		return items;
	};
#endif
};