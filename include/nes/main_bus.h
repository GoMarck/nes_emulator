#pragma once

// Bus connect CPU and Cartridges and memory。
// 主要职责有：
// 1. 读取程序代码（位于卡带中，称之为ROM）
// 2. 读写内存（16位内存表示）
// 3. 读写IO寄存器（和PPU的数据交换桥梁）
namespace nes {
class MainBus {
public:
  MainBus() = default;
  ~MainBus() = default;
};
}