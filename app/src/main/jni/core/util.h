//
// Created by Dmitry Kotov on 4/3/16.
//

#ifndef FLAPPYBIRD_UTIL_H
#define FLAPPYBIRD_UTIL_H

#include <string>
#include <sstream>

namespace util {

    std::string base64_encode(unsigned char const* data, unsigned int len);

    std::string base64_decode(std::string const& s);

// convert type to string ( c++11 std::to_string() is not fully supported by compilers)
    template<typename T> std::string toString(const T& x)
    {
        std::ostringstream sstream;
        sstream << x;
        return sstream.str();
    }

// encode data with base64
    template<typename T> std::string encode(const T& x)
    {
        return base64_encode(reinterpret_cast<unsigned char const*>(&x), sizeof(T));
    }

// decode float with base64
    float decodeF(const std::string& s);

// decode int with base64
    int decodeI(const std::string& s);
};

#endif //FLAPPYBIRD_UTIL_H
