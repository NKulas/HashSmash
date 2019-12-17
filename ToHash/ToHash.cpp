// ToHash.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <conio.h>
#include <vector>
#include <string>

bool* ConvertToBinary(int, int);
void AddBitsToVector(bool*, std::vector<bool>*, int);

int main()
{
	//Get user input
	std::string userInput;
    std::cout << "Enter characters:";
	std::getline(std::cin, userInput);

	//Convert the message to binary
	std::vector<bool>* p_mainMessageInBinary = new std::vector<bool>();
	
	for (int i = 0; i < userInput.length(); i++) {
		int charaterInAsciiDecimal = (int)userInput[i];
		bool* p_charaterInBinary = ConvertToBinary(charaterInAsciiDecimal,8);

		AddBitsToVector(p_charaterInBinary, p_mainMessageInBinary, 8);
	}
	
	//Add padding to the message
	//Factor of 512 bits needed / 8 bits per octet = 64 octets needed
	//Include 8 extra octets in existing message because they will be used below to represent length
	int messageOctets = p_mainMessageInBinary->size() / 8;
	int octetsRemaining = 64 - ((messageOctets + 8) % 64);
	bool first = true;

	for (int i = octetsRemaining; i > 0; i--) {
		if (first) {
			//First octet has a leading one followed by zeros
			AddBitsToVector(ConvertToBinary(128,8), p_mainMessageInBinary, 8);
			first = false;
		}
		else {
			AddBitsToVector(ConvertToBinary(0,8), p_mainMessageInBinary, 8);
		}
	}
	
	//Add 8 octets representing the length to the end
	AddBitsToVector(ConvertToBinary(userInput.length()*8, 64), p_mainMessageInBinary, 64);
	
	//Current termination point
	//The bits of the message to this point are written
	for (int i = 0; i < 512; i++) {
		std::cout << p_mainMessageInBinary->at(i);
	}
	exit;

	//Initialize buffers
	std::vector<bool*> bufferA;
	std::vector<bool*> bufferB;
	std::vector<bool*> bufferC;
	std::vector<bool*> bufferD;

	bufferA.push_back(ConvertToBinary(0x01,8));
	bufferA.push_back(ConvertToBinary(0x23,8));
	bufferA.push_back(ConvertToBinary(0x45,8));
	bufferA.push_back(ConvertToBinary(0x67,8));

	bufferB.push_back(ConvertToBinary(0x89,8));
	bufferB.push_back(ConvertToBinary(0xab,8));
	bufferB.push_back(ConvertToBinary(0xcd,8));
	bufferB.push_back(ConvertToBinary(0xef,8));

	bufferC.push_back(ConvertToBinary(0xfe,8));
	bufferC.push_back(ConvertToBinary(0xdc,8));
	bufferC.push_back(ConvertToBinary(0xba,8));
	bufferC.push_back(ConvertToBinary(0x98,8));

	bufferD.push_back(ConvertToBinary(0x76,8));
	bufferD.push_back(ConvertToBinary(0x54,8));
	bufferD.push_back(ConvertToBinary(0x32,8));
	bufferD.push_back(ConvertToBinary(0x10,8));

	//Begin working with the message

	//foreach 512 bit chunk in the message

	//Get the first 512 bit chunk
	std::vector<bool*> currentLargeChunk;
	for (int i = 0; i < 64; i++) {
		//currentLargeChunk.push_back(binaryMessage[i]);
	}

	std::vector<bool> chunkA;
	std::vector<bool> chunkB;
	std::vector<bool> chunkC;
	std::vector<bool> chunkD;
	//}
}

//Support functions

//Takes a decimal number and returns a pointer to an 8 bit array

bool* ConvertToBinary(int decimalNumber, int numberOfBits) {
	//Convert to binary
	bool* binary = new bool[numberOfBits];

	//Create a vector representing 8 bits
	for (int x = 0; x < numberOfBits; x++) {
		*(binary + x) = 0;
	}

	//For each bit, calculate it binary value, determine if this can be pulled out of the decimal number
	for (int j = (numberOfBits - 1); j >= 0; j--) {
		long long twoMultiple = pow(2, j);
		binary[(numberOfBits - 1) - j] = (int)floor(decimalNumber / twoMultiple);
		decimalNumber = decimalNumber % twoMultiple;
	}
	return binary;
}

void AddBitsToVector(bool* bits, std::vector<bool>* v, int numberOfBits) {
	for (int i = 0; i < numberOfBits; i++) {
		v->push_back(bits[i]);
	}
}