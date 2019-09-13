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

        std::vector<Protocol> data;

      public:
        Multiaddr(std::string const& address);
        std::string to_string();
    };
} // namespace Multiformats
