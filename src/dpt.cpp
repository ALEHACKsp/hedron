/*
 * Intel IOMMU Device Page Table (DPT) modification
 *
 * Copyright (C) 2019 Julian Stecklina, Cyberus Technology GmbH.
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

#include "dpt.hpp"
#include "mdb.hpp"

Dpt::level_t Dpt::supported_leaf_levels {-1};

Dpt::pte_t Dpt::hw_attr(mword a)
{
    auto const none {static_cast<decltype(Dpt::PTE_R)>(0)};

    if (a) {
        return
              (a & Mdb::MEM_R ? Dpt::PTE_R : none)
            | (a & Mdb::MEM_W ? Dpt::PTE_W : none);
    }

    return 0;
}

void Dpt::lower_supported_leaf_levels(Dpt::level_t level)
{
    assert (level > 0);

    supported_leaf_levels = supported_leaf_levels < 0 ? level : min(supported_leaf_levels, level);
}