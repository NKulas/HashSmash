// ToHash.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
//Created by Noah Kulas, 2019
//Some parts were taken from https://en.wikipedia.org/wiki/MD5

#include <iostream>
#include <conio.h>
#include <vector>
#include <string>

bool* ConvertToBinary(int, int);
bool* ConvertToBinary(long long, int);
void AddBitsToVector(bool*, std::vector<bool>*, int);
bool* SliceVector(std::vector<bool>*, int, int);
//bool ConvertVectorToArray(std::vector<bool>*);

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
	AddBitsToVector(ConvertToBinary((int)userInput.length()*8, 64), p_mainMessageInBinary, 64);

	//Constants that will be used in buffers
	const bool* A_VALUE = ConvertToBinary((int)0x67452301,32);
	const bool* B_VALUE = ConvertToBinary((int)0xefcdab89, 32);
	const bool* C_VALUE = ConvertToBinary((int)0x98badcfe, 32);
	const bool* D_VALUE = ConvertToBinary((int)0x10325476, 32);

	//Constants that will be added to message
	std::vector<bool*>* K = new std::vector<bool*>;
	for (int i = 0; i < 64; i++) {
		K->push_back(
			ConvertToBinary(
				(long long)(floor(pow(2, 32) * abs(sin(i + 1)))),
			32)
		);
	}

	//foreach 512 bit chunk in the message {

		//Get the first 512 bit chunk
		bool* FivtwelChunk = SliceVector(p_mainMessageInBinary, 0, 511);

		//Break this into 32 bit chuncks
		std::vector<bool*>* MessageHolder = new std::vector<bool*>;
		for (int i = 0; i < 512; i += 32) {
			bool* ThretoChunk = new bool[32];
			for (int j = 0; j < 32; j++) {
				ThretoChunk[j] = FivtwelChunk[i + j];
			}
			MessageHolder->push_back(ThretoChunk);
		}

		//Set buffer values
		bool bufferA = *A_VALUE;
		bool bufferB = *B_VALUE;
		bool bufferC = *C_VALUE;
		bool bufferD = *D_VALUE;

		//Do the actual operations
		bool f;
		int m;
		for (int i = 0; i < 64; i++) {
			if (i >= 0 && i <= 15) {
				f = ((bufferB & bufferC) | ((~bufferB) & bufferD));
				m = i;
			}
			else if (i >= 16 && i <= 31) {
				f = ((bufferD & bufferB) | ((~bufferD) & bufferC));
				m = (((5 * i) + 1) % 16);
			}
			else if (i >= 32 && i <= 47) {
				f = (bufferB ^ bufferC ^ bufferD);
				m = (((3 * i) + 5) % 16);
			}
			else if (i >= 48 && i <= 63) {
				f = (bufferC ^ (bufferB | (~bufferD)));
				m = ((7 * i) % 16);
			}
		}
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

bool* ConvertToBinary(long long decimalNumber, int numberOfBits) {
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

//Takes a pointer to an array of bits and adds them to a vector
void AddBitsToVector(bool* bits, std::vector<bool>* v, int numberOfBits) {
	for (int i = 0; i < numberOfBits; i++) {
		v->push_back(bits[i]);
	}
}

//Creates an array from part of a vector
bool* SliceVector(std::vector<bool>* v, int startIndex, int endIndex) {
	bool* returnArray = new bool[endIndex - startIndex + 1];

	for (int i = startIndex; i <= endIndex; i++) {
		returnArray[i - startIndex] = v->at(i);
	}
	return returnArray;
}

//Takes a vector and makes an array
/*bool ConvertVectorToArray(std::vector<bool>* v) {
	bool* returnArray = new bool[v->size];

	for (int i = 0; i < v->size; i++) {
		returnArray[i] = v->at(i);
	}
	return returnArray;
}*/