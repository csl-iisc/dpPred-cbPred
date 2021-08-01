#ifndef TLB_H
#define TLB_H

#include "fixed_types.h"
#include "cache.h"
#include "cache_cntlr.h"
#include <bits/stdc++.h>

namespace ParametricDramDirectoryMSI
{
   class TLB
   {
      private:
         static const UInt32 SIM_PAGE_SHIFT = 12; // 4KB
         static const IntPtr SIM_PAGE_SIZE = (1L << SIM_PAGE_SHIFT);
         static const IntPtr SIM_PAGE_MASK = ~(SIM_PAGE_SIZE - 1);

         UInt32 m_size;
         UInt32 m_associativity;
         Cache m_cache;
         CacheCntlr *m_last_level;

         TLB *m_next_level;

         UInt64 m_access, m_miss, m_alloc, m_bypass;
	 UInt32 m_conf_counter = 2;
      public:
         std::map<IntPtr, std::map<IntPtr, uint64_t> > hitCounter;
	 std::map<IntPtr, uint64_t> curHit;
         TLB(String name, String cfgname, core_id_t core_id, UInt32 num_entries, UInt32 associativity, TLB *next_level, UInt32 conf_count = 2);
         bool lookup(IntPtr address, SubsecondTime now, bool isIfetch, MemoryManager *mptr, bool allocate_on_miss = true);
         void allocate(IntPtr address, SubsecondTime now);
	 void setDeadBit (IntPtr address);
         UInt32 give_size();
         void setL3Controller(CacheCntlr*);
         void shadow_table_insert (IntPtr vpn);
         bool shadow_table_search (IntPtr vpn);
         IntPtr findHash (IntPtr ev_vpn, uint64_t bits);
   };
}

#endif // TLB_H
