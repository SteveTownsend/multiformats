// Tests for multihash
//
// Author: Matthew Knight
// File Name: multihash.cpp
// Date: 2019-10-10

#include "multiformats/multibase.hpp"
#include "multiformats/multihash.hpp"

#include <gtest/gtest.h>

//sha1,80,431fb5d4c9b735ba1a34d0df045118806ae2336f2c,110ae861e452cfd84dca9a17
//sha1,160,431fb5d4c9b735ba1a34d0df045118806ae2336f2c,1114e861e452cfd84dca9a176258c3d06e020a0c93d8

TEST(MultihashTests, Sha1) {
    using namespace Multiformats;

    auto raw = Multibase::decode("f431fb5d4c9b735ba1a34d0df045118806ae2336f2c");
//    Multihash multihash{raw, "sha1"};
}
