/* 
   SDS, a bridge single-suit double-dummy quick-trick solver.

   Copyright (C) 2015 by Soren Hein.

   See LICENSE and README.
*/


#include <iostream>

// #include <stdio.h>
#include <assert.h>

#include "portab.h"

#include "cst.h"
#include "DefList.h"
#include "MoveList.h"
#include "SimpleMoves.h"
#include "ComplexMoves.h"
#include "Stats.h"

/*
   The multi-threading is rather simple:  Each thread gets a
   single batch of holdings, and we wait for all batches to complete
   before giving any of them more holdings.

   OPENMP multi-threading is untested.
*/

#define SDS_BATCHSIZE 1000
#define SDS_MAXHOLDINGS (MAXNOOFCORES * SDS_BATCHSIZE)


extern int noOfCores;

extern singleType * singles[14];

extern MoveList moveList;

extern int histCount[HIST_SIZE][14], histRank[HIST_SIZE][14];
extern int histMoveCount[HIST_SIZE][14], histMoveRank[HIST_SIZE][14];

extern bool debugComplex;


// Globals for multi-threading.
Holding holdingList[SDS_MAXHOLDINGS];

DefList defList[SDS_MAXHOLDINGS];

int batchLen;



bool MakeComplexTables(
  int& slStart,
  int& cStart,
  const int dlen);

int MakeComplexDistribute();

void BestMoveAfterSide(
  Holding& holding,
  DefList& def);

void BestMoveAfterLead(
  Holding& holding,
  DefList& def);

void BestMoveAfterLho(
  Holding& holding,
  DefList& def);

bool BestMoveAfterPard(
  Holding& holding,
  DefList& def);

void DumpStatus(
  DefList& def,
  const char text[]);

void DumpStatus(
  DefList& def,
  const char text[],
  const int val);

void DumpStatus(
  DefList& def,
  const char text[],
  const int val1,
  const int val2);


void MakeComplexMoves()
{
  int hist[14] = {0};
  bool newFlag;

  // General case.
  Holding holding;

  for (int dlen = 2; dlen <= 13; dlen++)
  {
    for (int sl = 1; sl <= 13; sl++)
    {
      for (int c = 0; c < SDS_NUMSINGLES[sl]; c++)
      {
        if (singles[sl][c].defp)
          continue;

        if (singles[sl][c].declLen != dlen)
          continue;

// if (sl == 9 && c == 0xe18f)
  // debugComplex = true;
// else
  // debugComplex = false;
// cout << setw(2) << dlen << 
// setw(3) << sl << setw(3) << hex << c << dec << endl;
        holding.Set(sl, c);
        DefList sm;
        sm.SetMaxOppRank(holding.GetMaxOppRank());

        MakeComplexSingleMove(holding, sm);

        StartTimer();
        singles[sl][c].defp = moveList.AddMoves(sm, holding, newFlag);
        EndTimer();

        histCount[HIST_COMPLEX][dlen]++;
        Header& header = singles[sl][c].defp->GetHeader();
        int r = static_cast<int>(header.GetMaxRank());
        histRank[HIST_COMPLEX][r]++;

        if (newFlag)
        {
          histMoveCount[HIST_COMPLEX][dlen]++;
          histMoveRank[HIST_COMPLEX][r]++;
        }
      }
    }
  }
}


