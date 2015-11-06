/* 
   SDS, a bridge single-suit double-dummy quick-trick solver.

   Copyright (C) 2015 by Soren Hein.

   See LICENSE and README.
*/


#ifndef SDS_STATS_H
#define SDS_STATS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void InitTimer();

void SetTimerName(const char * name);

void StartTimer();

void EndTimer();

void PrintTimer();

void InitTimerList();

void StartTimerNo(const int n);

void EndTimerNo(const int n);

void EndTimerNoAndComp(
  const int n, 
  const int pred);

void PrintTimerList();

void InitCounter();

void PrintCounter();

#endif
