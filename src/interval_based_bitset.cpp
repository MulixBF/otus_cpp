#include "interval_based_bitset.h"

#include <optional>
#include <vector>
#include <iterator>
#include <algorithm>
#include <stdexcept>
#include <unordered_set>


template<std::size_t N, class OutputIterator>
void convertToSortedIntervals(const std::bitset<N> bitset, OutputIterator it) {
    std::vector<interval> result;
    bool currentValue;

    std::optional<bool> previousValue = std::nullopt;
    std::size_t currentIntervalSize = 0;

    for(std::size_t i = 0; i < N; i++) {
        currentValue = bitset[i];
        if(currentValue == previousValue) {
            currentIntervalSize++;
        } else {
            if (currentValue == true) {
                *it++ = interval{.start = i - currentIntervalSize, .size = currentIntervalSize};
            }
            currentIntervalSize = 0;
        }
        previousValue = currentValue;
    }
}

template<std::size_t N, class Allocator>
interval_based_bitset<N, Allocator>::interval_based_bitset(const Allocator& _allocator)
    : allocator(_allocator), 
      free_intervals(_allocator),
      free_interval_sizes(_allocator) {

    this->insert_interval({.start = 0, .size = N});
}

template<std::size_t N, class Allocator>
interval_based_bitset<N, Allocator>::interval_based_bitset(const std::bitset<N> bitset, const Allocator& _allocator)
    : allocator(_allocator), 
      free_intervals(_allocator),
      free_intervals_startpoints(_allocator) {

    convertToSortedIntervals(bitset, std::back_inserter(this->free_intervals));
    
    this->free_interval_sizes.reserve(this->free_intervals.size());
    std::transform(this->free_intervals.begin(), 
                this->free_intervals.end(),
                std::back_inserter(this->free_interval_sizes),
                [](interval a){ return std::ref(a); });

}

template<std::size_t N, class Allocator>
bool interval_based_bitset<N, Allocator>::operator==(const interval_based_bitset<N, Allocator>&) const {
    return this->free_intervals == other->free_intervals;
}

template<std::size_t N, class Allocator>
bool interval_based_bitset<N, Allocator>::operator[](key_type key) const {
    interval& lower_bound =  std::lower_bound(this->find_shortest_interval.begin(), 
                                              this->find_shortest_interval.end(),
                                              key,
                                              [](interval& a, key_type b){ return a->start < b; });

    return !lower_bound->contains(key);
}

template<std::size_t N, class Allocator>
bool interval_based_bitset<N, Allocator>::test(key_type key) const {
    if(key > N) {
        throw std::out_of_range();
    }

    return this->operator[](key);
}

template<std::size_t N, class Allocator>
bool interval_based_bitset<N, Allocator>::all() const {
    return this->free_intervals.empty();
}

template<std::size_t N, class Allocator>
bool interval_based_bitset<N, Allocator>::any() const {
    return !this->none();
}

template<std::size_t N, class Allocator>
bool interval_based_bitset<N, Allocator>::none() const {
    return this->free_intervals.size() == 1 
        && this->free_intervals[0].size == N; 
}

template<std::size_t N, class Allocator>
interval interval_based_bitset<N, Allocator>::find_shortest_interval(
    interval_based_bitset<N, Allocator>::size_type min_size, 
    interval_based_bitset<N, Allocator>::value_type value) const {

    return *this->free_intervals[0];
}

template<std::size_t N, class Allocator>
std::size_t interval_based_bitset<N, Allocator>::size() const {
    return N;
}

template<std::size_t N, class Allocator>
interval_based_bitset<N, Allocator>& interval_based_bitset<N, Allocator>::set() {
    this->free_intervals.clear();
    this->free_interval_sizes.clear();
}

template<std::size_t N, class Allocator>
interval_based_bitset<N, Allocator>& interval_based_bitset<N, Allocator>::set(
    key_type key, 
    interval_based_bitset<N, Allocator>::value_type value) {

    return this->set(interval(key, 1));
}

