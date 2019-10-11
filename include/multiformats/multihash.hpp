// Multihash -- self-describing hashes
//
// Author: Matthew Knight
// File Name: multihash.hpp
// Date: 2019-09-12

#pragma once

#include "multiformats/varint.hpp"

#include <vector>

#include <cstdint>

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
} // namespace Multiformats
