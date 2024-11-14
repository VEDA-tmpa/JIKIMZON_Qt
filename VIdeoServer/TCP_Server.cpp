#include <iostream>
#include <boost/asio.hpp>
#include <opencv2/opencv.hpp>
#include <vector>

using boost::asio::ip::tcp;
namespace asio = boost::asio;

// 클라이언트로 이미지를 보내는 함수 (이미지 크기를 먼저 전송)
void send_image_to_client(tcp::socket& socket, cv::Mat& frame) {
    // 이미지를 바이너리 형식으로 인코딩
    std::vector<uchar> buffer;
    std::vector<int> params = { cv::IMWRITE_JPEG_QUALITY, 90 };  // JPEG 품질 설정
    bool success = cv::imencode(".jpg", frame, buffer, params);

    if (!success) {
        std::cerr << "프레임 인코딩 실패!" << std::endl;
        return;
    }

    // 이미지 크기 보내기
    uint32_t size = buffer.size();
    boost::asio::write(socket, boost::asio::buffer(&size, sizeof(size)));

    // 이미지 데이터 보내기
    boost::asio::write(socket, boost::asio::buffer(buffer.data(), buffer.size()));

    std::cout << "프레임 전송 완료!" << std::endl;
}

int main() {
    try {
        asio::io_context io_context;

        // 서버 소켓 설정
        tcp::endpoint endpoint(tcp::v4(), 9999);  // 포트 9999에서 연결 대기
        tcp::acceptor acceptor(io_context, endpoint);

        std::cout << "서버가 시작되었습니다. 연결을 기다리는 중..." << std::endl;

        // 클라이언트 연결 수락
        tcp::socket socket(io_context);
        acceptor.accept(socket);
        std::cout << "클라이언트가 연결되었습니다." << std::endl;

        // 카메라 열기
        cv::VideoCapture cap(0);  // 기본 카메라 열기
        cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);  // 너비 설정
        cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480); // 높이 설정
        if (!cap.isOpened()) {
            std::cerr << "카메라 열기 실패!" << std::endl;
            return -1;
        }

        while (true) {
            cv::Mat frame;
            cap >> frame;  // 카메라에서 한 프레임을 읽기

            if (frame.empty()) {
                std::cerr << "프레임 읽기 실패!" << std::endl;
                break;
            }

            send_image_to_client(socket, frame);  // 클라이언트로 이미지 전송
        }

        cap.release();  // 카메라 리소스 해제
    } catch (const std::exception& e) {
        std::cerr << "예외 발생: " << e.what() << std::endl;
    }

    return 0;
}
