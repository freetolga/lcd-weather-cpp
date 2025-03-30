#include <cstdint>
#include <string>

struct current_units {
  std::string time;
  std::string interval;
  std::string temperature_2m;
};

struct current {
  std::string time;
  std::uint32_t interval;
  float temperature_2m;
};

struct weather_info_t {
  float latitude;
  float longitude;
  double generationtime_ms;
  std::uint32_t utc_offset_seconds;
  std::string timezone;
  std::string timezone_abbreviation;
  float elevation;
  struct current_units current_units;
  struct current current;
};
