#pragma once
#include <fstream>
#include <stdlib.h>
#include <iostream>

class Chip8 {
public:

	unsigned short opcode = 0;
	unsigned char memory[4096];
	unsigned char V[16];
	unsigned short I = 0;
	unsigned short pc = 0;

	/*
	0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
	0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
	0x200-0xFFF - Program ROM and work RAM
	*/

	unsigned char gfx[64 * 32];
	unsigned char delayTimer;
	unsigned char soundTimer;
	bool drawFlag = false;

	unsigned short stack[16];
	unsigned short sp;

	unsigned char key[16];
	unsigned char chip8_fontset[80] =
	{
	  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	  0x20, 0x60, 0x20, 0x20, 0x70, // 1
	  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};

public:
	void initialize() {
		pc = 0x200;
		opcode = 0;
		I = 0;
		sp = 0;

		// Clear display	
		// Clear stack
		// Clear registers V0-VF
		// Clear memory
		for (int i = 0; i < 16; i++) {
			V[i] = 0;
		}
		for (int i = 0; i < 4096; ++i) {
			memory[i] = 0;
		}
		for (int i = 0; i < 80; ++i) {
			memory[i] = chip8_fontset[i];
		}
		for (int i = 0; i < 16; i++) {
			key[i] = 0;
		}

		// REset timers

		srand(time(NULL));
	}

	void loadRom(const char* path) {
		std::ifstream file(path, std::ios::in | std::ios::binary);
		unsigned char temp;
		int i = 0;
		while (file.read((char*)&temp, sizeof(char))) {
			memory[512 + i] = temp;
			i++;
		}
		file.close();
	}

