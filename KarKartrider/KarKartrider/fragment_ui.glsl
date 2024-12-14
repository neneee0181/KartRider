#version 330 core

in vec3 Color;         // ���ؽ� ���̴����� ���޹��� ����
in vec2 TexCoords;     // �ؽ�ó ��ǥ (�ʿ� ���� ��� ���� ����)

out vec4 FragColor;    // ���� ��� ����

uniform bool isUI;          // UI ��� �÷���
uniform sampler2D texture1; // �ؽ�ó (�ɼ�: �ʿ� ���� ��� ���� ����)
uniform bool isRed;          // UI ��� �÷���
uniform bool isTimer;          // UI ��� �÷���

void main() {
      if(isRed){
            FragColor = vec4(1.0, 0.0, 0.0, 1.0);
      }
      if(isTimer){
            FragColor = vec4(1.0, 1.0, 1.0, 1.0);
      }
}
