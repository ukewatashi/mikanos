/**
 * @file main.cpp
 *
 * カーネル本体のプログラムを書いたファイル．
 */

#include <cstdint>
#include <cstddef>

#include "frame_buffer_config.hpp"

struct PixelColor {
  uint8_t r, g, b;
};

// #@@range_begin(pixel_writer)

class PixelWriter {
 public:
 //　コンストラクタ関数
  PixelWriter(const FrameBufferConfig& config) : config_{config} {
  }
  // デストラクタ関数
  virtual ~PixelWriter() = default;
  // 純粋仮想関数
  //virtual void Write(int x, int y, const PixelColor& c) = 0;

 protected:
  uint8_t* PixelAt(int x, int y) {
    return config_.frame_buffer + 4 * (config_.pixels_per_scan_line * y + x);
  }

 private:
 // メンバ変数
  const FrameBufferConfig& config_;
};
// #@@range_end(pixel_writer)

// #@@range_begin(derived_pixel_writer)
class RGBResv8BitPerColorPixelWriter : public PixelWriter {
 public:
 //　継承
  using PixelWriter::PixelWriter;
//　オーバーライド
  virtual void Write(int x, int y, const PixelColor& c) {
    auto p = PixelAt(x, y);
    p[0] = c.r;
    p[1] = c.g;
    p[2] = c.b;
  }
};

class BGRResv8BitPerColorPixelWriter : public PixelWriter {
 public:
  using PixelWriter::PixelWriter;

  virtual void Write(int x, int y, const PixelColor& c)  {
    auto p = PixelAt(x, y);
    p[0] = c.b;
    p[1] = c.g;
    p[2] = c.r;
  }
};
// #@@range_end(derived_pixel_writer)

// #@@range_begin(placement_new)
void* operator new(size_t size, void* buf) {
  return buf;
}

void operator delete(void* obj) noexcept {
}
// #@@range_end(placement_new)

char pixel_writer_buf[sizeof(RGBResv8BitPerColorPixelWriter)];
PixelWriter* pixel_writer;

// #@@range_begin(call_pixel_writer)
extern "C" void KernelMain(const FrameBufferConfig& frame_buffer_config) {
  switch (frame_buffer_config.pixel_format) {
    case kPixelRGBResv8BitPerColor:
    //　配置new　情報渡す instance
      pixel_writer = new(pixel_writer_buf) 
        RGBResv8BitPerColorPixelWriter{frame_buffer_config};
      break;
    case kPixelBGRResv8BitPerColor:
      pixel_writer = new
                      (pixel_writer_buf)
                       BGRResv8BitPerColorPixelWriter{frame_buffer_config};
      break;
  }

  for (int x = 0; x < frame_buffer_config.horizontal_resolution; ++x) {
    for (int y = 0; y < frame_buffer_config.vertical_resolution; ++y) {
      (new(pixel_writer_buf) 
        RGBResv8BitPerColorPixelWriter{frame_buffer_config})->Write(x, y, {255, 255, 255});
    }
  }
  for (int x = 0; x < 200; ++x) {
    for (int y = 0; y < 100; ++y) {
      (new(pixel_writer_buf) 
        RGBResv8BitPerColorPixelWriter{frame_buffer_config})->Write(x, y, {0, 255, 0});
    }
  }
  while (1) __asm__("hlt");
}
// #@@range_end(call_pixel_writer)
