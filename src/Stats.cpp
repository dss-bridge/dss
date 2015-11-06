/* 
   SDS, a bridge-suit single double-dummy quick-trick solver.

   Copyright (C) 2015 by Soren Hein.

   See LICENSE and README.
*/


#include <iostream>
#include <iomanip>
#include <string>
#include <stdexcept>

using namespace std;

#include "portab.h"
#include "Stats.h"

#define NUM_TIMERS 2000
#define COUNTER_SLOTS 200

#include <time.h>

#ifdef _WIN32
LARGE_INTEGER timerFreq;
LARGE_INTEGER timerUser0;
LARGE_INTEGER timerUser1;
LARGE_INTEGER timerListUser0[NUM_TIMERS];
LARGE_INTEGER timerListUser1[NUM_TIMERS];
#else
#include <sys/time.h>
int timevalDiff(timeval x, timeval y);

timeval timerUser0;
timeval timerUser1;
timeval timerListUser0[NUM_TIMERS];
timeval timerListUser1[NUM_TIMERS];
#endif

clock_t timerSys0;
clock_t timerSys1;
clock_t timerListSys0[NUM_TIMERS];
clock_t timerListSys1[NUM_TIMERS];

int timerCount;
int timerListCount[NUM_TIMERS];

int timerNameSet;

char timerName[80];

long long timerUserCum;
long long timerSysCum;
long long timerListUserCum[NUM_TIMERS];
long long timerListSysCum[NUM_TIMERS];
long long predError;
long long predAbsError;


void InitTimer()
{
  timerCount = 0;
  timerUserCum = 0;
  timerSysCum = 0;
  timerNameSet = 0;

  predError = 0;
  predAbsError = 0;
}


void SetTimerName(const char * name)
{
  strcpy(timerName, name);
  timerNameSet = 1;
}


void StartTimer()
{
  timerCount++;
  timerSys0 = clock();

#ifdef _WIN32
  QueryPerformanceCounter(&timerUser0);
#else
  gettimeofday(&timerUser0, nullptr);
#endif
}


void EndTimer()
{
  timerSys1 = clock();

#ifdef _WIN32
  // To get "real" seconds we would have to divide by
  // timerFreq.QuadPart which needs to be initialized.
  QueryPerformanceCounter(&timerUser1);
  int timeUser = static_cast<int>
                 ((timerUser1.QuadPart - timerUser0.QuadPart));
#else
  gettimeofday(&timerUser1, nullptr);
  int timeUser = timevalDiff(timerUser1, timerUser0);
#endif

  timerUserCum += timeUser;

  timerSysCum += static_cast<int>((1000 * (timerSys1 - timerSys0)) /
                 static_cast<double>(CLOCKS_PER_SEC));
}


void PrintTimer()
{
  if (timerNameSet)
    cout << setw(19) << left << "Timer name" << ": " << timerName << "\n";

  cout << setw(19) << left << "Number of calls" << ": " << 
    timerCount << "\n";
  if (timerCount == 0) return;

  if (timerUserCum == 0)
    cout << setw(19) << left << "User time" << ": zero" << "\n";
  else
  {
    cout << setw(19) << left << "User time/ticks" << ": " << 
      timerUserCum << "\n";
    cout << setw(19) << left << "User per call" << right << ": " <<
      setw(10) << std::fixed << std::setprecision(2) <<
      static_cast<float>(timerUserCum / timerCount) << "\n";
  }

  if (timerSysCum == 0)
    cout << setw(19) << left << "Sys time" << ": zero" << "\n";
  else
  {
    cout << setw(19) << left << "Sys time/ticks" << ": " << 
      timerSysCum << "\n";
    cout << setw(19) << left << "Sys per call" << right << ": " <<
      setw(10) << std::fixed << std::setprecision(2) <<
      static_cast<float>(timerSysCum / timerCount) << "\n";
    cout << setw(19) << left << "Ratio" << right << ": " <<
      setw(10) << std::fixed << std::setprecision(2) <<
      static_cast<float>(timerSysCum / timerUserCum) << "\n";
  }
  cout << "\n";
}


