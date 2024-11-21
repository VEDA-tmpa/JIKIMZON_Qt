#ifndef FRAME_H
#define FRAME_H

namespace cctv
{
constexpr int WIDTH = 1280;                // 프레임 너비
constexpr int HEIGHT = 720;               // 프레임 높이
constexpr int PIXEL_SIZE = 3;             // 픽셀당 바이트 크기 (RGB)
constexpr int FRAME_SIZE = WIDTH * HEIGHT * PIXEL_SIZE; // 프레임 크기
}

#endif // FRAME_H
