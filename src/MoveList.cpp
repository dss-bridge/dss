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
#include "MoveList.h"
#include "misc.h"



MoveList::MoveList()
{
  for (int key = 0; key < ML_MAXKEY; key++)
  {
    index[key] = nullptr;
    indexSide[0][key] = nullptr;
    indexSide[1][key] = nullptr;

    indexCount[key] = 0;
    indexCountSide[0][key] = 0;
    indexCountSide[1][key] = 0;
  }

  for (int i = 0; i < POOLSIZE; i++)
  {
    moveCount[i] = 0;
    moveCountSide[0][i] = 0;
    moveCountSide[1][i] = 0;
    noToSideNumbers[i][0] = 0;
    noToSideNumbers[i][1] = 0;
  }

  numEntries = 1;
  numEntriesNew = 1;
  numEntriesSide[0] = 1;
  numEntriesSide[0] = 1;
}


MoveList::~MoveList()
{
}


unsigned MoveList::AddMoves(
  DefList& def, 
  const Holding& holding, 
  bool& newFlag)
{
  // WholeMove whole;
  // whole.Add(def);
  // return MoveList::AddMoves(whole, holding, newFlag);

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
      if (def == lp->wholep->def1)
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

  assert(numEntries < POOLSIZE);

  if (newFlag)
    indexCount[key]++;

  moveCount[numEntries]++;

  lp->no = numEntries;
  list[numEntries].suitLengthExample = holding.GetSuitLength();
  list[numEntries].counterExample = holding.GetCounter();
  lp->wholep = &list[numEntries++].whole;
  lp->wholep->Add(def);
  lp->next = nullptr;

  return lp->no;
}


unsigned MoveList::AddMoves(
  WholeMove& whole,
  const Holding& holding, 
  bool& newFlag)
{
  Header& hp = whole.GetHeader();
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
      if (whole == *(lp->wholep))
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

  assert(numEntries < POOLSIZE);

  if (newFlag)
    indexCount[key]++;

  moveCount[numEntries]++;

  lp->no = numEntries;
  list[numEntries].suitLengthExample = holding.GetSuitLength();
  list[numEntries].counterExample = holding.GetCounter();
  lp->wholep = &list[numEntries++].whole;
  *(lp->wholep) = whole;
  lp->next = nullptr;

  return lp->no;
}



unsigned MoveList::AddSideMove(
  DefList& defSide,
  const Holding& holding, 
  const unsigned side,
  bool& newFlag)
{
  if (defSide.IsEmpty())
    return 0;

  Header& hp = defSide.GetHeader();
  unsigned key = hash.GetKey(hp);

  ListEntrySide * lp = indexSide[side][key];
  newFlag = false;

  if (lp == nullptr)
  {
    indexSide[side][key] = new ListEntrySide;
    lp = indexSide[side][key];
    newFlag = true;
  }
  else
  {
    while (lp)
    {
      if (defSide == *(lp->defp))
      {
        moveCountSide[side][lp->no]++;
        return lp->no;
      }

      if (! lp->next)
      {
        lp->next = new ListEntrySide;
        newFlag = true;
        lp = lp->next;
        break;
      }
      lp = lp->next;
    }
  }

  if (newFlag)
    indexCountSide[side][key]++;

  moveCountSide[side][numEntriesSide[side]]++;

  lp->no = numEntriesSide[side];
  list[numEntriesSide[side]].suitLengthExample = holding.GetSuitLength();
  list[numEntriesSide[side]].counterExample = holding.GetCounter();
  lp->defp = &listSide[side][numEntriesSide[side]++].def;
  *(lp->defp) = defSide;
  lp->next = nullptr;

  return lp->no;
}


unsigned MoveList::PairToNo(
  const unsigned noAB,
  const unsigned noP)
{
  string strAB = static_cast<ostringstream *>
    (&(ostringstream() << noAB))->str();
  string strP = static_cast<ostringstream *>
    (&(ostringstream() << noP))->str();
  string str = strAB + '.' + strP;

  map<string, unsigned>::iterator it = sideMap.find(str);
  if (it == sideMap.end())
    return 0;
  else
    return sideMap[str];
}


unsigned MoveList::SetPairNo(
  const unsigned noAB,
  const unsigned noP)
{
  // Concatenate to string, put in map
  // Also put in noToSideNumbers
  //

  string strAB = static_cast<ostringstream *>
    (&(ostringstream() << noAB))->str();
  string strP = static_cast<ostringstream *>
    (&(ostringstream() << noP))->str();
  string str = strAB + '.' + strP;

  unsigned no = numEntriesNew++;
  sideMap[str] = no;
  return no;
}


unsigned MoveList::AddMoves(
  DefList& defAB,
  DefList& defP,
  const Holding& holding, 
  bool& newFlag)
{
  bool newFlagAB = false;
  bool newFlagP = false;

  unsigned noAB = MoveList::AddSideMove(defAB, holding, 0, newFlagAB);
  unsigned noP = MoveList::AddSideMove(defAB, holding, 1, newFlagP);

  if (newFlagAB || newFlagP)
  {
    newFlag = true;
    return MoveList::SetPairNo(noAB, noP);
  }

  unsigned no = MoveList::PairToNo(noAB, noP);
  assert(no > 0);
  return no;
}


unsigned MoveList::GetMaxRank(
  const unsigned no)
{
  return list[no].whole.GetMaxRank();
}


DefList& MoveList::GetCombinedMove(
  const unsigned no)
{
  return list[no].whole.GetCombinedMove();
}


void MoveList::Print(
  const unsigned no) const
{
  return list[no].whole.Print();
}


void MoveList::PrintCaseCombos()
{
  int histd[LENCASE] = {0};
  int hista[LENCASE] = {0};

  for (int n = 1; n < numEntries; n++)
  {
    unsigned maxt = list[n].whole.GetKeyNew();
    unsigned d = maxt & 0xf;
    unsigned a = maxt >> 4;
    histd[d]++;
    hista[a]++;
  }

  cout << "Defense histogram\n" << right;
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


void MoveList::PrintCount() const
{
  cout << "Number of list entries: " << numEntries << "\n";
}


void MoveList::PrintMoveList(
  ostream& out)
{
  for (int n = 1; n < numEntries; n++)
    MoveList::PrintMove(out, n);
}


void MoveList::PrintMoveListByKeys(
  ostream& fout)
{
  const string divider(52, '=');
  for (int key = 0; key < ML_MAXKEY; key++)
  {
    ListEntry * lp = index[key];
    if (lp == nullptr)
      continue;

    lp->wholep->GetHeader().PrintKey(fout, key);
    fout << divider << "\n\n";

    while (lp)
    {
      MoveList::PrintMove(fout, lp->no);
      lp = lp->next;
    }
  }
}


void MoveList::PrintMove(
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

  Header& hp = list[n].whole.GetHeader();
  hp.Print(out, true);
  out << "\n";

  list[n].whole.Print(out);
}


void MoveList::PrintMoveStats(
  ostream& out) const
{
  SortEntry sortList[POOLSIZE];
  for (int i = 1; i < numEntries; i++)
  {
    sortList[i-1].no = i;
    sortList[i-1].count = moveCount[i];
  }
  sort(sortList, sortList + numEntries - 1, SortIsGreater);

  out << "Sorted move counts\n\n";
  for (int i = 0; i < numEntries-1; i++)
    out <<
      setw(6) << i <<
      setw(8) << sortList[i].no <<
      setw(10) << sortList[i].count << "\n";
  out << "\n";
}


void MoveList::PrintListStats(
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


void MoveList::PrintHashCounts() const
{
  hash.PrintCounts();
}
