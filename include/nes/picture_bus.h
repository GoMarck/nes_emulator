#pragma once

#include <algorithm>
#include <memory>

#include "nes/mmaper.h"
#include "nes/type.h"

// Bus connect PPU and Cartridges
// PPU和卡带的桥梁，用于从卡带中加载数据到PPU中进行渲染
namespace nes {

class PictureBus {
 public:
  explicit PictureBus(std::shared_ptr<MMapper> mmapper)
      : mmapper_(std::move(mmapper)) {}

  ~PictureBus() = default;

 private:
  /// Memory-mapper for CHR-ROM access.
  std::shared_ptr<MMapper> mmapper_;
};

}  // namespace nes