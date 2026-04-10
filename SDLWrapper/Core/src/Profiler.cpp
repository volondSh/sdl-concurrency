#include "Profiler.hpp"

#include <algorithm>

using namespace sdl::core;

void Profiler::begin(std::string_view name)
{
  m_starts[name] = std::chrono::steady_clock::now();
  if (!m_accum.contains(name))
    m_order.push_back(name);
}

void Profiler::end(std::string_view name)
{
  const auto it = m_starts.find(name);
  if (it == m_starts.end())
    return;

  const auto elapsed =
      std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - it->second);
  m_accum[name] += static_cast<float>(elapsed.count()) / 1000.f;
}

Profiler::Scope Profiler::profile(std::string_view name)
{
  return Scope{*this, name};
}

const std::vector<ProfileEntry>& Profiler::results() const
{
  return m_results;
}

void Profiler::accumulate()
{
  m_results.clear();
  m_results.reserve(m_order.size());
  for (const auto& name : m_order)
  {
    auto it = m_accum.find(name);
    if (it != m_accum.end())
      m_results.push_back({.name = std::string{name}, .timeMs = std::max(0.f, it->second)});
  }
  m_accum.clear();
  m_starts.clear();
  m_order.clear();
}

void Profiler::reset()
{
  m_accum.clear();
  m_starts.clear();
  m_order.clear();
  m_results.clear();
}
