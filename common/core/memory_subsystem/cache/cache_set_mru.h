#ifndef CACHE_SET_MRU_H
#define CACHE_SET_MRU_H

#include "cache_set.h"

class CacheSetMRU : public CacheSet
{
   public:
      CacheSetMRU(CacheBase::cache_t cache_type,
            UInt32 associativity, UInt32 blocksize);
      ~CacheSetMRU();

      UInt32 getReplacementIndex(CacheCntlr *cntlr, bool DLRU = false, bool isBypassed = false);
      void updateReplacementIndex(UInt32 accessed_index);

   private:
      UInt8* m_lru_bits;
};

#endif /* CACHE_SET_MRU_H */
