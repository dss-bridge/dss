/* 
   SDS, a bridge single-suit double-dummy quick-trick solver.

   Copyright (C) 2015 by Soren Hein.

   See LICENSE and README.
*/


#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>

using namespace std;
// #include <stdio.h>
#include <assert.h>

#include "portab.h"
#include "cst.h"
#include "MoveList.h"



MoveList::MoveList()
{
  for (int key = 0; key < ML_MAXKEY; key++)
  {
    index[key] = nullptr;
    indexCount[key] = 0;
  }

  for (int i = 0; i < POOLSIZE; i++)
    moveCount[i] = 0;

  numEntries = 0;

  for (int i = 0; i < LENTRICK; i++)
    trickKeyMap[i] = -1;

  for (int i = 0; i < LENRANK; i++)
    rankKeyMap[i] = -1;

  for (int i = 0; i < LENCASE; i++)
    caseKeyMap[i] = -1;

  for (int i = 0; i < LENRANKCASE; i++)
    rankCaseKeyMap[i] = -1;

  keyMapNo = 0;
  rankMapNo = 0;
  caseMapNo = 0;
  rankCaseMapNo = 0;
}


MoveList::~MoveList()
{
}


DefList * MoveList::AddMoves(
  DefList& def, 
  const Holding& holding, 
  bool& newFlag)
{
  Header& hp = def.GetHeader();
  // int key = hp.GetKey();
  // assert(key >= 0 && key < ML_MAXKEY);

  // TRICK: 12 bits (4096) --> 132 combinations, 8 bits.
  int tFullTrick = hp.GetTrickKey();
  int tRedTrick;
  if (trickKeyMap[tFullTrick] == -1)
    trickKeyMap[tFullTrick] = keyMapNo++;
  tRedTrick = trickKeyMap[tFullTrick];
  assert(tFullTrick < LENTRICK);
  assert(tRedTrick < 250);

  // RANK: 12 bits ( 4096) --> 399 combinations,  9 bits.
  // RANK: 16 bits (65536) --> 593 combinations, 10 bits.
  int tFullRank = hp.GetRankKey();
  int tRedRank;
  if (rankKeyMap[tFullRank] == -1)
    rankKeyMap[tFullRank] = rankMapNo++;
  tRedRank = rankKeyMap[tFullRank];
  assert(tFullRank < LENRANK);
  assert(tRedRank < 1000);

  // CASE: ? bits --> 20 combinations, 5 bits.
  unsigned tFullCase = def.GetKey();
  int tRedCase;
  if (caseKeyMap[tFullCase] == -1)
    caseKeyMap[tFullCase] = caseMapNo++;
  tRedCase = caseKeyMap[tFullCase];
  assert(tFullCase < LENCASE);
  assert(tRedCase < 128);

  // CASE*RANK(old): 5 + 9 = 14 bits -> 10 bits
  // CASE*RANK(new): 5 +10 = 15 bits -> ?? bits
  /*
  int tFullRankCase = (tRedRank << 5) | tRedCase;
  int tRedRankCase;
  if (rankCaseKeyMap[tFullRankCase] == -1)
    rankCaseKeyMap[tFullRankCase] = rankCaseMapNo++;
  tRedRankCase = rankCaseKeyMap[tFullRankCase];
  assert(tFullRankCase < LENRANKCASE);
  assert(tRedRankCase < 1024);
  */

  int tFullRankCase = (tRedRank << 7) | tRedCase;
  int tRedRankCase;
  if (rankCaseKeyMap[tFullRankCase] == -1)
    rankCaseKeyMap[tFullRankCase] = rankCaseMapNo++;
  tRedRankCase = rankCaseKeyMap[tFullRankCase];
  assert(tFullRankCase < LENRANKCASE);
  assert(tRedRankCase < 4096);

  int key = (tRedRankCase << 8) | tRedTrick;
  // int key = (tRedRank << 8) | tRedTrick;
  assert(key < ML_MAXKEY);


  ListEntry * lp = index[key];
  newFlag = false;

  if (lp == nullptr)
  {
    index[key] = new ListEntry;
    lp = index[key];
    newFlag = true;
  }
  else
  {
    while (lp)
    {
      if (def == *(lp->defp))
      {
        moveCount[lp->no]++;
        return lp->defp;
      }

      if (! lp->next)
      {
        lp->next = new ListEntry;
        newFlag = true;
        lp = lp->next;
        break;
      }
      lp = lp->next;
    }
  }

  assert(numEntries < POOLSIZE);

  if (newFlag)
    indexCount[key]++;

  moveCount[numEntries]++;

  lp->no = numEntries;
  list[numEntries].suitLengthExample = holding.GetSuitLength();
  list[numEntries].counterExample = holding.GetCounter();
  lp->defp = &list[numEntries++].def;
  *(lp->defp) = def;
  lp->next = nullptr;

  return lp->defp;
}


void MoveList::CountTrickCombos()
{
  int seen[LENTRICK] = {0};
  int count = 0;

  for (int n = 0; n < numEntries; n++)
  {
    Header& header = list[n].def.GetHeader();
    int maxt = header.GetTrickKey();
    if (seen[maxt])
      continue;

    count++;
    seen[maxt] = 1;
  }

  cout << "Trick count: " << count << "\n";
}


void MoveList::CountRankCombos()
{
  int seen[LENRANK] = {0};
  int count = 0;

  for (int n = 0; n < numEntries; n++)
  {
    Header& header = list[n].def.GetHeader();
    int maxt = header.GetRankKey();
    if (seen[maxt])
      continue;

    count++;
    seen[maxt] = 1;
  }

  cout << "Rank count: " << count << "\n";
}


