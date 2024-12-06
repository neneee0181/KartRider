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
#include "MapModel.h"

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

        loadModelWithProgress<MapModel>("village_road.obj", "asset/", "village_road", "box", glm::scale(glm::mat4(1.0f), glm::vec3(1.0, 1.0, 1.0)), selectMaps);

        glm::mat4 map2_matrix = glm::mat4(1.0f);
        map2_matrix = glm::translate(map2_matrix, glm::vec3(2.5, 0.0, 0.0));
        map2_matrix = glm::scale(map2_matrix, glm::vec3(1.0, 1.0, 1.0));
        loadModelWithProgress<MapModel>("villiage_unha.obj", "asset/", "villiage_unha", "box", map2_matrix, selectMaps);

        initializeModelsWithPhysics(karts);
        initializeModelsWithPhysics(roads);

        // 5. ������ �� ���� ������ ���� ���
        videoThread.join();
        soundThread.join();

        // 4. ���α׷� ���� �÷��� ����
        isRunning = false;
        SelectMapMode* selectMapMode = new SelectMapMode();
        MM.SetMode(selectMapMode);
    }

    void keyboard(unsigned char key, int x, int y) override {}

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