#pragma once
#include <iostream>

class Mode {
public:

	void init() {
		// play mode에 필요한 모델 로드
		loadModelWithProgress <KartModel>("kronos.obj", "obj/car/kronos/", "car", "sphere", glm::scale(glm::mat4(1.0f), glm::vec3(10.0, 10.0, 10.0)), karts);
		loadModelWithProgress <RoadModel>("road_all_1.obj", "obj/road/", "road_all", "cube", glm::scale(glm::mat4(1.0f), glm::vec3(20.0, 20.0, 20.0)), roads);

		initializeModelsWithPhysics(karts); // 모든 모델 Bullet world에 추가
		initializeModelsWithPhysics(roads);
	}

	virtual void init() = 0; // 모드 시작시 셋팅
	virtual void keyboard() = 0;
	virtual void draw_model() = 0;
	virtual void draw_bb() = 0;
	virtual void finish() = 0;

	virtual ~Mode() {}  // 가상 소멸자
};