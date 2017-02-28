#ifndef STATUS_H
#define STATUS_H

#ifdef STATUS
typedef enum{
  ST_alr = 0,
  ST_toobig = 1,
  ST_reg = 2,
  ST_coll = 3,
  ST_sat = 4,
  ST_proc = 5,
  ST_ext = 6,
  ST_rec = 7,
  ST_eal = 8,
  ST_csp = 9,
  ST_by0 = 10,
  ST_by1 = 11,
  ST_by2 = 12,
  ST_by3 = 13,
  ST_now = 14,
  STNUM
} ST_t;
static unsigned cnt[STNUM];
static char const* stn[STNUM]={
  "alr   ",
  "toobig",
  "reg   ",
  "coll  ",
  "sat   ",
  "proc  ",
  "ext   ",
  "rec   ",
  "eal   ",
  "csp   ",
  "by0   ",
  "by1   ",
  "by2   ",
  "by3   ",
  "now   "
};

static void stcnt(ST_t x){
  ++cnt[x];
}
#else
#define stcnt(x)
#endif

#endif