void MakeComplexSingleMove(
  Holding& holding,
  DefList& def)
{
holding.PrintNew(cout);
cout.flush();

  if (holding.GetLength(QT_ACE) == 0) 
  {
// cout << "MCSM0\n";
assert(false);
    assert(holding.GetLength(QT_PARD) > 1);
    holding.SetSide(QT_PARD);
    BestMoveAfterSide(holding, def);
  }
  else if (holding.GetLength(QT_PARD) == 0)
  {
    assert(holding.GetLength(QT_ACE) >= 1);
// cout << "MCSM1" << endl;
    holding.SetSide(QT_ACE);
// cout << "MCSM2" << endl;
    BestMoveAfterSide(holding, def);
// cout << "MCSM3" << endl;
  }
  else
  {
// cout << "MCSM4" << endl;
    DefList def1, def2;
    def1.SetMaxOppRank(holding.GetMaxOppRank());
    def2.SetMaxOppRank(holding.GetMaxOppRank());

    holding.SetSide(QT_ACE);
    BestMoveAfterSide(holding, def1);

    holding.SetSide(QT_PARD);
    BestMoveAfterSide(holding, def2);

    if (debugComplex)
    {
      DumpStatus(def1, "BestMove for side 0");
      DumpStatus(def2, "BestMove for side 2");
    }

    def.MergeSides(def1, def2);
  }

#if 0
  Header& header = def.GetHeader();
  int tFullTrick = header.GetTrickKey();
  int tFullRank = header.GetRankKey();

  int t1 = tFullTrick & 0xf;
  int t2 = (tFullTrick >> 4) & 0xf;
  int t3 = (tFullTrick >> 8) & 0xf;

  int r1 = tFullRank & 0xf;
  int r2 = (tFullRank >> 4) & 0xf;
  int r3 = (tFullRank >> 8) & 0xf;

  int gl1 = holding.GetLength(QT_ACE);
  int gl2 = holding.GetLength(QT_PARD);
  int gl = Max(gl1, gl2);
  int c = holding.GetCounter();
  int s = holding.GetSuitLength();
  int top = c >> (2*s - 8);

  if (t1 == t2 && t1 == t3 && r1 == r2 && r1 == r3)
  {
    // Suit doesn't cash (SHORT).
    // if (t1 < gl)

    // Suit cashes completely.  Same length ace-pard (EQUAL)
    // if (t1 == gl && gl1 == gl2)

    // Suit cashes completely.  Not same length ace-pard (DIFF)
    if (t1 == gl && gl1 != gl2)

    // Specialized: Short
    // if (t1 <= 2)

    // Specialized: AKQJ with ace.
    // if (top == 0)

    {
      holding.PrintNew(cout);
      def.Print(cout);
    }
  }
#endif

  if (debugComplex)
    DumpStatus(def, "MakeComplexSingleMove");
}


bool MakeComplexTables(
  int& slStart,
  int& cStart,
  const int dlen)
{
  batchLen = 0;

  for (int sl = slStart; sl <= 13; sl++)
  {
    int cs = (sl == slStart ? cStart : 0);
    for (int c = 0; c < SDS_NUMSINGLES[sl]; c++)
    {
      if (singles[sl][c].defp)
        continue;

      if (singles[sl][c].declLen != dlen)
        continue;

      holdingList[batchLen].Set(sl, c);

      batchLen++;
      // if (batchLen == SDS_MAXHOLDINGS)
      if (batchLen == noOfCores * SDS_BATCHSIZE)
      {
        slStart = sl;
        cStart = c+1;
        return true;
      }
    }
  }

  // Last batch
  return false;
}


void MakeComplexMovesParallel()
{
  bool newFlag;

  for (int dlen = 2; dlen <= 13; dlen++)
  {
    int slStart = 1, cStart = 0;
    bool flag;
    cout << "\nDeclaring length " << dlen << ":\n";

    double histRest = histCount[HIST_ORIG_COUNT][dlen];
    for (int hno = 1; hno < HIST_COMPLEX; hno++)
      histRest -= histCount[hno][dlen];
    if (histRest < 1)
      histRest = 1;

    int skip = 0;
    do
    {
      if (++skip == 10)
      {
        cout << "Count " << setw(8) << cStart << " (" <<
          setw(5) << std::fixed << std::setprecision(2) <<
          100. * histCount[HIST_COMPLEX][dlen] / 
            histRest << "%)" << endl;
        skip = 0;
      }

      flag = MakeComplexTables(slStart, cStart, dlen);

      StartTimer();
      MakeComplexDistribute();

      for (int i = 0; i < batchLen; i++)
      {
        int sl = holdingList[i].GetSuitLength();
        int c = holdingList[i].GetCounter();

        singles[sl][c].defp = moveList.AddMoves(defList[i], 
          holdingList[i], newFlag);

        histCount[HIST_COMPLEX][dlen]++;
        Header& header = singles[sl][c].defp->GetHeader();
        int r = static_cast<int>(header.GetMaxRank());
        histRank[HIST_COMPLEX][r]++;

        if (newFlag)
        {
          histMoveCount[HIST_COMPLEX][dlen]++;
          histMoveRank[HIST_COMPLEX][r]++;
        }
      }
      EndTimer();

    }
    while (flag);
  }
  cout << "\n";
}


