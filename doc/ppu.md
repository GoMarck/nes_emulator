# PPU
PPU 的全称为 Picture Process Unit，即图像处理单元，可以把它理解为 NES 的显卡，主要负责渲染每一帧游戏画面。PPU 的渲染原理分为如下几个小章节：
- PPU 和 CPU 的关系
- PPU 的内存布局
- PPU 渲染原理
- PPU 扫描线（Scanline）
- PPU 滚动
- PPU 设计

## 1. PPU 和 CPU 的关系
在 NES 中，CPU 主要负责执行程序、接受外部输入、向 PPU 传输数据等，而 PPU 则主要负责对每一帧画面进行渲染。
![](image/架构图.png)
## 2. PPU 的内存布局
![](image/PPU内存布局图.png)

## 3. PPU 渲染原理
PPU 的渲染可以分为两个部分：分别为背景渲染以及精灵渲染。精灵即为我们在游戏中控制的角色以及一些可移动的物体，如马里奥、库巴、蘑菇等，这两者渲染的结合最后便组成了我们的游戏画面。而这里面首先需要解决的就是如何获取到游戏画面的数据以及这些数据是如何进行渲染的。这里面会涉及到 PPU 几个核心的概念：
- 样式表（Pattern Table）
- 调色盘（Color Palette）
- 名字表（Name Table）
- 属性表（Attribute Table）
- OAM

### 3.1 Pattern Table
Pattern Table 即为样式表，它储存在卡带（Cartiage）的 CHR ROM 中，并且只能被 PPU 通过 PPU Bus 的方式进行访问。它里面储存的便是我们的游戏画面，它们以 Tile 为粒度进行存储。

卡带中有两个 Pattern Table，分别映射到了 PPU 的 $0000~$0FFFF 以及 $1000 ~$1FFF。无论是背景还是精灵，它们的图像数据均来源于 Pattern Table。

Tile 又称图元，是一个图像的最小单位，一个 Tile 占用 16 bytes，可以渲染 8x8 的像素，原理为将 Tile 对应的数据按照 8 bytes 分为前后两片，以下列数据为例：
```bash
# Address   Value
$0000       0x10
$0001       0x00
$0002       0x44
$0003       0x00
$0004       0xFE
$0005       0x00
$0006       0x82
$0007       0x00

$0008       0x00
$0009       0x28
$000A       0x44
$000B       0x82
$000C       0x00
$000D       0x82
$000E       0x82
$000F       0x00
```
将它们按照 8 bytes 分为两块（$0000~$0007 以及 $0008~$000F）并且按照bitmap 展开后如下所示：
![](image/tile.png)

然后将它们按位进行组合，$0000~$0007 的 bit 位为低位，$0008 ~ $000F 的为高位，便可以组成 00B，01B，10B，11B 这 4 种组合，例如图中所圈出来的 bit 位，它们便可以组合成 11B。最终它们可以组合成如下的图案：

![](image/tile组合.png)

如果我们将 01B，10B 以及 11B 着色，会发现显示出来的是一个字母 'A'。至此，我们便将一个 8x8 像素的图像给渲染出来了。而上面的数字，则是对应于 Color Palette（调色盘）的颜色编号，当将其颜色渲染到对应的像素点时，我们带有颜色的图像也就可以显现出来了。

而 Pattern Table 总共有 8 KB（$0000~$1FFFF），因此其总共可以存储 512 个 Tile。

### 3.2 Color Palette
Pattern Table（样式表）提供了每个 8x8 Tile 的像素点（00B～11B），这些像素点进行颜色填充之后，就会渲染出我们期望的游戏画面。而像素点中的 00B~11B 分别对应了那些具体的颜色，则由 Color Palette（调色板）来决定。

Color Palette 位于 $3F00～$3F1F，共 32 字节。其中前 16 字节的内存区域称为 Image palette，用于背景的渲染；而后 16 字节则称为 Sprite Palette，用于精灵的渲染。

每个字节上存储的并不是具体的颜色信息，而是颜色索引（Color Index）。NES 总共有 52 中颜色可用于渲染，对应的索引及其 RGB 列表如下所示：

![](image/调色板.png)

![](image/调色板RGB.png)


#### 3.2.1 Image Palette
对于 Image Palette 来说，16 个字节可分为 4 组 Color Set（分别是 $3F00~$3F03，$3F04~$3F07，$3F08~$3F0B 以及 $3F0C~$3F0F）。

