// Multibase -- self-describing base encodings
//
// Author: Matthew Knight
// File Name: multibase.cpp
// Date: 2019-09-12

#include "multiformats/multibase.hpp"
#include "multiformats/util.hpp"

#include <openssl/evp.h>

#include <algorithm>
#include <iomanip>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <unordered_map>

#include <cctype>
#include <cstring>

#include <iostream>

namespace {
    using namespace Multiformats::Multibase;

    Protocol get_protocol(std::string const& str) {
        if (str.empty())
            throw std::runtime_error("can't get protocol for empty string");

        char first = str.front();
        switch (first) {
        case '\0':
            return Protocol::Identity;
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

    std::array const patterns{std::regex{"^.*$"},
                              std::regex{"^0[0-1]*$"},
                              std::regex{"^7[0-7]*$"},
                              std::regex{"^9[0-9]*$"},
                              std::regex{"^f[0-9a-f]*"},
                              std::regex{"^F[0-9A-F]*$"},
                              std::regex{"^v[0-9a-v]*$"},
                              std::regex{"^V[0-9A-V]*$"},
                              std::regex{"^t[0-9a-v=]*$"},
                              std::regex{"^T[0-9A-V=]*$"},
                              std::regex{"^b[2-7a-z]*$"},
                              std::regex{"^B[2-7A-Z]*$"},
                              std::regex{"^c[2-7a-z=]*$"},
                              std::regex{"^C[2-7A-Z=]*$^C[2-7A-Z=]*$"},
                              std::regex{"^h[13-7a-km-uw-z]*$"},
                              std::regex{"^Z[1-9A-HJ-Za-km-z]*$"},
                              std::regex{"^(z|1|Q)[1-9A-HJ-Za-km-z]*$"},
                              std::regex{"^u[0-9a-zA-Z+/]*$"},
                              std::regex{"^u[0-9a-zA-Z+/=]*$"},
                              std::regex{"^u[0-9a-zA-Z_-]*$"},
                              std::regex{"^u[0-9a-zA-Z_=-]*$"}};

    Protocol validate(std::string const& str) {
        auto protocol = get_protocol(str);
        if (patterns.size() < static_cast<std::uint32_t>(protocol))
            throw std::runtime_error("unknown protocol");

        std::regex pattern = patterns[static_cast<std::uint32_t>(protocol)];

        if (!std::regex_match(str, pattern))
            throw std::runtime_error("invalid characters for protocol");

        return protocol;
    }

    /**
     * Encoders and Decoders
     *
     * Decoders only check the prefix to make sure it is correct, if they come
     * accross errenous coding, incorrect sizing, it will throw.
     *
     * The setup here is that each implementation of the encoder and decoder is
     * a template function specialization, so that we can easily generate a
     * table later
     */
    template <Protocol protocol>
    void encode(std::vector<std::uint8_t> const& input, std::string& output);

    template <Protocol protocol>
    void decode(std::string const& input, std::vector<std::uint8_t>& output);

    template <Protocol protocol>
    void encode_upper(std::vector<std::uint8_t> const& input,
                      std::string& output) {
        encode<protocol>(input, output);
        std::transform(output.begin(), output.end(), output.begin(),
                       [](auto elem) { return std::toupper(elem); });
    }

    template <Protocol protocol>
    void decode_upper(std::string const& input,
                      std::vector<std::uint8_t>& output) {
        std::string clone = input;
        std::transform(clone.begin(), clone.end(), clone.begin(),
                       [](auto elem) { return std::tolower(elem); });
        decode<protocol>(clone, output);
    }

    // Identity
    template <>
    void encode<Protocol::Identity>(std::vector<std::uint8_t> const& input,
                                    std::string& output) {
        output = std::string{input.cbegin(), input.cend()};
    }

    template <>
    void decode<Protocol::Identity>(std::string const& input,
                                    std::vector<std::uint8_t>& output) {
        output = std::vector<std::uint8_t>{input.cbegin(), input.cend()};
    }

    // Base2
    template <>
    void encode<Protocol::Base2>(std::vector<std::uint8_t> const& input,
                                 std::string& output) {
        output.reserve((8 * input.size()) + 1);
        auto inserter = std::back_inserter(output);
        inserter = '0';

        for (auto it = input.cbegin(); it != input.cend(); ++it)
            for (auto bit = 8; bit > 0; bit--)
                inserter = *it & (1 << (bit - 1)) ? '1' : '0';
    }

    template <>
    void decode<Protocol::Base2>(std::string const& input,
                                 std::vector<std::uint8_t>& output) {
        if ((input.size() - 1) % 8 != 0)
            throw std::runtime_error("Base2 encoding does not align to 8 bits");

        output.reserve((input.size() - 1) / 8);

        auto inserter = std::back_inserter(output);
        for (auto it = std::next(input.cbegin(), 1); it != input.cend();) {
            std::uint8_t value{0};
            for (auto bit = 8; bit > 0 && it != input.cend(); bit--) {
                if (*it == '1')
                    value |= 1 << (bit - 1);
                else if (*it != '0')
                    throw std::runtime_error("nope");

                it++;
            }

            inserter = value;
        }
    }

    // Base8
    template <>
    void encode<Protocol::Base8>(std::vector<std::uint8_t> const& input,
                                 std::string& output) {
        std::array const characters{'0', '1', '2', '3', '4', '5', '6', '7'};
        std::uint8_t const mask{0x7};

        output.reserve(input.size());

        auto it = input.crbegin();
        auto end = input.crend();
        auto inserter = back_inserter(output);

        // find end of leading zero bytes
        while (end != input.crbegin() && *std::prev(end) == '0')
            --end;

        std::uint8_t overflow;
        std::uint8_t offset{0};
        for (; it != end; ++it) {
            if (offset > 0) {
                auto value =
                    ((*it & (mask >> (3 - offset))) << (3 - offset)) | overflow;
                inserter = characters[value];
            }

            for (; offset < 8; offset += 3) {
                auto value = (*it & (mask << offset)) >> offset;
                if (offset > 5)
                    overflow = value;
                else
                    inserter = characters[value];
            }

            offset &= mask;
        }

        if (((offset - 3) & mask) > 5 && overflow != 0)
            inserter = characters[overflow];

        fill_n(inserter, std::distance(end, std::prev(input.crend())), '0');
        inserter = '7';
        std::reverse(output.begin(), output.end());
    }

    template <>
    void decode<Protocol::Base8>(std::string const& input,
                                 std::vector<std::uint8_t>& output) {

        std::uint8_t const mask{0x7};
        std::size_t leading_zeros{0};
        auto convert = [](auto num) { return num - 48; };

        if (input.empty())
            throw std::runtime_error("input is empty");

        if (input.size() > 1) {
            auto it = std::next(input.cbegin());
            for (; *it == '0' && it != input.cend(); ++it)
                leading_zeros++;

            std::size_t ms_bits{3};
            while ((convert(*it) & (1 << ms_bits)) == 0 && ms_bits > 0)
                ms_bits--;

            ++it;

            std::size_t bits = ms_bits + (3 * distance(it, input.cend()));
            std::fill_n(std::back_inserter(output), leading_zeros, 0);
            std::fill_n(std::back_inserter(output), bits / 8 + 1, 0);
        } else {
            return;
        }

        auto it = input.crbegin();
        auto out = output.rbegin();
        auto inserter = std::back_inserter(output);

        std::uint8_t overflow;
        std::uint8_t offset{0};
        for (; it != std::prev(input.crend(), leading_zeros + 1) &&
               out != output.rend();
             ++it) {
            auto value = convert(*it);

            if (offset > 0 && offset < 3)
                *out |= overflow;

            *out |= (value & mask) << offset;

            if (offset > 4)
                ++out;

            if (offset > 5)
                overflow = value >> (8 - offset);

            offset = (offset + 3) & mask;
        }
    }

    // Base10
    template <>
    void encode<Protocol::Base10>(std::vector<std::uint8_t> const& input,
                                  std::string& output) {
        std::uint8_t const mask{0x7};

        if (input.empty()) {
            output.push_back('9');
            return;
        }

        std::uint32_t overflow{0};

        auto it = input.cbegin();
        while (it != input.cend() && *it == 0)
            ++it;

        auto leading_zeros = std::distance(input.cbegin(), it);
        std::uint8_t bit{7};
        while ((*it & (1 << bit)) == 0 && bit)
            bit--;

        std::vector<std::uint32_t> buf{0};

        // double dabble algorithm
        for (; it != input.cend(); ++it) {
            for (; bit < 8; bit--) {
                // add 3
                for (auto& elem : buf) {
                    for (int i = 0; i < 32; i += 4) {
                        std::uint32_t msk = 0xf;
                        std::uint32_t min = 4;
                        std::uint32_t offset = 3;

                        if ((elem & (msk << i)) > (min << i))
                            elem += (offset << i);
                    }
                }

                // check to see if a new word is required
                if (buf.back() & (1 << 31))
                    buf.push_back(0);

                // shift
                bool overflow = (*it & (1 << bit)) != 0;
                for (auto& elem : buf) {
                    bool tmp = static_cast<bool>(elem & (1 << 31));
                    elem = (elem << 1) | overflow;
                    overflow = tmp;
                }
            }

            bit &= mask;
        }

        // formatting output
        std::stringstream ss;

        // print leading zeros
        ss << '9';
        for (auto i = 0; i < leading_zeros; i++)
            ss << '0';

        auto buf_it = buf.crbegin();
        ss << std::hex << *(buf_it++) << std::setfill('0') << std::setw(2);
        for (; buf_it != buf.crend(); ++buf_it)
            ss << *buf_it;

        output = ss.str();
    }

    template <>
    void decode<Protocol::Base10>(std::string const& input,
                                  std::vector<std::uint8_t>& output) {}

    // Base16
    template <>
    void encode<Protocol::Base16>(std::vector<std::uint8_t> const& input,
                                  std::string& output) {
        std::array const characters{'0', '1', '2', '3', '4', '5', '6', '7',
                                    '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

        output.reserve(2 * input.size());
        auto inserter = std::back_inserter(output);
        inserter = 'f';

        for (auto elem : input) {
            inserter = characters[(0xf0 & elem) >> 4];
            inserter = characters[0x0f & elem];
        }
    }

    template <>
    void decode<Protocol::Base16>(std::string const& input,
                                  std::vector<std::uint8_t>& output) {
        if (input.size() % 2 != 1)
            throw std::runtime_error("incorrect alignment for Base16");

        output.reserve(input.size() / 2);

        auto it = std::next(input.cbegin());
        auto inserter = std::back_inserter(output);
        auto convert = [](std::uint8_t num) {
            std::uint8_t value;
            if (num > 47 && num < 58)
                value = num - 48;
            else if (num > 96 && num < 123)
                value = num - 87;
            else
                throw std::runtime_error("invalid character");

            return value;
        };

        while (it != input.cend()) {
            auto ms = *it;
            ++it;
            auto ls = *it;
            ++it;

            inserter = (convert(ms) << 4) | convert(ls);
        }
    }

    // Base32 variables / functions
    std::array const base32_lookup{'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
                                   'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
                                   'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
                                   'y', 'z', '2', '3', '4', '5', '6', '7'};

    std::array const base32_hex_lookup{'0', '1', '2', '3', '4', '5', '6', '7',
                                       '8', '9', 'a', 'b', 'c', 'd', 'e', 'f',
                                       'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                       'o', 'p', 'q', 'r', 's', 't', 'u', 'v'};

    std::array const base32_z_lookup{'y', 'b', 'n', 'd', 'r', 'f', 'g', '8',
                                     'e', 'j', 'k', 'm', 'c', 'p', 'q', 'x',
                                     'o', 't', '1', 'u', 'w', 'i', 's', 'z',
                                     'a', '3', '4', '5', 'h', '7', '6', '9'};

    void base32_encode(std::array<char, 32> const& lookup, bool padding,
                       std::vector<std::uint8_t> const& input,
                       std::string& output) {
        std::uint8_t const mask{0x1f};
        std::size_t size{padding ? ((((input.size() - 1) / 5) + 1) * 8 + 1)
                                 : ((((input.size() * 8) + 4) / 5) + 1)};
        output.reserve(size);
        auto inserter = std::back_inserter(output);
        inserter = 'b';

        std::uint8_t offset{0};
        std::uint8_t overflow{0};

        auto it = input.cbegin();
        for (; it != input.cend(); ++it) {
            // grab overflow
            if (offset > 0) {
                auto shift = 8 - offset;
                auto value = (*it & (mask << shift)) >> shift;
                inserter = lookup[overflow + value];
            }

            for (; offset < 8; offset += 5) {
                if (offset > 3) {
                    auto shift = offset - 3;
                    auto value = (*it & (mask >> shift)) << shift;
                    if (it == std::prev(input.cend()))
                        inserter = lookup[value];
                    else
                        overflow = value;
                } else {
                    auto shift = 3 - offset;
                    auto value = (*it & (mask << shift)) >> shift;
                    inserter = lookup[value];
                }
            }

            offset &= 0x7;
        }

        if (padding) {
            auto mod = (output.size() - 1) % 8;
            std::size_t pads = mod == 0 ? 0 : 8 - mod;
            std::fill_n(inserter, pads, '=');
        }
    }

    void base32_decode(std::array<char, 32> const& lookup, bool padding,
                       std::string const& input,
                       std::vector<std::uint8_t>& output) {}

    // Base32Hex
    template <>
    void encode<Protocol::Base32Hex>(std::vector<std::uint8_t> const& input,
                                     std::string& output) {
        base32_encode(base32_hex_lookup, false, input, output);
        output.front() = 'v';
    }

    template <>
    void decode<Protocol::Base32Hex>(std::string const& input,
                                     std::vector<std::uint8_t>& output) {}

    // Base32HexPad
    template <>
    void encode<Protocol::Base32HexPad>(std::vector<std::uint8_t> const& input,
                                        std::string& output) {
        base32_encode(base32_hex_lookup, true, input, output);
        output.front() = 't';
    }

    template <>
    void decode<Protocol::Base32HexPad>(std::string const& input,
                                        std::vector<std::uint8_t>& output) {}

    // Base32
    template <>
    void encode<Protocol::Base32>(std::vector<std::uint8_t> const& input,
                                  std::string& output) {
        base32_encode(base32_lookup, false, input, output);
    }

    template <>
    void decode<Protocol::Base32>(std::string const& input,
                                  std::vector<std::uint8_t>& output) {}

    // Base32Pad
    template <>
    void encode<Protocol::Base32Pad>(std::vector<std::uint8_t> const& input,
                                     std::string& output) {
        base32_encode(base32_lookup, true, input, output);
        output.front() = 'c';
    }

    template <>
    void decode<Protocol::Base32Pad>(std::string const& input,
                                     std::vector<std::uint8_t>& output) {}

    // Base32Z
    template <>
    void encode<Protocol::Base32Z>(std::vector<std::uint8_t> const& input,
                                   std::string& output) {
        base32_encode(base32_z_lookup, false, input, output);
        output.front() = 'h';
    }

    template <>
    void decode<Protocol::Base32Z>(std::string const& input,
                                   std::vector<std::uint8_t>& output) {}

    // Base58 Stuff goes here

    // Base64Pad
    template <>
    void encode<Protocol::Base64Pad>(std::vector<std::uint8_t> const& input,
                                     std::string& output) {
        if (input.empty()) {
            output = "M";
            return;
        }

        std::vector<std::uint8_t> buf;
        std::fill_n(std::back_inserter(buf),
                    (((input.size() - 1) / 3) + 1) * 4 + 2, 0);
        buf[0] = 'M';
        EVP_EncodeBlock(buf.data() + 1, input.data(), input.size());
        output = std::string{buf.begin(), std::prev(buf.end())};
    }

    template <>
    void decode<Protocol::Base64Pad>(std::string const& input,
                                     std::vector<std::uint8_t>& output) {}

    // Base64
    template <>
    void encode<Protocol::Base64>(std::vector<std::uint8_t> const& input,
                                  std::string& output) {
        if (input.empty()) {
            output = "m";
            return;
        }

        encode<Protocol::Base64Pad>(input, output);
        output[0] = 'm';
        while (output.back() == '=')
            output.pop_back();
    }

    template <>
    void decode<Protocol::Base64>(std::string const& input,
                                  std::vector<std::uint8_t>& output) {}
    // Base64UrlPad
    template <>
    void encode<Protocol::Base64UrlPad>(std::vector<std::uint8_t> const& input,
                                        std::string& output) {
        if (input.empty()) {
            output = "U";
            return;
        }

        encode<Protocol::Base64Pad>(input, output);
        output[0] = 'U';
        std::transform(output.begin(), output.end(), output.begin(),
                       [](auto& elem) {
                           switch (elem) {
                           case '+':
                               return '-';
                           case '/':
                               return '_';
                           default:
                               return elem;
                           }
                       });
    }

    template <>
    void decode<Protocol::Base64UrlPad>(std::string const& input,
                                        std::vector<std::uint8_t>& output) {}

    // Base64Url
    template <>
    void encode<Protocol::Base64Url>(std::vector<std::uint8_t> const& input,
                                     std::string& output) {
        if (input.empty()) {
            output = "u";
            return;
        }

        encode<Protocol::Base64UrlPad>(input, output);
        output[0] = 'u';
        while (output.back() == '=')
            output.pop_back();
    }

    template <>
    void decode<Protocol::Base64Url>(std::string const& input,
                                     std::vector<std::uint8_t>& output) {}

    struct Coder {
        using Encoder = void (*)(std::vector<std::uint8_t> const&,
                                 std::string&);
        using Decoder = void (*)(std::string const&,
                                 std::vector<std::uint8_t>&);

        Encoder encoder;
        Decoder decoder;
    };

    template <Protocol protocol>
    constexpr auto make_coder_entry() {
        return std::make_pair(protocol,
                              Coder{encode<protocol>, decode<protocol>});
    }

    template <Protocol upper, Protocol lower>
    constexpr auto make_upper_coder_entry() {
        return std::make_pair(upper,
                              Coder{[](std::vector<std::uint8_t> const& input,
                                       std::string& output) {
                                        encode_upper<lower>(input, output);
                                    },
                                    [](std::string const& input,
                                       std::vector<std::uint8_t>& output) {
                                        decode_upper<lower>(input, output);
                                    }});
    }

    std::unordered_map<Protocol, Coder> const coders{
        make_coder_entry<Protocol::Identity>(),
        make_coder_entry<Protocol::Base2>(),
        make_coder_entry<Protocol::Base8>(),
        make_coder_entry<Protocol::Base10>(),
        make_coder_entry<Protocol::Base16>(),
        make_upper_coder_entry<Protocol::Base16Upper, Protocol::Base16>(),
        make_coder_entry<Protocol::Base32Hex>(),
        make_upper_coder_entry<Protocol::Base32HexUpper, Protocol::Base32Hex>(),
        make_coder_entry<Protocol::Base32HexPad>(),
        make_upper_coder_entry<Protocol::Base32HexPadUpper,
                               Protocol::Base32HexPad>(),
        make_coder_entry<Protocol::Base32>(),
        make_upper_coder_entry<Protocol::Base32Upper, Protocol::Base32>(),
        make_coder_entry<Protocol::Base32Pad>(),
        make_upper_coder_entry<Protocol::Base32PadUpper, Protocol::Base32Pad>(),
        make_coder_entry<Protocol::Base32Z>(),
        make_coder_entry<Protocol::Base64>(),
        make_coder_entry<Protocol::Base64Pad>(),
        make_coder_entry<Protocol::Base64Url>(),
        make_coder_entry<Protocol::Base64UrlPad>()};

    auto find_coder(Protocol protocol) {
        auto it = coders.find(protocol);
        if (it == coders.end())
            throw std::runtime_error("unsupported protocol");
        return it->second;
    }
} // namespace

namespace Multiformats::Multibase {
    std::string to_string(Protocol protocol) {
        switch (protocol) {
        case Protocol::Identity:
            return "Identity";
        case Protocol::Base2:
            return "Base2";
        case Protocol::Base8:
            return "Base8";
        case Protocol::Base10:
            return "Base10";
        case Protocol::Base16:
            return "Base16";
        case Protocol::Base16Upper:
            return "Base16Upper";
        case Protocol::Base32Hex:
            return "Base32Hex";
        case Protocol::Base32HexUpper:
            return "Base32HexUpper";
        case Protocol::Base32HexPad:
            return "Base32HexPad";
        case Protocol::Base32HexPadUpper:
            return "Base32HexPadUpper";
        case Protocol::Base32:
            return "Base32";
        case Protocol::Base32Upper:
            return "Base32Upper";
        case Protocol::Base32Pad:
            return "Base32Pad";
        case Protocol::Base32PadUpper:
            return "Base32PadUpper";
        case Protocol::Base32Z:
            return "Base32Z";
        case Protocol::Base58Flickr:
            return "Base58Flickr";
        case Protocol::Base58Btc:
            return "Base58Btc";
        case Protocol::Base64:
            return "Base64";
        case Protocol::Base64Pad:
            return "Base64Pad";
        case Protocol::Base64Url:
            return "Base64Url";
        case Protocol::Base64UrlPad:
            return "Base64UrlPad";
        }

        // if not found, throw
        throw std::runtime_error("unknown protocol to_string");
    }

    std::vector<std::uint8_t> decode(std::string const& str) {
        auto protocol = validate(str);
        std::vector<std::uint8_t> ret;
        find_coder(protocol).decoder(str, ret);

        return ret;
    }

    std::string encode(Protocol protocol,
                       std::vector<std::uint8_t> const& buf) {
        std::string ret;
        find_coder(protocol).encoder(buf, ret);

        return ret;
    }
} // namespace Multiformats::Multibase
