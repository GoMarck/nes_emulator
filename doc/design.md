# 任天堂模拟器设计说明书
本文档主要涉及任天堂模拟器 `NES Emulator` 的详细设计，包括：
- 模拟器的输入文件
- CPU的设计与模拟
- PPU的设计与模拟
- Cartridges的设计与模拟

调研问题：
1. PPU是如何进行图像渲染的：https://austinmorlan.com/posts/nes_rendering_overview/
2. CPU如何模拟指令集，指令的寻址方式：https://zhuanlan.zhihu.com/p/599355283
3. CPU与PPU如何进行数据层面的交互？
4. 卡带的数据如何读入到系统中，CPU如何取卡带中的数据？
5. CPU的指令周期如何模拟，合适刷新页面？


# CPU 内存
CPU的内存布局可分为 3 块：
1. ROM
2. RAM
3. I/O Registers

它们之间由总线进行串联，如下图所示：

![](image/cpu%E5%86%85%E5%AD%98%E5%B8%83%E5%B1%80.png)

### ROM
ROM 全称为 Read Only Memory，即只读内存，从卡带（Cartridge）加载而来。ROM 内部主要放置一些