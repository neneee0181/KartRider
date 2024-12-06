#pragma once
#include <iostream>
#include <thread>

#include "Mode.h"
#include "shaderMaker.h"
#include "root.h"
#include "KeyBoard.h"
#include "LoadVideo.h"
#include "LoadSound.h"
#include "SelectMapMode.h"
#include "PlayMode.h"

class LogoMode : public Mode {
public:
    bool isRunning; // ���α׷� ���� ���¸� ��Ÿ���� �÷���

    /* �⺻ ������ �ʼ�~~ */
    LogoMode() : isRunning(true) {}

    void init() override {
        // 1. ������ ��� ������ ����
        std::thread videoThread(&LogoMode::runVideo, this);

        // 2. ���� ��� ������ ����
        std::thread soundThread(&LogoMode::runSound, this);

        // 3. �� �ε� �� Bullet Physics �ʱ�ȭ (���� �����忡�� ����)
        loadModelWithProgress<KartModel>("kronos.obj", "obj/car/kronos/", "car", "sphere", glm::scale(glm::mat4(1.0f), glm::vec3(1.0, 1.0, 1.0)), karts);
        loadModelWithProgress<RoadModel>("road_all_1.obj", "obj/road/", "road_all", "cube", glm::scale(glm::mat4(1.0f), glm::vec3(100.0, 100.0, 100.0)), roads);
        initializeModelsWithPhysics(karts);
        initializeModelsWithPhysics(roads);

        // 5. ������ �� ���� ������ ���� ���
        videoThread.join();
        soundThread.join();

        // 4. ���α׷� ���� �÷��� ����
        isRunning = false;
    }

    void keyboard(unsigned char key, int x, int y) override {
        switch (key) {
        case '\r': {
            PlayMode* playMode = new PlayMode();
            SelectMapMode* selectMapMode = new SelectMapMode();
            MM.SetMode(playMode);
            break;
        }
        default:
            break;
        }
    }

    void draw_model() override {}

    void draw_bb() override {}

    void finish() override {}

private:
    // ������ ���� �Լ�
    void runVideo() {
        loadVideo("kartrider_intro.mp4", "./asset/", &isRunning);
    }

    // ���� ���� �Լ�
    void runSound() {
        play_sound2D("kartrider_intro.WAV", "./asset/", false, &isRunning);
    }
};