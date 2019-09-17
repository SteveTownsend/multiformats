// Multibase -- self-describing base encodings
//
// Author: Matthew Knight
// File Name: multibase.cpp
// Date: 2019-09-12

#include "multiformats/multibase.hpp"

#include <regex>
#include <stdexcept>

#include <cstring>

namespace {
    using namespace Multiformats::Multibase;
    std::string const identity{"0x00"};

    Protocol get_protocol(std::string const& str) {
        if (strncmp(str.c_str(), identity.c_str(), identity.size()) == 0)
            return Protocol::Identity;

        char first = str[0];
        switch (first) {
        case '0':
            return Protocol::Base2;
        case '7':
            return Protocol::Base8;
        case '9':
            return Protocol::Base10;
        case 'f':
            return Protocol::Base16;
        case 'F':
            return Protocol::Base16Upper;
        case 'v':
            return Protocol::Base32Hex;
        case 'V':
            return Protocol::Base32HexUpper;
        case 't':
            return Protocol::Base32HexPad;
        case 'T':
            return Protocol::Base32HexPadUpper;
        case 'b':
            return Protocol::Base32;
        case 'B':
            return Protocol::Base32Upper;
        case 'c':
            return Protocol::Base32Pad;
        case 'C':
            return Protocol::Base32PadUpper;
        case 'h':
            return Protocol::Base32Z;
        case 'Z':
            return Protocol::Base58Flickr;
        case 'z':
        case '1':
        case 'Q':
            return Protocol::Base58Btc;
        case 'm':
            return Protocol::Base64;
        case 'M':
            return Protocol::Base64Pad;
        case 'u':
            return Protocol::Base64Url;
        case 'U':
            return Protocol::Base64UrlPad;
        }

        throw std::runtime_error("invalid protocol");
    }

    std::regex get_pattern(Protocol protocol) {
        const char* pattern = "";

        switch (protocol) {
        case Protocol::Identity:
            pattern = "^0x00.*$";
            break;
        case Protocol::Base2:
            pattern = "^0[0-1]*$";
            break;
        case Protocol::Base8:
            pattern = "^7[0-7]*$";
            break;
        case Protocol::Base10:
            pattern = "^9[0-9]*$";
            break;
        case Protocol::Base16:
            pattern = "^f[0-9a-f]*$";
            break;
        case Protocol::Base16Upper:
            pattern = "^F[0-9A-F]*$";
            break;
        case Protocol::Base32Hex:
            pattern = "^v[0-9a-v]*$";
            break;
        case Protocol::Base32HexUpper:
            pattern = "^V[0-9A-V]*$";
            break;
        case Protocol::Base32HexPad:
            pattern = "^t[0-9a-v=]*$";
            break;
        case Protocol::Base32HexPadUpper:
            pattern = "^T[0-9A-V=]*$";
            break;
        case Protocol::Base32:
            pattern = "^b[2-7a-z]*$";
            break;
        case Protocol::Base32Upper:
            pattern = "^B[2-7A-Z]*$";
            break;
        case Protocol::Base32Pad:
            pattern = "^c[2-7a-z=]*$";
            break;
        case Protocol::Base32PadUpper:
            pattern = "^C[2-7A-Z=]*$";
            break;
        case Protocol::Base32Z:
            pattern = "^h[13-7a-km-uw-z]*$";
            break;
        case Protocol::Base58Flickr:
            pattern = "^Z[1-9A-HJ-Za-km-z]*$";
            break;
        case Protocol::Base58Btc:
            pattern = "^(z|1|Q)[1-9A-HJ-Za-km-z]*$";
            break;
        case Protocol::Base64:
            pattern = "^u[0-9a-zA-Z+/]*$";
            break;
        case Protocol::Base64Pad:
            pattern = "^u[0-9a-zA-Z+/=]*$";
            break;
        case Protocol::Base64Url:
            pattern = "^u[0-9a-zA-Z_-]*$";
            break;
        case Protocol::Base64UrlPad:
            pattern = "^u[0-9a-zA-Z+/=]*$";
            break;
        }

        if (strlen(pattern) == 0)
            throw std::runtime_error("unknown or unsupported protocol");

        return std::regex{pattern};
    }

    void validate(std::string const& str) {
        auto protocol = get_protocol(str);
        std::regex pattern = get_pattern(protocol);

        if (!std::regex_match(str, pattern))
            throw std::runtime_error("invalid characters for protocol");
    }
} // namespace

namespace Multiformats::Multibase {
    std::vector<std::uint8_t> decode(std::string const& str) { return {}; }

    std::string encode(Protocol protocol,
                       std::vector<std::uint8_t> const& buf) {
        return {};
    }
} // namespace Multiformats::Multibase
