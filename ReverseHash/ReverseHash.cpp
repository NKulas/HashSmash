// ReverseHash.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
//Created by Noah Kulas, 2020

#include <iostream>
#include <conio.h>
#include <vector>
#include <string>

uint8_t* ConvertHexToBinary(std::string);
uint8_t* ChangeEndianess(uint8_t*, int);

const char* hexValues = new char[16]{ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

int main()
{
	//Get user input
	std::string userInput;
	std::cout << "Enter hex value of hash:";
	std::getline(std::cin, userInput);

	uint8_t* oppositeEndianMessage = ConvertHexToBinary(userInput);
	uint8_t* mainMessageInBinary = new uint8_t[128];

	for (int i = 0; i < 128; i += 32) {
		uint8_t* chunk = new uint8_t[32];
		for (int j = 0; j < 32; j++) {
			chunk[j] = oppositeEndianMessage[i + j];
		}
		
		uint8_t* littleEndianChunk = ChangeEndianess(chunk, 32);

		for (int k = 0 + i; k < i + 32; k++) {
			mainMessageInBinary[k] = littleEndianChunk[k - i];
		}
	}

	//Current termination point
	//Outputs the binary representation of the hash, ready to be reversed
	for (int z = 0; z < 128; z++) {
		std::cout << (int)mainMessageInBinary[z];
	}
}

//Support functions

uint8_t* ConvertHexToBinary(std::string hex) {
	//Create the binary array
	uint8_t* binary = new uint8_t[128];

	for (int x = 0; x < 128; x++) {
		*(binary + x) = 0;
	}

	//Calculate decimal number
	int decimalNumber = 0;
	for (int i = 0; i < 32; i++) {
		for (int j = 0; j < 16; j++) {
			if (std::toupper(hex[i]) == hexValues[j]) {
				decimalNumber += (j * pow(16, (i + 1) % 2));
			}
		}

		//Convert to binary
		if ((i % 2) != 0) {
			int twoPlace = 7;
			for (int k = ((4 * i) - 4); k < (((4 * i) - 4) + 8); k++) {
				long long twoMultiple = pow(2, twoPlace);
				binary[k] = (int)floor(decimalNumber / twoMultiple);
				decimalNumber = decimalNumber % twoMultiple;
				twoPlace--;
			}
		}
	}
	return binary;
}

uint8_t* ChangeEndianess(uint8_t* bitList, int bitListLength) {
	uint8_t* newList = new uint8_t[bitListLength];

	int newIndex = 0;
	for (int i = (bitListLength)-8; i >= 0; i -= 8) {

		for (int j = 0; j < 8; j++) {
			newList[newIndex + j] = bitList[i + j];
			//int a = newList[newIndex + j];
			//std::cout << (int)a;
		}
		newIndex += 8;
	}
	return newList;
}