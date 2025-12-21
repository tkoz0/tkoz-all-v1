#pragma once

#include <vector>

namespace tkoz::_test {

template <typename T>
concept dummy_concept = true;

template <typename T, dummy_concept... Ts>
  requires(std::same_as<std::remove_cvref_t<Ts>, std::vector<T>> && ...)
std::vector<T> concat_vectors(Ts &&...vectors) {
  std::vector<T> result;
  std::size_t size = (vectors.size() + ... + 0);
  result.reserve(size);
  (std::ranges::copy(vectors, std::back_inserter(result)), ...);
  return result;
}

} // namespace tkoz::_test
