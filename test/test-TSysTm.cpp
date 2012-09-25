#include <gtest/gtest.h>

#include "Snap.h"

#if 0
void testtime() {
  time_t t;
  time_t t1;
  struct tm tms;
  struct tm tms1;
  uint64 ft;
  uint64 ft1;
  TTm Tm;

  printf("timezone %ld\n",timezone);
  printf("daylight %d\n",daylight);

  tms.tm_year = 2012 - 1900;
  tms.tm_mon = 8;
  tms.tm_mday = 10;
  tms.tm_hour = 22;
  tms.tm_min = 59;
  tms.tm_sec = 57;
  t = timegm(&tms);

  printf("t %d\n",(int) t);

  gmtime_r(&t, &tms1);
  t1 = timegm(&tms1);
  printf("t1 %d\n",(int) t1);

  ft = Epoch2Ft(t);
  printf("ft %ld\n",ft);

  Tm = TTm(1900+tms.tm_year, tms.tm_mon, tms.tm_mday, tms.tm_wday,
   tms.tm_hour, tms.tm_min, tms.tm_sec, 0);

  ft1 = TSysTm::GetMSecsFromTm(Tm);
  printf("ft1 %ld\n",ft1);
}
#endif

// Test the conversion between Windows FILETIME and Unix epoch time
TEST(TSysTm, DISABLED_Conversion) {
#if 0
  time_t t;
  time_t t1;
#endif
  uint64 ft;
  uint64 ft1;
  uint64 ms;
  uint64 ms1;
  TTm Tm;
  TTm Tm1;
  TTm Tm2;

  ft = 129917915977151808LL;
  ft1 = ft;
#if 0
  t = Ft2Epoch(ft);
  ft1 = Epoch2Ft(t);
  t1 = Ft2Epoch(ft1);

  EXPECT_EQ(1,(t == t1));
#endif
  EXPECT_EQ(1,(ft/10000000) == (ft1/10000000));

  Tm = TSysTm::GetTmFromMSecs(ft1/10000);
  EXPECT_EQ(2012,Tm.GetYear());
  EXPECT_EQ(8,Tm.GetMonth());
  EXPECT_EQ(10,Tm.GetDay());
  EXPECT_EQ(22,Tm.GetHour());
  EXPECT_EQ(59,Tm.GetMin());
  EXPECT_EQ(57,Tm.GetSec());

  ms = TSysTm::GetMSecsFromTm(Tm);
  EXPECT_EQ(ft1,ms*10000);

  Tm1 = TSysTm::GetLocTmFromUniTm(Tm);
  Tm2 = TSysTm::GetUniTmFromLocTm(Tm1);

  ms1 = TSysTm::GetMSecsFromTm(Tm2);
  EXPECT_EQ(1,ms == ms1);
}

// Test Sleep
TEST(TSysTm, Sleep) {
  time_t t;
  time_t t1;
#if 0
  int r;
#endif

  t = time(0);
  // r = TSysProc::Sleep(2*1000);
  TSysProc::Sleep(2*1000);
  t1 = time(0);

#if 0
  EXPECT_EQ(0,r);
#endif
  EXPECT_EQ(2,t1-t);
}

/*
Test output from another program from Windows:
Windows time: 09/10/2012  15:59:57
Mon Sep 10 15:59:57 2012

epoch 1347317997
FILETIME IN 129917915977151808, OUT 129917915970000000

*/


