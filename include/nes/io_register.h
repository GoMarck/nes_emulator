#pragma once

#include "nes/type.h"

namespace nes {

/// PPU 8 memory-mapped registers to CPU. These nominally sit at $2000 through
/// $2007 in the CPU's address space, but because they're incompletely decoded,
/// they're mirrored in every 8 bytes from $2008 through $3FFF.
/// Reference from: https://www.nesdev.org/wiki/PPU_registers
enum IORegister : Address {
  /// Description: PPU control register
  /// Access: Write
  /// Bitmap:
  ///   7  bit  0
  ///   ---- ----
  ///   VPHB SINN
  ///   |||| ||||
  ///   |||| ||++- Base nametable address
  ///   |||| ||    (0 = $2000; 1 = $2400; 2 = $2800; 3 = $2C00)
  ///   |||| |+--- VRAM address increment per CPU read/write of PPUDATA
  ///   |||| |     (0: add 1, going across; 1: add 32, going down)
  ///   |||| +---- Sprite pattern table address for 8x8 sprites
  ///   ||||       (0: $0000; 1: $1000; ignored in 8x16 mode)
  ///   |||+------ Background pattern table address (0: $0000; 1: $1000)
  ///   ||+------- Sprite size (0: 8x8 pixels; 1: 8x16 pixels – see PPU OAM#Byte1)
  ///   |+-------- PPU master/slave select
  ///   |          (0: read backdrop from EXT pins; 1: output color on EXT pins)
  ///   +--------- Generate an NMI at the start of the
  ///              vertical blanking interval (0: off; 1: on)
  PPUCTRL = 0x2000,

  /// Description: PPU mask register
  /// Access: Write
  /// Bitmap:
  ///   7  bit  0
  ///   ---- ----
  ///   BGRs bMmG
  ///   |||| ||||
  ///   |||| |||+- Greyscale (0: normal color, 1: produce a greyscale display)
  ///   |||| ||+-- 1: Show background in leftmost 8 pixels of screen, 0: Hide
  ///   |||| |+--- 1: Show sprites in leftmost 8 pixels of screen, 0: Hide
  ///   |||| +---- 1: Show background
  ///   |||+------ 1: Show sprites
  ///   ||+------- Emphasize red (green on PAL/Dendy)
  ///   |+-------- Emphasize green (red on PAL/Dendy)
  ///   +--------- Emphasize blue
  ///
  /// Render control:
  ///   - Bits 3 and 4 enable the rendering of background and sprites,
  ///   respectively.
  ///   - Bits 1 and 2 enable rendering of the background and sprites in the
  ///   leftmost 8 pixel columns. Setting these bits to 0 will mask these
  ///   columns, which is often useful in horizontal scrolling situations where
  ///   you want partial sprites or tiles to scroll in from the left.
  ///   - A value of $1E or %00011110 enables all rendering, with no color
  ///   effects. A value of $00 or %00000000 disables all rendering. It is
  ///   usually best practice to write this register only during vblank, to
  ///   prevent partial-frame visual artifacts.
  ///   - If either of bits 3 or 4 is enabled, at any time outside of the vblank
  ///   interval the PPU will be making continual use to the PPU address and
  ///   data bus to fetch tiles to render, as well as internally fetching sprite
  ///   data from the OAM. If you wish to make changes to PPU memory outside of
  ///   vblank (via $2007), you must set both of these bits to 0 to disable
  ///   rendering and prevent conflicts.
  ///   - Disabling rendering (clear both bits 3 and 4) during a visible part of
  ///   the frame can be problematic. It can cause a corruption of the sprite
  ///   state, which will display incorrect sprite data on the next frame. (See:
  ///   Errata) It is, however, perfectly fine to mask sprites but leave the
  ///   background on (set bit 3, clear bit 4) at any time in the frame.
  ///   - Sprite 0 hit does not trigger in any area where the background or
  ///   sprites are hidden.
  /// Color control:
  ///   - Bit 0 controls a greyscale mode, which causes the palette to use only
  ///   the colors from the grey column: $00, $10, $20, $30. This is implemented
  ///   as a bitwise AND with $30 on any value read from PPU $3F00-$3FFF, both
  ///   on the display and through PPUDATA. Writes to the palette through
  ///   PPUDATA are not affected. Also note that black colours like $0F will be
  ///   replaced by a non-black grey $00.
  ///   - Bits 5, 6 and 7 control a color "emphasis" or "tint" effect. See
  ///   Colour emphasis for details. Note that the emphasis bits are applied
  ///   independently of bit 0, so they will still tint the color of the grey
  ///   image.
  PPUMASK = 0x2001,

