/*
 * Host Page Table (HPT)
 *
 * Copyright (C) 2009-2010 Udo Steinberg <udo@hypervisor.org>
 * Economic rights: Technische Universitaet Dresden (Germany)
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

#pragma once

#include "assert.h"
#include "compiler.h"
#include "pte.h"
#include "user.h"

class Hpt : public Pte<Hpt, mword, 2, 10, false>
{
    friend class Vtlb;

    private:
        ALWAYS_INLINE
        static inline void flush()
        {
            mword cr3;
            asm volatile ("mov %%cr3, %0; mov %0, %%cr3" : "=&r" (cr3));
        }

        ALWAYS_INLINE
        static inline void flush (mword addr)
        {
            asm volatile ("invlpg %0" : : "m" (*reinterpret_cast<mword *>(addr)));
        }

    public:
        static mword ord;

        enum
        {
            HPT_P   = 1UL << 0,
            HPT_W   = 1UL << 1,
            HPT_U   = 1UL << 2,
            HPT_UC  = 1UL << 4,
            HPT_A   = 1UL << 5,
            HPT_D   = 1UL << 6,
            HPT_S   = 1UL << 7,
            HPT_G   = 1UL << 8,
            HPT_NX  = 0,

            PTE_P   = HPT_P,
            PTE_S   = HPT_S,
            PTE_N   = HPT_A | HPT_U | HPT_W | HPT_P,
        };

        ALWAYS_INLINE
        inline Paddr addr() const { return static_cast<Paddr>(val) & ~PAGE_MASK; }

        ALWAYS_INLINE
        static inline mword hw_attr (mword a) { return a ? a | HPT_D | HPT_A | HPT_U | HPT_P : 0; }

        ALWAYS_INLINE
        static inline mword current()
        {
            mword addr;
            asm volatile ("mov %%cr3, %0" : "=r" (addr));
            return addr;
        }

        ALWAYS_INLINE
        inline void make_current()
        {
            asm volatile ("mov %0, %%cr3" : : "r" (val) : "memory");
        }

        ALWAYS_INLINE
        inline bool mark (Hpt *e, mword bits)
        {
            return EXPECT_TRUE ((e->val & bits) == bits) || User::cmp_swap (&val, e->val, e->val | bits) == ~0UL;
        }

        bool sync_from (Hpt, mword);

        size_t sync_master (mword virt);
        void sync_master_range (mword s_addr, mword e_addr);

        Paddr replace (mword, mword);

        static void *remap (Paddr phys);
};

class Hptp : public Hpt
{
    public:
        ALWAYS_INLINE
        inline explicit Hptp (mword v = 0) { val = v; }
};
