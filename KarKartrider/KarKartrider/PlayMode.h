#pragma once
#include <iostream>
#include "Mode.h"
#include "RoadModel.h"
#include "KartModel.h"



class PlayMode : public Mode {
public:
	void init() override {

		// play mode�� �ʿ��� �� �ε�
		loadModelWithProgress <KartModel>("kronos.obj", "obj/car/kronos/", "car", "sphere", glm::scale(glm::mat4(1.0f), glm::vec3(10.0, 10.0, 10.0)), karts);
		loadModelWithProgress <RoadModel>("road_all_1.obj", "obj/road/", "road_all", "cube", glm::scale(glm::mat4(1.0f), glm::vec3(20.0, 20.0, 20.0)), roads);

		initializeModelsWithPhysics(karts); // ��� �� Bullet world�� �߰�
		initializeModelsWithPhysics(roads);
	}

	void keyboard() override {

	}

	void draw_model() override {
		for (const auto& kart : karts) { // ���� �� draw
			kart->draw(shaderProgramID, isKeyPressed_s);
		}
		for (const auto& road : roads) { // ���� �� draw
			road->draw(shaderProgramID, isKeyPressed_s);
		}
	}

	void draw_bb() override {
		for (const auto& model : karts) { // �� bb draw
			model->draw_rigidBody(shaderProgramID);
		}
		for (const auto& road : roads) { // �� bb draw
			road->draw_rigidBody(shaderProgramID);
		}
	}

	void finish() override {

	}

};
