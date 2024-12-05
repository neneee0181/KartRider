#pragma once
#include <iostream>
#include "Mode.h"
#include "RoadModel.h"
#include "KartModel.h"
#include "shaderMaker.h"
#include "LoadProgress.h"
#include "root.h"
#include "KeyBoard.h"
#include "PlayMode.h"

class LogoMode : public Mode {
public:
	void init() override {

		// play mode�� �ʿ��� �� �ε�
		loadModelWithProgress <KartModel>("kronos.obj", "obj/car/kronos/", "car", "sphere", glm::scale(glm::mat4(1.0f), glm::vec3(10.0, 10.0, 10.0)), karts);

		initializeModelsWithPhysics(karts); // ��� �� Bullet world�� �߰�
	}

	void keyboard(unsigned char key, int x, int y) override {
		switch (key)
		{
		case 'k':
		{
			PlayMode playMode;
			MM.SetMode(&playMode);
			break;
		}
		default:
			break;
		}
	}

	void draw_model() override {
		for (const auto& kart : karts) { // ���� �� draw
			kart->draw(shaderProgramID, isKeyPressed_s);
		}
	}

	void draw_bb() override {
		for (const auto& model : karts) { // �� bb draw
			model->draw_rigidBody(shaderProgramID);
		}
	}

	void finish() override {

	}

};
