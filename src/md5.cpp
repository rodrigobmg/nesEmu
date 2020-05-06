#include "md5.h"
#include <stdexcept>
#include <istream>

const uint32_t s[64] = {
	7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
	5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20,
	4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
	6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21
};

const uint32_t K[64] = {
	0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
	0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
	0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
	0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
	0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
	0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
	0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
	0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
	0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
	0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
	0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
	0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
	0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
	0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
	0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
	0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
};

static uint32_t leftRotate(uint32_t x, uint32_t c) {
	return (x << c) | (x >> (32 - c));
}

md5::md5(const std::string& str) : md5(str.c_str(), str.length()) { }

md5::md5(const char* data, uint64_t length) {
	uint8_t message[64];

	for(uint64_t i = 0; i < length; i++) {
		message[i % 64] = data[i];

		if((i + 1) % 64 == 0) {
			UpdateHash((char*)&message);
		}
	}

	int messageIndex = length % 64;

	message[messageIndex] = 0x80; // Append 1 bit, fill with 0
	messageIndex++;

	if(messageIndex > 56) {
		// Not enough space for length
		throw std::logic_error("Not implemented");
	}

	while(messageIndex < 56) {
		message[messageIndex] = 0;
		messageIndex++;
	}

	*reinterpret_cast<uint64_t*>(&message[56]) = length * 8;
	UpdateHash((char*)&message);
}

md5::md5(std::istream& stream) {
	if(!stream.good()) {
		throw std::runtime_error("Bad stream");
	}
	
	uint8_t message[64];
	int pos = 0;

	while(true) {
		stream.read(reinterpret_cast<char*>(&message), 64);
		auto count = stream.gcount();
		pos += count;
		
		if(count != 64) {
			break;
		}

		UpdateHash((char*)&message);
	}

	int messageIndex = pos % 64;

	message[messageIndex] = 0x80; // Append 1 bit, fill with 0
	messageIndex++;

	if(messageIndex > 56) {
		// Not enough space for length
		throw std::logic_error("Not implemented");
	}

	while(messageIndex < 56) {
		message[messageIndex] = 0;
		messageIndex++;
	}

	*reinterpret_cast<uint64_t*>(&message[56]) = pos * 8;
	UpdateHash((char*)&message);
}

std::string md5::ToString() const {
	const char hexChars[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
	char str[33];

	const auto toHex = [&](uint32_t val, int pos) {
		for(int i = 0; i < 4; i++) {
			str[pos + 0] = hexChars[(val >> 4) & 0xF];
			str[pos + 1] = hexChars[val & 0xF];

			val >>= 8;
			pos += 2;
		}
	};

	toHex(A, 0);
	toHex(B, 8);
	toHex(C, 16);
	toHex(D, 24);
	str[32] = '\0';

	return str;
}

void md5::UpdateHash(char* message) {
	auto M = reinterpret_cast<uint32_t*>(message);

	uint32_t a0 = A;
	uint32_t b0 = B;
	uint32_t c0 = C;
	uint32_t d0 = D;
	
	for(int i = 0; i < 64; i++) {
		uint32_t F, g;

		if(i < 16) {
			F = (b0 & c0) | (~b0 & d0);
			g = i;
		} else if(i < 32) {
			F = (d0 & b0) | (~d0 & c0);
			g = (5 * i + 1) & 0xF;
		} else if(i < 48) {
			F = b0 ^ c0 ^ d0;
			g = (3 * i + 5) & 0xF;
		} else {
			F = c0 ^ (b0 | ~d0);
			g = (7 * i) & 0xF;
		}

		F += a0 + K[i] + M[g];

		a0 = d0;
		d0 = c0;
		c0 = b0;
		b0 += leftRotate(F, s[i]);
	}

	A += a0;
	B += b0;
	C += c0;
	D += d0;
}

bool md5::operator==(const md5& other) const {
	return A == other.A &&
		B == other.B &&
		C == other.C &&
		D == other.D;
}

bool md5::operator!=(const md5& other) const {
	return A != other.A ||
		B != other.B ||
		C != other.C ||
		D != other.D;
}
