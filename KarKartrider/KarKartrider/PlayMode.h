#pragma once
#include <iostream>
#include "Mode.h"
#include "RoadModel.h"
#include "KartModel.h"
#include "shaderMaker.h"
#include "LoadProgress.h"
#include "root.h"
#include "KeyBoard.h"
#include "Light.h"


class PlayMode : public Mode {
public:

	glm::vec3 cameraPos = glm::vec3(0.0, 0.0, 5.0);
	glm::vec3 cameraDirection = glm::vec3(0.0, 0.0, -1.0);
	glm::vec3 cameraUp = glm::vec3(0.0, 1.0, 0.0);
	glm::mat4 projection = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);

	int map_num = 1;

	// ī�޶� ȸ�� ����
	float yaw = -90.0f; // ���� ȸ�� (�⺻: -Z��)
	float pitch = 0.0f; // ���� ȸ�� (�⺻: ����)

	PlayMode(){}

	void init() override {

	}

	void moveCamera(unsigned char key, int x, int y) {
		const float cameraSpeed = 0.1f; // ī�޶� �̵� �ӵ�
		float angleInRadians = glm::radians(1.0f); // 10�� ȸ��
		// ī�޶� ���� ����
		glm::vec3 forward = glm::normalize(cameraDirection - cameraPos);
		// ī�޶� ������ ����
		glm::vec3 right = glm::normalize(glm::cross(forward, cameraUp));

		switch (key) {
		case 'w': // ����
			cameraPos += cameraSpeed * forward;
			cameraDirection += cameraSpeed * forward;
			break;
		case 's': // ����
			cameraPos -= cameraSpeed * forward;
			cameraDirection -= cameraSpeed * forward;
			break;
		case 'a': // ���� �̵�
			cameraPos -= cameraSpeed * right;
			cameraDirection -= cameraSpeed * right;
			break;
		case 'd': // ������ �̵�
			cameraPos += cameraSpeed * right;
			cameraDirection += cameraSpeed * right;
			break;
		case 'i': // ���� ȸ�� (X�� ȸ��)
		{
			glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angleInRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			glm::vec4 rotatedDirection = rotation * glm::vec4(cameraDirection - cameraPos, 0.0f);
			cameraDirection = cameraPos + glm::vec3(rotatedDirection);
			cameraDirection = glm::normalize(cameraDirection - cameraPos) + cameraPos;
			break;
		}
		case 'k': // �Ʒ��� ȸ�� (X�� �ݴ� ����)
		{
			glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), -angleInRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			glm::vec4 rotatedDirection = rotation * glm::vec4(cameraDirection - cameraPos, 0.0f);
			cameraDirection = cameraPos + glm::vec3(rotatedDirection);
			cameraDirection = glm::normalize(cameraDirection - cameraPos) + cameraPos;
			break;
		}
		case 'j': // ���� ȸ�� (Y�� ȸ��)
		{
			glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angleInRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			glm::vec4 rotatedDirection = rotation * glm::vec4(cameraDirection - cameraPos, 0.0f);
			cameraDirection = cameraPos + glm::vec3(rotatedDirection);
			cameraDirection = glm::normalize(cameraDirection - cameraPos) + cameraPos;
			break;
		}
		case 'l': // ������ ȸ�� (Y�� �ݴ� ����)
		{
			glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), -angleInRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			glm::vec4 rotatedDirection = rotation * glm::vec4(cameraDirection - cameraPos, 0.0f);
			cameraDirection = cameraPos + glm::vec3(rotatedDirection);
			cameraDirection = glm::normalize(cameraDirection - cameraPos) + cameraPos;
			break;
		}
		default:
			break;
		}
	}

	void keyboard(unsigned char key, int x, int y) override {
		moveCamera(key, x, y);
	}

	void keySpecial(int key, int x, int y) override{
		
	}

	void draw_model() override {

		glClearColor(1.0, 1.0, 1.0, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shaderProgramID);


		GLenum error = glGetError();
		if (error != GL_NO_ERROR) {
			std::cout << "Error in glUseProgram: " << error << std::endl;
		}

		this->view = glm::mat4(1.0f);
		this->view = glm::lookAt(
			this->cameraPos,
			this->cameraDirection,
			this->cameraUp
		);
		unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "viewTransform");
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

		this->projection = glm::mat4(1.0f);
		this->projection = glm::perspective(glm::radians(45.0f), static_cast<float>(width) / static_cast<float>(height), 0.1f, 300.0f);
		unsigned int projectionLocation = glGetUniformLocation(shaderProgramID, "projectionTransform");
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

		GLint lightPosLoc = glGetUniformLocation(shaderProgramID, "lightPos");
		GLint lightColorLoc = glGetUniformLocation(shaderProgramID, "lightColor");
		glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));
		glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

		glEnable(GL_DEPTH_TEST);

		for (const auto& kart : karts) { // ���� �� draw
			kart->draw(shaderProgramID, isKeyPressed_s);
		}
		for (const auto& road : roads) { // ���� �� draw
			road->draw(shaderProgramID, isKeyPressed_s);
		}


		glDisable(GL_DEPTH_TEST);
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