void MoveList::CountCaseCombos()
{
  int seen[LENCASE] = {0};
  int histd[LENCASE] = {0};
  int hista[LENCASE] = {0};

  int count = 0;

  for (int n = 0; n < numEntries; n++)
  {
    unsigned maxt = list[n].def.GetKey();
    unsigned d = maxt & 0xf;
    unsigned a = maxt >> 4;
    histd[d]++;
    hista[a]++;

    if (seen[maxt])
      continue;

    count++;
    seen[maxt] = 1;
  }

  cout << "Case count: " << count << "\n";

  cout << "Defense histogram\n";
  for (int i = 0; i < LENCASE; i++)
  {
    if (histd[i])
      cout << 
        setw(2) << i+1 <<
        setw(10) << histd[i] << "\n";
  }

  cout << "Alternatives histogram\n";
  for (int i = 0; i < LENCASE; i++)
  {
    if (hista[i])
      cout << 
        setw(2) << i <<
        setw(10) << hista[i] << "\n";
  }
}


void MoveList::PrintCount()
{
  cout << "Number of list entries: " << numEntries << "\n";
}


void MoveList::PrintMoveList()
{
  for (int n = 0; n < numEntries; n++)
    MoveList::PrintMove(cout, n);
}


void MoveList::PrintMoveList(std::ostream& fout)
{
  for (int n = 0; n < numEntries; n++)
    MoveList::PrintMove(fout, n);
}


void MoveList::PrintMoveListByKeys()
{
  MoveList::PrintMoveListByKeys(cout);
}


void MoveList::PrintMoveListByKeys(std::ostream& fout)
{
  int movesSeen = 0;
  Header h;

  for (int key = 0; key < ML_MAXKEY; key++)
  {
    ListEntry * lp = index[key];
    if (lp == nullptr)
      continue;

    int trickKey = lp->defp->GetHeader().GetKey();
    ostringstream out;
    h.KeyToText(out, trickKey);
    fout << "\n\nKey " << 
      setw(6) << key <<
      " (" << 
      out.str() << ")\n";
    fout << "====================================================\n\n";

    while (lp)
    {
      MoveList::PrintMove(fout, lp->no);
      lp = lp->next;
    }
  }
}


void MoveList::PrintMove(
  std::ostream& fout,
  const int n)
{
  fout << "----------------------------------------------------\n";
  fout << "Entry " << n << " of " << numEntries-1 << " (" <<
    moveCount[n] << " times, len " <<
    list[n].suitLengthExample <<
    ", counter " <<
    hex << list[n].counterExample << dec << "):\n\n";

  ostringstream out;
  Holding holding;
  holding.Set(list[n].suitLengthExample, list[n].counterExample);
  holding.ToText(out);
  fout << out.str() << "\n";

  ostringstream out2;
  Header& hp = list[n].def.GetHeader();
  hp.ToText(out2);
  fout << out2.str() << "\n";

  list[n].def.Print(fout);
}


void MoveList::PrintMoveStats()
{
  MoveList::PrintMoveStats(cout);
}


void MoveList::PrintMoveStats(std::ostream& fout)
{
  struct SortEntry
  {
    int no;
    int count;
  };
  SortEntry sortList[POOLSIZE], tmp;

  for (int i = 0; i < numEntries; i++)
  {
    sortList[i].no = i;
    sortList[i].count = moveCount[i];
  }

  // Simple bubble sort.
  int n = numEntries;
  do
  {
    int new_n = 0;
    for (int i = 1; i < n; i++)
    {
      if (sortList[i-1].count > sortList[i].count) 
        continue;

      tmp = sortList[i-1];
      sortList[i-1] = sortList[i];
      sortList[i] = tmp;

      new_n = i;
    }
    n = new_n;
  }
  while (n > 0);

  fout << "Sorted move counts\n\n";
  for (int i = 0; i < numEntries; i++)
    fout <<
      setw(6) << i <<
      setw(8) << sortList[i].no <<
      setw(10) << sortList[i].count << "\n";
  fout << "\n";
}


void MoveList::PrintListStats()
{
  MoveList::PrintListStats(cout);
}


void MoveList::PrintListStats(std::ostream& fout)
{
  int p = 0;
  int mm = ML_MAXKEY;
  int clist[1 << 14] = {0};

  double ssum = 0., sumsq = 0.;

  fout << "Hash counts\n\n";
  fout << " k   count\n";
    
  for (int key = 0; key < mm; key++)
  {
    int i = indexCount[key];
    if (i == 0)
      continue;
    ssum += i;
    sumsq += static_cast<double>(i) * static_cast<double>(i);
    fout <<
      setw(2) << key <<
      setw(10) << i << "\n";
    clist[p++] = i;
  }

  cout << "p " << p << "\n";
  if (p == 0)
    return;

  // Simple bubble sort.
  int n = p;
  int tmp;
  do
  {
    int new_n = 0;
    for (int i = 1; i < n; i++)
    {
      if (clist[i-1] < clist[i]) 
        continue;

      tmp = clist[i-1];
      clist[i-1] = clist[i];
      clist[i] = tmp;

      new_n = i;
    }
    n = new_n;
  }
  while (n > 0);

  double sum = 0., psum = 0.;
  for (int i = 0; i < p; i++)
  {
    sum += clist[i];
    psum += i * clist[i];
  }
  double giniCoeff = 2 * psum / (p * sum) - (p+1.) / p;


  fout << "\nGini coefficient " <<
    setw(6) << std::fixed << std::setprecision(4) << giniCoeff <<
    ", number " << p << "\n";
  fout << "\nAverage search   " <<
    setw(6) << std::fixed << std::setprecision(4) << sumsq / (2. * ssum) <<
    "\n";
}

