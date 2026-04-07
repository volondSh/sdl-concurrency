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

TEST(VelocityComponent, DefaultInitialization)
{
  const auto vel = Velocity{};

  EXPECT_FLOAT_EQ(vel.dx, 0.f);
  EXPECT_FLOAT_EQ(vel.dy, 0.f);
}

TEST(VelocityComponent, ValueInitialization)
{
  const auto vel = Velocity{.dx = 10.f, .dy = -5.f};

  EXPECT_FLOAT_EQ(vel.dx, 10.f);
  EXPECT_FLOAT_EQ(vel.dy, -5.f);
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

TEST(MovementSystem, UpdatesPositionBasedOnVelocity)
{
  auto registry = entt::registry{};
  const auto entity = registry.create();
  registry.emplace<ecs::Position>(entity, 100.f, 100.f);
  registry.emplace<ecs::Velocity>(entity, 50.f, -30.f);

  const auto dt = 0.5f;
  auto view = registry.view<ecs::Position, ecs::Velocity>();
  for (auto [ent, pos, vel] : view.each())
  {
    pos.x += vel.dx * dt;
    pos.y += vel.dy * dt;
  }

  const auto& pos = registry.get<ecs::Position>(entity);
  EXPECT_FLOAT_EQ(pos.x, 125.f);
  EXPECT_FLOAT_EQ(pos.y, 85.f);
}

TEST(MovementSystem, OnlyAffectsEntitiesWithBothComponents)
{
  auto registry = entt::registry{};

  const auto stationary = registry.create();
  registry.emplace<ecs::Position>(stationary, 100.f, 100.f);

  const auto moving = registry.create();
  registry.emplace<ecs::Position>(moving, 100.f, 100.f);
  registry.emplace<ecs::Velocity>(moving, 100.f, 0.f);

  const auto dt = 1.f;
  registry.view<ecs::Position, ecs::Velocity>().each(
      [dt](ecs::Position& pos, const ecs::Velocity& vel)
      {
        pos.x += vel.dx * dt;
        pos.y += vel.dy * dt;
      });

  const auto& stationaryPos = registry.get<ecs::Position>(stationary);
  const auto& movingPos = registry.get<ecs::Position>(moving);

  EXPECT_FLOAT_EQ(stationaryPos.x, 100.f);
  EXPECT_FLOAT_EQ(movingPos.x, 200.f);
}

TEST(MovementSystem, WrapsAroundWindowBounds)
{
  auto registry          = entt::registry{};
  const auto entity      = registry.create();
  registry.emplace<ecs::Position>(entity, 790.f, 550.f);
  registry.emplace<ecs::Velocity>(entity, 100.f, 100.f);

  const auto windowWidth  = 800.f;
  const auto windowHeight = 600.f;
  const auto dt           = 1.f;
  const auto epsilon      = 1e-6f;

  auto view = registry.view<ecs::Position, ecs::Velocity>();
  for (auto [ent, pos, vel] : view.each())
  {
    pos.x += vel.dx * dt;
    pos.y += vel.dy * dt;

    if (pos.x < -epsilon)
      pos.x += windowWidth;
    else if (pos.x + epsilon >= windowWidth)
      pos.x -= windowWidth;

    if (pos.y < -epsilon)
      pos.y += windowHeight;
    else if (pos.y + epsilon >= windowHeight)
      pos.y -= windowHeight;
  }

  const auto& pos = registry.get<ecs::Position>(entity);
  
  EXPECT_FLOAT_EQ(pos.x, 90.f);
  EXPECT_FLOAT_EQ(pos.y, 50.f);
}
