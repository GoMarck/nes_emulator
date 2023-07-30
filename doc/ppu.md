# PPU
PPU 的全称为 Picture Process Unit，即图像处理单元，可以把它理解为 NES 的显卡，主要负责渲染每一帧游戏画面。PPU 的渲染原理分为如下几个小章节：
- PPU 和 CPU 的关系
- PPU 的内存布局
- PPU 的内部寄存器
- PPU 背景渲染原理
- PPU 精灵（Sprites）渲染原理
- PPU 扫描线（Scanline）

## 0. PPU 和 CPU 的关系
在 NES 中，CPU 主要负责执行程序，而 PPU 则主要负责对每一帧画面进行渲染。

## 1. PPU 的内存布局

## 2. PPU 的内部寄存器
## 3. PPU 背景渲染原理
## 4. PPU 精灵（Sprites）渲染原理
## 5. PPU 扫描线（Scanline）
![](https://www.nesdev.org/w/images/default/4/4f/Ppu.svg)

## 6. OAM
参考链接：https://www.nesdev.org/wiki/PPU_OAM

OAM 全称为 Object Attribute Memory，是PPU内部一块用于保存精灵（sprits）信息的内存。章节如下：
- 内存布局
- 角色（Character）和精灵（Sprite）的关系
- 渲染优先级
- Sprite 0 Hit
- 扫描线（Scanline）与精灵（Sprite）
- OAM 同步

### 6.0 内存布局
OAM 的内存大小为 256 bytes，至多可以存储 64 个精灵，每个精灵共占据4个字节，这4个字节的表示如下所示：

Byte 0
---
表示精灵的 Y 坐标，由于精灵永远不会显示在屏幕的第一行，因此精灵实际的 Y 坐标需要减去 1。在需要隐藏精灵时，可以通过将精灵向下移动到屏幕外来实现隐藏的效果，方法是将该字节写入 #EF ～ #FF 之间的任意值。精灵的隐藏永远是向下隐藏，它永远不会显示在屏幕的顶部之外。

Byte 1
---
表示精灵 Tile 的索引值，精灵的 Tile 有两种规格：8x8 的以及 8x16 的，这两种规格的取法略有不同，具体如下：

对于 8x8 的 Tile 来说，该值指的就是样式表（Pattern Table）上的索引，而具体为哪个样式表则由 PPUCTRL($2000) 的 bit 3 所决定；

对于 8x16 的 Tile 来说，样式表（Pattern Table）的选择则是直接由 bit 0 所决定的，具体如下所示：

```bash
76543210
||||||||
|||||||+- Bank ($0000 or $1000) of tiles
+++++++-- Tile number of top of sprite (0 to 254; bottom half gets the next tile)
```

因此，它的取值所对应的 Tile 索引如下所示：

```bash
$00: $0000-$001F
$01: $1000-$101F
$02: $0020-$003F
$03: $1020-$103F
$04: $0040-$005F
[...]
$FE: $0FE0-$0FFF
$FF: $1FE0-$1FFF
```

Byte 2
---
表示精灵的属性，各个 bit 位的具体表示如下：
```bash
76543210
||||||||
||||||++- Palette (4 to 7) of sprite
|||+++--- Unimplemented (read 0)
||+------ Priority (0: in front of background; 1: behind background)
|+------- Flip sprite horizontally
+-------- Flip sprite vertically
```
其中 bit 0 和 bit 1 用于表示调色板的编号，用于制定精灵的颜色；bit 5 表示与背景板的优先级，0 表示渲染于背景前，1 表示渲染于背景后；bit 6 和 bit 7 分别表示水平/垂直翻转。

Byte 3
---
精灵的 X 坐标（#00 ~ #FF）。

### 6.1 Character 和 Sprite 的关系
一个游戏内通常包含着不少角色（Character），例如超级马里奥中就有马里奥、库巴、蘑菇等可以移动的角色，这些角色通常由 1～n 个精灵（Sprite）组成，例如下面的马里奥就是由 8 个精灵组成的 4x2 的角色：
![](image/马里奥组成.png)

### 6.2 渲染优先级
精灵本身的渲染是有优先级的，它们的优先级即为它们在 OAM 中的索引值（0～63），其中 0 优先级最高。当多个不同的精灵渲染到同一个像素的时候，优先级高的精灵会覆盖掉优先级低的精灵。

除此之外，Byte 2 的 bit 5 表示了精灵和背景谁被渲染的优先级，如果该 bit 位为 1，那么精灵也将不会被渲染出来。这种可以表示角色的一部分身体藏在树后面的场景。

### 6.3 Sprite 0 Hit
当第 0 个 Sprite 被渲染（其首个像素覆盖到了非透明的背景像素）时，被称为Sprite Zero Hit。PPU 寄存器 PPUSTATUS 中的 bit 6 将会被设置，来记录Sprite Zero Hit。

### 6.4 Scanline & Sprite
每一条扫描线（Scanline）在渲染像素的时候，都会检查并计算是否有需要进行渲染的精灵（Sprite），如果有的话就会对相应的像素进行渲染。需要注意的是每条扫描线最多只能支持8个精灵的渲染，当某一行扫描线的精灵超过 8 个时候，只能渲染出前 8 个精灵，这种现象被称为 Sprite  Overflow，此时 PPUSTATUS 中的 bit 5 会被设置，用于记录 Sprite Overflow。

由于这种限制的存在，当游戏内的角色非常多时（例如魂斗罗），我们会看到画面中的角色出现闪烁的现象。

### 6.5 OAM 同步
OAM 的数据同步有两种方式：
1. 设置 OAMADDR 以及 OAMDATA 进行更新
2. 通过 DMA 进行更新

设置 OAMADDR 以及 OAMDATA 进行更新
---
CPU 通过写入 $2003（OAMADDR），将某个 OAM 的地址起始值写入 PPU 的 OAMADDR 寄存器。
接着，CPU 通过写入 $2004（OAMDATA），将数据写入 OAMADDR 所指向的地址，从而完成某个 OAM 的更新。

这种方式的缺点是执行的 CPU 周期非常多，更新慢，因此很少使用这种方式进行更新。

通过 DMA 进行更新
---
DMA 全称为 Direct Memory Access，它允许将 CPU 中某一页（256 bytes）的内存直接更新到 OAM 中。具体方式是 CPU 通过写入 $4014（OAMDMA），指定一个内存页，接着经过 513-514 个 CPU 时钟周期，这一页中的 256 个字节将会被拷贝到 OAM 中。这是最常用的更新 Sprite 的方式。 