其中每组的第一个字节（$3F00、$3F04、$3F08、$3F0C）用于表示背景的透明，他们索引都是相同的。而其余的一般则是各不相同，表示不同的颜色，因此 Image Palette 最多可以表示13 种颜色，即背景最多可以用 13 种颜色进行渲染。

#### 3.2.2 Sprite Palette
和 Image Palette 一样，Sprite Palette 也是分为 4 组 Color Set（$3F10~$3F13，$3F14~$3F17，$3F18~$3F1B 以及 $3F1C~$3F1F）。但是不同的是，其每组的第一个字节是无效颜色，不会被使用，因此 Sprite Palette 最多可以表示 12 种颜色，即精灵最多可以用 12 种颜色进行渲染。

#### 3.2.3 如何选择调色板
由 3.2.1 和 3.2.2 可知，背景和精灵的调色板都由 4 组 Color Set 组成，在渲染具体的 Tile 的时候，如下所示：

![](image/tile组合.png)

核心就是选中一组 Color Set，然后根据 Color Set 上的颜色，将像素点渲染出来，渲染完毕之后我们的 Tile 就会呈现出一副彩色图像了。

而每个 Tile 如何确定使用的是哪个 Color Set 则由 Attribute Table 决定，详细的可查看 Attritube Table 章节是如何做确定的。

### 3.3 Name Table
当我们知道如何渲染出 Pattern Table 中每个 Tile 的游戏画面之后，还需要解决的一个问题就是：如何将这些 Tile 按照一定的顺序进行排列，从而组成一幅完整的游戏画面，而这就是 Name Table 所需要做的事情。

Name Table 本质上就是一个 32x30 的 Tile 矩阵，Name Table 中的每个 bytes 指向的是 Tile 在 Pattern Table 中的索引，而每个 Tile 表示的是 8x8 的像素，因此 Name Table 即可表示 256x240 分辨率的画面。

Name Table 的矩阵可按照每 32 bytes 为一行，共 30 行看待，如下所示：
```bash
$2000, $2001, ... $201E, $201F,
$2020, $2021, ... $203E, $203F,
...
$2380, $2381, ... $24BE, $24BF,
$23A0, $23A1, ... $23BE, $23BF,
```
PPU 共有 4 个 Name Table，它们的分布如下所示：
```bash
+------------+------------+
|            |            |
|            |            |
| NameTable0 | NameTable1 |
|            |            |
|            |            |
+------------+------------+
|            |            |
|            |            |
| NameTable2 | NameTable3 |
|            |            |
|            |            |
+------------+------------+
```

4 个 NameTable 总共可渲染出 512x480 的图像，但是我们的屏幕只有 256x240，意味着总共可以渲染出 4 块屏幕，而这是为了能够平滑移动所设计的，图像的边缘已经渲染好，使得移动的时候图像渲染能够非常的平滑。

而实际上，PPU 仅有 2KB 的内存用于存储 Name Table 以及 Attribute Table，因此 PPU 的内存中只能同时存下两个 Name Table 的数据，所以会采用镜像的方式去填充数据，不同的游戏镜像方式会有所不同。

水平镜像如下所示：

![](image/水平镜像.png)

垂直镜像如下所示：

![](image/垂直镜像.png)

单屏镜像：

![](image/单屏镜像.png)

不使用镜像：

![](image/无镜像.png)

不使用镜像这种方式需要卡带额外再提供 2 KB 的内存。

### 3.4 Attribute Table
每一个 Name Table 的尾部都会跟有一个大小为 64 bytes 的 Attribute Table，它的作用是指明每个 Tile 需要用哪个调色板进行着色。由于 Attribute Table 只有 64 bytes，而 Name Table 则有 960 bytes，因此 Attribute Table 中的每一个字节需要管理 16 个 Tile，这些 Tile 按照 4x4 的大小进行分割，将这 4x4 的 Tile 称为一个 block，如下所示：

![](image/attribute_table.png)

而每个 block 又可继续分为 4 个 2x2 Tile，我们称之为 sub-block，如下所示：

![](image/block.png)

0～4 分别表示 1 个 block 下的 4 个 sub-block，每个 sub-block 代表了 Attribute Table 每个字节上 bit 位的分布，如下所示：
```bash
7654 3210
|||| ||++- Color bits 1-0 for sub-block 0
|||| ++--- Color bits 3-2 for sub-block 1
||++------ Color bits 3-2 for sub-block 2
++-------- Color bits 3-2 for sub-block 3
```

