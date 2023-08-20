#include "nes/cartridge.h"

#include <gtest/gtest.h>

namespace nes {

class CartridgeTest : public ::testing::Test {
};

TEST_F(CartridgeTest, TestCartridgeInitBasicFunction)
{
  Cartridge cartridge("../resource/Super_mario_brothers.nes");
  ASSERT_TRUE(cartridge.Init());
}

}  // namespace nes
