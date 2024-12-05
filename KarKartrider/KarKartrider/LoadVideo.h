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

    // 3. ������ ��� ����
    cv::Mat frame;
    while (true) {
        cap >> frame; // ������ �б�
        if (frame.empty()) {
            break; // ������ ��
        }

        // 4. ������ ȭ�鿡 ���
        cv::imshow("Video Playback", frame);

        // 5. ESC Ű�� ������ ����
        if (cv::waitKey(30) == 27) {
            break;
        }
    }

    cap.release(); // �ڿ� ����
    cv::destroyAllWindows(); // ��� â �ݱ�
    return;
}