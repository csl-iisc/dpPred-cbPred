#ifndef CACHE_SET_NMRU_H
#define CACHE_SET_NMRU_H

#include "cache_set.h"

class CacheSetNMRU : public CacheSet
{
   public:
      CacheSetNMRU(CacheBase::cache_t cache_type,
            UInt32 associativity, UInt32 blocksize);
      ~CacheSetNMRU();

      UInt32 getReplacementIndex(CacheCntlr *cntlr, bool DLRU = false, bool isBypassed = false);
      void updateReplacementIndex(UInt32 accessed_index);

   private:
      UInt8* m_lru_bits;
      UInt8  m_replacement_pointer;
};

#endif /* CACHE_SET_NMRU_H */
