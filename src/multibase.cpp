// Multibase -- self-describing base encodings
//
// Author: Matthew Knight
// File Name: multibase.cpp
// Date: 2019-09-12

#include "multiformats/multibase.hpp"
#include "multiformats/util.hpp"

#include <regex>
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

    std::unordered_map<Protocol, std::regex> const patterns{
        std::make_pair(Protocol::Identity, std::regex{"^0x00.*$"}),
        std::make_pair(Protocol::Base2, std::regex{"^0[0-1]*$"}),
        std::make_pair(Protocol::Base8, std::regex{"^7[0-7]*$"}),
        std::make_pair(Protocol::Base10, std::regex{"^9[0-9]*$"}),
        std::make_pair(Protocol::Base16, std::regex{"^f[0-9a-f]*"}),
        std::make_pair(Protocol::Base16Upper, std::regex{"^F[0-9A-F]*$"}),
        std::make_pair(Protocol::Base32Hex, std::regex{"^v[0-9a-v]*$"}),
        std::make_pair(Protocol::Base32HexUpper, std::regex{"^V[0-9A-V]*$"}),
        std::make_pair(Protocol::Base32HexPad, std::regex{"^t[0-9a-v=]*$"}),
        std::make_pair(Protocol::Base32HexPadUpper,
                       std::regex{"^T[0-9A-V=]*$"}),
        std::make_pair(Protocol::Base32, std::regex{"^b[2-7a-z]*$"}),
        std::make_pair(Protocol::Base32Upper, std::regex{"^B[2-7A-Z]*$"}),
        std::make_pair(Protocol::Base32Pad, std::regex{"^c[2-7a-z=]*$"}),
        std::make_pair(Protocol::Base32PadUpper,
                       std::regex{"^C[2-7A-Z=]*$^C[2-7A-Z=]*$"}),
        std::make_pair(Protocol::Base32Z, std::regex{"^h[13-7a-km-uw-z]*$"}),
        std::make_pair(Protocol::Base58Flickr,
                       std::regex{"^Z[1-9A-HJ-Za-km-z]*$"}),
        std::make_pair(Protocol::Base58Btc,
                       std::regex{"^(z|1|Q)[1-9A-HJ-Za-km-z]*$"}),
        std::make_pair(Protocol::Base64, std::regex{"^u[0-9a-zA-Z+/]*$"}),
        std::make_pair(Protocol::Base64Pad, std::regex{"^u[0-9a-zA-Z+/=]*$"}),
        std::make_pair(Protocol::Base64Url, std::regex{"^u[0-9a-zA-Z_-]*$"}),
        std::make_pair(Protocol::Base64UrlPad,
                       std::regex{"^u[0-9a-zA-Z+/=]*$"})};

    Protocol validate(std::string const& str) {
        auto protocol = get_protocol(str);
        auto it = patterns.find(protocol);

        if (it == patterns.end())
            throw std::runtime_error("unknown protocol");

        std::regex pattern = it->second;
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
        output.reserve(input.size());

        auto it = input.crbegin();
        auto end = input.crend();
        auto inserter = back_inserter(output);

        // find end of leading zero bytes
        while (end != input.crbegin() && *std::prev(end) == '0')
            --end;

        std::uint32_t overflow;
        std::uint32_t offset{0};
        for (; it != end; ++it) {
            if (offset > 0) {
                auto value =
                    ((*it & (0x7 >> (3 - offset))) << (3 - offset)) | overflow;
                inserter = characters[value];
            }

            for (; offset < 8; offset += 3) {
                auto value = (*it & (0x7 << offset)) >> offset;
                if (offset > 5)
                    overflow = value;
                else
                    inserter = characters[value];
            }

            offset = offset & 0x7;
        }

        if (((offset - 3) & 0x7) > 5 && overflow != 0)
            inserter = characters[overflow];

        fill_n(inserter, std::distance(end, std::prev(input.crend())), '0');
        inserter = '7';
        std::reverse(output.begin(), output.end());
    }

    template <>
    void decode<Protocol::Base8>(std::string const& input,
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

    // Base16Upper
    template <>
    void encode<Protocol::Base16Upper>(std::vector<std::uint8_t> const& input,
                                       std::string& output) {
        output.reserve(2 * input.size());
        encode<Protocol::Base16>(input, output);
        std::transform(output.begin(), output.end(), output.begin(),
                       [](auto elem) { return std::toupper(elem); });
    }

    template <>
    void decode<Protocol::Base16Upper>(std::string const& input,
                                       std::vector<std::uint8_t>& output) {
        std::string clone = input;

        std::transform(clone.begin(), clone.end(), clone.begin(),
                       [](auto elem) { return std::tolower(elem); });
        decode<Protocol::Base16>(clone, output);
    }

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

    std::unordered_map<Protocol, Coder> const coders{
        make_coder_entry<Protocol::Base2>(),
        make_coder_entry<Protocol::Base8>(),
        make_coder_entry<Protocol::Base16>(),
        make_coder_entry<Protocol::Base16Upper>()};

    auto find_coder(Protocol protocol) {
        auto it = coders.find(protocol);
        if (it == coders.end())
            throw std::runtime_error("unsupported protocol");
        return it->second;
    }
} // namespace

namespace Multiformats::Multibase {
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
