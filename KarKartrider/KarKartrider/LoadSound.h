#pragma once
#include <iostream>
#include <irrKlang.h>
using namespace irrklang;

void play_sound2D(std::string path, std::string name, bool repeat, bool* isRunning) {
    ISoundEngine* engine = createIrrKlangDevice();
    if (!engine) {
        std::cerr << "Error: Could not initialize irrKlang engine!" << std::endl;
        return;
    }

    // ��� ����� ���
    std::string fullPath = path + name;
    std::cout << "Attempting to play sound: " << fullPath << std::endl;

    // ���� ���
    ISound* sound = engine->play2D(fullPath.c_str(), repeat, false, true);
    if (!sound) {
        std::cerr << "Error: Could not play sound file!" << std::endl;
        engine->drop();
        return;
    }

    // �÷��װ� true�� ���� ���� ��� ����
    while (*isRunning) {
        if (!repeat && sound->isFinished()) break; // �ݺ����� ������ ����
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    sound->drop(); // ���� ��ü ����
    engine->drop(); // ���� ����
}