#ifndef __PR_L1_CACHE_BLOCK_INFO_H__
#define __PR_L1_CACHE_BLOCK_INFO_H__

#include "cache_state.h"
#include "cache_block_info.h"

class PrL1CacheBlockInfo : public CacheBlockInfo
{
//   private:
    bool deadbit;
   public:
      PrL1CacheBlockInfo( IntPtr tag = ~0,
            CacheState::cstate_t cstate = CacheState::INVALID, bool d = 0):
         CacheBlockInfo(tag, cstate)
      {
          deadbit = d;
      }
         bool getDead() { return (deadbit==1);}
         void setDead(int val) { deadbit = val;}

void cloneAIPTLB(PrL1CacheBlockInfo* c) {
//        this->c = c->getC();
        deadbit = c->getDead();
  //      cMaxPresent = c->getCMaxPresent();
    //    cMaxPast = c->getCMaxPast();
      //  conf = c->getConf();
}



      ~PrL1CacheBlockInfo() {}
};
#endif /* __PR_L1_CACHE_BLOCK_INFO_H__ */
