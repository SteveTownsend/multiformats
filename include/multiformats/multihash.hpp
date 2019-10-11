/**
 * Self-describing hashes
 *
 * @file multihash.hpp
 * @author Matthew Knight
 * @date 2019-10-11
 */

#pragma once

#include "multiformats/varint.hpp"

#include <vector>

#include <cstdint>

namespace Multiformats {
    /** @brief Object for containing specific hash */
    class Multihash {
        std::vector<std::uint8_t> buf;

      public:
        /** @brief Hash plaintext using explicit fuction code */
        Multihash(std::vector<std::uint8_t> const& plaintext,
                  Varint const& protocol);

        /** @brief Hash plaintext using string name */
        Multihash(std::vector<std::uint8_t> const& plaintext,
                  std::string const& protocol);

        /** @brief Extract function code from multihash */
        Varint func_code() const;

        /** @brief Extract digest length from multihash */
        Varint len() const;

        /** @brief Const iterator to begining of multihash */
        auto begin() const;

        /** @brief Const iterator to beginning of digest withing multihash */
        auto digest() const;

        /** @brief Const iterator to end of multihash */
        auto end() const;
    };
} // namespace Multiformats
