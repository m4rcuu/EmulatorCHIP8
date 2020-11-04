#pragma once

#include "Emulator.hpp"
#include <iostream>
#include <time.h>
#include <fstream>

Emulator::Emulator() {
	initialize();
	std::cout << "emulator has started" << std::endl;
}

Emulator::~Emulator() {
	std::cout << "emulator has closed" << std::endl;
}

void Emulator::initialize() {
	programCounter = 0x200;
	opcode = 0;
	indexRegister = 0;
	stackPointer = 0;

	beepFlag = false;

	clearScreen();
	clearStack();
	clearKeys();
	clearRegisters();
	clearMemory();
	fontsetLoad();

	delay_timer = 0;
	sound_timer = 0;

	srand(time(NULL));
}

template <typename T>
void Emulator::clearT(T* array, unsigned short lenght) {
	for (auto i = 0; i < lenght; ++i) {
		array[i] = 0;
	}
}

void Emulator::clearScreen() {
	clearT(gfx, 2048);
}

void Emulator::clearStack() {
	clearT(stack, 16);
}

void Emulator::clearKeys() {
	clearT(key, 16);
}

void Emulator::clearRegisters() {
	clearT(V, 16);
}

void Emulator::clearMemory() {
	clearT(memory, 4096);
}

void Emulator::fontsetLoad() {
	for (auto i = 0; i < 80; ++i) {
		memory[i] = fontset[i];
	}
}

void Emulator::executeOpcode(unsigned short& opcode) {
	switch (opcode & 0xF000)
	{
	case 0x0000:
		switch (opcode & 0x000F)
		{
		case 0x0000:
			clearScreen();
			programCounter += 2;
			break;
		case 0x000E:
			--stackPointer;
			programCounter = stack[stackPointer];
			programCounter += 2;
			break;
		default:
			std::cout << "error: unknown opcode [0x0000]: 0x" << opcode << std::endl;
			break;
		}
		break;
	case 0x1000:
		programCounter = opcode & 0x0FFF;
		break;
	case 0x2000:
		stack[stackPointer] = programCounter;
		++stackPointer;
		programCounter = opcode & 0x0FFF;
		break;
	case 0x3000:
		if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)) {
			programCounter += 4;
		}
		else {
			programCounter += 2;
		}
		break;
	case 0x4000:
		if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)) {
			programCounter += 4;
		}
		else {
			programCounter += 2;
		}
		break;
	case 0x5000:
		if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4]) {
			programCounter += 4;
		}
		else {
			programCounter += 2;
		}
		break;
	case 0x6000:
		V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
		programCounter += 2;
		break;
	case 0x7000:
		V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
		programCounter += 2;
		break;
	case 0x8000:
		switch (opcode & 0x000F)
		{
		case 0x0000:
			V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
			programCounter += 2;
			break;
		case 0x0001:
			V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
			programCounter += 2;
			break;
		case 0x0002:
			V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
			programCounter += 2;
			break;
		case 0x0003:
			V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
			programCounter += 2;
			break;
		case 0x0004:
			if (V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8])) {
				V[0xF] = 1;
			}
			else {
				V[0xF] = 0;
			}
			V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
			programCounter += 2;
			break;
		case 0x0005:
			if (V[(opcode & 0x00F0) >> 4] > V[(opcode & 0x0F00) >> 8]) {
				V[0xF] = 0;
			}
			else {
				V[0xF] = 1;
			}
			V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
			programCounter += 2;
			break;
		case 0x0006:
			V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x1;
			V[(opcode & 0x0F00) >> 8] >>= 1;
			programCounter += 2;
			break;
		case 0x0007:
			if (V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4]) {
				V[0xF] = 0;
			}
			else {
				V[0xF] = 1;
			}
			V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
			programCounter += 2;
			break;
		case 0x000E:
			V[0xF] = V[(opcode & 0x0F00) >> 8] >> 7;
			V[(opcode & 0x0F00) >> 8] <<= 1;
			programCounter += 2;
			break;
		default:
			std::cout << "error: unknown opcode [0x8000]: 0x" << opcode << std::endl;
			break;
		}
		break;
	case 0x9000:
		if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4]) {
			programCounter += 4;
		}
		else {
			programCounter += 2;
		}
		break;
	case 0xA000:
		indexRegister = opcode & 0x0FFF;
		programCounter += 2;
		break;
	case 0xB000:
		programCounter = (opcode & 0x0FFF) + V[0];
		break;
	case 0xC000:
		V[(opcode & 0x0F00) >> 8] = (rand() % 0xFF) & (opcode & 0x00FF);
		programCounter += 2;
		break;
	case 0xD000:
		updateDrawig(opcode);
		programCounter += 2;
		break;
	case 0xE000:
		switch (opcode & 0x00FF)
		{
		case 0x009E:
			if (key[V[(opcode & 0x0F00) >> 8]] != 0) {
				programCounter += 4;
			}
			else {
				programCounter += 2;
			}
			break;
		case 0x00A1:
			if (key[V[(opcode & 0x0F00) >> 8]] == 0) {
				programCounter += 4;
			}
			else {
				programCounter += 2;
			}
			break;
		default:
			std::cout << "error: unknown opcode [0xE000]: 0x" << opcode << std::endl;
			break;
		}
		break;
	case 0xF000:
		switch (opcode & 0x00FF) {
		case 0x0007:
			V[(opcode & 0x0F00) >> 8] = delay_timer;
			programCounter += 2;
			break;
		case 0x000A:
		{
			auto keyPress = false;
			for (auto i = 0; i < 16; ++i)
			{
				if (key[i] != 0)
				{
					V[(opcode & 0x0F00) >> 8] = i;
					keyPress = true;
				}
			}
			if (!keyPress) {
				return;
			}
			programCounter += 2;
		}
		break;
		case 0x0015:
			delay_timer = V[(opcode & 0x0F00) >> 8];
			programCounter += 2;
			break;
		case 0x018:
			sound_timer = V[(opcode & 0x0F00) >> 8];
			programCounter += 2;
			break;
		case 0x001E:
			if (indexRegister + V[(opcode & 0x0F00) >> 8] > 0xFFF) {
				V[0xF] = 1;
			}
			else {
				V[0xF] = 0;
			}
			indexRegister += V[(opcode & 0x0F00) >> 8];
			programCounter += 2;
			break;
		case 0x029:
			indexRegister = V[(opcode & 0x0F00) >> 8] * 0x5;
			programCounter += 2;
			break;
		case 0x0033:
			memory[indexRegister] = V[(opcode & 0x0F00 >> 8)] / 100;
			memory[indexRegister + 1] = (V[(opcode & 0x0F00 >> 8)] / 10) % 10;
			memory[indexRegister + 2] = (V[(opcode & 0x0F00 >> 8)] % 100) % 10;
			programCounter += 2;
			break;
		case 0x0055:
			for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i) {
				memory[indexRegister + i] = V[i];
			}

			indexRegister += ((opcode & 0x0F00) >> 8) + 1;
			programCounter += 2;
			break;
		case 0x0065:
			for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i) {
				V[i] = memory[indexRegister + i];
			}

			indexRegister += ((opcode & 0x0F00) >> 8) + 1;
			programCounter += 2;
			break;
		default:
			std::cout << "error: unknown opcode [0xF000]: 0x" << opcode << std::endl;
			break;
		}
		break;
	default:
		std::cout << "error: unknown opcode 0x" << opcode << std::endl;
		break;
	}
}

