#include <ECS/Components.h>

#include <entt/entt.hpp>

#include <gtest/gtest.h>

using namespace ecs;

TEST(PositionComponent, DefaultInitialization)
{
  const auto pos = Position{};

  EXPECT_FLOAT_EQ(pos.x, 0.f);
  EXPECT_FLOAT_EQ(pos.y, 0.f);
}

TEST(PositionComponent, ValueInitialization)
{
  const auto pos = Position{.x = 10.f, .y = 20.f};

  EXPECT_FLOAT_EQ(pos.x, 10.f);
  EXPECT_FLOAT_EQ(pos.y, 20.f);
}

TEST(PositionComponent, Assignment)
{
  auto pos = Position{.x = 0.f, .y = 0.f};
  pos.x    = 100.5f;
  pos.y    = 200.5f;

  EXPECT_FLOAT_EQ(pos.x, 100.5f);
  EXPECT_FLOAT_EQ(pos.y, 200.5f);
}

TEST(ColorComponent, DefaultInitialization)
{
  const auto color = Color{};

  EXPECT_EQ(color.color.r, 0);
  EXPECT_EQ(color.color.g, 0);
  EXPECT_EQ(color.color.b, 0);
  EXPECT_EQ(color.color.a, 0);
}

TEST(ColorComponent, ValueInitialization)
{
  const auto color = Color{{.r = 255, .g = 128, .b = 64, .a = 255}};

  EXPECT_EQ(color.color.r, 255);
  EXPECT_EQ(color.color.g, 128);
  EXPECT_EQ(color.color.b, 64);
  EXPECT_EQ(color.color.a, 255);
}

TEST(EcsRegistry, CanCreateEntityWithPositionAndColor)
{
  auto registry     = entt::registry{};
  const auto entity = registry.create();
  const auto& pos   = registry.emplace<Position>(entity, 1.f, 2.f);
  const auto& color = registry.emplace<Color>(entity, SDL_Color{.r = 255, .g = 255, .b = 255, .a = 255});

  EXPECT_FLOAT_EQ(pos.x, 1.f);
  EXPECT_FLOAT_EQ(pos.y, 2.f);
  EXPECT_EQ(color.color.r, 255);
  EXPECT_EQ(color.color.g, 255);
  EXPECT_EQ(color.color.b, 255);
  EXPECT_EQ(color.color.a, 255);
}

TEST(EcsRegistry, ViewIteratesAllMatchingEntities)
{
  auto registry = entt::registry{};

  const auto e1 = registry.create();
  registry.emplace<ecs::Position>(e1, 0.F, 0.F);
  registry.emplace<ecs::Color>(e1, SDL_Color{.r = 100, .g = 100, .b = 100, .a = 255});

  const auto e2 = registry.create();
  registry.emplace<ecs::Position>(e2, 5.f, 5.f);
  registry.emplace<ecs::Color>(e2, SDL_Color{.r = 200, .g = 200, .b = 200, .a = 255});

  auto count = 0;
  registry.view<ecs::Position, ecs::Color>().each(
      [&count](const ecs::Position&, const ecs::Color&)
      {
        ++count;
      });

  EXPECT_EQ(count, 2);
}

TEST(EcsRegistry, DestroyedEntityNotInView)
{
  auto registry = entt::registry{};

  const auto e1 = registry.create();
  registry.emplace<ecs::Position>(e1, 0.f, 0.f);
  registry.emplace<ecs::Color>(e1, SDL_Color{.r = 100, .g = 100, .b = 100, .a = 255});

  registry.destroy(e1);

  auto count = 0;
  registry.view<ecs::Position, ecs::Color>().each(
      [&count](const ecs::Position&, const ecs::Color&)
      {
        ++count;
      });

  EXPECT_EQ(count, 0);
}
