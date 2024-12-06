#pragma once
#include <iostream>
#include <irrKlang.h>
using namespace irrklang;

void play_sound2D(std::string path, std::string name, bool repeat) {
    // irrKlang ���� �ʱ�ȭ
    ISoundEngine* engine = createIrrKlangDevice();
    if (!engine) {
        std::cerr << "Error: Could not initialize irrKlang engine!" << std::endl;
        return;
    }

    // std::string�� const char*�� ��ȯ
    std::string fullPath = path + name;
    engine->play2D(fullPath.c_str(), repeat); // c_str() ���

    // ���� ���
    std::cout << "Press Enter to quit..." << std::endl;
    std::cin.get();

    // ���� ����
    engine->drop();
}