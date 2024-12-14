#version 330 core

layout (location = 0) in vec3 in_Position;  // ���� ��ġ
layout (location = 1) in vec3 in_Normal;    // ���� ����
layout (location = 2) in vec2 aTexCoords;  // �ؽ�ó ��ǥ (�ɼ�)
layout (location = 4) in vec3 in_Color;     // ���� ���� (�߰�)

out vec3 Color;        // �����׸�Ʈ ���̴��� ���޵� ����
out vec3 FragPos;      // �����׸�Ʈ ��ġ (world space)
out vec3 Normal;       // �����׸�Ʈ�� ���� ���� (world space)
out vec2 TexCoords;    // �ؽ�ó ��ǥ�� �����׸�Ʈ ���̴��� ����

uniform mat4 model;              // �� ��ȯ ���
uniform mat4 viewTransform;      // �� ��ȯ ���
uniform mat4 projectionTransform; // ���� ��ȯ ���
uniform mat3 normalMatrix;       // ���� ��ȯ ��� (C++���� ����Ͽ� ����)
uniform bool isUI;

void main() {
    if (isUI) {
        // ������ Ŭ�� ���� ��ġ ��� (UI �뵵��� ���� ����)
        gl_Position = vec4(in_Position.xy, 0.0, 1.0); // z = 0.0, w = 1.0�� 2D ��ġ ����

        // ���� ������ �����׸�Ʈ ���̴��� ����
        Color = in_Color;
    }else{
          // Ŭ�� ��ǥ ���
        gl_Position = projectionTransform * viewTransform * model * vec4(in_Position, 1.0);

        // ���� ���������� ���� ��ġ ���
        FragPos = vec3(model * vec4(in_Position, 1.0));

        // ���� ���͸� ���� ��ķ� ��ȯ
        Normal = normalize(normalMatrix * in_Normal);

        // �ؽ�ó ��ǥ ����
        TexCoords = aTexCoords;
    }
}
