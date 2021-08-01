#pragma once

#include "cache_state.h"
#include "cache_block_info.h"
#include "mem_component.h"
//#include "cache_set.h"
// Define to enable tracking of which previous-level caches share each cache line
// Currently this is only used for asserts (makeing sure no non-sharers send use evictions)
// but it takes up space, especially when sharing caches with a lot of cores
//#define ENABLE_TRACK_SHARING_PREVCACHES

#ifdef ENABLE_TRACK_SHARING_PREVCACHES
#include <bitset>

const UInt32 MAX_NUM_PREVCACHES = 8;
typedef std::bitset<MAX_NUM_PREVCACHES> CacheSharersType;
typedef UInt8 PrevCacheIndex; // Should hold an integer up to MAX_NUM_PREVCACHES
#endif

class SharedCacheBlockInfo : public CacheBlockInfo
{
   private:
      #ifdef ENABLE_TRACK_SHARING_PREVCACHES
      CacheSharersType m_cached_locs;
      #endif
   int deadbit;
   int conf;
   int cMaxPresent;
   int cMaxPast;
   int c;
//   int set_index;
   public:
//   int d;
      SharedCacheBlockInfo( IntPtr tag = ~0,
            CacheState::cstate_t cstate = CacheState::INVALID, int deadbit = 0, int conf = 0, int cMaxPresent = 0, int cMaxPast = 0, int c = 0)
         : CacheBlockInfo(tag, cstate)
         #ifdef ENABLE_TRACK_SHARING_PREVCACHES
         , m_cached_locs()
         #endif
      {
         this->deadbit = deadbit;  
         this->conf = conf;
         this->cMaxPresent = cMaxPresent;
         this->cMaxPast = cMaxPast;
      }
      int getDead() { return deadbit;}
      void setDead(int val) { deadbit = val;}
      
  //    void setSet(int s) { set_index = s; }
  
      int getC ()  { return c;}
      int getCMaxPresent () { return cMaxPresent; }
      int getCMaxPast ()  { return cMaxPast; }
      int getConf () { return conf;}
  

      void zeroC () { c = 0; }
      void incC () { c++; }

      void zeroCMaxPresent () { cMaxPresent = 0; }
      void setCMaxPresent ()  { cMaxPresent = (cMaxPresent > c) ? cMaxPresent : c ; }

      void setCMaxPast (int past) { cMaxPast = past; } 

      void setConf( int val) { conf = val;}
  
      ~SharedCacheBlockInfo() {}

      #ifdef ENABLE_TRACK_SHARING_PREVCACHES
      PrevCacheIndex getCachedLoc();
      bool hasCachedLoc() { return m_cached_locs.any(); }
      void setCachedLoc(PrevCacheIndex idx);
      void clearCachedLoc(PrevCacheIndex idx);
    //  int getDead() { return deadbit;}
  //    void setDead(int val) { deadbit = val;}
      CacheSharersType getCachedLocs() { return m_cached_locs; }
      #endif

      void invalidate();
      void clone(CacheBlockInfo* cache_block_info);
      void cloneAIP(SharedCacheBlockInfo* c);
};
