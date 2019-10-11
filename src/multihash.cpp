// Multihash -- self-describing hashes
//
// Author: Matthew Knight
// File Name: multihash.cpp
// Date: 2019-10-10

#include "multiformats/multihash.hpp"
#include "multiformats/multicodec.hpp"
#include "multiformats/varint.hpp"

#include "openssl/sha.h"

#include <array>

namespace {
    using namespace Multiformats::Multihash;
    std::uint64_t const sha1{0x11};

    std::vector<std::uint8_t>
    sha1_encode(std::vector<std::uint8_t> const& buf) {
        std::vector<std::uint8_t> ret;
        std::fill_n(std::back_inserter(ret), SHA_DIGEST_LENGTH, 0);
        SHA1(buf.data(), SHA_DIGEST_LENGTH, ret.data());
        return ret;
    }

    constexpr auto get_encode_func(Varint const& protocol) {
        switch (static_cast<std::uint64_t>(protocol)) {
        case sha1:
            return sha1_encode;
        }

        throw std::runtime_error("unsupported hash decode");
    }

    constexpr auto get_decode_func(Varint const& protocol) {
        switch (static_cast<std::uint64_t>(protocol)) {}

        throw std::runtime_error("unsupported hash decode");
    }

} // namespace

namespace Multiformats {
    class Multihash {
        std::vector<std::uint8_t> buf;

      public:
        Multihash(std::vector<std::uint8_t> const& plaintext,
                  Varint const& protocol);

        Multihash(std::vector<std::uint8_t> const& plaintext,
                  std::string const& protocol);

        Varint func_code() const;
        Varint len() const;

        auto begin() const;
        auto digest() const;
        auto end() const;
    };

    std::vector<std::uint8_t> encode(std::vector<std::uint8_t> const& buf,
                                     Varint const& protocol) {

        auto encode_impl = get_encode_func(protocol);
        std::vector<std::uint8_t> digest = encode_impl(buf);
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

    std::vector<std::uint8_t> get_digest(std::vector<std::uint8_t> const& buf) {
        auto [fn, len_it] = make_varint(buf.cbegin(), buf.cend());
        auto [len, digest_it] = make_varint(len_it, buf.cend());

        if (len > std::distance(digest_it, buf.cend()))
            throw std::runtime_error("invalid digest size");

        // auto decode_impl = get_decode_func(fn);
        std::vector<std::uint8_t> digest{digest_it, buf.cend()};

        // return decode_impl(digest);
        return {};
    }
} // namespace Multiformats
