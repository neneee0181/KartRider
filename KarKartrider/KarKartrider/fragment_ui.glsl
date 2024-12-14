#version 330 core

in vec3 FragPos;      // ���� ���������� ��ġ (�Ϲ� 3D ���������� ���)
in vec3 Normal;       // ���� ���� (�Ϲ� 3D ���������� ���)
in vec2 TexCoords;    // �ؽ�ó ��ǥ

out vec4 FragColor;

uniform sampler2D texture1; // �ؽ�ó (UI �Ǵ� ����)
uniform bool isUI;          // UI ��� �÷���

void main() {
   FragColor = texture(texture1, TexCoords);
}
