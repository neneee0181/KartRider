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

	/* �⺻ ������ �ʼ�~~ */
	LogoMode() {}

	void init() override {

	}

	void keyboard(unsigned char key, int x, int y) override {

		switch (key)
		{
		case '\r':
		{
			PlayMode* playMode = new PlayMode();
			MM.SetMode(playMode);
			break;
		}
		default:
			break;
		}
	}

	void draw_model() override {
	
	}

	void draw_bb() override {
		
	}

	void finish() override {

	}

};
