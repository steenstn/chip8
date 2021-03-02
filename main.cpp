#include "olcPixelGameEngine.h"
#include "chip8.h"

Chip8 chips8;
olc::Key keyMap[16] = { olc::Key::X,
			olc::Key::K1,
			olc::Key::K2,
			olc::Key::K3,
			olc::Key::Q,
			olc::Key::W,
			olc::Key::E,
			olc::Key::A,
			olc::Key::S,
			olc::Key::D,
		olc::Key::Z,
		olc::Key::C,
		olc::Key::K4,
		olc::Key::R,
		olc::Key::F,
		olc::Key::V };

class Example : public olc::PixelGameEngine
{
private: 
	float fTargetFrameTime = 1.0f / 200.0f; 
	float fAccumulatedTime = 0.0f;
public:
	Example()
	{
		sAppName = "Example";
	}

public:
	bool OnUserCreate() override
	{
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		for (int i = 0; i < 16; i++) {
			if (GetKey(keyMap[i]).bHeld) {
				chips8.key[i] = 1;
			}
		}

		for (int i = 0; i < 16; i++) {
			if (GetKey(keyMap[i]).bReleased) {
				chips8.key[i] = 0;
			}
		}
		fAccumulatedTime += fElapsedTime;
		if (fAccumulatedTime >= fTargetFrameTime)
		{
			fAccumulatedTime -= fTargetFrameTime;
			fElapsedTime = fTargetFrameTime;
		}
		else {
			return true;

		}
		
		chips8.emulateCycle();

		if (chips8.drawFlag) {
			for (int i = 0; i < 64 * 32; i++) {
				int x = i % 64;
				int y = i / 64;
				Draw(x, y, olc::Pixel(255 * chips8.gfx[i], 255 * chips8.gfx[i], 255 * chips8.gfx[i]));
			}
			
			chips8.drawFlag = false;
		}

		return true;
	}
};


int main()
{

	chips8.initialize();
	chips8.loadRom("C:\\a\\GAMES\\HIDDEN");

	Example demo;
	if (demo.Construct(64, 32, 10, 10, false, false))
		demo.Start();

	return 0;
}