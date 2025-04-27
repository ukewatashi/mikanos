#pragma once

#include "graphics.hpp"

class Console {
 public:
 // 縦　横
  static const int kRows = 25, kColumns = 80;

  Console(PixelWriter& writer,
      const PixelColor& fg_color, const PixelColor& bg_color);
  //　文字列出力
  void PutString(const char* s);

 private:
 //　改行
  void Newline();

  PixelWriter& writer_;
  const PixelColor fg_color_, bg_color_;
  char buffer_[kRows][kColumns + 1];
  //　カーソル
  int cursor_row_, cursor_column_;
};
