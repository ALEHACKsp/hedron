/*
 * Generic Bitmap tests
 *
 * Copyright (C) 2020 Markus Partheymüller, Cyberus Technology GmbH.
 *
 * This file is part of the NOVA microhypervisor.
 *
 * NOVA is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * NOVA is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License version 2 for more details.
 */

// Include the class under test first to detect any missing includes early
#include <bitmap.hpp>

#include <catch2/catch.hpp>

#include <cstring>
#include <vector>

TEST_CASE("Bit_accessor sets correct bits", "[bitmap]")
{
    constexpr size_t NUMBER_OF_BITS {128};

    std::vector<unsigned> bitmap_storage;
    bitmap_storage.assign(NUMBER_OF_BITS / sizeof(decltype(bitmap_storage)::value_type) / 8, 0);

    std::vector<unsigned> bitmap_compare = bitmap_storage;

    SECTION("Default 0") {
        Bitmap<unsigned, NUMBER_OF_BITS> bitmap(false);

        SECTION("First bit") {
            bitmap[0] = true;
            bitmap_compare.at(0) = 1;
            memcpy(bitmap_storage.data(), &bitmap, sizeof(decltype(bitmap_storage)::value_type) * bitmap_storage.size());
            CHECK(bitmap_storage == bitmap_compare);
        }

        SECTION("Last bit") {
            bitmap[NUMBER_OF_BITS - 1] = true;
            bitmap_compare.back() = 1u << (sizeof(decltype(bitmap_storage)::value_type) * 8 - 1);
            memcpy(bitmap_storage.data(), &bitmap, sizeof(decltype(bitmap_storage)::value_type) * bitmap_storage.size());
            CHECK(bitmap_storage == bitmap_compare);
        }

        SECTION("Guaranteed middle element") {
            bitmap[64] = true;
            bitmap_compare.at(2) = 1;
            memcpy(bitmap_storage.data(), &bitmap, sizeof(decltype(bitmap_storage)::value_type) * bitmap_storage.size());
            CHECK(bitmap_storage == bitmap_compare);
        }

        SECTION("Odd number of bits") {
            bitmap_storage.push_back(0);
            Bitmap<unsigned, NUMBER_OF_BITS + 1> bitmap_odd(false);
            bitmap_odd[NUMBER_OF_BITS] = true;
            bitmap_compare.push_back(1);
            memcpy(bitmap_storage.data(), &bitmap_odd, sizeof(decltype(bitmap_storage)::value_type) * bitmap_storage.size());
            CHECK(bitmap_storage == bitmap_compare);
        }
    }

    SECTION("Default 1") {
        Bitmap<unsigned, NUMBER_OF_BITS> bitmap(true);
        bitmap_compare.assign(bitmap_compare.size(), ~0u);

        SECTION("Clearing bits works") {
            bitmap[0] = false;
            bitmap[31] = false;
            bitmap_compare.at(0) = 0x7FFFFFFE;
            memcpy(bitmap_storage.data(), &bitmap, sizeof(decltype(bitmap_storage)::value_type) * bitmap_storage.size());
            CHECK(bitmap_storage == bitmap_compare);
        }
    }
}