  /// Description: PPU status register
  /// Access: Read
  /// Bitmap:
  ///   7  bit  0
  ///   ---- ----
  ///   VSO. ....
  ///   |||| ||||
  ///   |||+-++++- PPU open bus. Returns stale PPU bus contents.
  ///   ||+------- Sprite overflow. The intent was for this flag to be set
  ///   ||         whenever more than eight sprites appear on a scanline, but a
  ///   ||         hardware bug causes the actual behavior to be more complicated
  ///   ||         and generate false positives as well as false negatives; see
  ///   ||         PPU sprite evaluation. This flag is set during sprite
  ///   ||         evaluation and cleared at dot 1 (the second dot) of the
  ///   ||         pre-render line.
  ///   |+-------- Sprite 0 Hit.  Set when a nonzero pixel of sprite 0 overlaps
  ///   |          a nonzero background pixel; cleared at dot 1 of the pre-render
  ///   |          line.  Used for raster timing.
  ///   +--------- Vertical blank has started (0: not in vblank; 1: in vblank).
  ///              Set at dot 1 of line 241 (the line *after* the post-render
  ///              line); cleared after reading $2002 and at dot 1 of the
  ///              pre-render line.
  PPUSTATUS = 0x2002,

  /// Description: OAM address port
  /// Access: Write
  /// Detail: Write the address of OAM you want to access here. Most games just
  /// write $00 here and then use OAMDMA. (DMA is implemented in the 2A03/7 chip
  /// and works by repeatedly writing to OAMDATA).
  OAMADDR = 0x2003,

  /// Description: OAM data port
  /// Access: Read, Write
  /// Detail: Write OAM data here. Writes will increment OAMADDR after the
  /// write; reads do not. Reads during vertical or forced blanking return the
  /// value from OAM at that address.
  OAMDATA = 0x2004,

  /// Description: PPU scrolling position register
  /// Access: Write twice
  /// Detail: This register is used to change the scroll position, that is, to
  /// tell the PPU which pixel of the nametable selected through PPUCTRL should
  /// be at the top left corner of the rendered screen. Typically, this register
  /// is written to during vertical blanking, so that the next frame starts
  /// rendering from the desired location, but it can also be modified during
  /// rendering in order to split the screen. Changes made to the vertical
  /// scroll during rendering will only take effect on the next frame.
  PPUSCROLL = 0x2005,

  /// Description: PPU address register
  /// Access: Write twice
  /// Detail: Because the CPU and the PPU are on separate buses, neither has
  /// direct access to the other's memory. The CPU writes to VRAM through a pair
  /// of registers on the PPU. First it loads an address into PPUADDR, and then
  /// it writes repeatedly to PPUDATA to fill VRAM.
  PPUADDR = 0x2006,

  /// Description: PPU data port
  /// Access: Read, Write
  /// Detail: VRAM read/write data register. After access, the video memory
  /// address will increment by an amount determined by bit 2 of $2000.
  PPUDATA = 0x2007,

  /// Description: OAM DMA register (high byte)
  /// Access: Write
  /// Detail: This port is located on the CPU. Writing $XX will upload 256 bytes
  /// of data from CPU page $XX00–$XXFF to the internal PPU OAM. This page is
  /// typically located in internal RAM, commonly $0200–$02FF, but cartridge RAM
  /// or ROM can be used as well.
  OAMDMA = 0x4014
};

}  // namespace nes