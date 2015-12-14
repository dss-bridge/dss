/* 
   SDS, a bridge single-suit double-dummy quick-trick solver.

   Copyright (C) 2015 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <string>

// #include "stdio.h"
#include "assert.h"

#include "Holding.h"
#include "DefList.h"
#include "SimpleMoves.h"

using namespace std;


const int SDS_DISTLENGTH0 = 0x1;
const int SDS_DISTLENGTH1 = 0x4;
const int SDS_DISTLENGTH2 = 0x10;
const int SDS_DISTLENGTH3 = 0x40;
const int SDS_DISTLENGTH4 = 0x100;
const int SDS_DISTLENGTH5 = 0x400;
const int SDS_DISTLENGTH6 = 0x1000;
const int SDS_DISTLENGTH7 = 0x4000;
const int SDS_DISTLENGTH8 = 0x10000;
const int SDS_DISTLENGTH9 = 0x40000;
const int SDS_DISTLENGTH10 = 0x100000;

const int SDS_NUMDISTLENGTH = 11;

const int SDS_DISTLENGTH[SDS_NUMDISTLENGTH] =
{
  SDS_DISTLENGTH0,
  SDS_DISTLENGTH1,
  SDS_DISTLENGTH2,
  SDS_DISTLENGTH3,
  SDS_DISTLENGTH4,
  SDS_DISTLENGTH5,
  SDS_DISTLENGTH6,
  SDS_DISTLENGTH7,
  SDS_DISTLENGTH8,
  SDS_DISTLENGTH9,
  SDS_DISTLENGTH10
};

struct chainType
{
  int next;
  int dist;
};

chainType chain0[SDS_DISTLENGTH0];
chainType chain1[SDS_DISTLENGTH1];
chainType chain2[SDS_DISTLENGTH2];
chainType chain3[SDS_DISTLENGTH3];
chainType chain4[SDS_DISTLENGTH4];
chainType chain5[SDS_DISTLENGTH5];
chainType chain6[SDS_DISTLENGTH6];
chainType chain7[SDS_DISTLENGTH7];
chainType chain8[SDS_DISTLENGTH8];
chainType chain9[SDS_DISTLENGTH9];
chainType chain10[SDS_DISTLENGTH10];

chainType * chain[] =
{
  chain0, 
  chain1, 
  chain2, 
  chain3, 
  chain4,
  chain5, 
  chain6, 
  chain7, 
  chain8, 
  chain9, 
  chain10
};

const int SDS_NUMDIST = 0x10000;

struct distType
{
  int first;
  int last;
};


distType distToChain[SDS_NUMDISTLENGTH][SDS_NUMDIST];

extern singleType * singles[14];

void InitDist();
void SetChain();
void SetAllLowCards(
  const int& suitLength,
  const int& counter,
  unsigned moveNo,
  Holding& holding,
  const int rank,
  int histNo,
  bool &newFlag);
void PrintDist();
void PrintHistLowCards();


void InitDist()
{
  for (int d = 0; d < SDS_NUMDISTLENGTH; d++)
    for (int i = 0; i < SDS_NUMDIST; i++)
    {
      distToChain[d][i].first = -1;
      distToChain[d][i].last = -1;
    }


  for (int d = 0; d < SDS_NUMDISTLENGTH; d++)
    for (int i = 0; i < SDS_DISTLENGTH[d]; i++)
    {
      chain[d][i].next = i;
      chain[d][i].dist = -1;
    }
}


void SetChain()
{
  chain[0][0].dist = 0x0000;
  distToChain[0][0].first = 0;
  distToChain[0][0].last = 0;

  for (int d = 1; d < SDS_NUMDISTLENGTH; d++)
  {
    int step = 2*(d-1);
    for (int i = 0; i < SDS_DISTLENGTH[d-1]; i++)
    {
      for (int n = 0; n < 4; n++)
      {
        int c = (n << step) | i;
        int dNew = chain[d-1][i].dist + (1 << (4*n));
        int l = distToChain[d][dNew].last;
        if (l == -1)
          distToChain[d][dNew].first = c;
        else
        {
          chain[d][l].next = c;
          chain[d][c].next = distToChain[d][dNew].first;
        }

        chain[d][c].dist = dNew;
        distToChain[d][dNew].last = c;
      }
    }
  }
}

int histCalls = 0;
int histXes[11] = {0};
int histCycles[10000] = {0};


void SetAllLowCards(
  const int& suitLength,
  const int& counter,
  unsigned moveNo,
  Holding& holding,
  const int rank,
  int histNo,
  bool &newFlag)
{
  int numXes = suitLength - (SDS_VOID - rank);
  if (numXes > 10)
    numXes = 10;
  assert(numXes > 0);

  histCalls++;
  histXes[numXes]++;

  if (numXes == 1)
  {
    singles[suitLength][counter].moveNo = moveNo;
    UpdateHist(histNo, suitLength, counter, rank, newFlag);
    CompareRecurse(moveNo, holding, histNo);
    histCycles[1]++;

    return;
  }

  int mask = (1 << (2*numXes)) - 1;
  int first = counter & mask;
  int tops = counter ^ first;

  // cout << "sl " << suitLength << " c " << hex << counter << dec <<
    // " rank " << rank << "\n";
  int r = first;
  int num = 0;
  bool wasNew = false;
  do
  {
    int c = tops | r;

    Holding htmp;
    htmp.Set(suitLength, c);
    // htmp.Print();
    // cout << "\n";

    if (singles[suitLength][c].moveNo == 0)
    {
      singles[suitLength][c].moveNo = moveNo;
      if (! wasNew)
      {
        UpdateHist(histNo, suitLength, c, rank, newFlag);
        wasNew = true;
      }
      // CompareRecurse(moveNo, htmp, histNo);
      num++;
    }
    else
    {
      Holding htmp2;
      htmp2.Set(suitLength, c);
      htmp2.Print(cout);
      cout << endl;
      assert(false);
    }

    r = chain[numXes][r].next;
  }
  while (r != first);

  if (num >= 10000)
    num = 9999;
  histCycles[num]++;
}


void PrintDist()
{
  // for (int d = 1; d < SDS_NUMDISTLENGTH; d++)
  for (int d = 1; d < 5; d++)
  {
    cout << "Suit length " << d << "\n";
    cout << "--------------\n";
    int dCount = 0;

    for (int dNew = 0; dNew < SDS_NUMDIST; dNew++)
    {
      int f = distToChain[d][dNew].first;
      if (f == -1)
        continue;

      dCount++;
      cout << hex << dNew << dec << ": " <<
        (dNew & 0x3) << "-" <<
        ((dNew >> 4) & 0x3) << "-" <<
        ((dNew >> 8) & 0x3) << "-" <<
        ((dNew >> 12) & 0x3) << "\n";

      int r = f;
      do
      {
        Holding holding;
        holding.Set(d+1, r);
        holding.Print(cout);
        cout << "\n";

        r = chain[d][r].next;
      }
      while (r != f);
    }
    
    cout << "\n" << dCount << " distributions\n";
  }
}


void PrintHistLowCards()
{
  cout << "Number of calls: " << histCalls << "\n";

  cout << "\nNumber of Xes\n";
  for (int i = 0; i < 11; i++)
    cout << setw(2) << i << setw(10) << histXes[i] << "\n";
  cout << "\n";

  int sum = 0;
  cout << "\nNumber of cycles\n";
  for (int i = 0; i < 10000; i++)
  {
    if (histCycles[i] == 0)
      continue;

    cout << setw(4) << i << setw(10) << histCycles[i] << "\n";
    sum = i * histCycles[i];
  }

  if (sum == 0)
    return;

  cout << "Average cycle length " <<
    setw(6) << std::fixed << std::setprecision(2) << 
      sum / static_cast<double>(histCalls) << "\n";
}
