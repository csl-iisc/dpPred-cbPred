#include "tlb.h"
#include "stats.h"
#include "memory_manager.h"
#include <bits/stdc++.h>
#include "cache/cache_set_lru.h"

namespace ParametricDramDirectoryMSI
{

std::deque<IntPtr> recentPFN;

IntPtr lastPC;

std::deque<IntPtr> shadow_table;
uint64_t shadow_table_size = 2;

std::map<IntPtr, IntPtr> insert_pc;

TLB::TLB(String name, String cfgname, core_id_t core_id, UInt32 num_entries, UInt32 associativity, TLB *next_level, UInt32 conf_count)
   : m_size(num_entries)
   , m_associativity(associativity)
   , m_cache(name + "_cache", cfgname, core_id, num_entries / associativity, associativity, SIM_PAGE_SIZE, "lru", CacheBase::PR_L1_CACHE)
   , m_next_level(next_level)
   , m_access(0)
   , m_miss(0)
   , m_alloc(0)
   , m_bypass(0)
   , m_conf_counter(conf_count)
{
   LOG_ASSERT_ERROR((num_entries / associativity) * associativity == num_entries, "Invalid TLB configuration: num_entries(%d) must be a multiple of the associativity(%d)", num_entries, associativity);

   registerStatsMetric(name, core_id, "access", &m_access);
   registerStatsMetric(name, core_id, "miss", &m_miss);
   registerStatsMetric(name, core_id, "bypass", &m_bypass);
   registerStatsMetric(name, core_id, "allocs", &m_alloc);
}

UInt32 TLB::give_size()
{
   return m_size;
}

void TLB::setL3Controller(CacheCntlr *last_level)
{
   m_last_level = last_level;
}

void 
TLB::setDeadBit (IntPtr address){
  
   m_cache.setPrDeadBit (address);
}

bool
TLB::lookup(IntPtr address, SubsecondTime now, bool isIfetch, MemoryManager* mptr, bool allocate_on_miss)
{
   IntPtr temp = address & 0xfffffffffffff000;
   bool hit = m_cache.accessSingleLine(address, Cache::LOAD, NULL, 0, now, true);
   m_access++;

   if (isIfetch)
       lastPC = address;

   if (hit) {
       if (give_size() == 1024) {
           curHit[temp]++;
       }
       return true;
   }
   m_miss++;

   SubsecondTime temp1;

   // L1 TLB cycles.
   temp1.setTime(3007512);   

   if (m_next_level)
   { 
      mptr->incrElapsedTime(temp1, ShmemPerfModel::_USER_THREAD);
      hit = m_next_level->lookup(address, now, false, mptr, true /* no allocation */);
   }
   if (allocate_on_miss)
   {
         allocate(address, now);
   }
   return hit;
}

IntPtr
TLB::findHash (IntPtr ev_vpn, uint64_t bits)
{
   IntPtr last_part = ev_vpn;
   IntPtr lph = 0;
   int max_iter = 32 / bits;
   for (int i = 0; i < max_iter; ++i) {
        lph ^= (last_part % (1 << bits));
        last_part >>= bits;
   }
   return lph;
}

void
addRecentPFN(IntPtr addr) {
	addr >>= 17;
	if (recentPFN.size() < 8) {
		recentPFN.push_back(addr);
	} else {
		recentPFN.pop_front();
		recentPFN.push_back(addr);
	}
}

bool
TLB::shadow_table_search (IntPtr vpn)
{
    bool res = false;
    for (uint64_t i = 0; i < shadow_table.size(); i++)
    {
        if (shadow_table[i] == vpn)
        {
            return true;
        }
    }
    return res;
}

void
TLB::shadow_table_insert (IntPtr vpn)
{
    if (shadow_table.size() == shadow_table_size)
    {
        shadow_table.pop_front();
    }
    shadow_table.push_back(vpn);
}

void
TLB::allocate(IntPtr address, SubsecondTime now)
{
   if (give_size() == 1024)
        insert_pc[(address & 0xfffffffffffff000)] = lastPC;

   IntPtr temp_vpn = address & 0xfffffffffffff000;
   IntPtr temp_hash_vpn = findHash ((address & 0xfffffffffffff000), 4);
   IntPtr temp_hash_pc =  findHash (lastPC, 6);
   ++m_alloc;

   if (give_size() == 1024)
   {
       bool res = shadow_table_search (temp_vpn);
       if (res == true)
       {
           for (int i = 0;i < 64;i++)
           {
               hitCounter[temp_hash_vpn][i]= 0;
           }
       }
   }

   if (give_size() == 1024 && hitCounter[temp_hash_vpn][temp_hash_pc] > 6) {
        ++m_bypass;
        shadow_table_insert (temp_vpn);
	addRecentPFN(address & 0xfffffffffffff000);
        return;
   } else if (give_size() == 1024) {
        curHit[(address & 0xfffffffffffff000)] = 0;
   }
   bool eviction;
   IntPtr evict_addr;
   CacheBlockInfo evict_block_info;
   m_cache.insertSingleLine(address, NULL, &eviction, &evict_addr, &evict_block_info, NULL, now, NULL, false);
   if (eviction && give_size() == 1024) {

        IntPtr ev_vpn_hash = findHash ((evict_addr & 0xfffffffffffff000), 4);
        IntPtr ev_pc_hash = findHash ((insert_pc[(evict_addr & 0xfffffffffffff000)]), 6);
        if (!curHit[(evict_addr & 0xfffffffffffff000)]){
                hitCounter[ev_vpn_hash][ev_pc_hash]++;
        } else {
                hitCounter[ev_vpn_hash][ev_pc_hash] = 0;
        }
   }
}

}

