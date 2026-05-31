<div align="center">

![preview](preview.jpg)

[![C++23](https://img.shields.io/badge/C%2B%2B-23-blue?logo=cplusplus&logoColor=white)](https://en.cppreference.com/w/cpp/23)
[![CMake](https://img.shields.io/badge/CMake-3.12%2B-064F8C?logo=cmake&logoColor=white)](https://cmake.org/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

</div>

Учебная реализация STL-совместимого [развёрнутого связного списка](https://en.wikipedia.org/wiki/Unrolled_linked_list) на C++23. Написана в рамках курса по C++ в ИТМО.

```cpp
unrolled_list<int, 8> list = {1, 2, 3};
list.push_back(4);
for (auto& val : list) std::cout << val << " "; // 1 2 3 4
```

## Что реализовано

- Bidirectional iterators (+ reverse, const)
- Allocator awareness
- Copy / move semantics
- `push_back`, `push_front`, `pop_back`, `pop_front`
- `insert` и `erase` (iterator и index overloads)
- `front`, `back`, `size`, `empty`, `clear`
- `operator==`, `operator=`

## Что не реализовано

`emplace`, `emplace_front`, `emplace_back`, `assign_range`, `prepend_range`, `operator[]`, `swap`

## Сборка

```bash
git clone https://github.com/notakeith/stl-container.git
cd stl-container
cmake -B build && cmake --build build
ctest --test-dir build
```

Требования: CMake ≥ 3.12, GCC 13+ или Clang 17+.
