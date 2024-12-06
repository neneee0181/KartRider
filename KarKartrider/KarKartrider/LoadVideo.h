#pragma once
#include <iostream>
#include <opencv2/opencv.hpp>

void loadVideo(std::string fileName, std::string path) {
    // 1. OpenCV ���� ���
    std::cout << "OpenCV Version: " << CV_VERSION << std::endl;

    // 2. ������ ���� ����
    cv::VideoCapture cap(path + fileName);

    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open video file." << std::endl;
        return;
    }

    // 3. ������ ���� (â �̸� ����)
    cv::namedWindow("KarKartrider intro", cv::WINDOW_NORMAL); // `cv::WINDOW_NORMAL`�� ����ϸ� ũ�� ���� ����

    cv::moveWindow("KarKartrider intro", location_x, location_y);
    cv::resizeWindow("KarKartrider intro", window_width, window_height);

    // 5. ������ ��� ����
    cv::Mat frame;
    while (true) {
        cap >> frame; // ������ �б�
        if (frame.empty()) {
            break; // ������ ��
        }

        // 6. â ���� Ȯ��
        if (cv::getWindowProperty("KarKartrider intro", cv::WND_PROP_VISIBLE) < 1) {
            break; // â�� �������� ���� ����
        }

        // 7. ������ ȭ�鿡 ���
        cv::imshow("KarKartrider intro", frame);

        // 8. ESC Ű�� ������ ����
        if (cv::waitKey(30) == 27) {
            break;
        }
    }

    cap.release(); // �ڿ� ����
    cv::destroyAllWindows(); // ��� â �ݱ�
    return;
}