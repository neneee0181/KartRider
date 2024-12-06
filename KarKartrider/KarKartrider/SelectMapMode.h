#pragma once
#include <iostream>
#include "Mode.h"
#include "shaderMaker.h"
#include "root.h"
#include "KeyBoard.h"
#include "PlayMode.h"

class SelectMapMode : public Mode {
public:

	glm::vec3 cameraPosMapMode = glm::vec3(0.0, 0.0, 5.0);
	glm::vec3 cameraDirectionMapMode = glm::vec3(0.0, 0.0, 0.0);
	glm::vec3 cameraUpMapMode = glm::vec3(0.0, 1.0, 0.0);
	glm::mat4 projectionMapMode = glm::mat4(1.0f);
	glm::mat4 viewMapMode = glm::mat4(1.0f);

	SelectMapMode() {}

	void init() override {
		// ī�޶� ��ġ�� Y�� �������� ȸ��
		float angleInRadians = glm::radians(-15.0f); // 10�� ȸ��
		glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angleInRadians, glm::vec3(0.0f, 1.0f, 0.0f));

		glm::vec4 rotatedCameraPos = rotation * glm::vec4(cameraPosMapMode, 1.0f);
		cameraPosMapMode = glm::vec3(rotatedCameraPos); // ȸ���� ��ġ�� ī�޶� ��ġ�� ����

	}

	void keyboard(unsigned char key, int x, int y) override {
		switch (key) {
		case '\r': {
			PlayMode* playMode = new PlayMode();
			MM.SetMode(playMode);
			break;
		}
		default:
			break;
		}
	}

	void keySpecial(int key, int x, int y) override {

	}

	void draw_model() override {

		glClearColor(0.0, 0.0, 0.0, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shaderProgramID);


		GLenum error = glGetError();
		if (error != GL_NO_ERROR) {
			std::cout << "Error in glUseProgram: " << error << std::endl;
		}
		
		// View ��� ���� (ȸ���� ī�޶� ��ġ ���)
		viewMapMode = glm::lookAt(
			cameraPosMapMode,
			cameraDirectionMapMode, // ��ǥ ����(��: ����)�� �״�� ����
			cameraUpMapMode
		);
		unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "viewTransform");
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &viewMapMode[0][0]);

		projectionMapMode = glm::mat4(1.0f);
		projectionMapMode = glm::perspective(glm::radians(45.0f), static_cast<float>(width) / static_cast<float>(height), 0.1f, 300.0f);
		unsigned int projectionLocation = glGetUniformLocation(shaderProgramID, "projectionTransform");
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projectionMapMode[0][0]);

		glEnable(GL_DEPTH_TEST);

		for (const auto& maps : selectMaps) { // ���� �� draw
			maps->draw(shaderProgramID, isKeyPressed_s);
		}

		glDisable(GL_DEPTH_TEST);
	}

	void draw_bb() override {
	}

	void finish() override {

	}

};