根据 Attribute Table 的每个字节，我们就能确认每个 block 用什么调色板去进行着色。例如 Attribute Table 中的某一个字节为 0xAA，那么它的 bit 位表示为：
```bash
10 10 10 10
|| || || ++- Color bits 1-0 for sub-block 0
|| || ++---- Color bits 3-2 for sub-block 1
|| ++------- Color bits 3-2 for sub-block 2
++---------- Color bits 3-2 for sub-block 3
```
因此它表示这个 block 下的所有的 Tile 都是用 2 号调色板进行着色。

### 3.5 OAM
参考链接：https://www.nesdev.org/wiki/PPU_OAM

上述对于 Name Table，Attribute Table 解决的事背景的排布和渲染，而当背景渲染解决了之后，还有另一块需要渲染的就是精灵的渲染。不同于背景有固定的放置位置，精灵往往是可以移动的单位，因此它需要有更多的字节用于表示 X, Y 坐标、翻转等信息，而这一块就是 OAM 所需要解决的事情。

OAM 全称为 Object Attribute Memory，是PPU内部一块用于保存精灵（sprits）信息的独立内存。章节如下：
- 内存布局
- 角色（Character）和精灵（Sprite）的关系
- 渲染优先级
- Sprite 0 Hit
- 扫描线（Scanline）与精灵（Sprite）
- OAM 同步

#### 3.5.1 内存布局
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

### 3.5.2 Character 和 Sprite 的关系
一个游戏内通常包含着不少角色（Character），例如超级马里奥中就有马里奥、库巴、蘑菇等可以移动的角色，这些角色通常由 1～n 个精灵（Sprite）组成，例如下面的马里奥就是由 8 个精灵组成的 4x2 的角色：
![](image/马里奥组成.png)

### 3.5.3 渲染优先级
精灵本身的渲染是有优先级的，它们的优先级即为它们在 OAM 中的索引值（0～63），其中 0 优先级最高。当多个不同的精灵渲染到同一个像素的时候，优先级高的精灵会覆盖掉优先级低的精灵。

除此之外，Byte 2 的 bit 5 表示了精灵和背景谁被渲染的优先级，如果该 bit 位为 1，那么精灵也将不会被渲染出来。这种可以表示角色的一部分身体藏在树后面的场景。

### 3.5.4 Sprite 0 Hit
当第 0 个 Sprite 被渲染（其首个像素覆盖到了非透明的背景像素）时，被称为Sprite Zero Hit。PPU 寄存器 PPUSTATUS 中的 bit 6 将会被设置，来记录Sprite Zero Hit。

### 3.5.5 Scanline & Sprite
每一条扫描线（Scanline）在渲染像素的时候，都会检查并计算是否有需要进行渲染的精灵（Sprite），如果有的话就会对相应的像素进行渲染。需要注意的是每条扫描线最多只能支持8个精灵的渲染，当某一行扫描线的精灵超过 8 个时候，只能渲染出前 8 个精灵，这种现象被称为 Sprite  Overflow，此时 PPUSTATUS 中的 bit 5 会被设置，用于记录 Sprite Overflow。

由于这种限制的存在，当游戏内的角色非常多时（例如魂斗罗），我们会看到画面中的角色出现闪烁的现象。

### 3.5.6 OAM 同步
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

## 4. PPU 扫描线（Scanline）
PPU 渲染的方式是通过按行从顶至下渲染扫描线（Scanline）完成的。PPU 总共需要渲染 262 条扫描线，每条扫描线包含 341 个 PPU 时钟周期，每个时钟周期就会产生一个像素，扫描线可分为以下类型：
- Pre-Render Sanline
- Visible Scanlines
- Post-Render Scanline
- Vertical blanking Scalines

### 4.1 Pre-Render Scanline(-1 0r 261)
当扫描线处于 -1（初始化时期）或者 261 时，处于预渲染时期。这条扫描线的主要目的是为了将数据填充进 shift registers 中，而这些数据就是下一条扫描线的前 2 个 Tile 的数据。虽然这个时期并不会产生任何用于渲染的像素，但是 PPU 仍然会像 Visible Scanlines 一样执行相同的内存访问。

此外该 Scanline 的长度还会区分奇偶帧。当为奇数帧的时候，最后一个 PPU 周期将会被跳过（cycle 340不会被执行，直接从 (339, 261) 跳到 (0, 0)），而对于偶数帧来说，cycle 340 则是会被正常执行（从(340, 261) 跳到 (0, 0)）。