	void emulateCycle() {
		// Fetch opcode
		opcode = memory[pc] << 8 | memory[pc + 1];
		/*std::cout << "opcode: " << opcode << std::endl;
		std::cout << "pc: " << pc << std::endl;
		*/
		switch (opcode & 0xF000) {
		case 0xA000:
			I = opcode & 0x0FFF;
			pc += 2;
			break;

		case 0x0000:
			switch (opcode & 0x00FF)
			{
			case 0x00E0: // 0x00E0: Clears the screen
				for (int i = 0; i < 64 * 32; i++) {
					gfx[i] = 0;
				}
				drawFlag = true;
				pc += 2;
				break;

			case 0x00EE: // 0x00EE: Returns from subroutine   
				sp--;
				pc = stack[sp] + 2;

				break;

			default:
				std::cout << "Unknown opcode lol\n";
			}
			break;

		case 0x1000: // 1NNN JP addr
			pc = opcode & 0x0FFF;
			break;

		case 0x2000: // 2NNN Call adrr
			stack[sp] = pc;
			sp++;
			pc = opcode & 0x0FFF;
			break;

		case 0x3000: {// SE Vx, byte
			int index = (opcode & 0x0F00) >> 8;

			if (V[index] == (opcode & 0x00FF)) {
				pc += 4;
			}
			else {
				pc += 2;
			}
			break;
		}
		case 0x4000: {
			int index = (opcode & 0x0F00) >> 8;

			if (V[index] != (opcode & 0x00FF)) {
				pc += 4;
			}
			else {
				pc += 2;
			}
			break;
		}

		case 0x5000: {
			int index1 = (opcode & 0x0F00) >> 8;
			int index2 = (opcode & 0x00F0) >> 4;
			if (V[index1] == V[index2]) {
				pc += 4;
			}
			else {
				pc += 2;
			}
			break;
		}

		case 0x6000: {
			int index = (opcode & 0x0F00) >> 8;
			V[index] = (opcode & 0x00FF);
			pc += 2;
			break;
		}

		case 0x7000: {
			int index = (opcode & 0x0F00) >> 8;
			V[index] += opcode & 0x00FF;
			pc += 2;
			break;
		}

		case 0x8000:
		{
			switch (opcode & 0x000F) {
			case 0x0000: {
				int index1 = (opcode & 0x0F00) >> 8;
				int index2 = (opcode & 0x00F0) >> 4;
				V[index1] = V[index2];
				pc += 2;
				break;
			}

			case 0x0001: {
				int index1 = (opcode & 0x0F00) >> 8;
				int index2 = (opcode & 0x00F0) >> 4;
				V[index1] = V[index1] | V[index2];
				pc += 2;
				break;
			}

			case 0x0002: {
				int index1 = (opcode & 0x0F00) >> 8;
				int index2 = (opcode & 0x00F0) >> 4;
				V[index1] = V[index1] & V[index2];
				pc += 2;
				break;
			}

			case 0x0003: {
				int index1 = (opcode & 0x0F00) >> 8;
				int index2 = (opcode & 0x00F0) >> 4;
				V[index1] = V[index1] ^ V[index2];
				pc += 2;
				break;
			}

			case 0x0004: {
				if (V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8])) 
					V[0xF] = 1; //carry
				else
					V[0xF] = 0;
				V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
				pc += 2;
				break;
			}

			case 0x0005: {
				int index1 = (opcode & 0x0F00) >> 8;
				int index2 = (opcode & 0x00F0) >> 4;
				if (V[index1] > V[index2]) {
					V[0xF] = 1;
				}
				else {
					V[0xF] = 0;
				}
				V[index1] -= V[index2];
				pc += 2;
				break;
			}
			case 0x0006: {
				int index = (opcode & 0x0F00) >> 8;
				if (V[index] & 1) {
					V[0xF] = 1;
				}
				else {
					V[0xF] = 0;
				}
				V[index] /= 2;
				pc += 2;
				break;
			}
			case 0x0007: {
				int index1 = (opcode & 0x0F00) >> 8;
				int index2 = (opcode & 0x00F0) >> 4;
				if (V[index1] < V[index2]) {
					V[0xF] = 1;
				}
				else {
					V[0xF] = 0;
				}
				V[index1] = V[index2] - V[index1];
				pc += 2;
				break;
			}
			case 0x000E: {
				int index = (opcode & 0x0F00) >> 8;
				if (V[index] & 0x8000) {
					V[0xF] = 1;
				}
				else {
					V[0xF] = 0;
				}
				V[index] *= 2;
				pc += 2;
				break;
			}
			} break;

		}
		case 0x9000: {
			int index1 = (opcode & 0x0F00) >> 8;
			int index2 = (opcode & 0x00F0) >> 4;
			if (V[index1] != V[index2]) {
				pc += 4;
			}
			else {
				pc += 2;
			}
			break;

		}
		case 0xB000: {
			int location = opcode & 0x0FFF;
			pc = location + V[0];
			break;
		}
		case 0xC000: {
			int index = (opcode & 0x0F00) >> 8;
			int value = (opcode & 0x00FF);
			V[index] = (rand() % 256) & value;
			pc += 2;
			break;
		}
		case 0xD000: {
			unsigned short x = V[(opcode & 0x0F00) >> 8];
			unsigned short y = V[(opcode & 0x00F0) >> 4];
			unsigned short height = opcode & 0x000F;
			unsigned short pixel;

			V[0xF] = 0;
			for (int yline = 0; yline < height; yline++)
			{
				pixel = memory[I + yline];
				for (int xline = 0; xline < 8; xline++)
				{
					if ((pixel & (0x80 >> xline)) != 0)
					{
						if (gfx[(x + xline + ((y + yline) * 64))] == 1)
						{
							V[0xF] = 1;
						}
						gfx[x + xline + ((y + yline) * 64)] ^= 1;
					}
				}
			}

			drawFlag = true;
			pc += 2;
			//OutputDebugStringA("Drawing shit\n");
			break;
		}
		case 0xE000: {
			switch (opcode & 0x00FF) {
			case 0x009E:
				if (key[V[(opcode & 0x0F00) >> 8]] != 0) {
					pc += 4;
				}
				else {
					pc += 2;
				}
				break;
			case 0x00A1:
				if (key[V[(opcode & 0x0F00) >> 8]] == 0) {
					pc += 4;
				}
				else {
					pc += 2;
				}
				break;
			}
			break;
		}
		case 0xF000: {
			switch (opcode & 0x00FF) {
			case 0x0007: {
				int index = (opcode & 0x0F00) >> 8;
				V[index] = delayTimer;
				pc += 2;
				break;
			}
			case 0x000A: { 
				bool keyPress = false;

				for (int i = 0; i < 16; ++i)
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
					
				pc += 2;
				break;
			}
			case 0x0015: {
				int index = (opcode & 0x0F00) >> 8;
				delayTimer = V[index];
				pc += 2;
				break;
			}
			case 0x0018: {
				int index = (opcode & 0x0F00) >> 8;
				soundTimer = V[index];
				pc += 2;
				break;
			}
			case 0x001E: {
				int index = (opcode & 0x0F00) >> 8;
				I = I + V[index];
				pc += 2;
				break;
			}
			case 0x0029: { // Ska kanske multiplicera addressen med 5 för en sprite tar 5 byte
				int index = (opcode & 0x0F00) >> 8;
				I = V[index] * 5; // ??
				pc += 2;
				break;
			}
			case 0x0033: {
				int index = (opcode & 0x0F00) >> 8;
				int value = V[index];
				int hundred = value / 100;
				int ten = (value - hundred * 100) / 10;
				int one = value - hundred * 100 - ten * 10;
				memory[I] = hundred;
				memory[I + 1] = ten;
				memory[I + 2] = one;
				pc += 2;
				break;
			}
			case 0x0055: {
				int numRegisters = (opcode & 0x0F00) >> 8;
				for (int i = 0; i <= numRegisters; i++) {
					memory[I + i] = V[i];
				}
				pc += 2;
				break;
			}
			case 0x0065: {
				int numRegisters = (opcode & 0x0F00) >> 8;
				for (int i = 0; i <= numRegisters; i++) {
					V[i] = memory[I + i];
				}

				pc += 2;
				break;
			}

			}
			break;
		}

		default:

			std::cout << "Unknown opcode\n";
			break;

		}

		if (delayTimer > 0) {
			delayTimer--;
		}

		if (soundTimer > 0)
		{
			if (soundTimer == 1) {
				std::cout <<"BEEP\n";

			}
			soundTimer--;
		}

	}

	void print() {}
};