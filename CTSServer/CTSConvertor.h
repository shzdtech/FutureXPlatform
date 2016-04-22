#pragma once
#include <string>
#include <iostream>
using namespace std;
using namespace System;

public ref class CTSConvertor
{
public:
	static void MarshalString(String^ s, string& os);
};

