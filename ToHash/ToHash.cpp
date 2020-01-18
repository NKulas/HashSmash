// ToHash.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
//Created by Noah Kulas, 2019
//Some parts were taken from https://en.wikipedia.org/wiki/MD5

#include <iostream>
#include <conio.h>
#include <vector>
#include <string>

uint8_t* ConvertToBinary(int, int);
uint8_t* ConvertToBinary(long long, int);
void AddBitsToVector(uint8_t*, std::vector<uint8_t>*, int);
uint8_t* SliceVector(std::vector<uint8_t>*, int, int);
void LeftRotate(uint8_t*, int);
uint8_t* ModularBinaryAdd(uint8_t*, uint8_t*);
//bool ConvertVectorToArray(std::vector<bool>*);

int main()
{
	//Get user input
	std::string userInput;
    std::cout << "Enter characters:";
	std::getline(std::cin, userInput);

	//Convert the message to binary
	std::vector<uint8_t>* p_mainMessageInBinary = new std::vector<uint8_t>();
	
	for (int i = 0; i < userInput.length(); i++) {
		int charaterInAsciiDecimal = (int)userInput[i];
		uint8_t* p_charaterInBinary = ConvertToBinary(charaterInAsciiDecimal,8);

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
	//Values in hex are: A: 01234567, B: 89abcdef, C: fedcba98, D: 76543210
	const uint8_t* A_VALUE = ConvertToBinary((int)0x67452301,32);
	const uint8_t* B_VALUE = ConvertToBinary((int)0xefcdab89, 32);
	const uint8_t* C_VALUE = ConvertToBinary((int)0x98badcfe, 32);
	const uint8_t* D_VALUE = ConvertToBinary((int)0x10325476, 32);

	//Constants that will be added to message
	std::vector<uint8_t*>* K = new std::vector<uint8_t*>;
	for (int i = 0; i < 64; i++) {
		K->push_back(
			ConvertToBinary(
				(long long)(floor(pow(2, 32) * abs(sin(i + 1)))),
			32)
		);
	}

	//Running hash
	uint8_t* HashA = new uint8_t[32];
	uint8_t* HashB = new uint8_t[32];
	uint8_t* HashC = new uint8_t[32];
	uint8_t* HashD = new uint8_t[32];

	for (int i = 0; i < 32; i++) {
		HashA[i] = A_VALUE[i];
		HashB[i] = B_VALUE[i];
		HashC[i] = C_VALUE[i];
		HashD[i] = D_VALUE[i];
	}

	//Shift amounts for each round
	int* ShiftAmount = new int[64]{ 7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22, 5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20, 4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23, 6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21 };

	//foreach 512 bit chunk in the message {

		//Get the first 512 bit chunk
		uint8_t* FivtwelChunk = SliceVector(p_mainMessageInBinary, 0, 511);

		//Break this into 32 bit chuncks
		std::vector<uint8_t*>* MessageHolder = new std::vector<uint8_t*>;
		for (int i = 0; i < 512; i += 32) {
			uint8_t* ThretoChunk = new uint8_t[32];
			for (int j = 0; j < 32; j++) {
				ThretoChunk[j] = FivtwelChunk[i + j];
			}
			MessageHolder->push_back(ThretoChunk);
		}

		//Set buffer values
		uint8_t* bufferA = new uint8_t[32];
		uint8_t* bufferB = new uint8_t[32];
		uint8_t* bufferC = new uint8_t[32];
		uint8_t* bufferD = new uint8_t[32];

		for (int i = 0; i < 32; i++) {
			bufferA[i] = A_VALUE[i];
			bufferB[i] = B_VALUE[i];
			bufferC[i] = C_VALUE[i];
			bufferD[i] = D_VALUE[i];
		}

		//Put the buffers through the transform functions
		uint8_t* f = new uint8_t[32];
		int m;
		for (int i = 0; i < 64; i++) {
			for (int j = 0; j < 32; j++) {
				if (i >= 0 && i <= 15) {
					f[j] = ((bufferB[j] & bufferC[j]) | ((~bufferB[j]) & bufferD[j]));
					m = i;
				}
				else if (i >= 16 && i <= 31) {
					f[j] = ((bufferD[j] & bufferB[j]) | ((~bufferD[j]) & bufferC[j]));
					m = (((5 * i) + 1) % 16);
				}
				else if (i >= 32 && i <= 47) {
					f[j] = (bufferB[j] ^ bufferC[j] ^ bufferD[j]);
					m = (((3 * i) + 5) % 16);
				}
				else if (i >= 48 && i <= 63) {
					f[j] = (bufferC[j] ^ (bufferB[j] | (~bufferD[j])));
					m = ((7 * i) % 16);
				}
			}

			//Bitwise add bufferA, the result of the above functions, the appropriate constant, and the appropriate message block
			for (int k = 0; k < 32; k++) {
				uint8_t* rendom2 = ModularBinaryAdd(K->at(i), MessageHolder->at(m));
				uint8_t* rendom1 = ModularBinaryAdd(bufferA, rendom2);
				f = ModularBinaryAdd(f, rendom1);
			}

			LeftRotate(f, ShiftAmount[i]);

			//Rotate the buffers
			for (int m = 0; m < 32; m++) {
				bufferA[m] = bufferD[m];
				bufferD[m] = bufferC[m];
				bufferC[m] = bufferB[m];
				bufferB[m] = bufferB[m] + f[m];
			}
		}

		//Add the result of this 64 operation round to the running hash
		for (int i = 0; i < 32; i++) {
			HashA = ModularBinaryAdd(HashA, bufferA);
			HashB = ModularBinaryAdd(HashB, bufferB);
			HashC = ModularBinaryAdd(HashC, bufferC);
			HashD = ModularBinaryAdd(HashD, bufferD);
		}
	//}

	//FOR TESTING
	//Possibly the final result
	for (int z = 0; z < 32; z++) {
		std::cout << (double)HashA[z];
	}

	for (int z = 0; z < 32; z++) {
		std::cout << (double)HashB[z];
	}

	for (int z = 0; z < 32; z++) {
		std::cout << (double)HashC[z];
	}

	for (int z = 0; z < 32; z++) {
		std::cout << (double)HashD[z];
	}
}

//Support functions

//Takes a decimal number and returns a pointer to an 8 bit array
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

//Takes a pointer to an array of bits and adds them to a vector
void AddBitsToVector(uint8_t* bits, std::vector<uint8_t>* v, int numberOfBits) {
	for (int i = 0; i < numberOfBits; i++) {
		v->push_back(bits[i]);
	}
}

//Creates an array from part of a vector
uint8_t* SliceVector(std::vector<uint8_t>* v, int startIndex, int endIndex) {
	uint8_t* returnArray = new uint8_t[endIndex - startIndex + 1];

	for (int i = startIndex; i <= endIndex; i++) {
		returnArray[i - startIndex] = v->at(i);
	}
	return returnArray;
}

uint8_t* ModularBinaryAdd(uint8_t* number1, uint8_t* number2) {
	uint8_t* returnArray = new uint8_t[32];
	bool carry = false;

	for (int i = 31; i >= 0; i--) {
		uint8_t result = number1[i] + number2[i];

		if (carry) result += 1;

		returnArray[i] = result % 2;

		if (result > 1) carry = true;
		else carry = false;
	}

	//TODO modulus is not working right
	
	bool continueMod = true;

	while (continueMod) {
		bool doSubtract = false;

		if (carry) {
			carry = false;
			returnArray[0] = 1;
			doSubtract = true;
		}
		else if (returnArray[0] == 1) {
			doSubtract = true;
		}
		else {
			continueMod = false;
		}

		if (doSubtract) {
			//Create the modulus comparison
			uint8_t* modulus = new uint8_t[32]{ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

			//Do the subtraction
			for (int i = 31; i >= 0; i--) {
				if (returnArray[i] >= modulus[i]) {
					returnArray[i] = returnArray[i] - modulus[i];
				}
				else {
					int counter = 1;
					bool continueBorrow = true;

					while (continueBorrow) {
						if (returnArray[i + counter] >= modulus[i]) {
							returnArray[i + counter] -= 1;
							returnArray[i] += 1;
							continueBorrow = false;
						}
						else {
							counter += 1;
						}
					}
					returnArray[i] = returnArray[i] - modulus[i];
				}
			}
		}

		return returnArray;
	}
}

void LeftRotate(uint8_t* thingToRotate, int numberOfBits) {
	for (int i = 0; i < numberOfBits; i++) {
		uint8_t temporaryHolder = thingToRotate[0];

		for (int j = 0; j < 32; j++) {
			if (j < 31) {
				thingToRotate[j] = thingToRotate[j + 1];
			}
			else {
				thingToRotate[j] = temporaryHolder;
			}
		}
	}
}

//Takes a vector and makes an array
/*bool ConvertVectorToArray(std::vector<bool>* v) {
	bool* returnArray = new bool[v->size];

	for (int i = 0; i < v->size; i++) {
		returnArray[i] = v->at(i);
	}
	return returnArray;
}*/