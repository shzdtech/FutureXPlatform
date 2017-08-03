// CryptoApp.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <iostream>
#include "../databaseop/AESCrypto.h"

void testCrypto(const std::string& plain)
{
	AESCrypto e;
	auto cipher = e.Encrypt(plain);
	std::cout << "Encryption: " << cipher << std::endl;
	auto dec = e.Descrypt(cipher);
	std::cout << "Decryption: " << dec << std::endl;
}

int main()
{
	std::cout << "Please input text to encrypt: ";

	std::string line;

	std::getline(std::cin, line);
		
	testCrypto(line);

	std::getline(std::cin, line);

	return 0;
}