void Emulator::updateTimers() {
	if (delay_timer > 0) {
		--delay_timer;
	}

	if (sound_timer > 0)
	{
		if (sound_timer == 1) {
			beepFlag = true;
			std::cout << "BEEP" << std::endl;
		}
		--sound_timer;
	}
}

void Emulator::updateDrawig(unsigned short& opcode) {
	V[0xF] = 0;
	for (auto yline = 0; yline < (opcode & 0x000F); yline++)
	{
		for (auto xline = 0; xline < 8; xline++)
		{
			if ((memory[indexRegister + yline] & (0x80 >> xline)) != 0)
			{
				if (gfx[(V[(opcode & 0x0F00) >> 8] + xline + ((V[(opcode & 0x00F0) >> 4] + yline) * 64))] == 1)
					V[0xF] = 1;
				gfx[V[(opcode & 0x0F00) >> 8] + xline + ((V[(opcode & 0x00F0) >> 4] + yline) * 64)] ^= 1;
			}
		}
	}
}

void Emulator::consoleRender() {
	for (auto y = 0; y < 32; ++y) {
		for (auto x = 0; x < 64; ++x) {
			if (gfx[(y * 64) + x] == 0) {
				std::cout << "O";
			}
			else {
				std::cout << " ";
			}
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

void Emulator::emulateCycle() {
	opcode = memory[programCounter] << 8 | memory[programCounter + 1];
	executeOpcode(opcode);
	updateTimers();
}

void Emulator::setKey(unsigned short keyNumber, unsigned char isPressed) {
	key[keyNumber] = isPressed;
	emulateCycle();
}


bool Emulator::programLoad(int& argc, char** argv) {
	if (argc <= 1) {
		std::cout << "error: no input file" << std::endl;
		return false;
	}

	std::ifstream file(argv[1], std::ifstream::binary);

	if (!file.is_open()) {
		std::cout << "error: could not read file " << argv[1] << std::endl;
		return false;
	}

	file.seekg(0, file.end);
	auto length = file.tellg();
	file.seekg(0, file.beg);

	auto buffer = new char[length];
	file.read(buffer, length);
	file.close();

	if ((4096 - 512) > length) {
		for (auto i = 0; i < length; ++i) {
			memory[i + 512] = buffer[i];
		}
	}
	else {
		std::cout << "error: ROM too big for memory" << std::endl;
		return false;
	}

	delete[] buffer;

	std::cout << argv[1] << " successfully loaded into memory" << std::endl;
	return true;
}

bool Emulator::canBeep() {
	if (beepFlag) {
		beepFlag = false;
		return true;
	}
	return false;
}

unsigned char* Emulator::getGFX() {
	return gfx;
}