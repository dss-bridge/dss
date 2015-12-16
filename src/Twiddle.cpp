/* 
   SDS, a bridge single-suit double-dummy quick-trick solver.

   Copyright (C) 2015 by Soren Hein.

   See LICENSE and README.
*/


/*
  Twiddle functions: Generate all combinations of M elements drawn 
  without replacement from a set of N elements.  This routine may be 
  used in two ways:

  (0) To generate all combinations of M out of N objects, let a[0..N-1]
      contain the objects, and let c[0..M-1] initially be the combination
      a[N-M..N-1].  While Twiddle(&x, &y, &z, p) is false, set c[z] = a[x] 
      to produce a new combination.

  (1) To generate all sequences of 0's and 1's containing M 1's, let
      b[0..N-M-1] = 0 and b[N-M..N-1] = 1.  While Twiddle(&x, &y, &z, p) 
      is false, set b[x] = 1 and b[y] = 0 to produce a new sequence.

  In either of these cases, the array p[0..N+1] should be initialised as
  follows:

    p[0] = N+1
    p[1..N-M] = 0
    p[N-M+1..N] = 1..M
    p[N+1] = -2
    if M=0 then p[1] = 1

  In this implementation, this initialisation is accomplished by calling
  InitTwiddle(M, N, p), where p points to an array of N+2 ints.

  Coded by Matthew Belmonte <mkb4@Cornell.edu>, 23 March 1996.  This
  implementation Copyright (c) 1996 by Matthew Belmonte.  Permission for 
  use and distribution is hereby granted, subject to the restrictions that 
  this copyright notice and reference list be included in its entirety, 
  and that any and all changes made to the program be clearly noted in the 
  program text.

  This software is provided 'as is', with no warranty, express or implied,
  including but not limited to warranties of merchantability or fitness 
  for a particular purpose.  The user of this software assumes liability 
  for any and all damages, whether direct or consequential, arising from 
  its use.  The author of this implementation will not be liable for any 
  such damages.

  Reference:

  Phillip J Chase, `Algorithm 382: Combinations of M out of N Objects 
  [G6]', Communications of the Association for Computing Machinery 
  13:6:368 (1970).

  The returned indices x, y, and z in this implementation are decremented 
  by 1, in order to conform to the C language array reference convention.  
  Also, the parameter 'done' has been replaced with a Boolean return value.
*/

/*
  2015-04-16, Soren Hein:  Modified calling parameters of Twiddle.
  Using only way (1) above.
*/

#include <stdio.h>
#include <assert.h>

#include "cst.h"
#include "MoveList.h"
#include "SimpleMoves.h"
#include "Twiddle.h"

using namespace std;


extern singleType * singles[14];

extern MoveList moveList;


#define SDS_MAXGROUPS 5

struct PermGroup
{
  int bitShift;
  int numCards;
  int numLho;
};


bool Twiddle(
  int &x, 
  int &y, 
  int &z, 
  int p[]);

void InitTwiddle(
  const int m, 
  const int n, 
  int p[]);

void FillBits(
  const PermGroup group[],
  const int groupNo,
  const int numGroups,
  const int cBase,
  const int compBits,
  int suitLength,
  unsigned moveNo,
  Holding& holding,
  const int rank,
  const int histNo,
  bool& newFlag);


bool Twiddle(
  int& x, 
  int& y, 
  int& z, 
  int p[])
{
  int i, j = 1, k;
  while (p[j] <= 0)
    j++;

  if (p[j-1] == 0)
  {
    for (i = j-1; i != 1; i--)
      p[i] = -1;

    p[j] = 0;
    x = z = 0;
    p[1] = 1;
    y = j-1;
  }
  else
  {
    if (j > 1)
      p[j-1] = 0;

    do
      j++;
    while (p[j] > 0);

    k = j-1;
    i = j;

    while (p[i] == 0)
      p[i++] = -1;

    if (p[i] == -1)
    {
      p[i] = p[k];
      z = p[k]-1;
      x = i-1;
      y = k-1;
      p[k] = -1;
    }
    else
    {
      if (i == p[0])
        return true;
      else
      {
        p[j] = p[i];
        z = p[i]-1;
        p[i] = 0;
        x = j-1;
        y = i-1;
      }
    }
  }
  return false;
}


void InitTwiddle(
  const int m, 
  const int n, 
  int p[])
{
  int i;
  p[0] = n+1;
  for (i = 1; i != n-m+1; i++)
    p[i] = 0;

  while (i != n+1)
  {
    p[i] = i+m-n;
    i++;
  }
  p[n+1] = -2;

  if (m == 0)
    p[1] = 1;
}


