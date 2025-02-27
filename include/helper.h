#ifndef HELPER_H
#define HELPER_H

// #include "encoder.h"
#include "mbed.h"
#include <functional>

/**
 * @brief Clamp the value between [lo,high].
 *
 * @tparam T Type to work with. This project use mostly float.
 * @param v Original value.
 * @param lo Lower bound.
 * @param hi Upper bound.
 * @return constexpr const T& Clamped value.
 */
template <class T>
constexpr const T &clamp(const T &v, const T &lo, const T &hi) {
  return std::less<T>{}(v, lo) ? lo : std::less<T>{}(hi, v) ? hi : v;
}

#endif
