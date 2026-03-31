#include <SDLWrapper/Core/Context.hpp>

#include <SDL3/SDL.h>

#include <gtest/gtest.h>

TEST(ContextTests, InitializesEventsAndVideo)
{
  sdl::core::Context ctx{SDL_INIT_EVENTS | SDL_INIT_VIDEO};
  EXPECT_TRUE(ctx.initialized()) << ctx.init_error();
}