#if (defined(_WIN32) || defined(__CYGWIN__)) && !defined(_OPENMP)

DWORD CALLBACK MakeComplexBatch(void *);


HANDLE solveAllEvents[MAXNOOFCORES];
LONG volatile threadIndex;


DWORD CALLBACK MakeComplexBatch(void *)
{
  int thid;
  thid = InterlockedIncrement(&threadIndex);

  int startNo = SDS_BATCHSIZE * thid;
  int endNo = Min(batchLen, startNo + SDS_BATCHSIZE);

  for (int n = startNo; n < endNo; n++)
    MakeComplexSingleMove(holdingList[n], defList[n]);

  if (SetEvent(solveAllEvents[thid]) == 0)
    return 0;

  return 1;

}


int MakeComplexDistribute()
{
  DWORD solveAllWaitResult;
  int k, res;

  threadIndex = -1;

  for (k = 0; k < noOfCores; k++)
  {
    solveAllEvents[k] = CreateEvent(NULL, FALSE, FALSE, 0);
    if (solveAllEvents[k] == 0)
      return -1;
  }

  for (k = 0; k < noOfCores; k++)
  {
    res = QueueUserWorkItem(MakeComplexBatch, NULL, WT_EXECUTELONGFUNCTION);
    if (res != 1)
      return res;
  }

  solveAllWaitResult = WaitForMultipleObjects(
    static_cast<unsigned>(noOfCores),
    solveAllEvents, TRUE, INFINITE);

  if (solveAllWaitResult != WAIT_OBJECT_0)
    return -2;

  for (k = 0; k < noOfCores; k++)
    CloseHandle(solveAllEvents[k]);

  return 1;
}

#else

void MakeComplexBatch(int thid);


void MakeComplexBatch(int thid)
{
  int startNo = SDS_BATCHSIZE * thid;
  int endNo = Min(batchLen, startNo + SDS_BATCHSIZE);

  for (int n = startNo; n < endNo; n++)
    MakeComplexSingleMove(&holdingList[n], &defList[n]);

  return 1;

}


int MakeComplexDistribute()
{
  int k;
  fail = 1;

#if defined (_OPENMP)
  if (omp_get_dynamic())
    omp_set_dynamic(0);
  omp_set_num_threads(noOfCores);
#endif

  int index, thid, hint;
  schedType st;

  #pragma omp parallel default(none) shared(fail) private(thid, res)
  {
    #pragma omp while schedule(dynamic, chunk)

    while (1)
    {
#if defined (_OPENMP)
      thid = omp_get_thread_num();
#else
      thid = 0;
#endif

      res = MakeComplexBatch(thid);

      if (res != 1)
        fail = res;
    }
  }

  return fail;
}

#endif


void BestMoveAfterSide(
  Holding& holding,
  DefList& def)
{
  DefList defNew;
  defNew.SetMaxOppRank(holding.GetMaxOppRank());

  def.Reset();
  bool seen = false;

  holding.RewindLead();
  while (holding.NextLead())
  {
    if (! seen)
    {
      seen = true;
      BestMoveAfterLead(holding, def);
    }
    else
    {
      BestMoveAfterLead(holding, defNew);
      def *= defNew;
    }

    if (debugComplex)
      DumpStatus(def, "BestMoveAfterSide: leadNo");
  }
}


