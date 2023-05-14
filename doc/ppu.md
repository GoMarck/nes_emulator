PPU规格：
1. 扫描线（scanline）：PPU总共有262条扫描线需要渲染
2. 渲染时钟周期：每条扫描线的渲染需要耗费341个时钟周期
3. PPU时钟频率是CPU的时钟频率的3倍，即PPU指令执行3条，CPU指令执行1条

扫描线的4个时期：
1. Pre-Render（-1，261）：预渲染时期，一开始或者结尾的扫描线会处于这个时期
2. Render（0 ~ 239）
3. Post-Render（240） 
4. V-Blank（241 ~ 260）



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

参考链接：
1. https://www.nesdev.org/wiki/PPU_rendering
2. https://www.nesdev.org/w/images/default/4/4f/Ppu.svg
3. https://www.nesdev.org/wiki/PPU_scrolling

奇数帧和偶数帧：
