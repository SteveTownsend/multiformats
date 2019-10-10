// Multihash -- self-describing hashes
//
// Author: Matthew Knight
// File Name: multihash.cpp
// Date: 2019-10-10

#include "multiformats/multihash.hpp"
#include "multiformats/multicodec.hpp"
#include "multiformats/varint.hpp"

#include <array>

namespace {
    using namespace Multiformats::Multihash;
    struct Coder {};
    std::vector<std::tuple<std::string, Varint, Coder>> const table{};
} // namespace

namespace Multiformats::Multihash {
    std::vector<std::uint8_t> encode_impl(std::vector<std::uint8_t> const& buf,
                                          Varint const& protocol) {

        // TODO: find encoding function
        std::vector<std::uint8_t> digest;
        return digest;
    }

    template <typename Iterator>
    std::vector<std::uint8_t> decode_impl(Iterator begin, Iterator end,
                                          Varint const& protocol) {
        std::vector<std::uint8_t> digest{begin, end};
        std::vector<std::uint8_t> ret;

        // TODO: find decode function
        return ret;
    }

    std::vector<std::uint8_t> encode(std::vector<std::uint8_t> const& buf,
                                     Varint const& protocol) {
        std::vector<std::uint8_t> digest = encode_impl(buf, protocol);
        Varint len = digest.size();

        std::vector<std::uint8_t> ret;
        ret.reserve(protocol.size() + len.size() + digest.size());

        // bundle it all up
        std::copy(protocol.begin(), protocol.end(), std::back_inserter(ret));
        std::copy(len.begin(), len.end(), std::back_inserter(ret));
        std::copy(digest.cbegin(), digest.cend(), std::back_inserter(ret));

        return ret;
    }

    std::vector<std::uint8_t> encode(std::vector<std::uint8_t> const& buf,
                                     std::string const& protocol) {
        return encode(buf, Multicodec::table.at(protocol));
    }

    std::vector<std::uint8_t> decode(std::vector<std::uint8_t> const& buf) {
        auto [fn, len_it] = make_varint(buf.cbegin(), buf.cend());
        auto [len, digest_it] = make_varint(len_it, buf.cend());

        if (len > std::distance(digest_it, buf.cend()))
            throw std::runtime_error("invalid digest size");

        return decode_impl(digest_it, buf.cend(), fn);
    }
} // namespace Multiformats::Multihash