void BestMoveAfterLead(
  Holding& holding,
  DefList& def)
{
  DefList defNew;
  defNew.SetMaxOppRank(holding.GetMaxOppRank());

  def.Reset();
  bool seen = false;

  holding.RewindLho();
  while (holding.NextLho())
  {
    if (! seen)
    {
      seen = true;
      BestMoveAfterLho(holding, def);
    }
    else
    {
      BestMoveAfterLho(holding, defNew);
      def += defNew;
    }

    if (debugComplex)
      DumpStatus(def, "BestMoveAfterLead: lhoNo");
  }
}


void BestMoveAfterLho(
  Holding& holding,
  DefList& def)
{
  DefList defNew;
  defNew.SetMaxOppRank(holding.GetMaxOppRank());

  bool seen = false;

  holding.RewindPard();
  while (holding.NextPard())
  {
    if (! seen)
    {
      seen = true;
      seen = BestMoveAfterPard(holding, def);
    }
    else
    {
      seen = BestMoveAfterPard(holding, defNew);
      def *= defNew;
    }

    if (debugComplex)
      DumpStatus(def, "BestMoveAfterLho: pardNo");
  }
}


bool BestMoveAfterPard(
  Holding& holding,
  DefList& def)
{
  int slNew = 0, cNew = 0;

  holding.SetRhoNo();
  if (! holding.MakePlay(slNew, cNew))
  {
    const Trick& trick = holding.GetTrick();
    def.Set1(trick);

    if (debugComplex)
    {
      DumpStatus(def, "BestMoveAfterPard: Simple play");
      holding.PrintPlayNew(cout);
    }

    return true;
  }

  assert(slNew >= 1 && slNew <= 12);
  assert(cNew >= 0 && cNew < SDS_NUMSINGLES[slNew]);


  if (singles[slNew][cNew].defp == nullptr)
  {
    Holding tmpHolding;
    tmpHolding.Set(slNew, cNew);

    if (debugComplex)
    {
      cout << "Start recursing\n";
      tmpHolding.PrintNew(cout);
    }

    DefList stmp;
    stmp.SetMaxOppRank(tmpHolding.GetMaxOppRank());
      
    bool newFlag;
    MakeComplexSingleMove(tmpHolding, stmp);
    int sl = tmpHolding.GetSuitLength();
    int c = tmpHolding.GetCounter();
    // TODO: Does tmpHolding really change in the invocation?
    singles[sl][c].defp = moveList.AddMoves(stmp, tmpHolding, newFlag);

    if (debugComplex)
    {
      stmp.Print(cout);
      cout << "Done recursing\n";
    }
  }

  assert(singles[slNew][cNew].defp != nullptr);
  def = * singles[slNew][cNew].defp;
  def.SetMaxOppRank(holding.GetMaxOppRank());

  if (debugComplex)
  {
    DumpStatus(def, "BestMoveAfterPard: Stored play count", slNew, cNew);
    Holding hTmp;
    hTmp.Set(slNew, cNew);
    hTmp.PrintNew(cout);
    holding.PrintPlayNew(cout);
    fflush(stdout);
  }

  def += holding;
// cout << "After +=, dc " << debugComplex << endl;

  if (debugComplex)
    DumpStatus(def, "Prepended: Stored play count");
// cout.flush();

  return true;
}


void DumpStatus(
  DefList& def,
  const char text[])
{
  cout << text << "\n";
  def.GetHeader().PrintNew(cout);
  def.Print(cout);
  cout.flush();
}


void DumpStatus(
  DefList& def,
  const char text[],
  const int val)
{
  cout << text << " " << val << "\n";
  def.GetHeader().PrintNew(cout);
  def.Print(cout);
  cout.flush();
}


void DumpStatus(
  DefList& def,
  const char text[],
  const int val1,
  const int val2)
{
  cout << text  << " " << val1 << " 0x" << hex << val2 << dec << "\n";
  def.GetHeader().PrintNew(cout);
  def.Print(cout);
  cout.flush();
}
