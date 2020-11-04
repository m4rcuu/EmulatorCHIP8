#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Emulator.hpp"
#include <iostream>

void handleKeyDown(Emulator* emulator, sf::Event* event);
void handleKeyUp(Emulator* emulator, sf::Event* event);
void windowDraw(sf::RenderWindow* window, unsigned char* array);
void windowCycle(sf::RenderWindow* window, Emulator* emulator, sf::Sound* sound, unsigned char* array);

int main(int argc, char** argv)
{
	unsigned char toDraw[2048];
	sf::Event event;
	sf::RenderWindow window(sf::VideoMode(640, 320), "Emulator CHIP8");
	window.setFramerateLimit(60);

	sf::SoundBuffer soundBuffer;
	sf::Sound beepSound;
	if (!soundBuffer.loadFromFile("./sound/beep.wav")) {
		std::cout << "error: could not load sound file" << std::endl;
		return -1;
	}
	beepSound.setBuffer(soundBuffer);

	Emulator emulator;
	if (!emulator.programLoad(argc, argv)) {
		return -1;
	}

	while (window.isOpen()) {
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
			}
			handleKeyDown(&emulator, &event);
			handleKeyUp(&emulator, &event);
		}
		windowCycle(&window, &emulator, &beepSound, toDraw);
		window.display();
	}

	return 0;
}

void handleKeyDown(Emulator* emulator, sf::Event* event) {
	if (event->type == sf::Event::KeyPressed) {
		if (event->key.code == sf::Keyboard::Num1) emulator->setKey(0x1, 1);
		else if (event->key.code == sf::Keyboard::Num2) emulator->setKey(0x2, 1);
		else if (event->key.code == sf::Keyboard::Num3) emulator->setKey(0x3, 1);
		else if (event->key.code == sf::Keyboard::Num4) emulator->setKey(0xC, 1);

		else if (event->key.code == sf::Keyboard::Q) emulator->setKey(0x4, 1);
		else if (event->key.code == sf::Keyboard::W) emulator->setKey(0x5, 1);
		else if (event->key.code == sf::Keyboard::E) emulator->setKey(0x6, 1);
		else if (event->key.code == sf::Keyboard::R) emulator->setKey(0xD, 1);

		else if (event->key.code == sf::Keyboard::A) emulator->setKey(0x7, 1);
		else if (event->key.code == sf::Keyboard::S) emulator->setKey(0x8, 1);
		else if (event->key.code == sf::Keyboard::D) emulator->setKey(0x9, 1);
		else if (event->key.code == sf::Keyboard::F) emulator->setKey(0xE, 1);

		else if (event->key.code == sf::Keyboard::Z) emulator->setKey(0xA, 1);
		else if (event->key.code == sf::Keyboard::X) emulator->setKey(0x0, 1);
		else if (event->key.code == sf::Keyboard::C) emulator->setKey(0xB, 1);
		else if (event->key.code == sf::Keyboard::V) emulator->setKey(0xF, 1);
	}
}

void handleKeyUp(Emulator* emulator, sf::Event* event) {
	if (event->type == sf::Event::KeyPressed) {
		if (event->key.code == sf::Keyboard::Num1) emulator->setKey(0x1, 0);
		else if (event->key.code == sf::Keyboard::Num2) emulator->setKey(0x2, 0);
		else if (event->key.code == sf::Keyboard::Num3) emulator->setKey(0x3, 0);
		else if (event->key.code == sf::Keyboard::Num4) emulator->setKey(0xC, 0);

		else if (event->key.code == sf::Keyboard::Q) emulator->setKey(0x4, 0);
		else if (event->key.code == sf::Keyboard::W) emulator->setKey(0x5, 0);
		else if (event->key.code == sf::Keyboard::E) emulator->setKey(0x6, 0);
		else if (event->key.code == sf::Keyboard::R) emulator->setKey(0xD, 0);

		else if (event->key.code == sf::Keyboard::A) emulator->setKey(0x7, 0);
		else if (event->key.code == sf::Keyboard::S) emulator->setKey(0x8, 0);
		else if (event->key.code == sf::Keyboard::D) emulator->setKey(0x9, 0);
		else if (event->key.code == sf::Keyboard::F) emulator->setKey(0xE, 0);

		else if (event->key.code == sf::Keyboard::Z) emulator->setKey(0xA, 0);
		else if (event->key.code == sf::Keyboard::X) emulator->setKey(0x0, 0);
		else if (event->key.code == sf::Keyboard::C) emulator->setKey(0xB, 0);
		else if (event->key.code == sf::Keyboard::V) emulator->setKey(0xF, 0);
	}
}

void windowDraw(sf::RenderWindow* window, unsigned char* array) {
	window->clear();
	for (int y = 0; y < 32; y++) {
		for (int x = 0; x < 64; x++) {
			if (array[(y * 64) + x] == 1) {
				sf::RectangleShape pixel(sf::Vector2f(10, 10));
				pixel.setPosition(x * 10, y * 10);
				pixel.setFillColor(sf::Color(255, 255, 255, 255));
				window->draw(pixel);
			}
		}
	}
}

void windowCycle(sf::RenderWindow* window, Emulator* emulator, sf::Sound* sound, unsigned char* array) {
	emulator->emulateCycle();

	array = emulator->getGFX();

	if (emulator->canBeep()) {
		sound->play();
	}
	windowDraw(window, array);
}