template<std::size_t N, class Allocator>
interval_based_bitset<N, Allocator>& interval_based_bitset<N, Allocator>::set(
    interval slice, 
    interval_based_bitset<N, Allocator>::value_type value) {

    if(!value) {
        return this->reset(slice);
    }
    
    auto lowerBound = std::lower_bound(
        this->free_intervals.begin(), 
        this->free_intervals.end(),
        slice.start,
        interval::compare_by_position
    );

    if (lowerBound == this->free_intervals.end()) {
        return this;
    }

    auto upperBound = std::upper_bound(
        lowerBound,
        this->free_intervals.end(),
        slice.end(),
        interval::compare_by_position
    );

    if(slice->intersects(lowerBound)) {
        this->insert_interval({
            .start = lowerBound->start,
            .size = lowerBound->end() - slice.start 
        });
    }

    if(slice->intersects(upperBound)) {
       this->insert_interval({
           .start = slice.end(),
           .size = upperBound.end() - slice.end()
       });
    }

    this->remove_interval_if(lowerBound, 
                             upperBound, 
                             [&slice](const interval& a) { slice.intersects(a); });
}

template<std::size_t N, class Allocator>
interval_based_bitset<N, Allocator>& interval_based_bitset<N, Allocator>::reset() {
    this->free_intervals.clear();
    this->free_interval_sizes.clear();
    this->insert_interval({.start = 0, .size = N});
}

template<std::size_t N, class Allocator>
interval_based_bitset<N, Allocator>& interval_based_bitset<N, Allocator>::reset(key_type key) {
    return this->reset(interval(key, 1));
}

template<std::size_t N, class Allocator>
interval_based_bitset<N, Allocator>& interval_based_bitset<N, Allocator>::reset(interval slice) {

    auto lowerBound = std::lower_bound(
        this->free_intervals.begin(), 
        this->free_intervals.end(),
        slice.start,
        interval::compare_by_position
    );

    if (lowerBound == this->free_intervals.end()) {
        return this;
    }

    auto upperBound = std::upper_bound(
        lowerBound,
        this->free_intervals.end(),
        slice.end(),
        interval::compare_by_position
    );

    auto start = slice.intersects(lowerBound) 
        ? lowerBound.start 
        : slice.start;

    auto end = slice.intersects(upperBound)
        ? upperBound.end()
        : slice.end();
    
    this->remove_interval_if(lowerBound, 
                            upperBound, 
                            [&slice](const interval& a) { slice.intersects(a); }
    );

    this->insert_interval({
        .start = start,
        .size = end - start
    });
}

template<std::size_t N, class Allocator>
void interval_based_bitset<N, Allocator>::remove_interval(const interval& slice) {

    auto position = std::binary_search(this->free_intervals.begin(), 
                                       this->free_intervals.end(), 
                                       slice,
                                       interval::compare_by_position);

    this->free_interval_sizes.erase(slice);
    this->free_intervals.erase(position);
}

template<std::size_t N, class Allocator>
template<class Iterator>
void interval_based_bitset<N, Allocator>::remove_interval(Iterator begin, Iterator end) {

    for(auto it = begin; it < end; it++) {
        this->free_interval_sizes.erase(*it);
    }

    this->free_intervals.erase(begin, end);
}

template<std::size_t N, class Allocator>
template<class Iterator, class UnaryPredicate>
void interval_based_bitset<N, Allocator>::remove_interval_if(Iterator begin, Iterator end, UnaryPredicate predicate) {
    for(auto it = begin; it < end; it++) {
        if(predicate)(*it) {
            this->free_interval_sizes.erase(*it);
        }
    }

    std::erase(std::remove_if(begin, end, predicate));
}


template<std::size_t N, class Allocator>
std::list<interval>::iterator interval_based_bitset<N, Allocator>::insert_interval(interval slice) {

    auto position = std::lower_bound(this->free_intervals.begin(), 
                                     this->free_intervals.end(), 
                                     slice, 
                                     interval::compare_by_position);
    
    this->free_intervals.insert(position, interval);
    this->free_interval_sizes.insert(interval);
}