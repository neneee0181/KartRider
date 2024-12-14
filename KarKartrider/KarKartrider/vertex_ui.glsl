#version 330 core

layout (location = 0) in vec3 in_Position;  // ���� ��ġ
layout (location = 2) in vec2 aTexCoords;  // �ؽ�ó ��ǥ

out vec2 TexCoords;    // �ؽ�ó ��ǥ�� �����׸�Ʈ ���̴��� ����

uniform mat4 model;              // �� ��ȯ ���

void main() {
    // UI ���: ���� ��ġ�� ���� Ŭ�� �������� ����
    gl_Position = vec4(in_Position.xy, 0.0, 1.0); // z = 0.0, w = 1.0�� 2D ��ġ ����
    // �ؽ�ó ��ǥ�� UI�� ���� ������ ��� ����
    TexCoords = aTexCoords;
}