而在 cycle 280 ～ 304 期间，当允许渲染的时候，v 寄存器的 verital bits 将会被重置：
```bash
v: GHIA.BC DEF..... <- t: GHIA.BC DEF.....
```

### 4.2 Visible Scanlines(0 ~ 239)
可见扫描线，顾名思义就是真正用于产生渲染像素的扫描线，它包含了背景渲染以及精灵渲染两个部分。由于在这个时期内，PPU 正忙于从 PPU 内存中取数据，因此程序不应该在这个时期访问 PPU 的内存，除非渲染被禁止了。

#### cycle 0
这是一个空的 PPU 时钟周期，什么都不干。

#### cycle 1-256
从 cycle 1 开始，Tile 相关的数据将会被获取到，每次内存访问将会消耗 2 个 PPU 时钟周期，而一个 Tile 的相关数据必须进行 4 次内存访问，因此每个 Tile 的数据获取将会消耗 8 个 PPU 时钟周期：
- NameTable byte
- Attribute table byte
- Tile low
- Tile high（+8bytes from Tile low）

#### cycle 257-320
看不懂

#### cycle 321-336
下一行扫描线的前 2 个 Tile 相关的数据将被获取，共占用 16 个 PPU 时钟周期：
- NameTable byte
- Attribute table byte
- Tile low
- Tile high（+8bytes from Tile low）

#### cycle 337-340
什么都不做。

### 4.3 Post-Render Scanline(240)
PPU 此时处于空闲状态。

### 4.4 Vertical blanking Scalines(241~260)
在 Scanline 241 的 cycle 1 时，VBlank 标志位将会被设置，因此 VBlank 的 NMI 中断也会被触发。在这个期间 PPU 不会做任何的内存访问，因此在这个时期更新数据将会是十分安全的。

详细的 PPU 扫描线执行图如下所示：

