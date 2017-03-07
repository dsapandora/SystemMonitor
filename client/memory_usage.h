#ifdef _WIN32
#include "windows.h"
#else
#include "sys/sysinfo.h"
#include "sys/types.h"

struct sysinfo memInfo;
#endif

double memory() {
#if defined(WIN32)
  PROCESS_MEMORY_COUNTERS_EX pmc;
  GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
  SIZE_T virtualMemUsedByMe = pmc.PrivateUsage;
  DWORDLONG totalPhysMem = memInfo.ullTotalPhys;
  DWORDLONG physMemUsed = memInfo.ullTotalPhys - memInfo.ullAvailPhys;
  return ((double)physMemUsed / (double)totalPhysMem) * 100.00;
#else
  sysinfo(&memInfo);
  long long totalVirtualMem = memInfo.totalram;
  // Add other values in next statement to avoid int overflow on right hand
  // side...
  totalVirtualMem += memInfo.totalswap;
  totalVirtualMem *= memInfo.mem_unit;
  long long virtualMemUsed = memInfo.totalram - memInfo.freeram;
  // Add other values in next statement to avoid int overflow on right hand
  // side...
  virtualMemUsed += memInfo.totalswap - memInfo.freeswap;
  virtualMemUsed *= memInfo.mem_unit;
  return ((double)virtualMemUsed / (double)totalVirtualMem) * 100;
#endif
}
