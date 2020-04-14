// ReverseHash.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
//Created by Noah Kulas, 2020

#include <iostream>
#include <conio.h>
#include <vector>
#include <string>
#include <bitset>

uint8_t* ConvertHexToBinary(std::string);
uint8_t* ChangeEndianess(uint8_t*, int);
uint8_t* ModularBinarySubtract(uint8_t*, uint8_t*);
uint8_t* BinarySubtract(uint8_t*, uint8_t*);
uint8_t* BinaryAdd(uint8_t*, uint8_t*);
void RightRotate(uint8_t*, int);
uint8_t* ConvertToBinary(int, int);
uint8_t* ConvertToBinary(long long, int);

const char* hexValues = new char[16]{ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

int main()
{
	//Get user input
	std::string userInput;
	std::cout << "Enter hex value of hash:";
	std::getline(std::cin, userInput);

	//Create binary message
	uint8_t* wrongEndianMessage = ConvertHexToBinary(userInput);
	uint8_t* mainMessageInBinary = new uint8_t[128];

	for (int i = 0; i < 128; i += 32) {
		uint8_t* chunk = new uint8_t[32];
		for (int j = 0; j < 32; j++) {
			chunk[j] = wrongEndianMessage[i + j];
		}
		
		uint8_t* littleEndianChunk = ChangeEndianess(chunk, 32);

		for (int k = 0 + i; k < i + 32; k++) {
			mainMessageInBinary[k] = littleEndianChunk[k - i];
		}
	}

	//Prepare buffers
	uint8_t* bufferA = new uint8_t[32];
	uint8_t* bufferB = new uint8_t[32];
	uint8_t* bufferC = new uint8_t[32];
	uint8_t* bufferD = new uint8_t[32];

	//Split message into buffers
	for (int i = 0; i < 32; i++) {
		bufferA[i] = mainMessageInBinary[i];
	}

	for (int i = 32; i < 64; i++) {
		bufferB[i - 32] = mainMessageInBinary[i];
	}

	for (int i = 64; i < 96; i++) {
		bufferC[i - 64] = mainMessageInBinary[i];
	}

	for (int i = 96; i < 128; i++) {
		bufferD[i - 96] = mainMessageInBinary[i];
	}

	//Set initalizer values
	//A: 67452301 B: efcdab89 C: 98badcfe D: 10325476
	uint8_t* startA = new uint8_t[32] { 0,1,1,0,0,1,1,1,0,1,0,0,0,1,0,1,0,0,1,0,0,0,1,1,0,0,0,0,0,0,0,1 };
	uint8_t* startB = new uint8_t[32] { 1,1,1,0,1,1,1,1,1,1,0,0,1,1,0,1,1,0,1,0,1,0,1,1,1,0,0,0,1,0,0,1 };
	uint8_t* startC = new uint8_t[32] { 1,0,0,1,1,0,0,0,1,0,1,1,1,0,1,0,1,1,0,1,1,1,0,0,1,1,1,1,1,1,1,0 };
	uint8_t* startD = new uint8_t[32] { 0,0,0,1,0,0,0,0,0,0,1,1,0,0,1,0,0,1,0,1,0,1,0,0,0,1,1,1,0,1,1,0 };

	int* ShiftAmount = new int[64]{ 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21 };

	//Undo adding initializer values to buffers
	bufferA = ModularBinarySubtract(bufferA, startA);
	bufferB = ModularBinarySubtract(bufferB, startB);
	bufferC = ModularBinarySubtract(bufferC, startC);
	bufferD = ModularBinarySubtract(bufferD, startD);

	uint8_t* activeBranch;
	for (int round = 63; round >= 0; round--) {
		//Reverse shuffle the buffers
		activeBranch = bufferB;
		bufferB = bufferC;
		bufferC = bufferD;
		bufferD = bufferA;

		//Undo adding buffer B
		activeBranch = ModularBinarySubtract(activeBranch, bufferB);

		//Undo bit rotation
		RightRotate(activeBranch, ShiftAmount[round]);

		//Subtract constant
		uint8_t* constant = ConvertToBinary((long long)(pow(2, 32) * abs(sin(round + 1))), 32);
		activeBranch = ModularBinarySubtract(activeBranch, constant);

		//Change the order and subtract the result of the transform functions first
		uint8_t* f = new uint8_t[32];
		int m;

		for (int j = 0; j < 32; j++) {
			if (round >= 0 && round <= 15) {
				//(B and C) or (~B and D)
				f[j] = ((bufferB[j] && bufferC[j]) || ((!bufferB[j]) && bufferD[j]));
				m = round;
			}
			else if (round >= 16 && round <= 31) {
				//(D and B) or (~D and C)
				f[j] = ((bufferD[j] && bufferB[j]) || ((!bufferD[j]) && bufferC[j]));
				m = (((5 * round) + 1) % 16);
			}
			else if (round >= 32 && round <= 47) {
				//B xor C xor D
				f[j] = (!(!bufferB[j] != !bufferC[j]) != !bufferD[j]);
				m = (((3 * round) + 5) % 16);
			}
			else if (round >= 48 && round <= 63) {
				//C xor (B or ~D)
				f[j] = (!bufferC[j] != !(bufferB[j] || (!bufferD[j])));
				m = ((7 * round) % 16);
			}
		}

		activeBranch = ModularBinarySubtract(activeBranch, f);

		
		//activeBranch now contains the sum of the message chunk and bufferA from the round (numerically) before
		for (int z = 0; z < 32; z++) {
			std::cout << (int)activeBranch[z];
		}
		std::cout << "\n";

		//The concatination of the message chunck and bufferA for round 63 is the current termination point
		//Cleanup the heap
		delete[] bufferA;
		delete[] bufferB;
		delete[] bufferC;
		//delete[] bufferD; this still points to the same object as bufferA, so it is already deleted
		delete[] startA;
		delete[] startB;
		delete[] startC;
		delete[] startD;
		delete[] ShiftAmount;
		delete[] f;
		delete[] activeBranch;
		delete[] constant;
		
		//Terminate
		return 0;
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

uint8_t* ModularBinarySubtract(uint8_t* number1, uint8_t* number2) {
	//Create new arrays to avoid problems with this function changing the values of the ones passed in
	uint8_t* number1prime = new uint8_t[32];
	uint8_t* number2prime = new uint8_t[32];
	for (int i = 0; i < 32; i++) {
		number1prime[i] = number1[i];
	}

	for (int i = 0; i < 32; i++) {
		number2prime[i] = number2[i];
	}

	//Determine which is greater
	int greaterNumber;
	bool areEqual = true;
	uint8_t* greater = new uint8_t;
	uint8_t* lesser = new uint8_t;

	for (int i = 0; i < 32; i++) {
		if (number1prime[i] > number2prime[i]) {
			areEqual = false;
			greaterNumber = 1;
			greater = number1prime;
			lesser = number2prime;
			break;
		}
		else if (number2[i] > number1[i]) {
			areEqual = false;
			greaterNumber = 2;
			greater = number2prime;
			lesser = number1prime;
			break;
		}
	}

	if (areEqual) {
		return new uint8_t[32]{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	}
	else {
		//Subtract the two
		uint8_t* result = BinarySubtract(greater, lesser);

		//Determine if the result is negative or positive
		if (greaterNumber == 1) {
			return result;
		}
		else {
			//If negative, subtract absolute value from modulus
			//Adding one to the result is easier than representing the modulus with 33 bits
			uint8_t* modulus = new uint8_t[32]{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
			uint8_t* one = new uint8_t[32]{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 };

			uint8_t* intermediateResult = BinarySubtract(modulus, result);
			uint8_t* finalResult = BinaryAdd(intermediateResult, one);

			return finalResult;
		}
	}
}

uint8_t* BinarySubtract(uint8_t* number1, uint8_t* number2) {
	uint8_t* result = new uint8_t[32];

	for (int i = 31; i >= 0; i--) {
		if (number1[i] < number2[i]) {
			for (int j = i - 1; j >= 0; j--) {
				if (number1[j] == 1) {
					number1[j] = 0;

					j++;
					while (j < i) {
						number1[j] = 1;
						j++;
					}

					number1[i] = 1;
					number2[i] = 0;
					result[i] = 1;

					break;
				}
			}
		}
		else {
			result[i] = number1[i] - number2[i];
		}
	}

	return result;
}

uint8_t* BinaryAdd(uint8_t* number1, uint8_t* number2) {
	uint8_t* result = new uint8_t[32];
	uint8_t carry = 0;

	for (int i = 31; i >= 0; i--) {
		uint8_t sum = number1[i] + number2[i] + carry;
		result[i] = sum % 2;
		carry = floor(sum / 2);
	}

	return result;
}

void RightRotate(uint8_t* thingToRotate, int numberOfBits) {
	for (int i = 0; i < numberOfBits; i++) {
		uint8_t temporaryHolder = thingToRotate[31];

		for (int j = 31; j >= 0; j--) {
			if (j > 0) {
				thingToRotate[j] = thingToRotate[j - 1];
			}
			else {
				thingToRotate[j] = temporaryHolder;
			}
		}
	}
}

uint8_t* ConvertToBinary(int decimalNumber, int numberOfBits) {
	//Convert to binary
	uint8_t* binary = new uint8_t[numberOfBits];

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

uint8_t* ConvertToBinary(long long decimalNumber, int numberOfBits) {
	//Convert to binary
	uint8_t* binary = new uint8_t[numberOfBits];

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

uint8_t FindUnknown(uint8_t item1, uint8_t item2) {
	return 0;
}