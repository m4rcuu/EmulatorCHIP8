#pragma once

class Emulator {
private:
	unsigned short opcode;

	unsigned char memory[4096];

	unsigned char V[16];

	unsigned short indexRegister;
	unsigned short programCounter;

	unsigned char gfx[2048];

	unsigned char delay_timer;
	unsigned char sound_timer;

	unsigned short stack[16];
	unsigned short stackPointer;

	unsigned char fontset[80];
	unsigned char key[16];

	bool beepFlag;

	void initialize();

	template<typename T>
	void clearT(T* array, unsigned short lenght);

	void clearScreen();
	void clearStack();
	void clearKeys();
	void clearRegisters();
	void clearMemory();
	void fontsetLoad();
	void executeOpcode(unsigned short& opcode);
	void updateTimers();
	void updateDrawig(unsigned short& opcode);
	void consoleRender();

public:
	Emulator();
	~Emulator();

	void emulateCycle();
	void setKey(unsigned short keyNumber, unsigned char isPressed);
	bool programLoad(int& argc, char** argv);
	bool canBeep();
	unsigned char* getGFX();
};