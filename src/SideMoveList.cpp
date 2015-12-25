/* 
   SDS, a bridge single-suit double-dummy quick-trick solver.

   Copyright (C) 2015 by Soren Hein.

   See LICENSE and README.
*/


#include <iomanip>
#include <sstream>
#include <algorithm>

using namespace std;

#include <assert.h>

#include "cst.h"
#include "SideMoveList.h"
#include "misc.h"



SideMoveList::SideMoveList()
{
  list.clear();
  moveCount.clear();

  listLength = SIDEMOVE_CHUNK_SIZE;
  list.resize(SIDEMOVE_CHUNK_SIZE);
  moveCount.resize(SIDEMOVE_CHUNK_SIZE);

  for (int key = 0; key < ML_MAXKEY; key++)
  {
    index[key] = nullptr;
    indexCount[key] = 0;
  }

  for (int i = 0; i < SIDEMOVE_CHUNK_SIZE; i++)
    moveCount[i] = 0;

  numEntries = 1;

  for (unsigned i = 0; i < SDS_MAX_DEF; i++)
    histD[i] = 0;

  for (unsigned i = 0; i < SDS_MAX_DEF * SDS_MAX_ALT; i++)
    histAsum[i] = 0;
}


SideMoveList::~SideMoveList()
{
}


unsigned SideMoveList::AddMoves(
  DefList& def, 
  const Holding& holding, 
  bool& newFlag)
{
  Header& hp = def.GetHeader();
  unsigned key = hash.GetKey(hp);

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
      if (def == list[lp->no].def)
      {
        moveCount[lp->no]++;
        return lp->no;
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

  if (numEntries == listLength)
  {
    // Extend list.
    listLength += SIDEMOVE_CHUNK_SIZE;
    list.resize(listLength);
    moveCount.resize(listLength);

    for (unsigned i = numEntries; i < listLength; i++)
      moveCount[i] = 0;
  }

  if (newFlag)
    indexCount[key]++;

  moveCount[numEntries]++;

  list[numEntries].suitLengthExample = holding.GetSuitLength();
  list[numEntries].counterExample = holding.GetCounter();
  list[numEntries].def = def;
  lp->no = numEntries;
  lp->next = nullptr;
  numEntries++;

  unsigned d, asum;
  hp.GetAD(d, asum);
  histD[d-1]++;
  histAsum[asum-1]++;

  return lp->no;
}


unsigned SideMoveList::GetMaxRank(
  const unsigned no)
{
  return list[no].def.GetHeader().GetMaxRank();
}


DefList& SideMoveList::GetMove(
  const unsigned no)
{
  return list[no].def;
}


void SideMoveList::Print(
  const unsigned no) const
{
  return list[no].def.Print();
}


void SideMoveList::PrintMoveList(
  ostream& out)
{
  for (int n = 1; n < numEntries; n++)
    SideMoveList::PrintMove(out, n);
}


void SideMoveList::PrintMoveListByKeys(
  ostream& fout)
{
  const string divider(52, '=');
  for (int key = 0; key < ML_MAXKEY; key++)
  {
    ListEntry * lp = index[key];
    if (lp == nullptr)
      continue;

    list[lp->no].def.GetHeader().PrintKey(fout, key);
    fout << divider << "\n\n";

    while (lp)
    {
      SideMoveList::PrintMove(fout, lp->no);
      lp = lp->next;
    }
  }
}


void SideMoveList::PrintMove(
  ostream& out,
  const int n)
{
  const string divider(52, '-');
  out << divider << "\n";
  out << "Entry " << n << " of " << numEntries-1 << " (" <<
    moveCount[n] << " times, len " <<
    list[n].suitLengthExample <<
    ", counter " <<
    hex << list[n].counterExample << dec << "):\n\n";

  Holding holding;
  holding.Set(list[n].suitLengthExample, list[n].counterExample);
  holding.Print(out, false);
  out << right << "\n";

  Header& hp = list[n].def.GetHeader();
  hp.Print(out, true);
  out << "\n";

  list[n].def.Print(out);
}


void SideMoveList::PrintMoveStats(
  ostream& out) const
{
  vector<SortEntry> sortList(numEntries-1);
  for (int i = 1; i < numEntries; i++)
  {
    sortList[i-1].no = i;
    sortList[i-1].count = moveCount[i];
  }
  sort(sortList.begin(), sortList.end(), SortIsGreater);

  out << "Sorted move counts\n\n";
  for (int i = 0; i < numEntries-1; i++)
    out <<
      setw(6) << i <<
      setw(8) << sortList[i].no <<
      setw(10) << sortList[i].count << "\n";
  out << "\n";
}


void SideMoveList::PrintHashStats(
  ostream& out) const
{
  int p = 0;
  int clist[1 << 14] = {0};

  double ssum = 0., sumsq = 0.;

  out << "Hash counts\n\n";
  out << " k   count\n";
    
  for (int key = 0; key < ML_MAXKEY; key++)
  {
    int i = indexCount[key];
    if (i == 0)
      continue;
    ssum += i;
    sumsq += static_cast<double>(i) * static_cast<double>(i);
    out <<
      setw(2) << key <<
      setw(10) << i << "\n";
    clist[p++] = i;
  }

  cout << "p " << p << "\n";
  if (p == 0)
    return;

  sort(clist, clist + p);

  double sum = 0., psum = 0.;
  for (int i = 0; i < p; i++)
  {
    sum += clist[i];
    psum += i * clist[i];
  }
  double giniCoeff = 2 * psum / (p * sum) - (p+1.) / p;


  out << "\nGini coefficient " <<
    setw(6) << fixed << setprecision(4) << giniCoeff <<
    ", number " << p << "\n";
  out << "\nAverage search   " <<
    setw(6) << fixed << setprecision(4) << sumsq / (2. * ssum) <<
    "\n";
}


void SideMoveList::PrintList(
  const unsigned hist[],
  const unsigned l,
  const char text[]) const
{
  cout << text << "\n" << right;
  for (unsigned i = 0; i < l; i++)
  {
    if (hist[i])
      cout << 
        setw(2) << i+1 <<
        setw(10) << hist[i] << "\n";
  }
}


void SideMoveList::PrintLists(
  ostream& out,
  const string text) const
{
  if (numEntries == 1)
    return;

  out << text << "\n";
  SideMoveList::PrintMoveStats(out);

  out << text << "\n";
  SideMoveList::PrintHashStats(out);
}


void SideMoveList::PrintStats(
  const string text) const
{
  if (numEntries == 1)
    return;

  cout << text << "\n";
  hash.PrintCounts();

  cout << text << "\n";
  SideMoveList::PrintList(histD, SDS_MAX_DEF, "Defenses");
  SideMoveList::PrintList(histAsum, SDS_MAX_DEF * SDS_MAX_ALT, "Alts sum");

  cout << text << "\n";
  cout << "Number of list entries: " << 
    numEntries << " (list size " << listLength << ")\n";
}

