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

#include <thread>
#include <gl/glm/glm/gtc/quaternion.hpp> // ���ʹϾ� ����
#include <gl/glm/glm/gtx/quaternion.hpp> // SLERP(Spherical Linear Interpolation)
#include <unordered_map> // keystate

#define ACCELERATION 0.002f
#define DECELERATION 0.001f

class Map1_Mode : public Mode {
public:
    glm::quat cameraRotationQuat = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f)); // ���� ī�޶� ����� ���ʹϾ����� ����
    float reducedRotationInfluence = 0.0f; // ������ �ۼ�Ʈ

    GLfloat kart_speed = 0.0f;

    enum Move { NONE_M, UP, DOWN, LEFT, RIGHT, CTRL };
    float MAX_SPEED = 0.5;

    int start_count;

    bool Pause;

    //Ű
    std::unordered_map<Move, bool> kart_keyState;

    bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;

    glm::vec3 cameraTargetPos = glm::vec3(0.0, 0.0, 5.0); // ī�޶��� ��ǥ ��ġ
    glm::vec3 cameraPos = glm::vec3(0.0, 0.0, 5.0);       // ī�޶��� ���� ��ġ
    glm::vec3 cameraDirection = glm::vec3(0.0, 0.0, -1.0);
    glm::vec3 cameraUp = glm::vec3(0.0, 1.0, 0.0);
    glm::mat4 projection = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);

    // ī�޶� ȸ�� ����
    float yaw = -90.0f; // ���� ȸ�� (�⺻: -Z��)
    float pitch = 0.0f; // ���� ȸ�� (�⺻: ����)
    float TURN_ANGLE = 1.0f; // ȸ�� ���� (�⺻ 1��) (īƮ ȸ�� ����)

    //sound
    bool isBackgroundSound = false;
    std::thread backgroundSoundThread;
    bool isCountNSound = false;
    std::thread countNSoundThread;
    bool isCountGoSound = false;
    std::thread countGoSoundThread;
    bool isMotorsound = false;
    std::thread motorSoundThread;

    // ----- game ------

    int booster_cnt = 2;

    Map1_Mode() {
        Mode::currentInstance = this;  // Map1_Mode �ν��Ͻ��� currentInstance�� �Ҵ�
        isCountNSound = true;
        isCountGoSound = true;
    }

    void init() override {

        UpdateRigidBodyTransforms(road1_barricate);
        UpdateRigidBodyTransforms(karts);

        // Move ���� �ʱ�ȭ
        kart_keyState[UP] = false;
        kart_keyState[DOWN] = false;
        kart_keyState[LEFT] = false;
        kart_keyState[RIGHT] = false;

        start_count = -1;

        Pause = false;

        for (const auto& kart : karts) { // īƮ ��ġ �ʱ�ȭ
            kart->translateMatrix = glm::mat4(1.0f);
            kart->translateMatrix = glm::translate(kart->translateMatrix, glm::vec3(0.0, 2.6, 238.0));
        }
        for (const auto& c : character) { //īƮ�� ���� ��� ����
            c->translateMatrix = karts[0]->translateMatrix;
        }
        for (const auto& c : countDown) { //īƮ�� ���� ��� ����
            c->translateMatrix = karts[0]->translateMatrix;
            c->translateMatrix = glm::translate(c->translateMatrix, glm::vec3(0.0, 4.0, 0.0));
        }

        isBackgroundSound = true;
        backgroundSoundThread = std::thread(&Map1_Mode::backgroundSound, this);

        kart_speed = 0.0f;
        draw_model();
        glutTimerFunc(0, Map1_Mode::timerHelper, 0);

        cameraPos = glm::vec3(0.0, 6.0, 253.0);
        updateCameraDirection();
    }

    void playCountdown(int count) {
        if (count < 3) { // count_n ���� 3�� ����
            if (countNSoundThread.joinable()) {
                countNSoundThread.join();
            }

            countNSoundThread = std::thread(&Map1_Mode::count_n, this);

            countNSoundThread.join(); // �� �κ��� count_n.wav�� ���� ������ ��ٸ��ϴ�.
        }
        else if (count == 3) { // count_go ���� ����
            if (countGoSoundThread.joinable()) {
                countGoSoundThread.join();
            }

            countGoSoundThread = std::thread(&Map1_Mode::count_go, this);
            
            // �÷��� �ʱ�ȭ
            //isCountGoSound = false;
            isCountNSound = false;
            isBackgroundSound = true;
        }
    }


    void updateCameraDirection() {
        glm::mat3 rotationMatrix = glm::mat3(karts[0]->translateMatrix);

        // �⺻ ī�޶� ���� ���� ���
        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

        // ȸ�� ����� ������ ���� ���� ���
        glm::vec3 rotatedDirection = rotationMatrix * direction;

        //-----------------------
        // īƮ�� �ٶ󺸵��� ����
        glm::vec3 carPosition = glm::vec3(karts[0]->translateMatrix[3]);

        // �ڵ����� �ٶ󺸴� ���� ��� (�ڵ��� ��ġ - ī�޶� ��ġ)
        cameraDirection = glm::normalize(rotatedDirection) + carPosition;
    }

    void setCamera() {
        glm::vec3 carPosition = glm::vec3(karts[0]->translateMatrix[3]);

        // �ڵ����� ȸ�� ��� ���� (3x3 ���)
        glm::mat3 carRotationMatrix = glm::mat3(karts[0]->translateMatrix);

        // �ڵ��� ȸ�� ����� ���ʹϾ����� ��ȯ
        glm::quat carRotationQuat = glm::quat_cast(carRotationMatrix);

        // �ڵ��� ȸ���� �⺻ ȸ���� ����
        glm::quat interpolatedRotation = glm::slerp(cameraRotationQuat, carRotationQuat, reducedRotationInfluence);
        cameraRotationQuat = interpolatedRotation;

        // ������ ȸ���� ��ķ� ��ȯ
        glm::mat3 adjustedRotationMatrix = glm::mat3_cast(interpolatedRotation);

        // ī�޶� ��ǥ ��ġ ���� (�ӵ��� ���� �������� ����)
        glm::vec3 baseOffset = glm::vec3(0.0f, 6.0f + (kart_speed * 2.0f), 14.0f + (kart_speed * 10.0f));
        glm::vec3 rotatedOffset = adjustedRotationMatrix * baseOffset;

        // ī�޶� ��ǥ ��ġ ��� (�ڵ��� ��ġ + ȸ���� ������)
        cameraTargetPos = carPosition + rotatedOffset;

        // ī�޶� �ڵ����� �ٶ󺸵��� ���� ������Ʈ
        cameraDirection = carPosition; // �ڵ����� �׻� �ٶ�
    }

    void checkCollisionKart() {
        for (auto& kart : karts) {
            if (kart->name != "car") continue; // īƮ�� "car" �̸��� �ƴϸ� ��ŵ

            // �߷��� ���� (�� ���� ����)
            kart->rigidBody->setGravity(btVector3(0.0f, 0.0f, 0.0f));

            for (const auto& barri : road1_barricate) {
                if (barri->name != "baricate") continue; // �ٸ����̵尡 "baricate" �̸��� �ƴϸ� ��ŵ

                // �浹 �ݹ� ��ü ����
                CustomContactResultCallback resultCallback;

                // �� ��ü�� �浹 ����
                dynamicsWorld->contactPairTest(kart->rigidBody, barri->rigidBody, resultCallback);

                if (resultCallback.hitDetected) { // �浹�� �����Ǿ��� ��
                    // 1. �浹 ���� �� ���� ���� ��������
                    btVector3 collisionNormal = resultCallback.collisionNormal; // �浹 ����
                    collisionNormal.setY(0.0f); // y�� ���� ���� (xz ��鿡���� ó��)

                    // 2. ���� ����� �浹 ���� ���
                    glm::vec3 kartDirection = glm::normalize(glm::vec3(-kart->translateMatrix[2])); // ���� ���� (Z�� ����)
                    glm::vec3 collisionDirection = glm::normalize(glm::vec3(collisionNormal.x(), collisionNormal.y(), collisionNormal.z()));

                    // ���� ����� �浹 ������ ���� ��ġ�ϴ� ��츸 �ӵ� ����
                    float dotProduct = glm::dot(kartDirection, collisionDirection);

                    // 3. ���� ó�� (�浹 ������ ���� �ӵ��� ����)
                    if (dotProduct < 0.0f) { // �浹 ������ ���� ����� �ݴ��� ���� ó��
                        float decelerationFactor = 0.05f; // ���� ����
                        kart_speed *= 1.0f - decelerationFactor; // �ӵ��� õõ�� ����

                        if (kart_speed < 0.01f) { // �ʹ� �������� ����
                            kart_speed = 0.0f;
                        }
                    }

                    // 4. �浹 ��ġ ���� (ħ�� ���̸�ŭ �̵�)
                    btTransform kartTransform;
                    kart->rigidBody->getMotionState()->getWorldTransform(kartTransform);
                    btVector3 kartPos = kartTransform.getOrigin();

                    btVector3 correction = collisionNormal * std::abs(resultCallback.penetrationDepth);
                    correction.setY(0.0f); // y�� �̵� ����
                    btVector3 newKartPos = kartPos + correction;
                    newKartPos.setY(2.6f); // y�� ����

                    kartTransform.setOrigin(newKartPos);

                    // ������Ʈ�� Transform�� īƮ�� ����
                    kart->rigidBody->getMotionState()->setWorldTransform(kartTransform);
                    kart->rigidBody->setWorldTransform(kartTransform);

                    // OpenGL ��ȯ ��Ŀ��� �ݿ�
                    btScalar transformMatrix[16];
                    kartTransform.getOpenGLMatrix(transformMatrix);
                    kart->translateMatrix = glm::make_mat4(transformMatrix);
                }
            }
        }
    }

    void checkEngineSound() {
        if (kart_speed != 0.0f) {
            if (!isMotorsound) { // ���� ���尡 ��� ������ ���� ���� ����
                isMotorsound = true;
                motorSoundThread = std::thread(&Map1_Mode::engine_sound, this); // ���� ���� ����
            }
        }
        else { // �ӵ��� 0�� ���
            if (isMotorsound) {
                isMotorsound = false;
                if (motorSoundThread.joinable()) {
                    motorSoundThread.detach(); // ������ ���� (�ʿ��� ��� detach)
                }
            }
        }
    }

    void timer() {
        if (start_count < 4) {
            if (start_count >= 0)
                playCountdown(start_count);
            ++start_count;
        }
        else {

            // ����/���� ó��
            if (kart_keyState[UP]) {
                if (kart_speed < MAX_SPEED) {
                    kart_speed += ACCELERATION;
                }
            }
            else if (kart_keyState[DOWN]) {
                if (kart_speed > -MAX_SPEED / 2.0f) { // ���� �ӵ��� ������ ���ݱ����� ���
                    kart_speed -= ACCELERATION;
                }
            }
            else {
                if (kart_speed > 0.0f) {
                    kart_speed -= DECELERATION; // ���� ����
                    if (kart_speed < 0.0f) kart_speed = 0.0f;
                }
                else if (kart_speed < 0.0f) {
                    kart_speed += DECELERATION; // ���� ����
                    if (kart_speed > 0.0f) kart_speed = 0.0f;
                }
            }

            // īƮ �̵� ó��
            if (kart_speed > 0.0f) { // ����
                for (const auto& kart : karts) {
                    kart->translateMatrix = glm::translate(kart->translateMatrix, glm::vec3(0.0, 0.0, -kart_speed));
                }
            }
            else if (kart_speed < 0.0f) { // ����
                for (const auto& kart : karts) {
                    kart->translateMatrix = glm::translate(kart->translateMatrix, glm::vec3(0.0, 0.0, -kart_speed));
                }
            }

            // ���� ��ȯ ó��
            if (kart_keyState[LEFT]) {
                if (kart_speed != 0.0f) {
                    for (const auto& kart : karts) {
                        kart->translateMatrix = glm::translate(kart->translateMatrix, glm::vec3(0.0, 0.0, -1.5));
                        kart->translateMatrix = glm::rotate(kart->translateMatrix, glm::radians(TURN_ANGLE), glm::vec3(0.0f, 1.0f, 0.0f));
                        kart->translateMatrix = glm::translate(kart->translateMatrix, glm::vec3(0.0, 0.0, 1.5));
                    }
                }
            }

            if (kart_keyState[RIGHT]) {
                if (kart_speed != 0.0f) {
                    for (const auto& kart : karts) {
                        kart->translateMatrix = glm::translate(kart->translateMatrix, glm::vec3(0.0, 0.0, -1.5));
                        kart->translateMatrix = glm::rotate(kart->translateMatrix, glm::radians(-TURN_ANGLE), glm::vec3(0.0f, 1.0f, 0.0f));
                        kart->translateMatrix = glm::translate(kart->translateMatrix, glm::vec3(0.0, 0.0, 1.5));
                    }
                }
            }

            //ĳ���� 
            for (const auto& c : character) { //īƮ�� ���� ��� ����
                c->translateMatrix = karts[0]->translateMatrix;
            }

            // ī�޶� ȸ�� ������ ������Ʈ (�ӵ��� ���� ī�޶� ȸ���� ������)
            if (kart_speed != 0.0f) {
                reducedRotationInfluence = 0.1f + (std::abs(kart_speed) / MAX_SPEED) * 0.4f; // �ӵ� ��� ������
            }
            else {
                reducedRotationInfluence += 0.01f; // Ű�� �� ���� �� õõ�� ȸ��
                if (reducedRotationInfluence > 1.0f) reducedRotationInfluence = 1.0f;
            }

            // ī�޶� ������Ʈ
            setCamera();
            // ���� ī�޶� ��ġ�� ��ǥ ��ġ�� ���������� �̵�
            float cameraFollowSpeed = 0.1f; // ī�޶� ���󰡴� �ӵ� (0.0 ~ 1.0 ������ ��)
            cameraPos = glm::mix(cameraPos, cameraTargetPos, cameraFollowSpeed);

            checkCollisionKart();
            checkEngineSound();
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

    void mouseClick(int button, int state, int x, int y) override {
        if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {

        }
    }

    void keyboard(unsigned char key, int x, int y) override {
        moveCamera(key, x, y);
        if (key == 27) { //esc
            if (Pause) {
                glutTimerFunc(16, timerHelper, 0); // Ÿ�̸� ȣ��
            }
            else {
                glm::vec3 zAxis = glm::normalize(cameraPos - glm::vec3(karts[0]->translateMatrix[3]));

                // ������ ���� (X��) ���
                glm::vec3 xAxis = glm::normalize(glm::cross(cameraUp, zAxis));

                // ��� ���� (Y��) ���
                glm::vec3 yAxis = glm::cross(zAxis, xAxis);

                // 3x3 ȸ�� ��� ����
                glm::mat3 rotationMatrix = glm::mat3(
                    xAxis, // X��
                    yAxis, // Y��
                    zAxis  // Z��
                );

                // 4x4 ��ķ� Ȯ��
                glm::mat4 modelMatrix = glm::mat4(1.0f); // ���� ��ķ� �ʱ�ȭ
                modelMatrix[0] = glm::vec4(rotationMatrix[0], 0.0f); // X��
                modelMatrix[1] = glm::vec4(rotationMatrix[1], 0.0f); // Y��
                modelMatrix[2] = glm::vec4(rotationMatrix[2], 0.0f); // Z��
                modelMatrix[3] = glm::vec4(cameraPos, 1.0f);          // ��ġ �߰�

                pause[0]->translateMatrix = modelMatrix;
                pause[0]->translateMatrix = glm::translate(pause[0]->translateMatrix, glm::vec3(0.0, 0.0, -2.0));
            }
            Pause = !Pause;
        }
    }

    void specialKey(int key, int x, int y) override {

        int modifiers = glutGetModifiers();

        // Ctrl �ܵ� ����
        if (modifiers & GLUT_ACTIVE_CTRL) {
            std::cout << "Ctrl key is pressed (with special key: " << key << ")" << std::endl;
        }

        switch (key) {
        case GLUT_KEY_UP:
            kart_keyState[UP] = true;
            break;
        case GLUT_KEY_DOWN:
            kart_keyState[DOWN] = true;
            break;
        case GLUT_KEY_LEFT:
            kart_keyState[LEFT] = true;
            break;
        case GLUT_KEY_RIGHT:
            kart_keyState[RIGHT] = true;
            break;
        }
    }

    void specialKeyUp(int key, int x, int y) override {
        switch (key) {
        case GLUT_KEY_UP:
            kart_keyState[UP] = false;
            break;
        case GLUT_KEY_DOWN:
            kart_keyState[DOWN] = false;
            break;
        case GLUT_KEY_LEFT:
            kart_keyState[LEFT] = false;
            break;
        case GLUT_KEY_RIGHT:
            kart_keyState[RIGHT] = false;
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
        this->projection = glm::perspective(glm::radians(45.0f), static_cast<float>(width) / static_cast<float>(height), 0.1f, 10000.0f);
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
        for (const auto& c : character) { // ���� �� draw
            c->draw(shaderProgramID, isKeyPressed_s);
        }
        for (const auto& barricate : road1_barricate) { // ���� �� draw
            barricate->draw(shaderProgramID, isKeyPressed_s);
        }
        if (start_count >= 0 && start_count < 4) {
            countDown[start_count]->draw(shaderProgramID, isKeyPressed_s);
        }
        if (Pause)
            pause[0]->draw(shaderProgramID, isKeyPressed_s);

        glDisable(GL_DEPTH_TEST);
    }

    void draw_bb() override {

        for (const auto& model : karts) { // �� bb draw
            model->draw_rigidBody(shaderProgramID);
        }
        for (const auto& barricate : road1_barricate) { // �� bb draw
            barricate->draw_rigidBody(shaderProgramID);
        }
    }

    void finish() override {

    }
private:

    void updatePhysics(float deltaTime) {
        // ���� ���� ������Ʈ (deltaTime�� ���� ��Ȯ�� ����)
        dynamicsWorld->stepSimulation(deltaTime);

        // ���� �������� ��ü�� Transform ������Ʈ
        UpdateRigidBodyTransforms(karts);
        UpdateRigidBodyTransforms(road1_barricate);

        // �浹 ó�� (���� ���� ������Ʈ �� ����)
        checkCollisionKart();
    }

    static void timerHelper(int value) {
        if (Map1_Mode* instance = dynamic_cast<Map1_Mode*>(Mode::currentInstance)) {
            // ���� �ùķ��̼��� ���� �� �����Ͽ� ���� ��Ȯ�� ����
            const int physicsSteps = 2;  // ���� ������ ������ �����Ӹ��� �� �� ����
            const float deltaTime = 1.0f / 120.0f; // 120FPS (1�ʿ� 120�� ������Ʈ)

            for (int i = 0; i < physicsSteps; ++i) {
                instance->updatePhysics(deltaTime); // ���� �ùķ��̼� ������Ʈ
            }

            instance->timer(); // ������ ���� ������Ʈ
        }

        // ������ ������Ʈ�� 60FPS�� ����
        glutPostRedisplay();
        glutTimerFunc(16, timerHelper, value); // 60FPS ������ �ֱ�
    }


    // bgm ���� �Լ�
    void backgroundSound() {
        play_sound2D("village_04.ogg", "./asset/map_1/", true, &isBackgroundSound);
    }
    void count_go() {
        play_sound2D("count_go.wav", "./asset/map_1/", false, &isCountGoSound);
    }
    void count_n() {
        play_sound2D("count_n.wav", "./asset/map_1/", false, &isCountNSound);
    }
    void engine_sound() {
        play_sound2D("motor_x.ogg", "./asset/map_1/", true, &isMotorsound);
    }
};