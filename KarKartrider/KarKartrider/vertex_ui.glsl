#version 330 core

layout (location = 0) in vec3 in_Position;  // ���� ��ġ
layout (location = 1) in vec3 in_Normal;    // ���� ����
layout (location = 2) in vec2 aTexCoords;  // �ؽ�ó ��ǥ (�ɼ�)
layout (location = 4) in vec3 in_Color;     // ���� ���� (�߰�)

out vec3 Color;        // �����׸�Ʈ ���̴��� ���޵� ����
out vec2 TexCoords;    // �ؽ�ó ��ǥ�� �����׸�Ʈ ���̴��� ����

uniform mat4 model;              // �� ��ȯ ���
uniform bool isRed;          // UI ��� �÷���
uniform bool isTimer;          // UI ��� �÷���
uniform bool isTextrue;

void main() {
    // gl_Position�� �׻� 2D ȭ�� �������� ����
    gl_Position = vec4(in_Position.xy, 0.0, 1.0); // z = 0.0, w = 1.0�� ����

    // isRed ��忡���� ���� ����
    if (isRed) {
        Color = in_Color;
    }
    // isTimer ��忡���� Ÿ�̸� ���� ����
    else if (isTimer) {
        Color = in_Color; 
    }
    // �ؽ�ó ����� ��� �ؽ�ó ��ǥ ����
    else if (isTextrue) {
        TexCoords = aTexCoords; // �ؽ�ó ��ǥ�� �����׸�Ʈ ���̴��� ����
    }
}
