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
    using namespace Multiformats;
    std::uint64_t const sha1{0x11};

    std::vector<std::uint8_t>
    sha1_encode(std::vector<std::uint8_t> const& buf) {
        std::vector<std::uint8_t> ret;
        std::fill_n(std::back_inserter(ret), SHA_DIGEST_LENGTH, 0);
        SHA1(buf.data(), SHA_DIGEST_LENGTH, ret.data());
        return ret;
    }

    auto get_hash_func(Varint const& protocol) {
        switch (static_cast<std::uint64_t>(protocol)) {
        case sha1:
            return sha1_encode;
        }

        throw std::invalid_argument("unsupported hash function");
    }

} // namespace

namespace Multiformats {
    /**
     * @param plaintext binary to hash
     * @param protocol function code of hash
     * @throw std::invalid_argument if function code is not supported */
    Multihash::Multihash(std::vector<std::uint8_t> const& plaintext,
                         Varint const& protocol) {
        auto hash = get_hash_func(protocol);
        std::vector<std::uint8_t> digest = hash(plaintext);
        Varint len{digest.size()};

        buf.reserve(protocol.size() + len.size() + digest.size());

        // bundle it all up
        std::copy(protocol.begin(), protocol.end(), std::back_inserter(buf));
        std::copy(len.begin(), len.end(), std::back_inserter(buf));
        std::copy(digest.cbegin(), digest.cend(), std::back_inserter(buf));
    }

    /**
     * @param plaintext binary to hash
     * @param protocol name of hash function
     * @throw std::out_of_range if protocol isn't in multicodec table */
    Multihash::Multihash(std::vector<std::uint8_t> const& plaintext,
                         std::string const& protocol)
        : Multihash(plaintext, Multicodec::table.at(protocol)) {}

    Varint Multihash::func_code() const {
        auto [fn, len_it] = make_varint(buf.cbegin(), buf.cend());
        return fn;
    }

    Varint Multihash::len() const {
        auto [fn, len_it] = make_varint(buf.cbegin(), buf.cend());
        auto [len, digest_it] = make_varint(len_it, buf.cend());
        return len;
    }

    auto Multihash::begin() const { return buf.cbegin(); }

    auto Multihash::digest() const {
        auto [fn, len_it] = make_varint(buf.cbegin(), buf.cend());
        auto [len, digest_it] = make_varint(len_it, buf.cend());
        return digest_it;
    }

    auto Multihash::end() const { return buf.cend(); }

} // namespace Multiformats
