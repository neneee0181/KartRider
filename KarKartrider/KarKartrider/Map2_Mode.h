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

#include <gl/glm/glm/gtc/quaternion.hpp> // ���ʹϾ� ����
#include <gl/glm/glm/gtx/quaternion.hpp> // SLERP(Spherical Linear Interpolation)

#define MAX_SPEED 0.3
#define ACCELERATION 0.002f
#define DECELERATION 0.001f

class Map2_Mode : public Mode {
public:
	glm::quat cameraRotationQuat = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f)); // ���� ī�޶� ����� ���ʹϾ����� ����
	float reducedRotationInfluence = 0.0f; // ������ �ۼ�Ʈ

	GLfloat kart_speed = 0.0f;

	enum Move { NONE_M, UP, DOWN, };
	enum Direction { NONE_D, LEFT, RIGHT };
	Move prev_move = NONE_M;
	Direction prev_dir = NONE_D;

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

	Map2_Mode() {
		Mode::currentInstance = this;  // Map1_Mode �ν��Ͻ��� currentInstance�� �Ҵ�
	}

	void init() override {
		for (const auto& kart : karts) { // īƮ ��ġ �ʱ�ȭ
			kart->translateMatrix = glm::mat4(1.0f);
			kart->translateMatrix = glm::translate(kart->translateMatrix, glm::vec3(165.0, 1.0, 30.0));

			// ������ timer �Լ� ȣ��
			glutTimerFunc(0, Map2_Mode::timerHelper, 0);
		}

		kart_speed = 0.0f;
		cameraPos = glm::vec3(0.0, 5.6, 283.0);
		updateCameraDirection();
	}

	void updateCameraDirection() {
		glm::mat3 rotationMatrix = glm::mat3(karts[0]->translateMatrix);

		glm::vec3 direction;
		direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		direction.y = sin(glm::radians(pitch));
		direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

		glm::vec3 rotatedDirection = rotationMatrix * direction;

		glm::vec3 carPosition = glm::vec3(karts[0]->translateMatrix[3]);

		cameraDirection = glm::normalize(rotatedDirection) + carPosition;
	}

	void setCamera() {
		glm::vec3 carPosition = glm::vec3(karts[0]->translateMatrix[3]);

		glm::mat3 carRotationMatrix = glm::mat3(karts[0]->translateMatrix);

		glm::quat carRotationQuat = glm::quat_cast(carRotationMatrix);

		glm::quat interpolatedRotation = glm::slerp(cameraRotationQuat, carRotationQuat, reducedRotationInfluence);

		cameraRotationQuat = interpolatedRotation;

		glm::mat3 adjustedRotationMatrix = glm::mat3_cast(interpolatedRotation);

		glm::vec3 baseOffset = glm::vec3(0.0f, 3.0f, 15.0f);

		glm::vec3 rotatedOffset = adjustedRotationMatrix * baseOffset;

		cameraPos = carPosition + rotatedOffset;

		updateCameraDirection();
	}

	void timer() {
		UpdateRigidBodyTransform(karts[0]);
		if (up || down || left || right) {
			if (up) prev_move = UP;
			if (down) prev_move = DOWN;
			if (left) prev_dir = LEFT;
			if (right) prev_dir = RIGHT;

			reducedRotationInfluence = 0.1f;

			if ((up || down) && kart_speed <= MAX_SPEED) { // �ӵ� ����
				kart_speed += ACCELERATION;
			}
		}
		else {
			if (kart_speed >= 0.0f) // �ӵ� ����
				kart_speed -= DECELERATION;
			else if (kart_speed == 0.0f)
				prev_move = NONE_M;

			prev_dir = NONE_D;

			if (reducedRotationInfluence <= 1.0f)
				reducedRotationInfluence += 0.01f;
		}
		if (prev_move == UP) {
			for (const auto& kart : karts) {
				kart->translateMatrix = glm::translate(kart->translateMatrix, glm::vec3(0.0, 0.0, -kart_speed));
			}
		}
		if (prev_move == DOWN) {
			for (const auto& kart : karts) {
				kart->translateMatrix = glm::translate(kart->translateMatrix, glm::vec3(0.0, 0.0, kart_speed));
			}
		}
		if (prev_dir == LEFT) {
			if (kart_speed != 0.0f) {
				for (const auto& kart : karts) {
					kart->translateMatrix = glm::translate(kart->translateMatrix, glm::vec3(0.0, 0.0, -1.5));
					kart->translateMatrix = glm::rotate(kart->translateMatrix, glm::radians(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
					kart->translateMatrix = glm::translate(kart->translateMatrix, glm::vec3(0.0, 0.0, 1.5));
				}
			}
		}
		if (prev_dir == RIGHT) {
			if (kart_speed != 0.0f) {
				for (const auto& kart : karts) {
					kart->translateMatrix = glm::translate(kart->translateMatrix, glm::vec3(0.0, 0.0, -1.5));
					kart->translateMatrix = glm::rotate(kart->translateMatrix, glm::radians(-1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
					kart->translateMatrix = glm::translate(kart->translateMatrix, glm::vec3(0.0, 0.0, 1.5));
				}
			}
		}
		setCamera();
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
		for (const auto& road : road2) { // ���� �� draw
			road->draw(shaderProgramID, isKeyPressed_s);
		}


		glDisable(GL_DEPTH_TEST);
	}

	void draw_bb() override {
		for (const auto& model : karts) { // �� bb draw
			model->draw_rigidBody(shaderProgramID);
		}
		for (const auto& road : road2) { // �� bb draw
			road->draw_rigidBody(shaderProgramID);
		}
	}

	void finish() override {

	}
private:

	static void timerHelper(int value) {
		if (Map2_Mode* instance = dynamic_cast<Map2_Mode*>(Mode::currentInstance)) {
			instance->timer(); // �ν��Ͻ��� timer ȣ��
		}
		glutPostRedisplay();
		glutTimerFunc(16, timerHelper, value); // Ÿ�̸� �ݺ� ȣ��
	}

};