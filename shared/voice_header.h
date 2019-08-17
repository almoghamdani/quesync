#pragma once

struct VoiceHeader {
	unsigned char iv[16];
	unsigned char hmac[20];
};