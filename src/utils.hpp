/*
 * utils.hpp
 *
 *  Created on: Apr 15, 2014
 *      Author: louis
 */

#ifndef UTILS_HPP_
#define UTILS_HPP_


/*
 * Code for C++1y generator taken from
 * http://en.wikipedia.org/wiki/Generator_%28computer_programming%29#C.2B.2B
 */
class range
{
    private:
    int last;
    int iter;

    public:
    range(int begin, int end):
        last(end),
        iter(begin)
    {}
    range(int end):
        last(end),
        iter(0)
    {}

    // Iterable functions
    const range& begin() const { return *this; }
    const range& end() const { return *this; }

    // Iterator functions
    bool operator!=(const range&) const { return iter < last; }
    void operator++() { ++iter; }
    int operator*() const { return iter; }
};


#endif /* UTILS_HPP_ */
