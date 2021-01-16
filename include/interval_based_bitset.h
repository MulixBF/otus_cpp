#pragma once

#include <bitset>
#include <list>
#include <set>


struct interval {
    std::size_t start;
    std::size_t size;

    interval() = default;

    bool contains(std::size_t key) const {
        return key >= start && key < start + size;
    }

    bool contains(const interval& other) const {
        return this->contains(other.start) 
        && this->contains(other.end());
    }

    bool intersects(const interval& other) const {
        return this->contains(other.start) 
        || this->contains(other.end());
    }

    std::size_t end() const {
        return start + size;
    }

    struct compare_by_size {
        constexpr bool operator()(const interval& a, const interval& b) const {
            return a.size < b.size;
        }
    };

    struct compare_by_position {
        constexpr bool operator()(const interval& a, const interval& b) const {
            return a.start < b.start;
        }
    };
};


template<std::size_t N, class Allocator = std::allocator<interval>>
class interval_based_bitset {
public:

    using allocator_type = Allocator;
    using value_type = bool;
    using key_type = std::size_t;
    using size_type = std::size_t;

    explicit interval_based_bitset(const Allocator& a = Allocator());
    interval_based_bitset(const interval_based_bitset<N, Allocator>&) = default;
    interval_based_bitset(interval_based_bitset<N, Allocator>&&) = default;
    interval_based_bitset(std::bitset<N> bitset, const Allocator& a = allocator_type());

    ~interval_based_bitset() = default;

    interval_based_bitset<N, Allocator>& operator= (const interval_based_bitset<N, Allocator>&) = default;
    interval_based_bitset<N, Allocator>& operator= (interval_based_bitset<N, Allocator>&&) = default;

    bool operator==(const interval_based_bitset&) const;
    bool operator[](key_type key) const;

    bool test(key_type key) const;
    bool all() const;
    bool any() const;
    bool none() const;
    interval find_shortest_interval(size_type min_size = 0, value_type value = false) const;

    size_type size() const;

    interval_based_bitset<N, Allocator>& set();
    interval_based_bitset<N, Allocator>& set(key_type key, value_type value = true);
    interval_based_bitset<N, Allocator>& set(interval slice, value_type value = true);

    interval_based_bitset<N, Allocator>& reset();
    interval_based_bitset<N, Allocator>& reset(key_type key);
    interval_based_bitset<N, Allocator>& reset(interval slice);

private:

    const Allocator& allocator;
    // intervals of consecutive zeroes, sorted by interval start point
    std::list<interval, Allocator> free_intervals;
    // set of same intervals, used as priority queue
    std::set<interval, interval::compare_by_size, Allocator> free_interval_sizes; 

    // Private methods
    void remove_interval(const interval& slice);

    template<class Iterator>
    void remove_interval(Iterator begin, Iterator end);

    template<class Iterator, class UnaryPredicate>
    void remove_interval_if(Iterator begin, Iterator end, UnaryPredicate p);

    std::list<interval>::iterator insert_interval(interval slice);    
};