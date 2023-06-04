PPU规格：
1. 扫描线（scanline）：PPU总共有262条扫描线需要渲染
2. 渲染时钟周期：每条扫描线的渲染需要耗费341个时钟周期
3. PPU时钟频率是CPU的时钟频率的3倍，即PPU指令执行3条，CPU指令执行1条

扫描线的4个时期：
1. Pre-Render（-1，261）：预渲染时期，一开始或者结尾的扫描线会处于这个时期
2. Render（0 ~ 239）
3. Post-Render（240） 
4. V-Blank（241 ~ 260）：The PPU generates a video signal for one frame of animation, then it rests for a brief period called vertical blanking. The CPU can load graphics data into the PPU only during this rest period. From NMI to the pre-render scanline, the NTSC NES PPU stays off the bus for 20 scanlines or 2273 cycles. Taking into account overhead to get in and out of the NMI handler, you can probably use roughly 2250 cycles. To get the most out of limited vblank time, don't compute your changes in vblank time. Instead, prepare a buffer in main RAM (for example, use unused parts of the stack at $0100-$019F) before vblank, and then copy from that buffer into VRAM during vblank. On NTSC, count on being able to copy 160 bytes to nametables or the palette using a moderately unrolled loop, plus one 256-byte display list to OAM.



每条scanline执行的341个指令周期如下：

cycle 0: 空闲指令周期，啥都不做

cycle 1 ~ 256：获取tile的时期，每个tile的获取需要访问4次内存，每访问一次内存都需要消耗2个PPU时钟周期，因此1个tile的获取就需要耗费8个PPU时钟周期，256个PPU时钟周期可以获取32个tile。4次内存访问的内容如下：
1. Nametable byte
2. Attribute table byte
3. Pattern table tile low
4. Pattern table tile high (+8 bytes from pattern table tile low)

cycle 257 ~ 320：渲染tile的时期，将tile都渲染到屏幕上

cycle 321 ~ 336：获取下一个scanline的最开始的2个tile，总共耗费16个PPU时钟周期

cycle 337 ~ 340：耗费4个PPU时钟周期获取Nametable bytes，但是目的未定义

PPU的内部寄存器：
1. v: 存储的是当前的 VRAM 地址 (15 bits)
2. t: Temporary VRAM address (15 bits); can also be thought of as the address of the top left onscreen tile.
3. x: Fine X scroll (3 bits)
4. w: First or second write toggle (1 bit)

而在渲染时期，v和t的组成如下：
```bash
yyy NN YYYYY XXXXX
||| || ||||| +++++-- coarse X scroll
||| || +++++-------- coarse Y scroll
||| ++-------------- nametable select
+++----------------- fine Y scroll
```
另外由于PPU的内存地址表示只需要 14 bits，因此 v 寄存器在作为PPU的内存地址表示时，需要屏蔽最高位。

参考链接：
1. https://www.nesdev.org/wiki/PPU_rendering
2. https://www.nesdev.org/w/images/default/4/4f/Ppu.svg
3. https://www.nesdev.org/wiki/PPU_scrolling

奇数帧和偶数帧：

我们为什么需要vblank：当PPU处于vblank时期时，说明PPU处于休息期间，此时CPU就可以与PPU进行数据交互，传输下一帧画面等。当PPU进入到vblank时期时，它会通过NMI中断通知CPU。链接：https://www.nesdev.org/wiki/The_frame_and_NMIs


---



OAM渲染原理以及数据upload理解，之后设计架构图直接开干，下周输出架构图，并开始尝试设计接口等。

剩余工作：
1. 输出整体的架构图，CPU、PPU、卡带、Bus等的关系
2. 确认CPU和PPU之间的配合关系
3. 理清楚PPU OAM人物的渲染逻辑以及数据装载等逻辑 *(2023.6.4)
4. 学习下SFML的用法