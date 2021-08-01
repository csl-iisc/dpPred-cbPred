#ifndef CACHE_SET_LRU_H
#define CACHE_SET_LRU_H

#include "cache_set.h"

class CacheSetInfoLRU : public CacheSetInfo
{
   public:
      CacheSetInfoLRU(String name, String cfgname, core_id_t core_id, UInt32 associativity, UInt8 num_attempts);
      virtual ~CacheSetInfoLRU();
      void increment(UInt32 index)
      {
         LOG_ASSERT_ERROR(index < m_associativity, "Index(%d) >= Associativity(%d)", index, m_associativity);
         ++m_access[index];
      }
      void incrementAttempt(UInt8 attempt)
      {
         if (m_attempts)
            ++m_attempts[attempt];
         else
            LOG_ASSERT_ERROR(attempt == 0, "No place to store attempt# histogram but attempt != 0");
      }
  // private:
      const UInt32 m_associativity;
    private:
      UInt64* m_access;
      UInt64* m_attempts;
};

class CacheSetLRU : public CacheSet
{
   public:
      CacheSetLRU(CacheBase::cache_t cache_type,
            UInt32 associativity, UInt32 blocksize, CacheSetInfoLRU* set_info, UInt8 num_attempts);
      virtual ~CacheSetLRU();

      virtual UInt32 getReplacementIndex(CacheCntlr *cntlr, bool LLC = false, bool L2TLB = false);
      virtual UInt32 getMyReplacementIndex(CacheCntlr *cntlr);
      void updateReplacementIndex(UInt32 accessed_index);
      virtual  void moveToLRU(UInt32 accessed_index);
      void incrementSetCount();
      void modifySetCounters(IntPtr addr, std::map<IntPtr, std::pair<uint64_t, IntPtr>> &phy_table);

   protected:
      const UInt8 m_num_attempts;
      UInt8* m_lru_bits;
      CacheSetInfoLRU* m_set_info;
      UInt64  m_change;

      void moveToMRU(UInt32 accessed_index);
     // void moveToLRU(UInt32 accessed_index);

};

#endif /* CACHE_SET_LRU_H */
