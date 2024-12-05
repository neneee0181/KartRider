#pragma once
#include <iostream>

class Mode {
public:

	void init() {
		// play mode�� �ʿ��� �� �ε�
		loadModelWithProgress <KartModel>("kronos.obj", "obj/car/kronos/", "car", "sphere", glm::scale(glm::mat4(1.0f), glm::vec3(10.0, 10.0, 10.0)), karts);
		loadModelWithProgress <RoadModel>("road_all_1.obj", "obj/road/", "road_all", "cube", glm::scale(glm::mat4(1.0f), glm::vec3(20.0, 20.0, 20.0)), roads);

		initializeModelsWithPhysics(karts); // ��� �� Bullet world�� �߰�
		initializeModelsWithPhysics(roads);
	}

	virtual void init() = 0; // ��� ���۽� ����
	virtual void keyboard() = 0;
	virtual void draw_model() = 0;
	virtual void draw_bb() = 0;
	virtual void finish() = 0;

	virtual ~Mode() {}  // ���� �Ҹ���
};