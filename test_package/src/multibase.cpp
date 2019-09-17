// Tests for multibase
//
// Author: Matthew Knight
// File Name: multibase.cpp
// Date: 2019-09-17

#include <gtest/gtest.h>

using namespace Multiformats::Multibase;

std::string const yes_mani_str{yes_mani};
std::vector<std::uint8_t> yes_mani{yes_mani_str.begin(), yes_mani_str.end()};
std::vector<std::uint8_t> f{0x66};
std::vector<std::uint8_t> fo{0x66, 0x6f};
std::vector<std::uint8_t> foo{0x66, 0x6f, 0x6f};
std::vector<std::uint8_t> foob{0x66, 0x6f, 0x6f, 0x62};
std::vector<std::uint8_t> fooba{0x66, 0x6f, 0x6f, 0x62, 0x61};
std::vector<std::uint8_t> foobar{0x66, 0x6f, 0x6f, 0x62, 0x61, 0x72};

struct Parameter {
    Protocol protocol;
    std::vector<std::uint8_t> buf;
    std::string encoded;
};

std::vector<Parameter> parameters{
    {Protocol::Base2, yes_mani,
     "0111100101100101011100110010000001101101011000010110111001101001001000000"
     "0100001"},
    {Protocol::Base8, yes_mani, "7171312714403326055632220041"},
    {Protocol::Base10, yes_mani, "9573277761329450583662625"},
    {Protocol::Base16, yes_mani, "f796573206d616e692021"},

    {Protocol::Base16, {0x01}, "f01"},
    {Protocol::Base16, {0x0f}, "f0f"},
    {Protocol::Base16, f, "f66"},
    {Protocol::Base16, fo, "f666f"},
    {Protocol::Base16, foo, "f666f6f"},
    {Protocol::Base16, foob, "f666f6f62"},
    {Protocol::Base16, fooba, "f666f6f6261"},
    {Protocol::Base16, foobar, "f666f6f626172"},

    {Protocol::Base32, yes_mani, "bpfsxgidnmfxgsibb"},
    {Protocol::Base32, f, "bmy"},
    {Protocol::Base32, fo, "bmzxq"},
    {Protocol::Base32, foo, "bmzxw6"},
    {Protocol::Base32, foob, "bmzxw6yq"},
    {Protocol::Base32, fooba, "bmzxw6ytb"},
    {Protocol::Base32, foobar, "bmzxw6ytboi"},

    {Protocol::Base32Pad, yes_mani, "cpfsxgidnmfxgsibb"},
    {Protocol::Base32Pad, f, "cmy======"},
    {Protocol::Base32Pad, fo, "cmzxq===="},
    {Protocol::Base32Pad, foo, "cmzxw6==="},
    {Protocol::Base32Pad, foob, "cmzxw6yq="},
    {Protocol::Base32Pad, fooba, "cmzxw6ytb"},
    {Protocol::Base32Pad, foobar, "cmzxw6ytboi======"},

    {Protocol::Base32Hex, yes_mani, "vf5in683dc5n6i811"},
    {Protocol::Base32Hex, f, "vco"},
    {Protocol::Base32Hex, fo, "vcpng"},
    {Protocol::Base32Hex, foo, "vcpnmu"},
    {Protocol::Base32Hex, foob, "vcpnmuog"},
    {Protocol::Base32Hex, fooba, "vcpnmuoj1"},
    {Protocol::Base32Hex, foobar, "vcpnmuoj1e8"},

    {Protocol::Base32HexPad, yes_mani, "tf5in683dc5n6i811"},
    {Protocol::Base32HexPad, f, "tco======"},
    {Protocol::Base32HexPad, fo, "tcpng===="},
    {Protocol::Base32HexPad, foo, "tcpnmu==="},
    {Protocol::Base32HexPad, foob, "tcpnmuog="},
    {Protocol::Base32HexPad, fooba, "tcpnmuoj1"},
    {Protocol::Base32HexPad, foobar, "tcpnmuoj1e8======"},

    {Protocol::Base32Z, yes_mani, "hxf1zgedpcfzg1ebb"},
    {Protocol::Base58Flickr, yes_mani, "Z7Pznk19XTTzBtx"},
    {Protocol::Base58Btc, yes_mani, "z7paNL19xttacUY"},

    {Protocol::Base64, "÷ïÿ", "mw7fDr8O/"},
    {Protocol::Base64, f, "mZg"},
    {Protocol::Base64, fo, "mZm8"},
    {Protocol::Base64, foo, "mZm9v"},
    {Protocol::Base64, foob, "mZm9vYg"},
    {Protocol::Base64, fooba, "mZm9vYmE"},
    {Protocol::Base64, foobar, "mZm9vYmFy"},
    {Protocol::Base64, "÷ïÿ🥰÷ïÿ😎🥶🤯",
     "mw7fDr8O/8J+lsMO3w6/Dv/CfmI7wn6W28J+krw"},

    {Protocol::Base64Pad, f, "MZg=="},
    {Protocol::Base64Pad, fo, "MZm8="},
    {Protocol::Base64Pad, foo, "MZm9v"},
    {Protocol::Base64Pad, foob, "MZm9vYg=="},
    {Protocol::Base64Pad, fooba, "MZm9vYmE="},
    {Protocol::Base64Pad, foobar, "MZm9vYmFy"},

    {Protocol::Base64Url, "÷ïÿ", "uw7fDr8O_"},
    {Protocol::Base64Url, "÷ïÿ🥰÷ïÿ😎🥶🤯",
     "uw7fDr8O_8J-lsMO3w6_Dv_CfmI7wn6W28J-krw"},

    {Protocol::Base64UrlPad, f, "UZg=="},
    {Protocol::Base64UrlPad, fo, "UZm8="},
    {Protocol::Base64UrlPad, foo, "UZm9v"},
    {Protocol::Base64UrlPad, foob, "UZm9vYg=="},
    {Protocol::Base64UrlPad, fooba, "UZm9vYmE="},
    {Protocol::Base64UrlPad, foobar, "UZm9vYmFy"},
    {Protocol::Base64UrlPad, "÷ïÿ🥰÷ïÿ😎🥶🤯",
     "Uw7fDr8O_8J-lsMO3w6_Dv_CfmI7wn6W28J-krw=="}};
