#pragma once

// Bus connect PPU and Cartridges
// PPU和卡带的桥梁，用于从卡带中加载数据到PPU中进行渲染
namespace nes {

class PictureBus {
public:
  PictureBus() = default;
  ~PictureBus() = default;
};

}