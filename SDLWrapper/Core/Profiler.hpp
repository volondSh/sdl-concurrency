#pragma once

#include <chrono>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace sdl::core
{
  struct ProfileEntry
  {
    std::string name;
    float timeMs = 0.f;
  };

  class Profiler final
  {
  public:
    class Scope
    {
    public:
      Scope(Profiler& profiler, std::string_view name) : m_profiler{profiler}, m_name{name}
      {
        m_profiler.begin(m_name);
      }

      ~Scope()
      {
        m_profiler.end(m_name);
      }

      Scope(const Scope&)            = delete;
      Scope& operator=(const Scope&) = delete;
      Scope(Scope&&)                 = delete;
      Scope& operator=(Scope&&)      = delete;

    private:
      Profiler& m_profiler;
      std::string_view m_name;
    };

    void begin(std::string_view name);
    void end(std::string_view name);
    [[nodiscard]] Scope profile(std::string_view name);

    void accumulate();
    [[nodiscard]] const std::vector<ProfileEntry>& results() const;
    void reset();

  private:
    std::unordered_map<std::string_view, std::chrono::steady_clock::time_point> m_starts;
    std::unordered_map<std::string_view, float> m_accum;
    std::vector<std::string_view> m_order;
    std::vector<ProfileEntry> m_results;
  };
}
