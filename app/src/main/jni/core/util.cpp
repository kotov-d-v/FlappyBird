#include "util.h"
#include <vector>

namespace util {

//base64 code taken from http://stackoverflow.com/
    std::string base64_encode(unsigned char const* data, unsigned int len)
    {
        std::string out;

        int val=0, valb=-6;
        for (auto end = data+len; data != end; ++data) {
            unsigned char c = *data;
            val = (val<<8) + c;
            valb += 8;
            while (valb>=0) {
                out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[(val>>valb)&0x3F]);
                valb-=6;
            }
        }
        if (valb>-6) out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[((val<<8)>>(valb+8))&0x3F]);
        while (out.size()%4) out.push_back('=');
        return out;
    }

    std::string base64_decode(const std::string &in)
    {
        std::string out;

        std::vector<int> T(256,-1);
        for (int i=0; i<64; i++) T["ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[i]] = i;

        int val=0;
        int valb=-8;
        for (unsigned char c : in) {
            if (T[c] == -1) break;
            val = (val<<6) + T[c];
            valb += 6;
            if (valb>=0) {
                out.push_back(char((val>>valb)&0xFF));
                valb-=8;
            }
        }
        return out;
    }

// decode float with base64
    float decodeF(const std::string& s)
    {
        if(s.empty()) return 0.f;
        return *reinterpret_cast<const float*>(base64_decode(s).c_str());
    }

// decode int with base64
    int decodeI(const std::string& s)
    {
        if(s.empty()) return 0;
        return *reinterpret_cast<const int*>(base64_decode(s).c_str());
    }
};

