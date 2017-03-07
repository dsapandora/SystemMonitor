#ifdef __linux__
#include <sys/sysinfo.h>
#elif defined(__APPLE__)
#include <sys/sysctl.h>
#elif defined(WIN32)
#include <Psapi.h>
#endif

int GetTotalNumProcesses() {
#if defined(__linux__)
  struct sysinfo si;
  return (sysinfo(&si) == 0) ? (int)si.procs : (int)-1;
#elif defined(__APPLE__)
  size_t length = 0;
  static const int names[] = {CTL_KERN, KERN_PROC, KERN_PROC_ALL, 0};
   return (sysctl((int *)names, ((sizeof(names)/sizeof(names[0]))-1, NULL, &length, NULL, 0) == 0) ? (int)(length/sizeof(kinfo_proc)) : (int)-1;
#elif defined(WIN32)
  DWORD aProcesses[1024], cbNeeded;
  return EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded)
             ? (cbNeeded / sizeof(DWORD))
             : -1;
#else
  return -1;
#endif
}
