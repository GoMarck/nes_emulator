#pragma once

namespace nes {
// 用于扩展内存的一个装置，位于卡带中，不同的卡带携带有不同的mmaper
class MMapper {
 public:
  MMapper() = default;
  ~MMapper() = default;
};
}  // namespace nes