int pctr = 0;
bool pnew = true;
int histGroup[SDS_MAXGROUPS] = {0};
int nCall = 0;
int nSet = 0;

void PrintGroupHist()
{
  cout << "Group histogram\n";
  for (int g = 0; g < SDS_MAXGROUPS; g++)
    cout << setw(2) << g << setw(10) << histGroup[g] << "\n";
  cout << "\n";
  cout << "nCall " << nCall << " nSet " << nSet << "\n";
}


void SetAllPermutations(
  const int& suitLength,
  const int& counter,
  unsigned moveNo,
  Holding& holding,
  const int rank,
  const int histNo,
  bool& newFlag)
{
  PermGroup group[SDS_MAXGROUPS];
  int numGroups = 0;
  bool run = false;
  int blockNo = 0;
  group[0].numCards = 0;
  group[0].numLho = 0;

pnew = true;
nCall++;

  // Detect groups to be permuted.
  // Consciously include a zero byte to get the end of a run.
  while (blockNo < suitLength)
  {
    int p = (counter >> (2*blockNo)) & 0x3;
    if (p == QT_ACE || p == QT_PARD)
    {
      run = false;
      if (group[numGroups].numCards > 0 &&
          group[numGroups].numLho > 0 &&
          group[numGroups].numLho < group[numGroups].numCards)
        numGroups++;

      group[numGroups].numCards = 0;
      group[numGroups].numLho = 0;
    }
    else
    {
      if (! run)
        group[numGroups].bitShift = 2*blockNo;

      run = true;
      group[numGroups].numCards++;
      if (p == QT_LHO)
        group[numGroups].numLho++;
    }
    blockNo++;
  }

histGroup[numGroups]++;
// if (histNo == HIST_ACE_SHORT)
// if (histNo == HIST_BLOCKED)
// if (histNo == HIST_PARD_VOID)
// if (histNo == HIST_CRASH)
// if (histNo == HIST_SINGLE)
// if (histNo == HIST_CASHES)
  // CompareRecurse(moveNo, holding, histNo);

  if (numGroups == 0)
  {
nSet++;
    singles[suitLength][counter].moveNo = moveNo;
    UpdateHist(histNo, suitLength, counter, rank, newFlag);
    // CompareRecurse(moveNo, holding, histNo);
    return;
  }


  // Find the part of the counter that does not change.
  int mask = (1 << (2*(suitLength-1))) - 1;
  int groupNo = 0;
  for (groupNo = 0; groupNo < numGroups; groupNo++)
  {
    mask ^= ((1 << (2*group[groupNo].numCards)) - 1) <<
      group[groupNo].bitShift;
  }
  int cBase = counter & mask;

  // Cycle recursively through the groups.
  FillBits(group, 0, numGroups, cBase, 0, 
    suitLength, moveNo, holding, rank, histNo, newFlag);
}


void FillBits(
  const PermGroup group[],
  const int groupNo,
  const int numGroups,
  const int cBase,
  const int compBits,
  int suitLength,
  unsigned moveNo,
  Holding& holding,
  const int rank,
  const int histNo,
  bool& newFlag)
{
  int x, y, z, p[14], b[12];
  bool newLocal = false;

  const int n = group[groupNo].numCards;
  const int m = group[groupNo].numLho;
  InitTwiddle(m, n, p);

  for (int i = 0; i < n-m; i++)
    b[i] = 0;

  for (int i = n-m; i < n; i++)
    b[i] = 1;

  bool ret;
  do
  {
    int compGroup = 0;
    for (int i = 0; i < n; i++)
    {
      if (b[i] == 0)
        compGroup |= (QT_RHO << (2*i));
      else
        compGroup |= (QT_LHO << (2*i));
    }

    compGroup = (compGroup << group[groupNo].bitShift);

    if (groupNo == numGroups-1)
    {
      int c = cBase | compBits | compGroup;
      if (singles[suitLength][c].moveNo == 0)
      {
        singles[suitLength][c].moveNo = moveNo;

        // if (histNo == HIST_CRASH)
        UpdateHist(histNo, suitLength, c, rank, newFlag);
nSet++;

        // if (pnew)
          // CompareRecurse(moveNo, holding, histNo);
        newFlag = false;
        pnew = false;
      }
    }
    else
    {
      FillBits(group, groupNo+1, numGroups, cBase, compBits | compGroup,
        suitLength, moveNo, holding, rank, histNo, newFlag);
    }

    ret = Twiddle(x, y, z, p);
    b[x] = 1;
    b[y] = 0;
  }
  while (! ret);
}

