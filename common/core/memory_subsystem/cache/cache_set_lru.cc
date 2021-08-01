#include "cache_set_lru.h"
#include "cache_block_info.h"
#include "pr_l1_cache_block_info.h"
#include "shared_cache_block_info.h"
#include "log.h"
#include "stats.h"

// Implements LRU replacement, optionally augmented with Query-Based Selection [Jaleel et al., MICRO'10]

extern bool isDLRU;
extern UInt32 miss_index;
extern UInt64 q1;


extern std::map<IntPtr, uint64_t> live_distance;
UInt32 current_Index;
CacheSetLRU* cur_obj;
CacheSetLRU* cur_obj1;

CacheSetLRU::CacheSetLRU(
      CacheBase::cache_t cache_type,
      UInt32 associativity, UInt32 blocksize, CacheSetInfoLRU* set_info, UInt8 num_attempts)
   : CacheSet(cache_type, associativity, blocksize)
   , m_num_attempts(num_attempts)
   , m_set_info(set_info)
   , m_change(0)
{
   m_lru_bits = new UInt8[m_associativity];
   for (UInt32 i = 0; i < m_associativity; i++)
      m_lru_bits[i] = i;


}



CacheSetLRU::~CacheSetLRU()
{
   delete [] m_lru_bits;
}


void
CacheSetLRU::incrementSetCount()
{
   for (UInt32 i = 0; i < m_associativity; i++)
   {
     ((SharedCacheBlockInfo*)m_cache_block_info_array[i])->incC();
   }
}

void
CacheSetLRU::modifySetCounters(IntPtr addr, std::map<IntPtr, std::pair<uint64_t, IntPtr>> &phy_table)
{
   int count = 0;
  
   IntPtr temp_tag = addr >> 12;
   for (UInt32 i = 0; i < m_associativity; i++)
   {
     if(((SharedCacheBlockInfo*)m_cache_block_info_array[i])->getDead())
     {
         phy_table[temp_tag].first--;
         count++;
     }
   }
}



UInt32
CacheSetLRU::getReplacementIndex(CacheCntlr *cntlr, bool LLC, bool L2TLB)
{
   cur_obj = this;
   
   // First try to find an invalid block
   for (UInt32 i = 0; i < m_associativity; i++)
   {
      if (!m_cache_block_info_array[i]->isValid())
      {
         // Mark our newly-inserted line as most-recently used
         moveToMRU(i);
         current_Index = i;
         return i;
      }
   }  

   UInt32 index = 0;
   // Make m_num_attemps attempts at evicting the block at LRU position
   for(UInt8 attempt = 0; attempt < m_num_attempts; ++attempt)
   {
      UInt8 max_bits = 0;
      for (UInt32 i = 0; i < m_associativity; i++)
      {
         if (m_lru_bits[i] > max_bits && isValidReplacement(i))
         {
            index = i;
            max_bits = m_lru_bits[i];
         }
      }
      LOG_ASSERT_ERROR(index < m_associativity, "Error Finding LRU bits");

      bool qbs_reject = false;
      if (attempt < m_num_attempts - 1)
      {
         LOG_ASSERT_ERROR(cntlr != NULL, "CacheCntlr == NULL, QBS can only be used when cntlr is passed in");
         qbs_reject = cntlr->isInLowerLevelCache(m_cache_block_info_array[index]);
      }

      if (qbs_reject)
      {
         // Block is contained in lower-level cache, and we have more tries remaining.
         // Move this block to MRU and try again
         moveToMRU(index);
         cntlr->incrementQBSLookupCost();
         continue;
      }
      else
      {
         moveToMRU(index);
         m_set_info->incrementAttempt(attempt);
         current_Index = index;
      }
   }


      UInt32 newIndex = index;
      UInt32 newDist = m_associativity + 1;
   return newIndex;


   LOG_PRINT_ERROR("Should not reach here");
}

UInt32
CacheSetLRU::getMyReplacementIndex(CacheCntlr *cntlr)
{
   // First try to find an invalid block
   for (UInt32 i = 0; i < m_associativity; i++)
   {
      if (!m_cache_block_info_array[i]->isValid())
      {
         // Mark our newly-inserted line as most-recently used
         moveToMRU(i);
         return i;
      }
   }

   // Make m_num_attemps attempts at evicting the block at LRU position
   for(UInt8 attempt = 0; attempt < m_num_attempts; ++attempt)
   {
      UInt32 index = 0;
      UInt8 max_bits = 0;
      for (UInt32 i = 0; i < m_associativity; i++)
      {
         if (m_lru_bits[i] > max_bits && isValidReplacement(i))
         {
            index = i;
            max_bits = m_lru_bits[i];
         }
      }
      LOG_ASSERT_ERROR(index < m_associativity, "Error Finding LRU bits");

      bool qbs_reject = false;
      if (attempt < m_num_attempts - 1)
      {
         LOG_ASSERT_ERROR(cntlr != NULL, "CacheCntlr == NULL, QBS can only be used when cntlr is passed in");
         qbs_reject = cntlr->isInLowerLevelCache(m_cache_block_info_array[index]);
      }

      if (qbs_reject)
      {
         // Block is contained in lower-level cache, and we have more tries remaining.
         // Move this block to MRU and try again
         moveToMRU(index);
         cntlr->incrementQBSLookupCost();
         continue;
      }
      else
      {
         // Mark our newly-inserted line as least-recently used
         moveToLRU(index);
         m_set_info->incrementAttempt(attempt);
         return index;
      }
   }

   LOG_PRINT_ERROR("Should not reach here");
}






void
CacheSetLRU::updateReplacementIndex(UInt32 accessed_index)
{
   m_set_info->increment(m_lru_bits[accessed_index]);
   current_Index = accessed_index;
   cur_obj = this;
   moveToMRU(accessed_index);
}

void
CacheSetLRU::moveToMRU(UInt32 accessed_index)
{
   //std::cout<<"IN MRU"<<std::endl;
   for (UInt32 i = 0; i < m_associativity; i++)
   {
      if (m_lru_bits[i] < m_lru_bits[accessed_index])
         m_lru_bits[i] ++;
   }
   m_lru_bits[accessed_index] = 0;
}



void
CacheSetLRU::moveToLRU(UInt32 accessed_index)
{
//std::cout<<"IN LRU" <<std::endl;
   for (UInt32 i = 0; i < m_associativity; i++)
   {
      if (m_lru_bits[i] > m_lru_bits[accessed_index])
         m_lru_bits[i] --;
   }
   m_lru_bits[accessed_index] = m_associativity - 1;
}


CacheSetInfoLRU::CacheSetInfoLRU(String name, String cfgname, core_id_t core_id, UInt32 associativity, UInt8 num_attempts)
   : m_associativity(associativity)
   , m_attempts(NULL)
{
   m_access = new UInt64[m_associativity];
   for(UInt32 i = 0; i < m_associativity; ++i)
   {
      m_access[i] = 0;
      registerStatsMetric(name, core_id, String("access-mru-")+itostr(i), &m_access[i]);
   }

   if (num_attempts > 1)
   {
      m_attempts = new UInt64[num_attempts];
      for(UInt32 i = 0; i < num_attempts; ++i)
      {
         m_attempts[i] = 0;
         registerStatsMetric(name, core_id, String("qbs-attempt-")+itostr(i), &m_attempts[i]);
      }
   }
};

CacheSetInfoLRU::~CacheSetInfoLRU()
{
   delete [] m_access;
   if (m_attempts)
      delete [] m_attempts;
}