void InitTimerList()
{
  for (int i = 0; i < NUM_TIMERS; i++)
  {
    timerListCount [i] = 0;
    timerListUserCum[i] = 0;
    timerListSysCum [i] = 0;
  }
}


void StartTimerNo(const int no)
{
  timerListCount[no]++;
  timerListSys0[no] = clock();

#ifdef _WIN32
  QueryPerformanceCounter(&timerListUser0[no]);
#else
  gettimeofday(&timerListUser0[no], nullptr);
#endif
}


void EndTimerNo(const int no)
{
  timerListSys1[no] = clock();

#ifdef _WIN32
  QueryPerformanceCounter(&timerListUser1[no]);
  int timeUser = static_cast<int>
    ((timerListUser1[no].QuadPart - timerListUser0[no].QuadPart));
#else
  gettimeofday(&timerListUser1[no], nullptr);
  int timeUser = timevalDiff(timerListUser1[no], timerListUser0[no]);
#endif

  timerListUserCum[no] += static_cast<long long>(timeUser);

  timerListSysCum[no] +=
    static_cast<long long>((1000 *
      (timerListSys1[no] - timerListSys0[no])) /
      static_cast<double>(CLOCKS_PER_SEC));
}


void EndTimerNoAndComp(
  const int no, 
  const int pred)
{
  timerListSys1[no] = clock();

#ifdef _WIN32
  QueryPerformanceCounter(&timerListUser1[no]);
  int timeUser = static_cast<int>
    ((timerListUser1[no].QuadPart - timerListUser0[no].QuadPart));
#else
  gettimeofday(&timerListUser1[no], nullptr);
  int timeUser = timevalDiff(timerListUser1[no], timerListUser0[no]);
#endif

  timerListUserCum[no] += static_cast<long long>(timeUser);

  predError += timeUser - pred;

  predAbsError += (timeUser >= pred ?
                   timeUser - pred : pred - timeUser);

  timerListSysCum[no] +=
    static_cast<long long>(
      (1000 * (timerListSys1[no] - timerListSys0[no])) /
      static_cast<double>(CLOCKS_PER_SEC));
}


void PrintTimerList()
{
  cout << setw(5) << "n" <<
    setw(11) << "Number" <<
    setw(13) << "User ticks" <<
    setw(11) << "Avg" <<
    setw(11) << "Syst time" << "\n";

  int totNum = 0;
  for (int no = 0; no < NUM_TIMERS; no++)
  {
    if (timerListCount[no] == 0)
      continue;

    totNum += timerListCount[no];

    double avg = static_cast<double>(timerListUserCum[no]) /
                 static_cast<double>(timerListCount[no]);

    // For some reason I have trouble when putting it on one line...
    cout << setw(5) << no <<
      setw(11) << timerListCount[no] <<
      setw(13) << timerListUserCum[no] << 
      setw(11) << std::fixed << std::setprecision(2) << avg <<
      setw(10) << timerListSysCum[no] << "\n";
  }
  cout << "\n";
  if (predError != 0)
  {
    cout << "Total number " << setw(10) << totNum << "\n";
    cout << "Prediction mean " <<
      setw(10) << std::fixed << std::setprecision(0) <<
        static_cast<double>(predError) /
        static_cast<double>(totNum) << "\n";
    cout << "Prediction abs mean %10.0f" <<
      setw(10) << std::fixed << std::setprecision(0) <<
        static_cast<double>(predAbsError) /
        static_cast<double>(totNum) << "\n";
    cout << "\n";
  }
}


#ifndef _WIN32
int timevalDiff(timeval x, timeval y)
{
  /* Elapsed time, x-y, in milliseconds */
  return 1000 * (x.tv_sec - y.tv_sec )
         + (x.tv_usec - y.tv_usec) / 1000;
}
#endif


long long counter[COUNTER_SLOTS];

void InitCounter()
{
  for (int i = 0; i < COUNTER_SLOTS; i++)
    counter[i] = 0;
}


void PrintCounter()
{
  for (int i = 0; i < COUNTER_SLOTS; i++)
  {
    if (counter[i])
      cout << setw(8) << i << setw(12) << counter[i] << "\n";
  }
  cout << "\n";
}

