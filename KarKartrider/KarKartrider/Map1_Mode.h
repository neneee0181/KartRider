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


class Map1_Mode : public Mode {
public:

	bool up = false;
	bool down = false;
	bool left = false;
	bool right = false;

	glm::vec3 cameraPos = glm::vec3(0.0, 0.0, 5.0);
	glm::vec3 cameraDirection = glm::vec3(0.0, 0.0, -1.0);
	glm::vec3 cameraUp = glm::vec3(0.0, 1.0, 0.0);
	glm::mat4 projection = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);

	// ī�޶� ȸ�� ����
	float yaw = -90.0f; // ���� ȸ�� (�⺻: -Z��)
	float pitch = 0.0f; // ���� ȸ�� (�⺻: ����)

	Map1_Mode() {
		Mode::currentInstance = this;  // Map1_Mode �ν��Ͻ��� currentInstance�� �Ҵ�
	}

	void init() override {
		for (const auto& kart : karts) { // īƮ ��ġ �ʱ�ȭ
			kart->translateMatrix = glm::mat4(1.0f);
			kart->translateMatrix = glm::translate(kart->translateMatrix, glm::vec3(0.0, 2.6, 238.0));
			cameraPos = glm::vec3(0.0, 3.6, 243.0);
			updateCameraDirection();

			// ������ timer �Լ� ȣ��
			glutTimerFunc(0, Map1_Mode::timerHelper, 0);
		}
	}

	void updateCameraDirection() {

		glm::vec3 direction;
		direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		direction.y = sin(glm::radians(pitch));
		direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraDirection = glm::normalize(direction) + cameraPos;
	}

	void setCamera() {
		cameraPos = glm::vec3(karts[0]->translateMatrix[3][0], karts[0]->translateMatrix[3][1] + 1.0, karts[0]->translateMatrix[3][2] + 5.0);
		updateCameraDirection();
	}

	void timer() {
		if (up) {
			for (const auto& kart : karts) {
				kart->translateMatrix = glm::translate(kart->translateMatrix, glm::vec3(0.0, 0.0, -0.1));
			}
			setCamera();
		}
		if (down) {
			for (const auto& kart : karts) {
				kart->translateMatrix = glm::translate(kart->translateMatrix, glm::vec3(0.0, 0.0, 0.1));
			}
			setCamera();
		}
		if (left) {
			for (const auto& kart : karts) {
				//kart->translateMatrix = glm::translate(kart->translateMatrix, glm::vec3(-0.1, 0.0, 0.0));
				kart->translateMatrix = glm::rotate(kart->translateMatrix, glm::radians(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			}
			setCamera();
		}
		if (right) {
			for (const auto& kart : karts) {
				//kart->translateMatrix = glm::translate(kart->translateMatrix, glm::vec3(0.1, 0.0, 0.0));
				kart->translateMatrix = glm::rotate(kart->translateMatrix, glm::radians(-1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			}
			setCamera();
		}
	}

	void moveCamera(unsigned char key, int x, int y) {
		const float cameraSpeed = 0.1f; // ī�޶� �̵� �ӵ�
		float angleInRadians = glm::radians(5.0f); // 5�� ȸ��

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
			pitch += glm::degrees(angleInRadians);
			if (pitch > 89.0f) pitch = 89.0f; // ��� ����
			updateCameraDirection();
			break;
		}
		case 'k': // �Ʒ��� ȸ�� (X�� �ݴ� ����)
		{
			pitch -= glm::degrees(angleInRadians);
			if (pitch < -89.0f) pitch = -89.0f; // �ϴ� ����
			updateCameraDirection();
			break;
		}
		case 'j': // ���� ȸ�� (Y�� ȸ��)
		{
			yaw -= glm::degrees(angleInRadians);
			updateCameraDirection();
			break;
		}
		case 'l': // ������ ȸ�� (Y�� �ݴ� ����)
		{
			yaw += glm::degrees(angleInRadians);
			updateCameraDirection();
			break;
		}
		default:
			break;
		}
	}

	void keyboard(unsigned char key, int x, int y) override {
		moveCamera(key, x, y);
	}

	void specialKey(int key, int x, int y) override {
		switch (key) {
		case GLUT_KEY_UP:
			up = true;
			break;
		case GLUT_KEY_DOWN:
			down = true;
			break;
		case GLUT_KEY_LEFT:
			left = true;
			break;
		case GLUT_KEY_RIGHT:
			right = true;
			break;
		}
	}

	void specialKeyUp(int key, int x, int y) override {
		switch (key) {
		case GLUT_KEY_UP:
			up = false;
			break;
		case GLUT_KEY_DOWN:
			down = false;
			break;
		case GLUT_KEY_LEFT:
			left = false;
			break;
		case GLUT_KEY_RIGHT:
			right = false;
			break;
		}
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
		for (const auto& road : road1) { // ���� �� draw
			road->draw(shaderProgramID, isKeyPressed_s);
		}


		glDisable(GL_DEPTH_TEST);
	}

	void draw_bb() override {
		for (const auto& model : karts) { // �� bb draw
			model->draw_rigidBody(shaderProgramID);
		}
		for (const auto& road : road1) { // �� bb draw
			road->draw_rigidBody(shaderProgramID);
		}
	}

	void finish() override {

	}
private:

	static void timerHelper(int value) {
		if (Map1_Mode* instance = dynamic_cast<Map1_Mode*>(Mode::currentInstance)) {
			instance->timer(); // �ν��Ͻ��� timer ȣ��
		}
		glutPostRedisplay();
		glutTimerFunc(16, timerHelper, value); // Ÿ�̸� �ݺ� ȣ��
	}

};