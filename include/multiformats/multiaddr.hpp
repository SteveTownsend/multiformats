// Multiaddr -- self-describing network addresses
//
// Author: Matthew Knight
// File Name: multiaddr.hpp
// Date: 2019-09-12

#pragma once

#include "varint.hpp"

#include <string>
#include <vector>

#include <cstdint>

namespace Multiformats {
    class Multiaddr {
        struct Protocol {
            Varint code;
            std::vector<std::uint8_t> value;
        };

        std::vector<Protocol> addr;

      public:
        using ConstIterator = typename decltype(addr)::const_iterator;

        /** @brief Construct from human-readable string */
        Multiaddr(std::string const& address);

        /** @brief Construct from binary */
        Multiaddr(std::vector<std::uint8_t> const& raw);

        /** @brief Get human-readable string */
        std::string to_string() const;

        /** @brief Get binary form */
        std::vector<std::uint8_t> to_binary() const;

        /** @brief Get iterator to beginning of multiaddr */
        ConstIterator begin() const;

        /** @brief Get iterator to end of multiaddr */
        ConstIterator end() const;
    };
} // namespace Multiformats
