#version 330 core

layout (location = 0) in vec3 in_Position;  // ���� ��ġ
layout (location = 1) in vec3 in_Color;     // ���� ���� (�߰�)
layout (location = 2) in vec2 aTexCoords;  // �ؽ�ó ��ǥ (�ɼ�)

out vec3 Color;        // �����׸�Ʈ ���̴��� ���޵� ����
out vec2 TexCoords;    // �ؽ�ó ��ǥ (���� ����)

uniform mat4 model; // �� ��ȯ ��� (�ʿ� ���� ��� ���� ����)

void main() {
    // ������ Ŭ�� ���� ��ġ ��� (UI �뵵��� ���� ����)
    gl_Position = vec4(in_Position.xy, 0.0, 1.0); // z = 0.0, w = 1.0�� 2D ��ġ ����

    // ���� ������ �����׸�Ʈ ���̴��� ����
    Color = in_Color;
}
