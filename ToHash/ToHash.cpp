// ToHash.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
//Created by Noah Kulas, 2019

//The following sources were very useful to me in making this and deserve credit:
//Some code was taken from Wikipedia: https://en.wikipedia.org/wiki/MD5
//Official IETF md5 specification: https://www.ietf.org/rfc/rfc1321.txt
//Dr. Herong Yang's cryptography tutorial: http://www.herongyang.com/Cryptography/MD5-Message-Digest-Algorithm-Overview.html
//Md5 padding demo: https://fthb321.github.io/MD5-Hash/MD5OurVersion2.html
//University of Nebraska - Lincoln md5 demo: https://cse.unl.edu/~ssamal/crypto/genhash.php

#include <iostream>
#include <conio.h>
#include <vector>
#include <string>

uint8_t* ConvertToBinary(int, int);
uint8_t* ConvertToBinary(long long, int);
void AddBitsToVector(uint8_t*, std::vector<uint8_t>*, int);
uint8_t* SliceVector(std::vector<uint8_t>*, int, int);
uint8_t* ChangeEndianess(uint8_t*, int);
void LeftRotate(uint8_t*, int);
uint8_t* ModularBinaryAdd(uint8_t*, uint8_t*);

int main()
{
	//Get user input
	std::string userInput;
    std::cout << "Enter characters:";
	std::getline(std::cin, userInput);

	//Convert the message to binary
	std::vector<uint8_t>* mainMessageInBinary = new std::vector<uint8_t>();
	
	for (int i = 0; i < userInput.length(); i++) {
		int charaterInAsciiDecimal = (int)userInput[i];
		uint8_t* p_charaterInBinary = ConvertToBinary(charaterInAsciiDecimal,8);

		AddBitsToVector(p_charaterInBinary, mainMessageInBinary, 8);
	}

	//Add padding to the message
	//Factor of 512 bits needed / 8 bits per octet = 64 octets needed
	//Include 8 extra octets in existing message because they will be used below to represent length
	int messageOctets = mainMessageInBinary->size() / 8;
	int octetsRemaining = 64 - ((messageOctets + 8) % 64);
	bool first = true;

	for (int i = octetsRemaining; i > 0; i--) {
		if (first) {
			//First octet has a leading one followed by zeros
			AddBitsToVector(ConvertToBinary(128,8), mainMessageInBinary, 8);
			first = false;
		}
		else {
			AddBitsToVector(ConvertToBinary(0,8), mainMessageInBinary, 8);
		}
	}

	//Invert endianess
	int size = mainMessageInBinary->size();
	
	std::vector<uint8_t>* newMessage = new std::vector<uint8_t>();
	for (int i = 0; i <= size - 32; i += 32) {
		 uint8_t* littleEndianByte = ChangeEndianess(SliceVector(mainMessageInBinary, i, i + 31), 32);

		 for (int j = 0; j < 32; j++) {
			 newMessage->push_back((int)littleEndianByte[j]);
		 }
	}
	mainMessageInBinary = newMessage;
	
	//Add 8 octets representing the length to the end
	uint8_t* lengthInBinary = ConvertToBinary((int)userInput.length() * 8, 64);
	uint8_t* lowOrderBits = new uint8_t[32];
	uint8_t* highOrderBits = new uint8_t[32];

	for (int i = 32; i < 64; i++) {
		lowOrderBits[(i - 32)] = lengthInBinary[i];
	}

	for (int i = 0; i < 32; i++) {
		highOrderBits[i] = lengthInBinary[i];
	}

	AddBitsToVector(lowOrderBits, mainMessageInBinary, 32);
	AddBitsToVector(highOrderBits, mainMessageInBinary, 32);

	//Set constants that will be used for the values in the buffers
	//A: 67452301 B: efcdab89 C: 98badcfe D: 10325476
	const uint8_t* A_VALUE = new uint8_t[32]{ 0,1,1,0,0,1,1,1,0,1,0,0,0,1,0,1,0,0,1,0,0,0,1,1,0,0,0,0,0,0,0,1 };
	const uint8_t* B_VALUE = new uint8_t[32]{ 1,1,1,0,1,1,1,1,1,1,0,0,1,1,0,1,1,0,1,0,1,0,1,1,1,0,0,0,1,0,0,1 };
	const uint8_t* C_VALUE = new uint8_t[32]{ 1,0,0,1,1,0,0,0,1,0,1,1,1,0,1,0,1,1,0,1,1,1,0,0,1,1,1,1,1,1,1,0 };
	const uint8_t* D_VALUE = new uint8_t[32]{ 0,0,0,1,0,0,0,0,0,0,1,1,0,0,1,0,0,1,0,1,0,1,0,0,0,1,1,1,0,1,1,0 };

	//Constants that will be added to message
	std::vector<uint8_t*>* K = new std::vector<uint8_t*>;
	for (int i = 0; i < 64; i++) {
		K->push_back(
			ConvertToBinary(
				(long long)(pow(2,32) * abs(sin(i+1))),
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
	int* ShiftAmount = new int[64]{ 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21 };

	//Prepare buffers
	uint8_t* bufferA = new uint8_t[32];
	uint8_t* bufferB = new uint8_t[32];
	uint8_t* bufferC = new uint8_t[32];
	uint8_t* bufferD = new uint8_t[32];

	//Set the starting values of the hash chuncks
	//These are later used in the buffers
	for (int i = 0; i < 32; i++) {
		HashA[i] = A_VALUE[i];
		HashB[i] = B_VALUE[i];
		HashC[i] = C_VALUE[i];
		HashD[i] = D_VALUE[i];
	}

	//Foreach 512 bit chunk in the message
	for (int z = 0; z < mainMessageInBinary->size(); z += 512) {

		//Get this 512 bit chunk
		uint8_t* FivtwelChunk = SliceVector(mainMessageInBinary, z, z + 511);

		//Break this into 32 bit chuncks
		std::vector<uint8_t*>* MessageHolder = new std::vector<uint8_t*>;
		for (int i = 0; i < 512; i += 32) {
			uint8_t* ThretoChunk = new uint8_t[32];
			for (int j = 0; j < 32; j++) {
				ThretoChunk[j] = FivtwelChunk[i + j];
			}
			MessageHolder->push_back(ThretoChunk);
		}

		//Start the buffers with the current values in the hash
		for (int i = 0; i < 32; i++) {
			bufferA[i] = HashA[i];
			bufferB[i] = HashB[i];
			bufferC[i] = HashC[i];
			bufferD[i] = HashD[i];
		}

		//Put the buffers through the transform functions
		uint8_t* f = new uint8_t[32];
		int m;

		for (int i = 0; i < 64; i++) {
			for (int j = 0; j < 32; j++) {
				if (i >= 0 && i <= 15) {
					f[j] = ((bufferB[j] && bufferC[j]) || ((!bufferB[j]) && bufferD[j]));
					m = i;
				}
				else if (i >= 16 && i <= 31) {
					f[j] = ((bufferD[j] && bufferB[j]) || ((!bufferD[j]) && bufferC[j]));
					m = (((5 * i) + 1) % 16);
				}
				else if (i >= 32 && i <= 47) {
					f[j] = (!(!bufferB[j] != !bufferC[j]) != !bufferD[j]);
					m = (((3 * i) + 5) % 16);
				}
				else if (i >= 48 && i <= 63) {
					f[j] = (!bufferC[j] != !(bufferB[j] || (!bufferD[j])));
					m = ((7 * i) % 16);
				}
			}

			//Bitwise modular add bufferA, the result of the above functions, the appropriate constant, and the appropriate message block
			uint8_t* firstLevel = ModularBinaryAdd(K->at(i), MessageHolder->at(m));
			uint8_t* secondLevel = ModularBinaryAdd(bufferA, firstLevel);
			f = ModularBinaryAdd(f, secondLevel);

			LeftRotate(f, ShiftAmount[i]);

			//Shuffle the buffers
			bufferA = bufferD;
			bufferD = bufferC;
			bufferC = bufferB;
			bufferB = ModularBinaryAdd(bufferB, f);
		}

		//Add the result of this 64 operation round to the running hash
		HashA = ModularBinaryAdd(HashA, bufferA);
		HashB = ModularBinaryAdd(HashB, bufferB);
		HashC = ModularBinaryAdd(HashC, bufferC);
		HashD = ModularBinaryAdd(HashD, bufferD);
	}

	//Convert back to big endian
	std::vector<uint8_t>* littleEndianVector = new std::vector<uint8_t>();

	for (int i = 0; i < 32; i++) {
		littleEndianVector->push_back(HashA[i]);
	}
	for (int i = 0; i < 32; i++) {
		littleEndianVector->push_back(HashB[i]);
	}
	for (int i = 0; i < 32; i++) {
		littleEndianVector->push_back(HashC[i]);
	}
	for (int i = 0; i < 32; i++) {
		littleEndianVector->push_back(HashD[i]);
	}

	std::vector<uint8_t>* bigEndianVector = new std::vector<uint8_t>();
	for (int i = 0; i < 128; i += 32) {
		uint8_t* littleEndianByte = ChangeEndianess(SliceVector(littleEndianVector, i, i + 31), 32);

		for (int j = 0; j < 32; j++) {
			bigEndianVector->push_back((int)littleEndianByte[j]);
		}
	}

	//Output the final result
	for (int z = 0; z < 128; z++) {
		std::cout << (int)bigEndianVector->at(z);
	}

	//Cleanup the heap
	//I know this does not cover them all; that is still in the works
	delete mainMessageInBinary;
	//delete newMessage;
	delete[] lengthInBinary;
	delete[] lowOrderBits;
	delete[] highOrderBits;
	delete[] A_VALUE;
	delete[] B_VALUE;
	delete[] C_VALUE;
	delete[] D_VALUE;
	delete[] HashA;
	delete[] HashB;
	delete[] HashC;
	delete[] HashD;
	delete[] bufferA;
	delete[] bufferB;
	delete[] bufferC;
	delete[] bufferD;
	delete K;
	delete[] ShiftAmount;
	delete littleEndianVector;
	delete bigEndianVector;
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

uint8_t* ChangeEndianess(uint8_t* bitList, int bitListLength) {
	uint8_t* newList = new uint8_t[bitListLength];

	int newIndex = 0;
	for (int i = (bitListLength) - 8; i >= 0; i -= 8) {

		for (int j = 0; j < 8; j++) {
			newList[newIndex + j] = bitList[i + j];
			//int a = newList[newIndex + j];
			//std::cout << (int)a;
		}
		newIndex += 8;
	}

	return newList;
}

uint8_t* ModularBinaryAdd(uint8_t* number1, uint8_t* number2) {
	long long* decimalNumber1 = new long long();
	long long* decimalNumber2 = new long long();

	//Convert to decimal
	for (int i = 31; i >= 0; i--) {
		*decimalNumber1 += (pow(2, 31 - i) * number1[i]);
		*decimalNumber2 += (pow(2, 31 - i) * number2[i]);
	}

	//Add and check modulus
	long long* sum = new long long();
	*sum = *decimalNumber1 + *decimalNumber2;

	long long* remainder = new long long();
	long long* modulus = new long long();
	*modulus = pow(2, 32);
	*remainder = *sum % *modulus;
	
	//Convert back to binary to return
	return ConvertToBinary(*remainder, 32);
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