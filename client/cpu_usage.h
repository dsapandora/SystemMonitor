#ifdef _WIN32
#include "TCHAR.h"
#include "pdh.h"
#include "windows.h"

static PDH_HQUERY cpuQuery;
static PDH_HCOUNTER cpuTotal;
#else
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

static unsigned long long lastTotalUser, lastTotalUserLow, lastTotalSys,
    lastTotalIdle;
#endif

double cpu() {
#if defined(WIN32)
  PDH_FMT_COUNTERVALUE counterVal;
  PdhCollectQueryData(cpuQuery);
  PdhGetFormattedCounterValue(cpuTotal, PDH_FMT_DOUBLE, NULL, &counterVal);
  return counterVal.doubleValue;
#else
  double percent;
  FILE *file;
  unsigned long long totalUser, totalUserLow, totalSys, totalIdle, total;
  file = fopen("/proc/stat", "r");
  fscanf(file, "cpu %llu %llu %llu %llu", &totalUser, &totalUserLow, &totalSys,
         &totalIdle);
  fclose(file);

  if (totalUser < lastTotalUser || totalUserLow < lastTotalUserLow ||
      totalSys < lastTotalSys || totalIdle < lastTotalIdle) {
    percent = -1.0;
  } else {
    total = (totalUser - lastTotalUser) + (totalUserLow - lastTotalUserLow) +
            (totalSys - lastTotalSys);
    percent = total;
    total += (totalIdle - lastTotalIdle);
    percent /= total;
    percent *= 100;
  }
  return percent;
#endif
}
