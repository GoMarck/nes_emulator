#pragma once

#include <memory>
#include <vector>

#include "nes/picture_bus.h"
#include "nes/screen.h"
#include "nes/type.h"

namespace nes {
class PPU {
public:
  PPU(std::shared_ptr<PictureBus> bus, std::shared_ptr<IScreen> screen);

  ~PPU() noexcept;

  void Tick();

  void SetData(Byte data);

  void GetData(Byte data);

  void SetMask(Byte data);
  
private:
  enum State {
    PRE_RENDER = 0,
    RENDER,
    POST_RENDER,
    VERTICAL_BLANK
  };

  bool EnableRender() const {
    return enable_background_render_ && enable_sprite_render_;
  }

  void IncreaseCoarseX();

  void IncreaseCoarseY();

  // PPU access memory via Picture Bus. 
  std::shared_ptr<PictureBus> picture_bus_;

  // Screen for receiving the frame data.
  std::shared_ptr<IScreen> screen_;

  // Current VRAM address, only have 15 valid bits actually.
  Word v;

  // Tempoary VRAM address, only have 15 valid bits actually.
  Word t;

  // Fine X position.
  Byte x;

  // First/second write toggle. 
  bool w;

  // Shift register for background first tile.
  Word background_tile_low_;

  // Shift register for background second tile.
  Word background_tile_high_;

  // Shift register for background second attribute byte.
  Byte background_attr_low_;

  // Shift register for background first attribute byte.
  Byte background_attr_high_;

  // OAM.
  std::vector<Byte> oam_;

  // Secondary OAM
  std::vector<Byte> secondary_oam_;

  // Current Scanline number.
  int scanline_number_;

  // Current state.
  State state_;

  // Output frame data.
  std::vector<std::vector<Byte>> frame_data_;

  // Indicate enable render background or not.
  bool enable_background_render_;

  // Indicate enable render sprites or not.
  bool enable_sprite_render_;
};
}  // namespace nes