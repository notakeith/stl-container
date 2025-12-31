#include <iostream>
#include <vector>

#include "unrolled_list.h"

template <typename T>
class Sorted {
   public:
    Sorted(const T& container)
        : container(container) {}

    class iterator {
       public:
        using value_type = typename T::value_type;
        using reference = const value_type&;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::forward_iterator_tag;

        iterator(const T* container, bool is_end = false)
            : container(container), is_end(is_end), min_count(0) {
            if (!is_end) {
                find_current_min();
            }
        }

        reference operator*() const { return current_min; }

        iterator& operator++() {
            if (++min_count >= current_min_total) {
                find_next_min();
                min_count = 0;
            }
            return *this;
        }

        iterator operator++(int) {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const iterator& other) const {
            return is_end == other.is_end;
        }

        bool operator!=(const iterator& other) const {
            return !(*this == other);
        }

       private:
        void find_current_min() {
            auto it = container->begin();
            if (it == container->end()) {
                is_end = true;
                return;
            }

            current_min = *it;
            current_min_total = 1;

            for (++it; it != container->end(); ++it) {
                if (*it < current_min) {
                    current_min = *it;
                    current_min_total = 1;
                } else if (*it == current_min) {
                    current_min_total++;
                }
            }
        }

        void find_next_min() {
            value_type next_min = std::numeric_limits<value_type>::max();
            size_t next_count = 0;
            bool found = false;

            for (auto it = container->begin(); it != container->end(); ++it) {
                if (*it > current_min) {
                    if (*it < next_min) {
                        next_min = *it;
                        next_count = 1;
                        found = true;
                    } else if (*it == next_min) {
                        next_count++;
                    }
                }
            }

            if (found) {
                current_min = next_min;
                current_min_total = next_count;
            } else {
                is_end = true;
            }
        }

        const T* container;
        value_type current_min;
        size_t current_min_total;
        size_t min_count;
        bool is_end;
    };

    iterator begin() const {
        return iterator(&container);
    }

    iterator end() const {
        return iterator(&container, true);
    }

   private:
    const T& container;
};

int main() {
    unrolled_list<int> a = {1, 2, 1, 1};
    Sorted b(a);

    for (auto j : b) {
        std::cout << j << " ";
    }
    std::cout << std::endl;

    for (auto j : a) {
        std::cout << j << " ";
    }
    std::cout << std::endl;
}