//
// Created by childcity on 14.09.20.
//

#ifndef MULTITHREADEDUDPRECEIVER_SERVERERROR_HPP
#define MULTITHREADEDUDPRECEIVER_SERVERERROR_HPP

#include <stdexcept>

struct ServerError final : public std::runtime_error {
    using std::runtime_error::runtime_error;
};


#endif //MULTITHREADEDUDPRECEIVER_SERVERERROR_HPP