![](https://www.nesdev.org/w/images/default/4/4f/Ppu.svg)

## 5. PPU 滚动（Scorlling）
当游戏中的角色开始移动时，背景也会随之移动，因此在渲染背景的时候，我们需要考虑镜头在何处开始渲染。通常而言，程序会通过写入两个 PPU 寄存器用于在 NMI 处理程序中设置滚动位置：

![](image/NTS_scrolling_seam.gif)

### 5.1 内部寄存器
PPU 内部拥有 4 个内部寄存器，用于处理画面的滚动：
- v：当前的 VRAM 地址（15 bits）；
- t: 临时的 VRAM 地址（15 bits）；同时也可以被当作是左上角的 Tile 的地址
- x：Fine X（3 bits）；
- w：标记是第一次或者第二次写（1 bits）。

当前的 VRAM 地址有两个作用：
- PPU 通过当前的 VRAM 地址（v 寄存器）来读写 PPUDATA（$2007） 传过来的数据；需要注意的是虽然 v 寄存器有 15 位，但是实际的 PPU 内存只有 14 位，因此用作此用途时需要将最高位屏蔽；
- 通过当前的 VRAM 地址获取 NameTable 的数据用来绘制背景。

而当用于绘制背景的时候，v 和 t 寄存器的 15 个 bit 位的功能如下：
```bash
yyy NN YYYYY XXXXX
||| || ||||| +++++-- coarse X scroll
||| || +++++-------- coarse Y scroll
||| ++-------------- nametable select
+++----------------- fine Y scroll
```

### 5.2 内部寄存器控制
CPU 通过一组 I/O 寄存器与 PPU 进行数据交换：
- PPUCTRL($2000)
- PPUMASK($2001)
- PPUSTATUS($2002)
- OAMADDR($2003)
- OAMDATA($2004)
- PPUSCROL($2005)
- PPUADDR($2006)
- PPUDATA($2007)

用 d 表示写入的数据，A ～ H 表示有效的 bit 位，当数据写入各个 I/O 寄存器时，影响如下：

PPUCTRL write
---
```bash
t: ...GH.. ........ <- d: ......GH
   <used elsewhere> <- d: ABCDEF..
```

PPUSTATUS read
---
```bash
w:                  <- 0
```

PPUSCROL first write(w = 0)
---
```bash
t: ....... ...ABCDE <- d: ABCDE...
x:              FGH <- d: .....FGH
w:                  <- 1
```

PPUSCROLL second write(w = 1)
---
```bash
t: FGH..AB CDE..... <- d: ABCDEFGH
w:                  <- 0
```

PPUADDR first write(w = 0)
---
```bash
t: .CDEFGH ........ <- d: ..CDEFGH
       <unused>     <- d: AB......
t: Z...... ........ <- 0 (bit Z is cleared)
w:                  <- 1
```

PPUADDR second write(w = 1)
---
```bash
t: ....... ABCDEFGH <- d: ABCDEFGH
v: <...all bits...> <- t: <...all bits...>
w:                  <- 0
```
需要注意的是 PPUADDR 和 PPUSCROL 是共享 w 寄存器的，因此它们两个读写的行为会影响到彼此。

PPUDATA read and write
---
读写 PPUDATA 的时候，将会使 v 寄存器的值增加 1 或者 32，这取决于 PPUCTRL 的 bit 5。

除了上述的寄存器之外，当扫描线位于某些点的时候，也会影响到内部寄存器的值，如下：

所有扫描线的 Dot 256
---
Dot 256 是每条扫描线可见 Dot 的最后一个，因此 PPU 需要增加 v 寄存器的纵坐标 Y。这是一个相对复杂的计算，因为有可能涉及到 NameTable 的切换等，伪代码如下：
```bash
if ((v & 0x7000) != 0x7000)        // if fine Y < 7
  v += 0x1000                      // increment fine Y
else
  v &= ~0x7000                     // fine Y = 0
  int y = (v & 0x03E0) >> 5        // let y = coarse Y
  if (y == 29)
    y = 0                          // coarse Y = 0
    v ^= 0x0800                    // switch vertical nametable
  else if (y == 31)
    y = 0                          // coarse Y = 0, nametable not switched
  else
    y += 1                         // increment coarse Y
  v = (v & ~0x03E0) | (y << 5)     // put coarse Y back into v
```
Bit 12 ～14 为 fine Y，Bit 5～9 为 coarse Y，Bit 11 用于选择纵向的 NameTable。

Row 29 是一个 NameTable 最后一行的 Tile，因此当需要从第 29 行继续向下渲染的时候，我们就需要切换 NameTable（例如从 NameTable 0 切换到 NameTable 2）。此时需要做的就是将 coarse Y 清零，并且将 NameTable 的标志位切换到对应的下一个 NameTable 上。

而在部分情况下，coarse 的值是有可能大于 29 的，而这将会导致 attribute data 被当作 tile data 读取进来。当它的值增大到 31 时，它将会重新回到 0，并且 NameTable 并不会切换。因此，当一个大于 240 的值被写入到 PPUSCROL 的时候可能会出现负的滚动，1到2行的attribute data 会被读进来并当作 Tile 渲染，此时真实的 Tile 数据便会在attribute data 读取完成之后才会再进行渲染，而一些游戏利用了这个特性来将顶部的 NameTable 移除。

所有扫描线的 Dot 257
---
当允许渲染的时候，t 寄存器和 v 寄存器产生如下行为：
```bash
v: ....A.. ...BCDEF <- t: ....A.. ...BCDEF
```

Pre-Render 时期的 Dot 280 ～ 304
---
当允许渲染的时候，在这个时期 t 寄存器和 v 寄存器会重复如下行为：
```bash
v: GHIA.BC DEF..... <- t: GHIA.BC DEF.....
```

扫描线 Dot 328 ～ 下一条扫描的 Dot 256
---
当允许渲染的时候，PPU 会不断地增加 v 寄存器中 coarse X 的值，用以不断地更新水平位置的 Tile。它会在 Dot 328 以及 336 开始更新，并且在下一条扫描线中的 8，16，24 ... 240，248，256 不断地进行更新。

需要在上一条扫描线进行两次 coarse 更新的原因是：Dot 321 ～ 336 是读取下一条扫描线 Tile 的时期，在此时更新 coarse X 的值才能保证可以不断地读取到对的 Tile。
```bash
if ((v & 0x001F) == 31) // if coarse X == 31
  v &= ~0x001F          // coarse X = 0
  v ^= 0x0400           // switch horizontal nametable
else
  v += 1                // increment coarse X
```

### 5.3 Tile 获取

v 寄存器的低 12 位 分别对应了 NameTable 以及 coarse X 和 coarse Y 的值，因此去 Tile 地址的方式如下：
```bash
tile address = 0x2000 | (v & 0x0FFF)
```
而每个 attribute byte 需要管理 16 个 tile，它的地址获取如下：
```bash
attribute address = 0x23C0 | (v & 0x0C00) | ((v >> 4) & 0x38) | ((v >> 2) & 0x07)
```

## 6. PPU 设计