// Multibase -- self-describing base encodings
//
// Author: Matthew Knight
// File Name: multibase.hpp
// Date: 2019-09-12

#pragma once

#include "multiformats/exported.hpp"

#include <string>
#include <vector>

#include <cstdint>

namespace Multiformats::Multibase {
    enum class Protocol {
        Identity,
        Base2,
        Base8,
        Base10,
        Base16,
        Base16Upper,
        Base32Hex,
        Base32HexUpper,
        Base32HexPad,
        Base32HexPadUpper,
        Base32,
        Base32Upper,
        Base32Pad,
        Base32PadUpper,
        Base32Z,
        Base58Flickr,
        Base58Btc,
        Base64,
        Base64Pad,
        Base64Url,
        Base64UrlPad
    };

    std::vector<std::uint8_t> EXPORTED decode(std::string const& str);

    std::string EXPORTED encode(Protocol protocol, std::vector<std::uint8_t> const& buf);
} // namespace Multiformats::Multibase
