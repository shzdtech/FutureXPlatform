#include "AESCrypto.h"
#include "../include/cryptopp/aes.h"
#include "../include/cryptopp/modes.h"
#include "../include/cryptopp/filters.h"
#include "../include/cryptopp/hex.h"
#include "../include/cryptopp/osrng.h"

// {6338134B-1A7F-4ABF-88B2-8328352DBC5D}
static const byte AESDBKEY[] = { 0x4b, 0x13, 0x38, 0x63, 0x7f, 0x1a, 0xbf, 0x4a,  0x88, 0xb2, 0x83, 0x28, 0x35, 0x2d, 0xbc, 0x5d };
static const byte IV[] = { 0x88, 0xb2, 0x83, 0x28, 0x35, 0x1a, 0xbf, 0x4a, 0x2d, 0xbc, 0x5d, 0x4b, 0x13, 0x38, 0x63, 0x7f };

std::string AESCrypto::Encrypt(const std::string& plain)
{
	std::string ret;

	using CryptoPP::AES;
	using CryptoPP::CFB_Mode;


	// The StreamTransformationFilter removes
	//  padding as required.
	using CryptoPP::StringSink;
	using CryptoPP::StringSource;
	using CryptoPP::StreamTransformationFilter;

	std::string cipher;

	try
	{

		CFB_Mode< AES >::Encryption e;
		e.SetKeyWithIV(AESDBKEY, sizeof(AESDBKEY), IV);

		// CFB mode must not use padding. Specifying
		//  a scheme will result in an exception
		StringSource(plain, true,
			new StreamTransformationFilter(e,
				new StringSink(cipher)));
	}
	catch (const CryptoPP::Exception& e)
	{
	}

	StringSource(cipher, true,
		new CryptoPP::HexEncoder(
			new StringSink(ret)
		) // HexEncoder
	); // StringSo

	return ret;
}

std::string AESCrypto::Descrypt(const std::string& cipher)
{
	std::string decoder, ret;

	using CryptoPP::AES;
	using CryptoPP::CFB_Mode;

	// The StreamTransformationFilter removes
	//  padding as required.
	using CryptoPP::StringSink;
	using CryptoPP::StringSource;
	using CryptoPP::StreamTransformationFilter;


	StringSource(cipher, true,
		new CryptoPP::HexDecoder(
			new StringSink(decoder)
		) // HexEncoder
	); // StringSo

	try
	{
		CFB_Mode< AES >::Decryption d;
		d.SetKeyWithIV(AESDBKEY, sizeof(AESDBKEY), IV);

		StringSource(decoder, true,
			new StreamTransformationFilter(d,
				new StringSink(ret)
			) // StreamTransformationFilter
		); // StringSource
	}
	catch (const CryptoPP::Exception& e)
	{
	}

	return ret;
}