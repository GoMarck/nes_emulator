#pragma once

#include <memory>
#include <vector>

#include "nes/picture_bus.h"
#include "nes/screen.h"
#include "nes/type.h"

namespace nes {

/// Picture Process Unit for rendering each frame. Responsible for outputting
/// the picture pixel data of each frame and transmitting it to the screen.
class PPU {
 public:
  /// Construct from PictureBus and Screen objects.
  ///
  /// \param bus Picture bus shared pointer.
  /// \param screen Screen shared pointer.
  PPU(std::shared_ptr<PictureBus> bus, std::shared_ptr<IScreen> screen);

  ~PPU() noexcept;

  /// Reset PPU status.
  void Reset();

  /// Tick once, run a PPU clock cycle.
  void Tick();

  /// Write PPUCTRL($2000) from CPU to PPU.
  ///
  /// \param data 8 bits data write from CPU.
  void WriteControl(Byte data);

  /// Write PPUMASK($2001) from CPU to PPU.
  ///
  /// \param data 8 bits data write from CPU.
  void WriteMask(Byte data);

  /// Read PPUSTATUS($2002) from PPU to CPU.
  ///
  /// \return 8 bits data.
  Byte ReadStatus();

  /// Write PPUSCROL($2005) from CPU to PPU.
  ///
  /// \param data 8 bits data write from CPU.
  void WriteScroll(Byte data);

  /// Write PPUADDR($2006) from CPU to PPU.
  ///
  /// \param data 8 bits data write from CPU.
  void WriteAddress(Byte data);

  /// Write PPUDATA($2007) from CPU to PPU.
  ///
  /// \param data 8 bits data write from CPU.
  void WriteData(Byte data);

  /// Read PPUDATA($2007) from PPU to CPU.
  ///
  /// \return 8 bits data.
  Byte Readata();

  /// Write OAMADDR($2003) from CPU to PPU.
  ///
  /// \param data 8 bits data write from CPU.
  void WriteOAMAddress(Byte data);

  /// Write OAMDATA($2004) from CPU to PPU.
  ///
  /// \param data 8 bits data write from CPU.
  void WriteOAMData(Byte data);

  /// Write OAMDMA($4014) from CPU to PPU.
  ///
  /// \param data 8 bits data write from CPU.
  void WriteDMA(Byte data);

 private:
  enum State { PRE_RENDER = 0, RENDER, POST_RENDER, VERTICAL_BLANK };

  bool EnableRender() const {
    return enable_background_render_ && enable_sprite_render_;
  }

  /// Increase the current coarse X position.
  void IncreaseCoarseX();

  /// Increase the current coarse Y position.
  void IncreaseCoarseY();

  /// PPU access memory via Picture Bus.
  std::shared_ptr<PictureBus> picture_bus_;

  /// Screen for receiving the frame data.
  std::shared_ptr<IScreen> screen_;

  /// Current VRAM address, only have 15 valid bits actually.
  Word v;

  /// Tempoary VRAM address, only have 15 valid bits actually.
  Word t;

  /// Fine X position.
  Byte x;

  /// First/second write toggle.
  bool w;

  /// Shift register for background first tile.
  Word background_tile_low_;

  /// Shift register for background second tile.
  Word background_tile_high_;

  /// Shift register for background second attribute byte.
  Byte background_attr_low_;

  /// Shift register for background first attribute byte.
  Byte background_attr_high_;

  /// OAM.
  std::vector<Byte> oam_;

  /// Secondary OAM, current scanline 8 sprites data stored here.
  std::vector<Byte> secondary_oam_;

  /// Current Scanline number.
  int scanline_number_;

  /// Current state.
  State state_;

  /// Output frame data.
  std::vector<std::vector<Byte>> frame_data_;

  /// Indicate enable render background or not.
  bool enable_background_render_;

  /// Indicate enable render sprites or not.
  bool enable_sprite_render_;

  /// Indicate enable render leftmost 8 pixel column background or not.
  bool enable_edge_background_render_;

  /// Indicate enable render leftmost 8 pixel column sprite or not.
  bool enable_edge_sprite_render_;

  /// Indicate is odd frame or even frame.
  bool odd_frame_;

  /// Indicate vertical blank or not.
  bool vertical_blank_;

  /// Indicate trigger sprite 0 hit or not.
  bool sprite_zero_hit_;

  /// VRAM address increasement according to PPUCTRL bit 3, 1 or 32.
  int increasement_;
};
}  // namespace nes