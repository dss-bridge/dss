/* 
   SDS, a bridge single-suit double-dummy quick-trick solver.

   Copyright (C) 2015 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <string>

// #include <stdio.h>
// #include <string.h>
// #include <stdlib.h>
#include <assert.h>

#include "cst.h"
#include "LoopHold.h"

using namespace std;

extern vector<unsigned> holdCtr;
bool pickFlag;


LoopHold::LoopHold()
{
  // For historical reasons the combinations and functions
  // are labeled as 16*Jack + 4*Queen + King.
  // The lookup is done with 16*King * 4*Queen + Jack.
  // The dispatch table includes both this translation and the
  // fact that one function sometime covers several combination.

  SolveStoppedFunction[0] = &LoopHold::SolveStopped0;
  SolveStoppedFunction[1] = &LoopHold::SolveStopped1;
  SolveStoppedFunction[2] = &LoopHold::SolveStopped2;
  SolveStoppedFunction[3] = &LoopHold::SolveStopped48;
  SolveStoppedFunction[4] = &LoopHold::SolveStopped1;
  SolveStoppedFunction[5] = &LoopHold::SolveStopped20;
  SolveStoppedFunction[6] = &LoopHold::SolveStopped36;
  SolveStoppedFunction[7] = &LoopHold::SolveStopped28;
  SolveStoppedFunction[8] = &LoopHold::SolveStopped2;
  SolveStoppedFunction[9] = &LoopHold::SolveStopped18;

  SolveStoppedFunction[10] = &LoopHold::SolveStopped2;
  SolveStoppedFunction[11] = &LoopHold::SolveStopped26;
  SolveStoppedFunction[12] = &LoopHold::SolveStopped12;
  SolveStoppedFunction[13] = &LoopHold::SolveStopped28;
  SolveStoppedFunction[14] = &LoopHold::SolveStopped44;
  SolveStoppedFunction[15] = &LoopHold::SolveStopped60;
  SolveStoppedFunction[16] = &LoopHold::SolveStopped1;
  SolveStoppedFunction[17] = &LoopHold::SolveStopped15;
  SolveStoppedFunction[18] = &LoopHold::SolveStopped9;
  SolveStoppedFunction[19] = &LoopHold::SolveStopped49;

  SolveStoppedFunction[20] = &LoopHold::SolveStopped5;
  SolveStoppedFunction[21] = &LoopHold::SolveStopped5;
  SolveStoppedFunction[22] = &LoopHold::SolveStopped5;
  SolveStoppedFunction[23] = &LoopHold::SolveStopped5;
  SolveStoppedFunction[24] = &LoopHold::SolveStopped9;
  SolveStoppedFunction[25] = &LoopHold::SolveStopped15;
  SolveStoppedFunction[26] = &LoopHold::SolveStopped41;
  SolveStoppedFunction[27] = &LoopHold::SolveStopped57;
  SolveStoppedFunction[28] = &LoopHold::SolveStopped7;
  SolveStoppedFunction[29] = &LoopHold::SolveStopped15;

  SolveStoppedFunction[30] = &LoopHold::SolveStopped7;
  SolveStoppedFunction[31] = &LoopHold::SolveStopped15;
  SolveStoppedFunction[32] = &LoopHold::SolveStopped2;
  SolveStoppedFunction[33] = &LoopHold::SolveStopped18;
  SolveStoppedFunction[34] = &LoopHold::SolveStopped2;
  SolveStoppedFunction[35] = &LoopHold::SolveStopped26;
  SolveStoppedFunction[36] = &LoopHold::SolveStopped6;
  SolveStoppedFunction[37] = &LoopHold::SolveStopped20;
  SolveStoppedFunction[38] = &LoopHold::SolveStopped14;
  SolveStoppedFunction[39] = &LoopHold::SolveStopped28;

  SolveStoppedFunction[40] = &LoopHold::SolveStopped2;
  SolveStoppedFunction[41] = &LoopHold::SolveStopped26;
  SolveStoppedFunction[42] = &LoopHold::SolveStopped2;
  SolveStoppedFunction[43] = &LoopHold::SolveStopped18;
  SolveStoppedFunction[44] = &LoopHold::SolveStopped14;
  SolveStoppedFunction[45] = &LoopHold::SolveStopped28;
  SolveStoppedFunction[46] = &LoopHold::SolveStopped6;
  SolveStoppedFunction[47] = &LoopHold::SolveStopped60;
  SolveStoppedFunction[48] = &LoopHold::SolveStopped3;
  SolveStoppedFunction[49] = &LoopHold::SolveStopped19;

  SolveStoppedFunction[50] = &LoopHold::SolveStopped11;
  SolveStoppedFunction[51] = &LoopHold::SolveStopped15;
  SolveStoppedFunction[52] = &LoopHold::SolveStopped7;
  SolveStoppedFunction[53] = &LoopHold::SolveStopped15;
  SolveStoppedFunction[54] = &LoopHold::SolveStopped7;
  SolveStoppedFunction[55] = &LoopHold::SolveStopped15;
  SolveStoppedFunction[56] = &LoopHold::SolveStopped11;
  SolveStoppedFunction[57] = &LoopHold::SolveStopped27;
  SolveStoppedFunction[58] = &LoopHold::SolveStopped43;
  SolveStoppedFunction[59] = &LoopHold::SolveStopped15;

  SolveStoppedFunction[60] = &LoopHold::SolveStopped15;
  SolveStoppedFunction[61] = &LoopHold::SolveStopped15;
  SolveStoppedFunction[62] = &LoopHold::SolveStopped15;
  SolveStoppedFunction[63] = &LoopHold::SolveStopped15;
}


LoopHold::~LoopHold()
{
}


int LoopHold::GetNumTopsOverOpp(
  const posType opp) const
{
  if (length[opp] == 0)
    return 0;

  int oppRank = completeList[opp][0];
    
  int aceOverOpp = 0;
  bool found = false;

  for (int i = 0; i < static_cast<int>(length[QT_ACE]) && ! found; i++)
  {
    if (completeList[QT_ACE][i] > oppRank)
      aceOverOpp++;
    else
      found = true;
  }

  return Min(aceOverOpp, static_cast<int>(length[opp]));
}


void LoopHold::SolveShortAce(int& rank)
{
  int l = static_cast<int>(Max(length[QT_LHO], length[QT_RHO]));
  // l = Max(l, length[QT_PARD]);
  // if (length[QT_PARD] > l && length[QT_ACE] > length[QT_PARD])
  // if (length[QT_PARD] > l)
    // l++;
  if (l == 0 && length[QT_PARD] > 0)
    l++;
  else if (static_cast<int>(length[QT_PARD]) > l && length[QT_ACE] != length[QT_PARD])
  {
    if (length[QT_ACE] > length[QT_PARD])
      l++;
    else
      l = static_cast<int>(length[QT_ACE]);
  }

  l = Min(l, static_cast<int>(length[QT_ACE]));

  rank = SDS_VOID - l;
}


void LoopHold::CashoutAce(
  int& tricks,
  int& ranks)
{
  int la = static_cast<int>(length[QT_ACE]);
  int ll = static_cast<int>(length[QT_LHO]);
  int lr = static_cast<int>(length[QT_RHO]);

  int numCashingLho = LoopHold::GetNumTopsOverOpp(QT_LHO);
  int numCashingRho = LoopHold::GetNumTopsOverOpp(QT_RHO);

  int tricksOverLho = (numCashingLho >= ll ? la : numCashingLho);
  int tricksOverRho = (numCashingRho >= lr ? la : numCashingRho);

  int numTricks = Min(tricksOverLho, tricksOverRho);

  int numCashing = (numTricks < la ? numTricks :
    Max(numCashingLho, numCashingRho));

  if (numCashing == 0)
  {
    tricks = numTricks;
    ranks = SDS_VOID;
    return;
  }

  int aceRank = completeList[QT_ACE][0];
  int s = 0;

  for (int i = 0; i < la; i++)
  {
    s++;
    if (s < numCashing)
      continue;

    tricks = numTricks;
    ranks = SDS_ACE - (aceRank - completeList[QT_ACE][i]);
    return;
  }

  assert(false);
}


bool LoopHold::CashoutPard(
  int& tricks,
  int& rank)
{
  // Aceholder has blocking tops that also count: AK / JT9.
  int la = static_cast<int>(length[QT_ACE]);
  int lp = static_cast<int>(length[QT_PARD]);
  int ll = static_cast<int>(length[QT_LHO]);
  int lr = static_cast<int>(length[QT_RHO]);
  int ld = lp - la;

  if (ll <= la && lr <= la)
  {
    tricks = ld;
    rank = SDS_VOID;
    return true;
  }

  int numCashingLho = static_cast<int>(Holding::PlayerTopsOverRank(QT_PARD, 
    completeList[QT_LHO][0]));
  int numCashingRho = static_cast<int>(Holding::PlayerTopsOverRank(QT_PARD, 
    completeList[QT_RHO][0]));

  numCashingLho = Min(numCashingLho, ld);
  numCashingRho = Min(numCashingRho, ld);

  int tricksOverLho = (numCashingLho + la >= ll ? ld : numCashingLho);
  int tricksOverRho = (numCashingRho + la >= lr ? ld : numCashingRho);

  tricks = Min(tricksOverLho, tricksOverRho);
  if (tricks == 0)
    return false;

  int numPardTops = Max(ll-la, lr-la);
  numPardTops = Min(numPardTops, tricks);
  rank = static_cast<int>(Holding::ListToRank(completeList[QT_PARD][numPardTops-1]));
  return true;
}


#define SLF 14
#define COF 0x9002aa

bool LoopHold::CashoutBoth(
  DefList& def,
  int& lowestRank)
{
  posType pLong, pShort;
  if (length[QT_ACE] >= length[QT_PARD])
  {
    pLong = QT_ACE;
    pShort = QT_PARD;
  }
  else
  {
    pLong = QT_PARD;
    pShort = QT_ACE;
  }

  int lenLong = static_cast<int>(length[pLong]);
  int lenShort = static_cast<int>(length[pShort]);
  assert(lenLong > 0);
  assert(lenShort > 0);

  int pOppHighest, pOppLowest;
  int lenOppHighest, lenOppLowest;
  if (length[QT_LHO] == 0)
  {
    pOppHighest = QT_RHO;
    pOppLowest = QT_LHO;
    lenOppHighest = static_cast<int>(length[QT_RHO]);
    lenOppLowest = 0;
  }
  else if (length[QT_RHO] == 0)
  {
    pOppHighest = QT_LHO;
    pOppLowest = QT_RHO;
    lenOppHighest = static_cast<int>(length[QT_LHO]);
    lenOppLowest = 0;
  }
  else
  {
    int mlhi = cardListHi[QT_LHO][0];
    int mllo = cardListLo[QT_LHO][0];
    int mrhi = cardListHi[QT_RHO][0];
    int mrlo = cardListLo[QT_RHO][0];

    if (mlhi+1 == mrlo || mrhi+1 == mllo)
    {
      // Really the same rank: Ignore the shorter opponent.
      if (length[QT_LHO] >= length[QT_RHO])
      {
        pOppHighest = QT_LHO;
        pOppLowest = QT_RHO;
        lenOppHighest = static_cast<int>(length[QT_LHO]);
        lenOppLowest = 0;
      }
      else
      {
        pOppHighest = QT_RHO;
        pOppLowest = QT_LHO;
        lenOppHighest = static_cast<int>(length[QT_RHO]);
        lenOppLowest = 0;
      }
    }
    else if (mlhi > mrhi)
    {
      if (length[QT_RHO] <= length[QT_LHO])
      {
        // Worse suit is also shorter.
        pOppHighest = QT_LHO;
        pOppLowest = QT_RHO;
        lenOppHighest = static_cast<int>(length[QT_LHO]);
        lenOppLowest = 0;
      }
      else
      {
        pOppHighest = QT_LHO;
        pOppLowest = QT_RHO;
        lenOppHighest = static_cast<int>(length[QT_LHO]);
        lenOppLowest = static_cast<int>(length[QT_RHO]);
      }
    }
    else if (length[QT_LHO] <= length[QT_RHO])
    {
      // Worse suit is also shorter.
      pOppHighest = QT_RHO;
      pOppLowest = QT_LHO;
      lenOppHighest = static_cast<int>(length[QT_RHO]);
      lenOppLowest = 0;
    }
    else
    {
      pOppHighest = QT_RHO;
      pOppLowest = QT_LHO;
      lenOppHighest = static_cast<int>(length[QT_RHO]);
      lenOppLowest = static_cast<int>(length[QT_LHO]);
    }
  }

  pickFlag = true;

  assert(lenOppHighest > 0 || lenOppLowest > 0);
  int lenOppMax = Max(lenOppHighest, lenOppLowest);

  int oppMaxHighest = completeList[pOppHighest][0];
  int oppMaxLowest = completeList[pOppLowest][0];

  int numTopsLongHigh = static_cast<int>(Holding::PlayerTopsOverRank(pLong, oppMaxHighest));
  int numTopsShortHigh = static_cast<int>(Holding::PlayerTopsOverRank(pShort, oppMaxHighest));
  int numTopsLongLow = static_cast<int>(Holding::PlayerTopsOverRank(pLong, oppMaxLowest));
  int numTopsShortLow = static_cast<int>(Holding::PlayerTopsOverRank(pShort, oppMaxLowest));

  int xLongHigh = lenLong - numTopsLongHigh;
  int xShortHigh = lenShort - numTopsShortHigh;

  assert(xLongHigh > 0 && xShortHigh > 0);

  int numTopsHigh = numTopsLongHigh + numTopsShortHigh;
  int numTopsLow = numTopsLongLow + numTopsShortLow;

  int lenCashHigh = Min(lenLong, lenOppHighest);
  int lenCashLow = Min(lenLong, lenOppLowest);

  int minAce = completeList[QT_ACE][length[QT_ACE]-1];
  int maxPard = completeList[QT_PARD][0];

  int minOpp;
  if (length[QT_LHO] == 0)
    minOpp = completeList[QT_RHO][0];
  else if (length[QT_RHO] == 0)
    minOpp = completeList[QT_LHO][0];
  else
    minOpp = Min(completeList[QT_LHO][0], completeList[QT_RHO][0]);

  Trick trick, trick2;


// cout << "len " << lenShort << "-" << lenLong << 
  // ", numTopsLong " << numTopsLongLow << "-" << numTopsLongHigh <<
  // ", numTopsShort numTopsShortLow << "-" << numTopsShortHigh) << "\n";

  // ===================================================================
  //
  // lenLong == lenShort
  //
  // ===================================================================

  if (lenLong == lenShort)
  {
    if (numTopsHigh < lenCashHigh || numTopsLow < lenCashLow)
    {
      if (pickFlag) holdCtr[900]++;
      return false;
    }
    else if (numTopsLongHigh > 0 && numTopsShortHigh > 0 &&
      numTopsHigh == lenLong && lenLong <= lenOppMax)
    {
      // AJ / Q98 / KT / -.
      if (pickFlag) holdCtr[901]++;
      trick.Set(QT_BOTH, QT_BOTH, SDS_VOID-lenLong, static_cast<unsigned>(lenLong));

      return def.Set1(trick);
    }
    else if (numTopsHigh >= lenOppLowest)
    {
      if (numTopsLongHigh > 0 && numTopsShortHigh > 0)
      {
        lowestRank = SDS_VOID - lenOppMax;

        int n = Min(completeList[QT_ACE][0], maxPard);
        int m = static_cast<int>(Holding::ListToRank(n));
        if (m >= lowestRank)
        {
          if (pickFlag) holdCtr[904]++;
          trick.Set(QT_BOTH, QT_BOTH, lowestRank, static_cast<unsigned>(lenLong));
          return def.Set1(trick);
        }
        else
        {
          if (pickFlag) holdCtr[905]++;
          int r2 = lowestRank;
          lowestRank = m;

          if (lowestRank > r2)
          {
            assert(false);
            trick.Set(QT_PARD, QT_ACE, r2, static_cast<unsigned>(lenLong));
            trick2.Set(QT_ACE, QT_BOTH, lowestRank, 
              static_cast<unsigned>(lenLong));
          }
          else
          {
            trick.Set(QT_BOTH, QT_ACE, r2, static_cast<unsigned>(lenLong));
            trick2.Set(QT_ACE, QT_PARD, lowestRank, 
              static_cast<unsigned>(lenLong));
          }
          def.Set11(trick, trick2);
          // def.SetAsymmCash(lenLong, lowestRank, r2);
          return true;
        }
      }
      else if (minAce > maxPard)
      {
        // AKT / - / 987 / QJ.
        if (pickFlag) holdCtr[902]++;
        int n = completeList[QT_ACE][lenOppMax-1];
        lowestRank = static_cast<int>(Holding::ListToRank(n));
        trick.Set(QT_BOTH, QT_ACE, lowestRank, 
          static_cast<unsigned>(lenLong));
        return def.Set1(trick);
      }
      else
      {
        // AK8 / Q7 / JT9 / -.
        if (pickFlag) holdCtr[903]++;
        int r2 = static_cast<int>(Holding::ListToRank(completeList[QT_ACE][lenOppMax-1]));
        lowestRank = static_cast<int>(Holding::ListToRank(maxPard));
        // No particular reason, just the way it comes out.
        // posType p = (numTopsHigh > lenOppHighest ||
          // completeList[QT_ACE][numTopsHigh] >
          // maxPard ? QT_PARD : QT_PARD);
          // maxPard ? QT_BOTH : QT_PARD);

        trick.Set(QT_BOTH, QT_ACE, r2, 
          static_cast<unsigned>(lenLong));
        trick2.Set(QT_ACE, QT_PARD, lowestRank, 
          static_cast<unsigned>(lenLong));
        def.Set11(trick, trick2);
        // def.SetAsymmCash(lenLong, lowestRank, r2, p);
        return true;
      }
    }
    else if (numTopsLongLow > 0 && numTopsShortLow > 0 &&
      numTopsHigh >= lenOppHighest &&
      numTopsLow >= lenCashLow)
    {
      // Skip over lenCashLow top cards with declarer's side.
      int na = 0, np = 0, no = 0;
      while (no < lenCashLow)
      {
        no++;
        if (completeList[QT_ACE][na] > completeList[QT_PARD][np])
          na++;
        else
          np++;
      }

      int pa = completeList[QT_ACE][na-1];
      int pl = (na == no ? pa : Min(pa, completeList[QT_PARD][np-1]));

      if (maxPard >= pl)
      {
        if (pickFlag) holdCtr[906]++;
        lowestRank = static_cast<int>(Holding::ListToRank(pl));
        trick.Set(QT_BOTH, QT_BOTH, lowestRank, static_cast<unsigned>(lenLong));
        return def.Set1(trick);
      }
      else if (minAce > maxPard)
      {
        if (pickFlag) holdCtr[907]++;
        lowestRank = static_cast<int>(Holding::ListToRank(
          completeList[QT_ACE][lenCashLow-1]));
        trick.Set(QT_BOTH, QT_ACE, lowestRank, static_cast<unsigned>(lenLong));
        return def.Set1(trick);
      }
      else
      {
        if (pickFlag) holdCtr[908]++;
        lowestRank = static_cast<int>(Holding::ListToRank(maxPard));
        int r2 = static_cast<int>(Holding::ListToRank(pa));
        assert(lowestRank < r2);
        /*
        if (lowestRank > r2)
        {
          trick.Set(QT_PARD, QT_ACE, r2, lenLong);
          trick2.Set(QT_ACE, QT_BOTH, lowestRank, lenLong);
          holdCtr[898]++;
        }
        else
        {
        */
          trick.Set(QT_BOTH, QT_ACE, r2, static_cast<unsigned>(lenLong));
          trick2.Set(QT_ACE, QT_PARD, lowestRank, static_cast<unsigned>(lenLong));
          // holdCtr[899]++;
        // }
        def.Set11(trick, trick2);
        // def.SetAsymmCash(lenLong, lowestRank, r2);
        return true;
      }
    }
    else if (minAce > maxPard)
    {
      if (pickFlag) holdCtr[909]++;
      lowestRank = static_cast<int>(Holding::ListToRank(
        completeList[QT_ACE][lenCashLow-1]));
      trick.Set(QT_BOTH, QT_ACE, lowestRank, static_cast<unsigned>(lenLong));
      return def.Set1(trick);
    }
    else
    {
      if (pickFlag) holdCtr[910]++;
      lowestRank = static_cast<int>(Holding::ListToRank(maxPard));
      int r2 = static_cast<int>(Holding::ListToRank(completeList[QT_ACE][lenCashLow-1]));
      // posType p = (completeList[QT_ACE][lenCashLow] >
        // maxPard ? QT_PARD : QT_PARD);
        // maxPard ? QT_BOTH : QT_PARD);

      trick.Set(QT_BOTH, QT_ACE, r2, static_cast<unsigned>(lenLong));
      trick2.Set(QT_ACE, QT_PARD, lowestRank, static_cast<unsigned>(lenLong));
      def.Set11(trick, trick2);
      // def.SetAsymmCash(lenLong, lowestRank, r2, p);
      return true;
    }
  }


  // ===================================================================
  //
  // lenLong != lenShort
  //
  // ===================================================================

  if (numTopsHigh < lenCashHigh || numTopsLow < lenCashLow)
  {
    if (pickFlag) holdCtr[820]++;
    return false;
  }
  else if (length[QT_PARD] > length[QT_ACE] && minAce > maxPard)
  {
    // Blocked.
    if (pickFlag) holdCtr[821]++;
    return false;
  }
  else if (numTopsHigh >= lenOppMax)
  {
    assert(lenLong > lenOppMax);
    if (numTopsLongHigh > 0 && numTopsShortHigh > 0)
    {
      int m = SDS_VOID - lenOppMax;
      int n = static_cast<int>(Holding::ListToRank(maxPard));
      if (n >= m)
      {
        int maxOpp = Max(completeList[QT_RHO][0],
          completeList[QT_LHO][0]);

        if (! LoopHold::GetAsymmRanks(pLong, pShort, lenOppMax,
          maxOpp, lowestRank))
        {
          if (pickFlag) holdCtr[822]++;
          return false;
        }
        else
        {
          if (pickFlag) holdCtr[920]++;
          trick.Set(QT_BOTH, QT_BOTH, lowestRank, static_cast<unsigned>(lenLong));
          return def.Set1(trick);
        }
      }
      else
      {
        if (pickFlag) holdCtr[921]++;
        // Holding::Print();
        lowestRank = n;
        trick.Set(QT_BOTH, QT_BOTH, lowestRank, static_cast<unsigned>(lenLong));
        return def.Set1(trick);
      }
    }
    else if (maxPard < minAce && length[QT_ACE] > length[QT_PARD])
    {
      if (pickFlag) holdCtr[999]++;
      return false;
      // Holding::Print();
      // Is probably right -- check recursion.
      // -------------------------------------
      // int lowestRank = Holding::ListToRank(
        // completeList[pLong][lenOppMax-1]);
      // trick.Set(QT_BOTH, pLong, lowestRank, lenLong);
      // return def.Set1(trick);
      // return def.Set(QT_BOTH, pLong, lowestRank, lenLong);
    }
    else if (lenShort <= lenOppHighest ||
      (lenShort <= lenOppLowest && maxPard < minOpp))
    {
      if (pickFlag) holdCtr[922]++;
      int lRank = static_cast<int>(Holding::ListToRank(
        completeList[pLong][lenOppMax-1]));
      trick.Set(QT_BOTH, pLong, lRank, static_cast<unsigned>(lenLong));
      return def.Set1(trick);
    }
    else if (length[QT_PARD] > length[QT_ACE])
    {
      if (pickFlag) holdCtr[923]++;
      lowestRank = static_cast<int>(Holding::ListToRank(maxPard));
      trick.Set(QT_BOTH, QT_BOTH, lowestRank, static_cast<unsigned>(lenLong));
      return def.Set1(trick);
    }
    else if (static_cast<int>(length[QT_PARD]) == lenOppHighest + 1)
    {
      // Either blocked, or block/crash.
      if (pickFlag) holdCtr[823]++;
      return false;
    }
    else if (lenOppLowest > lenOppHighest)
    {
      // Not addressed yet
      // -------------------------------------
      if (pickFlag) holdCtr[998]++;
      // Holding::Print();
    }
    else if (completeList[QT_ACE][lenOppHighest] < maxPard)
    {
      if (pOppHighest == QT_RHO)
      {
        // Might be finesses, so don't attempt.
        // AK654 / - / JT98 / Q7.
        // -------------------------------------
        if (pickFlag) holdCtr[824]++;
        return false;
      }
      if (completeList[QT_ACE][lenOppHighest] <
        completeList[QT_PARD][length[QT_PARD]-1])
      {
        // Blocked
        if (pickFlag) holdCtr[825]++;
        return false;
      }
      else
      {
        if (pickFlag) holdCtr[924]++;
        lowestRank = static_cast<int>(Holding::ListToRank(
          completeList[QT_ACE][lenOppHighest]));
        trick.Set(QT_BOTH, QT_BOTH, lowestRank, static_cast<unsigned>(lenLong));
        return def.Set1(trick);
      }
    }
    else
    {
      if (pickFlag) holdCtr[929]++;
      return false;

      int r2 = static_cast<int>(Holding::ListToRank( completeList[QT_ACE][lenOppHighest]));
      lowestRank = static_cast<int>(Holding::ListToRank(maxPard));
      // Holding::Print();


      trick.Set(QT_BOTH, QT_ACE, r2, static_cast<unsigned>(lenLong));
      trick2.Set(QT_ACE, QT_BOTH, lowestRank, static_cast<unsigned>(lenLong));
      def.Set11(trick, trick2);
      // def.SetAsymmCash(lenLong, lowestRank, r2, QT_BOTH);
      return true;
    }
  }
  else if (numTopsLongLow > 0 && numTopsShortLow > 0 &&
    lenOppLowest > 0 && numTopsHigh >= lenOppHighest)
  {
    // if (pickFlag) holdCtr[921]++;
    // Holding::Print();
  }


  if (pickFlag) holdCtr[932]++;

  if (numTopsLongHigh == 0 || numTopsShortHigh == 0)
  {
    if (pickFlag) holdCtr[800]++;
    return false;
  }


  if (numTopsLongLow + numTopsShortHigh < lenOppLowest)
  {
    if (pickFlag) holdCtr[804]++;
    return false;
  }
  else if (lenLong == lenShort)
  {
    assert(false); // Why?
  }
  else
  {
    if (lenShort <= numTopsHigh)
    {
      // This is the general case where we can also cash out the 
      // longer opponent with the lower highest card.  Our short
      // partner's intermediate cards are ignored, as they will fall.
      // Example: AJ / Qx / KTx / 987, the jack won't matter for here.

      int minorsNeeded = 
        Min(lenOppLowest, lenLong) - numTopsLongHigh - numTopsShortHigh;

      // Skip over the long hand's tops.
      int i = 0;
      while (i < lenLong && completeList[pLong][i] > oppMaxHighest)
        i++;

      int c = 1; // We're at the first minor card.
      while (i < lenLong && c < minorsNeeded)
      {
        i++;
        c++;
      }

      int shortSecond = completeList[pShort][numTopsShortHigh];

if (shortSecond > completeList[pLong][i])
{
  // AK98 / J / QT7 / 6543.  The T also cashes over the 6.
  // Skip for now.
  if (pickFlag) holdCtr[805]++;
  return false;
}
      if (pickFlag) holdCtr[706]++;
      assert(i < lenLong);
      lowestRank = static_cast<int>(Holding::ListToRank(completeList[pLong][i]));
      trick.Set(QT_BOTH, QT_BOTH, lowestRank, 
        static_cast<unsigned>(lenLong));
      return def.Set1(trick);
    }
    else
    {
      assert(numTopsHigh == 2);
      if (lenOppHighest == 1)
      {
        if (lenOppLowest == 3)
        {
          if (lenShort == 4)
          {
            // Too complex, e.g. A765 / T98 / KJ432 / Q blocks.
            if (pickFlag) holdCtr[806]++;
            return false;
          }
          else if (completeList[pLong][numTopsLongHigh] <
                   completeList[pShort][numTopsShortHigh])
          {
            // Don't want the jack on the short side.
            if (pickFlag) holdCtr[807]++;
            return false;
          }
          else
          {
            // 4=3, 5=3, 6=3.
            if (pickFlag) holdCtr[707]++;
            lowestRank = static_cast<int>(Holding::ListToRank(
              completeList[pLong][numTopsLongHigh]));

            trick.Set(QT_BOTH, QT_BOTH, lowestRank, 
              static_cast<unsigned>(lenLong));
            return def.Set1(trick);
          }
        }
        else // (lenOppLowest == 4 or 5)
        {
          if (completeList[pLong][numTopsLongHigh] <
                   completeList[pShort][numTopsShortHigh])
          {
            // Don't want the jack on the short side.
            if (pickFlag) holdCtr[808]++;
            return false;
          }
          else
          {
            // Need JT.
            if (pickFlag) holdCtr[708]++;
            lowestRank = static_cast<int>(Holding::ListToRank(
                   completeList[pLong][numTopsLongHigh] - 1));
            trick.Set(QT_BOTH, QT_BOTH, lowestRank, 
              static_cast<unsigned>(lenLong));
            return def.Set1(trick);
          }
        }
      }
      else // lenOppHighest == 2
      {
        if (lenOppLowest == 3)
        {
          if (completeList[pLong][numTopsLongHigh] <
                   completeList[pShort][numTopsShortHigh])
          {
            // Don't want the "jack" on the short side.
            if (pickFlag) holdCtr[809]++;
            return false;
          }
          else
          {
            if (pickFlag) holdCtr[709]++;
            lowestRank = static_cast<int>(Holding::ListToRank(
                   completeList[pLong][numTopsLongHigh]));
            trick.Set(QT_BOTH, QT_BOTH, lowestRank, 
              static_cast<unsigned>(lenLong));
            return def.Set1(trick);
          }
        }
        else
        {
          // 4=2=3=4 with Qx, possibly QJ stiff.
// AJ7 / Q8 / KT93 / 6542, 13/0x9293fb, what's even right?
            if (pickFlag) holdCtr[810]++;
return false;

          lowestRank = static_cast<int>(Holding::ListToRank(
                 completeList[pLong][numTopsLongHigh+1]));
          trick.Set(QT_BOTH, QT_BOTH, lowestRank, 
            static_cast<unsigned>(lenLong));
          return def.Set1(trick);
        }
      }
    }
  }

  return false;
}


void LoopHold::SetDetails(
  HoldingDetails& hdet)
{
  hdet.declLen = static_cast<int>(length[QT_ACE] + length[QT_PARD]);

  hdet.cFlipped = counter;
  hdet.cFlippedUp = counter;
  hdet.maskFull = (1 << (2*(suitLength-1))) - 1;
  for (int n = 0; n < static_cast<int>(suitLength)-1; n++)
  {
    int mask = 0x3 << (2*n);
    int p = counter & mask;
    // TODO: This look wrong! Only works for bottom bit?
    if (p == QT_ACE)
      hdet.cFlipped = (hdet.cFlipped & (hdet.maskFull ^ mask)) | 
        (QT_PARD << (2*n));
  }

  for (int n = 0; n < static_cast<int>(suitLength)-1; n++)
  {
    int mask = 0x3 << (2*n);
    int p = (counter >> (2*n)) & 0x3;
    if (p == QT_PARD)
      hdet.cFlippedUp = (hdet.cFlippedUp & (hdet.maskFull ^ mask)) | 
        (QT_ACE << (2*n));
  }

  hdet.lenMaxOpp = static_cast<int>(Max(length[QT_LHO], length[QT_RHO]));

  if (length[QT_ACE] >= length[QT_PARD])
  {
    hdet.pLong = QT_ACE;
    hdet.pShort = QT_PARD;
  }
  else
  {
    hdet.pLong = QT_PARD;
    hdet.pShort = QT_ACE;
  }

  hdet.lenLong = static_cast<int>(length[hdet.pLong]);
  hdet.lenShort = static_cast<int>(length[hdet.pShort]);

  int maxCardOpps = (hdet.lenMaxOpp == 0 ? -1 :
    Max(completeList[QT_LHO][0], completeList[QT_RHO][0]));

  LoopHold::UpdateDetailsForOpp(hdet, maxCardOpps, false, QT_ACE);
}


int LoopHold::ShiftTops(
  const HoldingDetails& hdet,
  const int moveNumTops)
{
  // Effectively, first flip the entire ACE/PARD holdings, and 
  // then flip a number of tops back to ACE.  So one might say that
  // the defenders have been swapped relative to the starting point,
  // but it doesn't matter.

  int maskTop = ((1 << (2*(moveNumTops-1))) - 1) << 
    (2*(static_cast<int>(suitLength)-moveNumTops));

  return (maskTop & counter) | ((hdet.maskFull ^ maskTop) & hdet.cFlipped);
}


bool LoopHold::GetAsymmRanks(
  const int pLong,
  const int pShort,
  const int cashLength,
  const int toBeat,
  int& lowestRank)
{
  // First cash on the short side, then if necessary the long side.
  // After that, if the first unused card on the long side is large
  // enough, then for sure the suit doesn't block.  Otherwise it might.

  int cashed = 0;
  while (cashed < static_cast<int>(length[pShort]) &&
    cashed < cashLength &&
    completeList[pShort][cashed] > toBeat)
  {
    cashed++;
  }

  int shortStop = cashed;
  int longStop = 0;

  if (cashed < cashLength)
  {
    while (longStop < static_cast<int>(length[pLong]) &&
      cashed < cashLength &&
      completeList[pLong][longStop] > toBeat)
    {
      cashed++;
      longStop++;
    }
  }

// cout << "longStop " << longStop << shortStop " << shortStop" << \n";

  // Couldn't cash enough tops.
  if (cashed < cashLength)
    return false;

  // Blocked for sure.
  if (completeList[pLong][longStop] < completeList[pShort][shortStop])
      // completeList[pShort][length[pShort]-1])
    return false;

  if (shortStop == 0)
  {
    // Might be QT_BOTH or pLong.  For now we give up.
    return false;
  }

  // lowestRank = Min(completeList[pShort][shortStop-1],
    // completeList[pLong][longStop]);
  if (static_cast<int>(length[pShort]) <= cashLength)
  {
    int m = Min(completeList[pShort][shortStop-1],
      completeList[pLong][longStop-1]);
    lowestRank = static_cast<int>(Holding::ListToRank(m));
  }
  else
    lowestRank = static_cast<int>(Holding::ListToRank(completeList[pLong][longStop]));
  
  return true;
}
  


void LoopHold::UpdateDetailsForOpp(
  HoldingDetails& hdet,
  const int& oppRank,
  const bool oppSkippedFlag,
  const posType& oppSkipped)
{
  hdet.numTopsLong = 0;
  hdet.numTopsShort = 0;

  int i = 0;
  while (i < hdet.lenLong && completeList[hdet.pLong][i] > oppRank)
  {
    hdet.numTopsLong++;
    hdet.minTopLong = completeList[hdet.pLong][i];
    i++;
  }

  i = 0;
  while (i < hdet.lenShort && completeList[hdet.pShort][i] > oppRank)
  {
    hdet.numTopsShort++;
    hdet.minTopShort = completeList[hdet.pShort][i];
    i++;
  }

  if (oppSkippedFlag)
  {
    // Compensate for skipped ranks with other opponent.
    // This is a bit of a kludge -- maybe there is a better way.

    int used[SDS_MAX_RANKS] = {0};
    i = 0;
    int m = Min(hdet.minTopLong, hdet.minTopShort);
    while (i < static_cast<int>(length[oppSkipped]) && completeList[oppSkipped][i] > m)
    {
      used[completeList[oppSkipped][i]] = 1;
      i++;
    }

    i = static_cast<int>(suitLength)-1;
    int c = static_cast<int>(suitLength)-1;
    hdet.mapRealToShifted[suitLength] = static_cast<int>(suitLength); // Void
    hdet.mapShiftedToReal[suitLength] = static_cast<int>(suitLength); // Void

    do
    {
      hdet.mapRealToShifted[i] = c;
      hdet.mapShiftedToReal[c] = i;
// cout << "  forward[" << i << "] = " << c << 
  // ", backward[" << c << "] = " << i << "\n";
      c--;
      i--;
      while (used[i])
      {
        i--;
      }
    }
    while (i >= m);

// cout << "  before shift " << hdet.minTopLong ", " <<
  // hdet.minTopShort << "\n";
    hdet.minTopLong = hdet.mapRealToShifted[hdet.minTopLong];
    hdet.minTopShort = hdet.mapRealToShifted[hdet.minTopShort];
// cout << "  after shift << hdet.minTopLong << ", " <<
  // hdet.minTopShort) << "\n";
  }

  hdet.maxTopLong = completeList[hdet.pLong][0];
  hdet.maxTopShort = completeList[hdet.pShort][0];

  int delta = SDS_VOID - static_cast<int>(suitLength);
// cout << "delta " << delta " << "\n";
  hdet.maxTopLong += delta;
  hdet.minTopLong += delta;
  hdet.maxTopShort += delta;
  hdet.minTopShort += delta;

  hdet.xLong = hdet.lenLong - hdet.numTopsLong;
  hdet.xShort = hdet.lenShort - hdet.numTopsShort;
  hdet.numTopsAll = hdet.numTopsLong + hdet.numTopsShort;
}


void LoopHold::PrintDetails(
  const HoldingDetails& hdet)
{
  cout << "Long player " << static_cast<int>(hdet.pLong) << 
    ", short " << static_cast<int>(hdet.pShort) << "\n";
  cout << "Lengths long " << hdet.lenLong << 
    ", short " << hdet.lenShort << 
    ", maxOpp " << hdet.lenMaxOpp << "\n";
  cout << "Tops long " << hdet.numTopsLong <<
    ", short " << hdet.numTopsShort << 
    ", together " << hdet.numTopsAll << "\n";
  cout << "Long " << hdet.minTopLong << "-" << hdet.maxTopLong <<
    ", short " << hdet.minTopShort << " - " << hdet.maxTopShort << "\n";
}


void LoopHold::SolveCrashTricks(
  HoldingDetails& hdet,
  const posType& oppBest,
  posType& bend,
  posType& cend,
  int& brank,
  int& rrank,
  int& crank,
  int& crankr2,
  int& btricks,
  int& rtricks,
  int& ctricks)
{
  if (oppBest == QT_BOTH)
  {
    LoopHold::SolveCrashTricksHand(hdet, hdet.lenMaxOpp,
      bend, cend, brank, rrank, crank, crankr2, btricks, rtricks, ctricks);

    // See explanation below.
    if (rtricks > 0 && hdet.lenShort > 1 && btricks+rtricks == ctricks)
      rtricks = 0;
  }
  else if (oppBest == QT_LHO && length[QT_LHO] >= length[QT_RHO])
  {
    LoopHold::SolveCrashTricksHand(hdet, static_cast<int>(length[QT_LHO]),
      bend, cend, brank, rrank, crank, crankr2, btricks, rtricks, ctricks);

    // See explanation below.
    if (rtricks > 0 && hdet.lenShort > 1 && btricks+rtricks == ctricks)
      rtricks = 0;
  }
  else if (oppBest == QT_RHO && length[QT_RHO] >= length[QT_LHO])
  {
    LoopHold::SolveCrashTricksHand(hdet, static_cast<int>(length[QT_RHO]),
      bend, cend, brank, rrank, crank, crankr2, btricks, rtricks, ctricks);

    // See explanation below.
    if (rtricks > 0 && hdet.lenShort > 1 && btricks+rtricks == ctricks)
      rtricks = 0;
  }
  else
  {
// if (suitLength == 13 && counter == 0xa405ff)
 // cout << "LHO " << completeList[QT_LHO][0] <<
 //   << " RHO " << completeList[QT_RHO][0]) << "\n";
    LoopHold::UpdateDetailsForOpp(hdet, completeList[QT_LHO][0],
      true, QT_RHO);
// LoopHold::PrintDetails(hdet);
    LoopHold::SolveCrashTricksHand(hdet, static_cast<int>(length[QT_LHO]),
      bend, cend, brank, rrank, crank, crankr2, btricks, rtricks, ctricks);

// cout << "LHO before shift back " << bend << " " << cend << " " <<
  // ", " << brank << " " << rrank << " " << crank <<
  // ", " << btricks << " " << rtricks << " " << ctricks << "\n";
      int delta = SDS_VOID - static_cast<int>(suitLength);
      brank = hdet.mapShiftedToReal[brank-delta] + delta;
      rrank = hdet.mapShiftedToReal[rrank-delta] + delta;
      crank = hdet.mapShiftedToReal[crank-delta] + delta;
      crankr2 = hdet.mapShiftedToReal[crankr2-delta] + delta;

// cout << "LHO bend " << bend << endl;
// if (suitLength == 13 && counter == 0xa405ff)
// {
// cout << "LHO before shift back " << bend << " " << cend << " " <<
  // ", " << brank << " " << rrank << " " << crank <<
  // ", " << btricks << " " << rtricks << " " << ctricks << "\n";
  // cout << "maxTopShort " << hdet.maxTopShort << "\n";
// }
    posType bend2, cend2;
    int brank2, rrank2, crank2, crank22, btricks2, rtricks2, ctricks2;
    LoopHold::UpdateDetailsForOpp(hdet, completeList[QT_RHO][0],
      true, QT_LHO);
// LoopHold::PrintDetails(hdet);
    LoopHold::SolveCrashTricksHand(hdet, static_cast<int>(length[QT_RHO]),
      bend2, cend2, brank2, rrank2, crank2, crank22,
      btricks2, rtricks2, ctricks2);

      brank2 = hdet.mapShiftedToReal[brank2-delta] + delta;
      rrank2 = hdet.mapShiftedToReal[rrank2-delta] + delta;
      crank2 = hdet.mapShiftedToReal[crank2-delta] + delta;
      crank22 = hdet.mapShiftedToReal[crank22-delta] + delta;


// if (suitLength == 13 && counter == 0xa405ff)
// {
 // cout << "RHO " << bend2 << " " << cend2 <<
   // ", " << brank2 << " " << rrank2 << " " << crank2 << " " <<
   // ", " << btricks2 << " " << rtricks2 << " " << ctricks2 << "\n";
  // cout << "maxTopShort " << hdet.maxTopShort << "\n";
// }

    if (ctricks2 < ctricks || (ctricks2 == ctricks && crank2 < crank))
    {
      cend = cend2;
      crank = crank2;
      ctricks = ctricks2;
    }

    if (crank22 == SDS_VOID)
      crankr2 = SDS_VOID;
    else if (crankr2 != SDS_VOID)
      crankr2 = Min(crank22, crankr2);

    // if (rtricks2 == 0)
      // return;

    if (rtricks2 > 0)
    {

    int m = Min(brank, rrank);
    int m2 = Min(brank2, rrank2);
// cout << "m " << m << " m2 " << m2 << "\n";
// cout << "bend " << bend << " bend2 " << bend2 << endl;
    if (rtricks == 0 || rtricks2+btricks2 < rtricks+btricks ||
       (rtricks2+btricks2 == rtricks+btricks && m2 < m))

    {
      bend = bend2;
      brank = brank2;
      rrank = rrank2;
      btricks = btricks2;
      rtricks = rtricks2;
    }
    }

    // It can happen that the blocking trick from one side is
    // more demanding, while the cashing trick from the other
    // side is more demanding:  AT98 / J76 / KQ / 5432.
    // So we end up with BB49 or BB2Q+AA2T.  This is actually
    // a legitimate way to view the situation, but we probably
    // prefer to reduce this to BB49, as the number of tricks is
    // the same.

    if (rtricks > 0 && hdet.lenShort > 1 && btricks+rtricks == ctricks)
    {
      rtricks = 0;
    }
  }
}


void LoopHold::SolveCrashTricksHand(
  const HoldingDetails& hdet,
  const int& lenOpp,
  posType& bend,
  posType& cend,
  int& brank,
  int& rrank,
  int& crank,
  int& crank2,
  int& btricks,
  int& rtricks,
  int& ctricks)
{
  // The crash trick, always present.

  if (hdet.numTopsAll >= lenOpp+1)
    ctricks = hdet.lenLong;
  else
  {
    ctricks = hdet.numTopsLong;
    if (hdet.lenShort >= 2 && hdet.numTopsAll < hdet.declLen)
      ctricks += Min(hdet.lenShort-1, hdet.xLong);
  }

  if (hdet.lenShort == 1 || 
     (hdet.maxTopShort < hdet.minTopLong && hdet.xLong == 0))
    cend = hdet.pLong;
  else
    cend = QT_BOTH;

  int ocash = Max(lenOpp, 1);
  int ccash = SDS_VOID - Min(ocash, ctricks);

  int cextra = ccash;
  if ((ccash <= hdet.minTopShort && hdet.xShort == 0) || 
     (hdet.lenShort >= 2 && lenOpp <= 1))
    cextra--;

  int cspecial = SDS_VOID;
  if (hdet.lenLong > hdet.lenShort && hdet.lenShort >= 3 &&
      lenOpp < hdet.lenShort)
    cspecial = SDS_VOID - (lenOpp+1);
      // lenOpp == hdet.lenShort-1)
    // cspecial = SDS_VOID - hdet.lenShort;

  crank = Min(cextra, cspecial);
  if (cend == QT_BOTH)
  {
    int minMaxTops = Min(hdet.maxTopShort, hdet.maxTopLong);
    crank = Min(crank, minMaxTops);
  }

  crank2 = SDS_VOID;
  if (lenOpp == 0 && hdet.pShort >= 2 &&
    length[QT_ACE] >= length[QT_PARD])
  {
    // Special case where the cash from the ace side only 
    // requires the ace, no matter what else.
    crank2 = SDS_VOID - 1;
  }
  else if (lenOpp == 1 && length[QT_ACE] == length[QT_PARD])
  {
    // Another special case where the ace is enough, starting
    // from partner's side.  Example AJT9 / Q / K876 / x.
    crank2 = SDS_VOID - 1;
  }
  else if (lenOpp > 0 && lenOpp < static_cast<int>(length[QT_ACE]) &&
    length[QT_ACE] == length[QT_PARD] &&
    completeList[QT_ACE][lenOpp-1] > completeList[QT_PARD][0])
  {
    // AKJ / 87 / QT9 / -.
    crank2 = SDS_VOID - lenOpp;
  }
  else if (length[QT_ACE] > length[QT_PARD] &&
      length[QT_PARD] >= 2 &&
      hdet.maxTopShort <= SDS_ACE - static_cast<int>(length[QT_PARD]) &&
      lenOpp <= SDS_ACE - hdet.maxTopShort)
  {
    crank2 = static_cast<int>(Holding::ListToRank(
      completeList[QT_ACE][Max(lenOpp, static_cast<int>(length[QT_PARD])) - 1]));
  }


  // The block trick, sometimes present (if not, rtricks == 0).

  // To have something, in case of no block.
  brank = SDS_VOID; 
  rrank = SDS_VOID; 

  // Figure out whether there is a block trick.
  bool poss = (hdet.lenLong > 1 &&
    (hdet.declLen > hdet.numTopsAll || 
     hdet.minTopLong < hdet.maxTopShort) ?  true : false);

  bool must = ((hdet.numTopsAll <= Min(lenOpp, hdet.lenLong) ||
    (hdet.xShort == 0 && hdet.minTopShort > crank) ) &&
    hdet.lenLong > hdet.lenShort ? true : false);

  if ((! poss) || (hdet.lenShort > 1 && ! must))
  {
    btricks = 0;
    rtricks = 0;
    return;
  }

  // So now there is a trick.

  btricks = hdet.lenShort;
  brank = hdet.minTopShort;
  bend = hdet.pLong;

  if (hdet.numTopsAll >= lenOpp)
    rtricks = hdet.lenLong - hdet.numTopsShort;
  else
    rtricks = Min(hdet.numTopsLong, hdet.lenLong - hdet.numTopsShort);

  ocash = SDS_VOID - Min(lenOpp, hdet.lenLong);
  if (ocash >= brank)
    rrank = SDS_VOID;
  else
  {
    int x = (hdet.minTopLong >= brank ? SDS_VOID : hdet.minTopLong);
    rrank = Max(ocash, x);
  }
}


bool LoopHold::SolveStopped(
  HoldingSimpleMove& move)
{
  assert(suitLength >= 4);
  assert(length[QT_PARD] > 0);

  htop.K = (counter >> (2*(suitLength-2))) & 0x3;
  htop.Q = (counter >> (2*(suitLength-3))) & 0x3;
  htop.J = (counter >> (2*(suitLength-4))) & 0x3;
  htop.T = (counter >> (2*(suitLength-5))) & 0x3;
  htop.N = (counter >> (2*(suitLength-6))) & 0x3;
  htop.E = (counter >> (2*(suitLength-7))) & 0x3;

  hopp.K = (htop.K == QT_LHO || htop.K == QT_RHO ? true : false);
  hopp.Q = (htop.Q == QT_LHO || htop.Q == QT_RHO ? true : false);
  hopp.J = (htop.J == QT_LHO || htop.J == QT_RHO ? true : false);
  hopp.T = (htop.T == QT_LHO || htop.T == QT_RHO ? true : false);
  hopp.N = (htop.N == QT_LHO || htop.N == QT_RHO ? true : false);
  hopp.E = (htop.E == QT_LHO || htop.E == QT_RHO ? true : false);

  distHex = static_cast<int>((length[QT_ACE] << 12) | (length[QT_LHO] << 8) |
    (length[QT_PARD] << 4) | length[QT_RHO]);

  pickFlag = false;
  // if (! hopp.K && htop.Q == QT_RHO && htop.J == QT_RHO)
    // pickFlag = true;
  pickFlag = true;

  int topIndex = (counter >> (2*(suitLength-4))) & 0x3f;
  return (this->*SolveStoppedFunction[topIndex])(move);
}


inline bool LoopHold::SetMove(
  HoldingSimpleMove& move,
  const posType& start,
  const posType& end,
  const int rank,
  const int tricks)
{
  move.start = start;
  move.end = end;
  move.rank = rank;
  move.tricks = tricks;
  return true;
}


bool LoopHold::StopFinesse(
  const int numFinesses,
  const int firstNonTopAce,
  const int firstNonTopPard,
  const bool ignoreOtherOpp,
  const posType anchor)
{
  // Take 1 finesse through the opponent indicated.
  // Return true if opponent prevents the finesse.

  posType pa, pl, pp, pr;
  int fna, fnp;
  if (anchor == QT_ACE)
  {
    // Finesse through RHO toward the ace.
    pa = QT_ACE;
    pl = QT_LHO;
    pp = QT_PARD;
    pr = QT_RHO;
    fna = firstNonTopAce;
    fnp = firstNonTopPard;
  }
  else
  {
    // Finesse through LHO behind the ace.
    pa = QT_PARD;
    pl = QT_RHO;
    pp = QT_ACE;
    pr = QT_LHO;
    fna = firstNonTopPard;
    fnp = firstNonTopAce;
  }

  bool skipOnePard = false;
  bool skipTwoPard = false;
  int effLenPard = static_cast<int>(length[pp]) - fnp;

  if (! ignoreOtherOpp)
  {
    if (completeList[pl][0] > completeList[pp][fnp] &&
        completeList[pl][0] > completeList[pa][fna])
    {
      if (length[pl] == 1)
      {
        skipOnePard = true;
        effLenPard--;
      }
      else
      {
        skipTwoPard = true;
        effLenPard -= 2;
      }
    }
    else if (length[pl] > 1 &&
      completeList[pl][0] > completeList[pp][fnp+1] &&
      completeList[pl][0] > completeList[pa][fna])
    {
      // if (length[pp] == 2)
      {
        skipOnePard = true;
        effLenPard--;
      }
    }
  }

  if (effLenPard <= 0 || (skipTwoPard && numFinesses <= 2))
  {
    if (completeList[pr][0] < completeList[pa][fna])
      return false;
    else
      return true;
  }


  // Figure out the top situation of RHO vs partner.
  int topOf3 = 1, topOf5 = 1, topOf7 = 1, num = 1;
  int defNo = 1, pNo = fnp;
  while (num < 7 && defNo < static_cast<int>(length[pr]))
  {
    if (completeList[pr][defNo] >= completeList[pp][pNo])
    {
      if (num < 3) topOf3++;
      if (num < 5) topOf5++;
      topOf7++;
      defNo++;
    }
    else
    {
      pNo++;
      if (skipTwoPard && pNo == static_cast<int>(length[pp])-2)
        pNo += 2;
      else if (skipOnePard && pNo == static_cast<int>(length[pp])-1)
        pNo++;
    }
    num++;
  }

#if 0
  cout << "StopFinesse:\n";
  cout << "numFinesses     " << numFinesses << "\n";
  cout << "firstNonTopAce  " << firstNonTopAce << 
    " (" << fna << ")\n";
  cout << "firstNonTopPard " << firstNonTopPard << 
    " (" <<  fnp << "\n";
  cout << "ignoreOtherOpp  " << ignoreOtherOpp << "\n";
  cout << "anchor          " << anchor << "\n";
  cout << "\n";
  cout "skip     " << skipOnePard << " " << skipTwoPard <<
    " eff " << effLenPard << "\n";
  cout << "tops     " << topOf3 << " " << topOf5 << " " topOf7 << "\n";
  cout << "effrho   " <<
    completeList[pr][0] << " " <<
    completeList[pr][1] << " " <<
    completeList[pr][2] << " " <<
    completeList[pr][3]) << "\n";
  count << "efface-x " << completeList[pa][fna] <<
    "effpard-x " << completeList[pp][fnp] << "\n";
#endif

  if (topOf3 < 2 ||
      completeList[pr][0] < completeList[pp][fnp] ||
      completeList[pr][1] < completeList[pa][fna])
    return false;

  if (effLenPard == 1 || numFinesses == 1)
    return true;

  if (topOf5 < 3 ||
      completeList[pr][2] < completeList[pa][fna])
    return false;

  if (effLenPard == 2 || numFinesses == 2 || skipOnePard)
    return true;
  
  if (topOf7 < 4 ||
      completeList[pr][3] < completeList[pa][fna] ||
      completeList[pr][3] < completeList[pp][3])
    return false;

  return true;
}


bool LoopHold::SolveStopped0(HoldingSimpleMove& move)
{
  // ==== G0 ===========================================================
  //      AKQJ+
  // +          +
  //      +
  // ==== G0 ===========================================================

  if (length[QT_ACE] < 5 || htop.T == QT_PARD)
    return false;

  if (htop.T == QT_ACE)
  {
    if (hopp.N && length[htop.N] >= 6 && (htop.N == QT_LHO ||
      LoopHold::StopFinesse(1, 5, 0, true, QT_ACE)))
    {
      // AKQJTx / 9xxxxx / x / -.
      // AKQJTx / - / x / 9xxxxx.
      if (pickFlag) holdCtr[0]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_ACE, SDS_VOID-5, 5);
    }
  }
  else if (length[QT_LHO] >= 5)
  {
    if (completeList[QT_LHO][0] > completeList[QT_ACE][4] &&
      ((completeList[QT_LHO][0] > completeList[QT_PARD][0] ||
       (htop.T == QT_RHO && length[QT_PARD] == 1))))
    {
      // AKQJx+ / xxxxx+ / x+ / ?.
      // AKQJx+ / 8xxxx / 9 / T(x).
      if (pickFlag) holdCtr[1]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_ACE, SDS_VOID-4, 4);
    }
  }
  else if (length[QT_RHO] >= 5)
  {
    if (completeList[QT_RHO][0] < completeList[QT_ACE][4])
      return false;

    if (length[QT_PARD] == 1)
    {
      if (htop.T == QT_LHO || LoopHold::StopFinesse(1, 4, 0, true, QT_ACE))
      {
        // AKQJx+ / ? / x / xxxxx+.
        if (pickFlag) holdCtr[2]++;
        return LoopHold::SetMove(move, QT_BOTH, QT_ACE, SDS_VOID-4, 4);
      }
    }
    else if (length[QT_PARD] == 2)
    {
      if (completeList[QT_LHO][0] > completeList[QT_PARD][0] &&
          completeList[QT_LHO][0] > completeList[QT_ACE][4])
      {
        if (LoopHold::StopFinesse(1, 4, 0, false, QT_ACE))
        {
          // AKQJx+ / ? / x / xxxxx+.
          if (pickFlag) holdCtr[3]++;
          return LoopHold::SetMove(move, QT_BOTH, QT_ACE, SDS_VOID-4, 4);
        }
      }
      else if (LoopHold::StopFinesse(2, 4, 0, true, QT_ACE))
      {
        // AKQJx+ / ? / x / xxxxx+.
        if (pickFlag) holdCtr[4]++;
        return LoopHold::SetMove(move, QT_BOTH, QT_ACE, SDS_VOID-4, 4);
      }
    }
    else if (LoopHold::StopFinesse(3, 4, 0, false, QT_ACE))
    {
      // AKQJx+ / ? / x / xxxxx+.
      if (pickFlag) holdCtr[5]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_ACE, SDS_VOID-4, 4);
    }
  }
  return false;
}


bool LoopHold::SolveStopped1(HoldingSimpleMove& move)
{
  // ==== G1 ================= G4 ================= G16 ================
  //      AQJ+       |         AKJ+       |         AKQ+
  // +          K+   |    +          Q+   |    +          J+
  //      +          |         +          |         + 
  // ==== G1 ================= G4 ================= G16 ================

  int r3 = (htop.J == QT_LHO ? SDS_VOID-3 : SDS_VOID-4);

  if (htop.K == QT_LHO && length[QT_LHO] >= 2)
  {
    // Protected K behind the ace.
    if (pickFlag) holdCtr[10]++;
    return LoopHold::SetMove(move, QT_BOTH, QT_ACE, SDS_VOID-1, 1);
  }
  else if (htop.Q == QT_LHO && length[QT_LHO] >= 3)
  {
    // Protected queen behind AK.
    if (pickFlag) holdCtr[11]++;
    return LoopHold::SetMove(move, QT_BOTH, QT_ACE, SDS_VOID-2, 2);
  }
  else if (htop.J == QT_LHO && length[QT_LHO] >= 4)
  {
    // Protected jack behind AKQ.
    if (pickFlag) holdCtr[12]++;
    return LoopHold::SetMove(move, QT_BOTH, QT_ACE, SDS_VOID-3, 3);
  }

  if (length[QT_RHO] <= 3)
    return false;

  if (length[QT_PARD] <= length[QT_LHO])
  {
    if (length[QT_RHO] >= 5 &&
      completeList[QT_RHO][0] < completeList[QT_ACE][3] &&
      completeList[QT_RHO][0] > completeList[QT_ACE][4])
    {
      // AHHT+ / H / x / 9xxxx+.
      // AHHT+ / H / 9 / 8xxxx+.
      // AHH9+ / H / T / 8xxxx+.
      // AKJ9+ / Qx / x(x) / xxxxx+.
      // AKQ9+ / Jx / x(x) / xxxxx+.
      if (pickFlag) holdCtr[13]++;
      int r = static_cast<int>(Holding::ListToRank(completeList[QT_ACE][3]));
      return LoopHold::SetMove(move, QT_BOTH, QT_ACE, r, 4);
    }
    else if (completeList[QT_RHO][0] < completeList[QT_ACE][2] &&
      completeList[QT_RHO][0] > completeList[QT_ACE][3])
    {
      // AHH+ / H / x / Txxx.
      // AHH+ / H / T / 9xxx.
      // AHHx / Hx / x(x) / xxxx.
      if (pickFlag) holdCtr[14]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_ACE, r3, 3);
    }
  }
  else if (length[QT_ACE] == 3)
  {
    if (completeList[QT_RHO][0] > completeList[QT_PARD][0])
    {
      // AHH / H / ? / Txxx.
      if (pickFlag) holdCtr[15]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_ACE, r3, 3);
    }
  }
  else if (length[QT_PARD] == 2)
  {
    if (length[QT_RHO] >= 5 && htop.T == QT_ACE && hopp.N && 
      LoopHold::StopFinesse(1, 4, 0, false, QT_ACE))
    {
      // AHHT+ / H / xx / 9xxxx+.
      if (pickFlag) holdCtr[16]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_ACE, SDS_VOID-5, 4);
    }
    else if (htop.T == QT_RHO &&
      LoopHold::StopFinesse(1, 3, 0, false, QT_ACE))
    {
      // AHHx+ / H / xx / Txxx+.
      if (pickFlag) holdCtr[17]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_ACE, r3, 3);
    }
  }
  else if (htop.K == QT_LHO && htop.T == QT_RHO &&
    LoopHold::StopFinesse(2, 3, 0, false, QT_ACE))
  {
    // AHHx / H / xxx+ / Txxx.
    if (pickFlag) holdCtr[18]++;
    return LoopHold::SetMove(move, QT_BOTH, QT_ACE, r3, 3);
  }
  else if (htop.K == QT_ACE && length[QT_LHO] == 1 &&
    LoopHold::StopFinesse(3, 3, 0, false, QT_ACE))
  {
    // AKHx+ / H / xx+ / xxxx+.
    if (pickFlag) holdCtr[19]++;
    return LoopHold::SetMove(move, QT_BOTH, QT_ACE, r3, 3);
  }
  else if (distHex == 0x4234 && htop.K == QT_ACE &&
    completeList[QT_RHO][1] > completeList[QT_ACE][3] &&
    completeList[QT_RHO][0] > completeList[QT_PARD][0])
  {
    // AKHx / Hx / xxX / xxxx.
    if (pickFlag) holdCtr[40]++;
    return LoopHold::SetMove(move, QT_BOTH, QT_ACE, r3, 3);
  }
  return false;
}


bool LoopHold::SolveStopped2(HoldingSimpleMove& move)
{
  // ==== G2 ================= G8 ================= G32 ================
  //      AQJ+       |         AKJ+       |         AKQ+
  // +          +    |    +          +    |    +          +
  //      K+         |         Q+         |         J+
  // ==== G1 ================= G8 ================= G32 ================

  // ==== G10 ================ G34 ================ G40 ================
  //      AJ+        |         AQ+        |         AK+ 
  // +          +    |    +          +    |    +          +
  //      KQ+        |         KJ+        |         QJ+
  // ==== G10 ================ G34 ================ G40 ================

  // ==== G42 ==========================================================
  //      A+
  // +          +
  //      KQJ+
  // ==== G42 ==========================================================


  if (! hopp.T)
   return false;

  int pardTops = 0;
  if (htop.K == QT_PARD) pardTops++;
  if (htop.Q == QT_PARD) pardTops++;
  if (htop.J == QT_PARD) pardTops++;

  if (length[QT_ACE] >= 5)
  {
    if (length[QT_RHO] >= 5)
    {
      if (length[QT_PARD] == 2 || pardTops == 2)
      {
        if (completeList[QT_RHO][0] > completeList[QT_PARD][pardTops] &&
            completeList[QT_RHO][1] > completeList[QT_ACE][4-pardTops])
        {
          // AHHxx+ / ? / Hx / hxxxx+.
          // AHxxx / - / HHx / Txxxx.
          if (pickFlag) holdCtr[20]++;
          return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-4, 4);
        }
      }
      else if (distHex == 0x5035 &&
        LoopHold::StopFinesse(2, 3, 1, true, QT_ACE))
      {
        // AHHxx / - / Hxx / Txxxx.
        if (pickFlag) holdCtr[21]++;
        return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-4, 4);
      }
    }
    else if (length[QT_LHO] >= 5)
    {
      if (pardTops == 1 && htop.T && (hopp.N ||
         (completeList[QT_LHO][0] > completeList[QT_ACE][3] &&
          completeList[QT_LHO][1] > completeList[QT_ACE][4] &&
          completeList[QT_LHO][1] > completeList[QT_PARD][1])))
      {
        // AHHxx / Txxxx / Hxx / -.
        if (pickFlag) holdCtr[22]++;
        return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-4, 4);
      }
      else if (pardTops == 2 && 
        LoopHold::StopFinesse(2, 2, 2, true, QT_PARD))
      {
        // AHxxx / Txxxx / HHx / -.
        if (pickFlag) holdCtr[23]++;
        return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-4, 4);
      }
    }
  }
  else if (length[QT_PARD] >= 5)
  {
    if (length[QT_LHO] >= 5)
    {
      if (length[QT_ACE] == 2 || pardTops == 2)
      {
        if (completeList[QT_LHO][0] > completeList[QT_ACE][4-pardTops] &&
            completeList[QT_LHO][1] > completeList[QT_PARD][pardTops])
        {
          // AHx / Txxxx / HHxxx / -.
          // Ax / xxxxx / KQJxx / ?.
          if (pickFlag) holdCtr[24]++;
          return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-4, 4);
        }
      }
      else if (distHex == 0x3550 &&
        LoopHold::StopFinesse(2, 1, 3, true, QT_PARD))
      {
        // Axx / Txxxx / KQJxx / -.
        if (pickFlag) holdCtr[25]++;
        return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-4, 4);
      }
    }
    else if (length[QT_RHO] >= 5)
    {
      if (pardTops == 3 && hopp.T && (hopp.N ||
         (completeList[QT_RHO][0] > completeList[QT_PARD][3] &&
          completeList[QT_RHO][1] > completeList[QT_PARD][4] &&
          completeList[QT_RHO][1] > completeList[QT_ACE][1])))
      {
        // Ax / ? / KQJxx / Txxxx.
        if (pickFlag) holdCtr[26]++;
        return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-4, 4);
      }
      else if (pardTops == 2 && 
        LoopHold::StopFinesse(2, 2, 2, true, QT_ACE))
      {
        // AHx / - / HHxxx / Txxxx.
        if (pickFlag) holdCtr[27]++;
        return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-4, 4);
      }
    }
  }
  return false;
}


bool LoopHold::SolveStopped3(HoldingSimpleMove& move)
{
  // ==== G3 ===========================================================
  //      AQJ+
  // K+         +
  //      +
  // ==== G3 ===========================================================

  if (length[QT_RHO] >= 2 || length[QT_LHO] <= 3)
    return false;

  if (hopp.T || (length[htop.T] == 1 && hopp.N))
  {
    // AQJ+ / Txxx+ / x+ / K.
    // AQJ+ / 9xxx+ / T / K.
    if (pickFlag) holdCtr[30]++;
    return LoopHold::SetMove(move, QT_BOTH, QT_ACE, SDS_VOID-4, 3);
  }

  if (length[QT_LHO] == 4)
    return false;

  if (htop.T == QT_ACE && hopp.N)
  {
    // AQJT+ / 9xxxx+ / x+ / K.
    if (pickFlag) holdCtr[31]++;
    return LoopHold::SetMove(move, QT_BOTH, QT_ACE, SDS_VOID-5, 4);
  }
  else if (! hopp.T && ! hopp.N && htop.T != htop.N && hopp.E &&
    length[QT_PARD] == 1)
  {
    // AQJT+ / 8xxxx+ / 9 / K.
    // AQJ9+ / 8xxxx+ / T / K.
    if (pickFlag) holdCtr[32]++;
    int r = static_cast<int>(Holding::ListToRank(completeList[QT_ACE][3]));
    return LoopHold::SetMove(move, QT_BOTH, QT_ACE, r, 4);
  }
  return false;
}


bool LoopHold::SolveStopped5(HoldingSimpleMove& move)
{
  // ==== G5 ============== G21 =========== G37 =========== G53 ========
  //      AJ+        |      A+       |      A+       |      A+
  // +          KQ+  |  +       KQJ+ |  +       KQ+  |  J+      KQ+
  //      +          |      +        |      J+       |      +
  // ==== G5 ============== G21 =========== G37 =========== G53 ========

  if (pickFlag) holdCtr[50]++;
  return LoopHold::SetMove(move, QT_BOTH, QT_ACE, SDS_VOID-1, 1);
}


bool LoopHold::SolveStopped6(HoldingSimpleMove& move)
{
  // ==== G6 ================= G46 =====================================
  //      AJ+        |         A+
  // +          Q+   |  Q+            +
  //      K+         |         KJ+
  // ==== G6 ================= G46 =====================================

  posType pa, pl, pp, pr;
  if (htop.Q == QT_LHO)
  {
    pa = QT_ACE;
    pl = QT_LHO;
    pp = QT_PARD;
    pr = QT_RHO;
  }
  else
  {
    pa = QT_PARD;
    pl = QT_RHO;
    pp = QT_ACE;
    pr = QT_LHO;
  }

  if (length[pa] >= 3 && length[pp] >= 3 && length[pl] >= 4 &&
    (htop.T == pa || 
      (htop.T == pr && htop.N == pa && length[pr] == 1)) &&
    completeList[pl][1] > completeList[pa][3] &&
    completeList[pl][1] > completeList[pp][1])
  {
    // AJT+ / Q9xx+ / Kxx+ / +.
    // AJ9+ / Q8xx+ / Kxx+ / T.
    if (pickFlag) holdCtr[59]++;
    int r = static_cast<int>(Holding::ListToRank(completeList[pa][2]));
    return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, r, 3);
  }
  else if (((length[pa] == 3 && length[pp] == 3 && length[pl] >= 3) ||
    (length[pa] == 4 && length[pp] == 4 && length[pl] == 3)) &&
    (htop.T == pa ||
      (htop.T == pr && length[pr] <= 2 && 
        (htop.N == pa || (htop.N == pr && htop.E == pa)))))
  {
    // AJT / Q+ / Kxx / +.
    // AJ9 / Q+ / Kxx / T, Tx.
    // AJ8 / Q+ / Kxx / T9.
    if (pickFlag) holdCtr[58]++;
    int r = static_cast<int>(Holding::ListToRank(completeList[pa][2]));
    return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, r, 
      static_cast<int>(length[pa]));
  }
  else if (length[pa] >= 4 && length[pp] == length[pa] &&
    length[pl] == 3 && length[pr] <= 3 && htop.T == pa)
  {
    // AJTx+ / Q(xx) / Kxxx+ / (xxx).
    if (pickFlag) holdCtr[57]++;
    int r = static_cast<int>(Holding::ListToRank(completeList[pa][2]));
    return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, r, 
      static_cast<int>(length[pa]));
  }
  else if (length[pa] == 4 && length[pl] >= 4 && length[pp] == 4 &&
    htop.T == pa && htop.N == pa)
    // ! hopp.T && htop.N == pa)
  {
    // Going to restrict this one a bit for now.
    // AJT9 / Qxxx+ / Kxxx / +.
    // AJ9x / Qxxx+ / KTxx / +.
    if (pickFlag) holdCtr[56]++;
    return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-6, 4);
  }
  else if (length[pa] == 4 && length[pr] == 4 && 
    length[pp] == 4 && length[pl] == 1 &&
    htop.T == pr && htop.N == pp && htop.E == pp)
  {
    // AJxx / Q / K98x / Txxx.
    // A98x / Txxx / KJxx / Q.
    if (pickFlag) holdCtr[55]++;
    return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-7, 4);
  }
  else if (length[pa] == 4 && length[pp] == 4 && 
    length[pr] == 1 && length[pl] == 4 &&
    htop.T == pr && ! hopp.N && ! hopp.E && 
    (htop.N == pa && htop.E == pa))
    // (htop.N == pa || htop.E == pa))
  {
    // AJ8x / Qxxx / K9xx / T.
    // AJ9x / Qxxx / K8xx / T.
    // A9xx / T / KJ8x / Qxxx.
    // A8xx / T / KJ9x / Qxxx.
    if (pickFlag) holdCtr[54]++;
    return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-7, 4);
  }
  else if (length[pa] == 4 && length[pp] == 4 && 
    length[pr] == 1 && length[pl] == 4 &&
    htop.N == pr && ! hopp.T && ! hopp.E && 
    (htop.T == pa && htop.E == pa))
    // (htop.T == pa || htop.E == pa))
  {
    // AJ8x / Qxxx / KTxx / 9.
    // AJTx / Qxxx / K8xx / 9.
    // ATxx / 9 / KJ8x / Qxxx.
    // A8xx / 9 / KJTx / Qxxx.
    if (pickFlag) holdCtr[53]++;
    return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-7, 4);
  }


  if (length[pp] == 2)
  {
    if (length[pl] == 1)
    {
      if (htop.T == pa && length[pa] >= 5 &&
        htop.N == pr && length[pr] >= 5)
      {
        if (completeList[pr][1] > completeList[pa][3])
        {
          // AJTxx / Q / Kx / 9xxxx.
          if (pickFlag) holdCtr[60]++;
          return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-5, 4);
        }
      }
      else if (length[pa] >= 3 && 
        htop.T == pr && length[pr] >= 4 &&
        completeList[pr][1] > completeList[pa][2])
      {
        // AJx+ / Q / Kx / Txxx+.
        if (pickFlag) holdCtr[61]++;
        return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-4, 3);
      }
    }
    else if (length[pl] == 2)
    {
      if (length[pa] >= 3 && length[pr] >= 4 &&
        completeList[pr][0] > completeList[pa][2] &&
        (completeList[pl][1] > completeList[pp][1] ||
         completeList[pr][0] > completeList[pp][1]))
      {
        // AJx+ / Qx / Kx / Txxx+.
        if (pickFlag) holdCtr[62]++;
        return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-4, 3);
      }
    }
    else if (completeList[pl][1] > completeList[pa][2] &&
      ((length[pr] >= 2 && htop.T == pr) ||
       completeList[pl][1] > completeList[pp][1]))
    {
      // AJx / Qxx+ / Kx / Tx+.
      if (pickFlag) holdCtr[63]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-2, 2);
    }
    else if (length[pr] >= 3 && htop.T == pr &&
      completeList[pr][1] > completeList[pa][2])
    {
      // AJx / Qxx / Kx / Txx.
      if (pickFlag) holdCtr[64]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-2, 2);
    }

    return false;
  }

  if (length[pa] == 2)
  {
    if (length[pl] >= 3 && completeList[pl][1] > completeList[pp][1])
    {
      // AJ / QTx+ / K+ / ?.
      if (pickFlag) holdCtr[65]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-2, 2);
    }
    else if (length[pl] >= 2 && length[pr] >= 3 &&
      completeList[pr][0] > completeList[pp][1])
    {
      // AJ / Qx+ / K+ / Txx+.
      if (pickFlag) holdCtr[66]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-2, 2);
    }
    else if (length[pl] == 2 && length[pr] >= 4 &&
      completeList[pr][0] < completeList[pp][1] &&
      completeList[pr][0] > completeList[pp][2])
    {
      // AJ / Qx / Kxx+ / xxxx.
      if (pickFlag) holdCtr[67]++;
      int r = static_cast<int>(Holding::ListToRank(completeList[pp][1]));
      return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, r, 3);
    }

    return false;
  }

  if (length[pl] == 1)
  {
    if (length[pr] <= 3)
      return false;

    if (htop.T == pr)
    {
      if (length[pa] == 3)
      {
        if ((pa == QT_ACE && LoopHold::StopFinesse(1, 2, 1, true, pa)) ||
           (pa == QT_PARD && LoopHold::StopFinesse(1, 1, 2, true, pa)))
        {
          // AJx / Q / Kxxx+ / Txxx+.
          if (pickFlag) holdCtr[68]++;
          return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-4, 3);
        }
      }
      else if ((pa == QT_ACE && LoopHold::StopFinesse(2, 2, 1, true, pa))
           || (pa == QT_PARD && LoopHold::StopFinesse(2, 1, 2, true, pa)))
      {
        // AJx+ / Q / Kxx+ / T98x+, T97x+, T87x, etc.
        if (pickFlag) holdCtr[69]++;
        return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-4, 3);
      }
    }

    return false;
  }

  if (length[pl] == 2)
  {
    if (length[pr] <= 3)
      return false;

    if (length[pa] == 3 && length[pp] == 3)
    {
      // AJx / Qx / Kxx / ?.
      if (pickFlag) holdCtr[460]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-4, 3);
    }
    else if (length[pa] == 3 && length[pp] == 4 &&
    completeList[pr][0] > completeList[pp][1])
    {
      // AJx / Qx / Kxxx / 9xxx or so.
      if (pickFlag) holdCtr[461]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-4, 3);
    }
    else if (length[pa] == 4 && length[pp] == 3 &&
      completeList[pr][1] > completeList[pa][2] &&
      completeList[pr][0] > completeList[pp][1])
    {
      // AJxx / Qx / Kxx / T9xx.
      if (pickFlag) holdCtr[462]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-4, 3);
    }

    return false;
  }

  if (completeList[pl][1] > completeList[pa][2] &&
    completeList[pl][1] > completeList[pp][1])
  {
    // AJ+ / QTx+ / Kxx+ / ?.
    // AJ+ / Q9x+ / Kxx+ / T+.
    if (pickFlag) holdCtr[463]++;
    return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-2, 2);
  }

  if (htop.T != pr)
    return false;

  if (length[pr] == 2)
  {
    if (((htop.Q == QT_LHO && distHex == 0x3442) ||
         (htop.Q == QT_RHO && distHex == 0x4234)) &&
      completeList[pl][1] < completeList[pa][2] &&
      completeList[pl][1] > completeList[pp][1])
    {
      // AJ9 / Q8xx / Kxxx / Tx.
      if (pickFlag) holdCtr[464]++;
      int r = static_cast<int>(Holding::ListToRank(completeList[pa][2]));
      return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, r, 3);
    }
    else if (((htop.Q == QT_LHO && distHex == 0x4432) ||
      (htop.Q == QT_RHO && distHex == 0x3244)) && 
      completeList[pl][1] > completeList[pa][3] &&
      completeList[pl][1] > completeList[pp][1])
    {
      // AJxx / Qxxx / Kxx / Tx.
      if (pickFlag) holdCtr[465]++;
      int r = static_cast<int>(Holding::ListToRank(completeList[pa][2]));
      return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, r, 3);
    }
  }
  else if (length[pr] >= 3)
  {
    if ((htop.N == pp && hopp.E) || htop.N == pr)
    {
      // AJ+ / Qxx+ / K9x+ / T8x+.
      // AJ+ / Q8x+ / K9x+ / Txx+.
      // AJ+ / Qxx+ / Kxx+ / T9x+.
      if (pickFlag) holdCtr[466]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-2, 2);
    }
  }
  return false;
}


bool LoopHold::SolveStopped7(HoldingSimpleMove& move)
{
  // ==== G7 ============== G13 =========== G39 =========== G45 ========
  //      AJ+        |      AJ+      |      A+       |      A+
  // K+         Q+   | Q+       K+   |  K+      Q+   |  Q+      K+ 
  //      +          |      +        |      J+       |      J+
  // ==== G7 ============== G13 =========== G39 =========== G45 ========

  if (length[QT_LHO] >= 2 || length[QT_RHO] >= 2)
  {
    if (pickFlag) holdCtr[70]++;
    return LoopHold::SetMove(move, QT_BOTH, QT_ACE, SDS_VOID-1, 1);
  }
  else if (length[QT_PARD] == 1)
  {
    // Ax / K / x / x.
    // This and the following are tops, not real stops.
    if (pickFlag) holdCtr[71]++;
    return LoopHold::SetMove(move, QT_BOTH, QT_ACE, SDS_VOID-1, 
      static_cast<int>(length[QT_ACE]));
  }
  else if (length[QT_ACE] == 2 && length[QT_PARD] == 2)
  {
    if (htop.J == QT_ACE)
    {
      // AJ / K / xx / Q.
      if (pickFlag) holdCtr[72]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_ACE, SDS_VOID-1, 2);
      // return LoopHold::SetMove(move, QT_BOTH, QT_ACE, SDS_VOID-4, 2);
    }
    else if (htop.T == QT_PARD)
    {
      return false;
      // A9 / K / JT / Q.
      if (pickFlag) holdCtr[73]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-4, 2);
    }
  }
  return false;
}


bool LoopHold::SolveStopped9(HoldingSimpleMove& move)
{
  // ==== G9 ================= G33 =====================================
  //      AJ+        |         AQ+    
  // +          K+   |    +          K+
  //      Q+         |         J+      
  // ==== G9 ================= G33 =====================================

  if (length[QT_LHO] >= 2)
  {
    // Protected K behind the ace.
    if (pickFlag) holdCtr[90]++;
    return LoopHold::SetMove(move, QT_BOTH, QT_ACE, SDS_ACE, 1);
  }

  if (length[QT_ACE] == 2 && length[QT_PARD] <= 2 && htop.Q == QT_ACE)
  {
    if (length[QT_RHO] >= 2)
    {
      // AQ / K / J(x) / xx+.
      if (pickFlag) holdCtr[91]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_ACE, SDS_VOID-3, 2);
    }
    else
    {
      // AQ / K / J(x) / (x).
      if (pickFlag) holdCtr[92]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_ACE, SDS_VOID-1, 2);
    }
  }

  if (length[QT_PARD] == 1)
  {
    if (htop.T == QT_RHO && length[QT_RHO] >= 3)
    {
      // AJ+ / K / Q / Txx+.
      if (pickFlag) holdCtr[93]++;
      int r = static_cast<int>(Holding::ListToRank(completeList[QT_ACE][1]));
      return LoopHold::SetMove(move, QT_BOTH, QT_ACE, r, 2);
    }
    else if (htop.T == QT_ACE && htop.N == QT_RHO && length[QT_RHO] >= 4)
    {
      // AJT+ / K / Q / 9xxx+.
      if (pickFlag) holdCtr[94]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_ACE, SDS_VOID-5, 3);
    }
    else if (htop.T == QT_ACE && htop.N == QT_ACE &&
      htop.E == QT_RHO && length[QT_RHO] >= 5)
    {
      // AJT9+ / K / Q / 8xxxx+.
      if (pickFlag) holdCtr[95]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_ACE, SDS_VOID-6, 4);
    }

    return false;
  }

  if (length[QT_PARD] <= 2 || length[QT_ACE] <= 2 || 
    length[QT_RHO] <= 3 || htop.T != QT_RHO)
    return false;

  if (distHex == 0x4144 && htop.N == QT_PARD && htop.E == QT_PARD)
  {
    // AQxx / K / J98x / Txxx.
    if (pickFlag) holdCtr[96]++;
    return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-7, 4);
  }
  else if ((length[QT_ACE] == 3 && length[QT_PARD] == 3) ||
    LoopHold::StopFinesse(1, 2, 1, false, QT_ACE))
  {
    // AJx / K / Qxxx+ / Txxx+.
    // AJxx / K / Qxxx / Txxx.
    // AJxxx / K / Qxx / Txxx.
    if (pickFlag) holdCtr[97]++;
    return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-4, 3);
  }
  return false;
}


bool LoopHold::SolveStopped11(HoldingSimpleMove& move)
{
  // ==== G11 ================ G35 =====================================
  //      AJ+        |         AQ+    
  // K+         +    |   K+          + 
  //      Q+         |         J+      
  // ==== G11 ================ G35 =====================================

  if (length[QT_RHO] >= 2)
    return false;

  if (length[QT_ACE] == 2 && length[QT_PARD] <= 2 && htop.Q == QT_ACE)
  {
    if (length[QT_LHO] >= 2)
    {
      if (pickFlag) holdCtr[110]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_ACE, SDS_VOID-3, 2);
    }
    else
    {
      if (pickFlag) holdCtr[111]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_ACE, SDS_VOID-1, 2);
    }
  }

  if (length[QT_ACE] == 2 && length[QT_PARD] <= 2 && 
    (htop.Q == QT_ACE || length[QT_PARD] == 1))
  {
return false;
// Probably simplifies to 2=1 with AJ+ / Q.
    // AQ / + / J(x) / K.
    if (pickFlag) holdCtr[110]++;
    int r = (length[QT_LHO] <= 1 && length[QT_PARD] <= 1 ?
      SDS_VOID-1 : SDS_VOID - (htop.Q == QT_ACE ? 3 : 4));
    return LoopHold::SetMove(move, QT_BOTH, QT_ACE, r, 2);
  }
  else if (length[QT_LHO] <= 2 && length[QT_PARD] == 1 && 
    htop.Q == QT_ACE)
  {
    // AQ+ / x(x) / J(x) / K.
    if (pickFlag) holdCtr[111]++;
    int r = (length[QT_LHO] <= 1 && length[QT_PARD] <= 1 ?
      SDS_VOID-1 : SDS_VOID-3);
    return LoopHold::SetMove(move, QT_BOTH, QT_ACE, r, 
      static_cast<int>(length[QT_ACE]));
  }
  else if (length[QT_PARD] == 1)
  {
    if (length[QT_LHO] <= 2)
    {
      // AH / (x) / H / H.
      if (pickFlag) holdCtr[112]++;
      int r = (length[QT_LHO] == 2 ? 
        static_cast<int>(Holding::ListToRank(completeList[QT_ACE][1])) : SDS_VOID-1);
      return LoopHold::SetMove(move, QT_BOTH, QT_ACE, r, 
        static_cast<int>(length[QT_ACE]));
    }
    else if (htop.T == QT_LHO)
    {
      // AH+ / Txx+ / H / K.
      if (pickFlag) holdCtr[113]++;
      int r = static_cast<int>(Holding::ListToRank(completeList[QT_ACE][1]));
      return LoopHold::SetMove(move, QT_BOTH, QT_ACE, r, 2);
    }
    else if (htop.T == QT_ACE && htop.N == QT_LHO && length[QT_LHO] >= 4)
    {
      // AQT+ / K / J / 9xxx+.
      if (pickFlag) holdCtr[114]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_ACE, SDS_VOID-5, 3);
    }
    else if (htop.T == QT_ACE && htop.N == QT_ACE &&
      htop.E == QT_LHO && length[QT_LHO] >= 5)
    {
      // AQT9+ / K / J / 8xxxx+.
      if (pickFlag) holdCtr[115]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_ACE, SDS_VOID-6, 4);
    }
  }
  else if (distHex == 0x4441 && htop.T == QT_LHO && 
    (htop.N == QT_PARD || (htop.N == QT_ACE && ! hopp.E)))
  {
    // AJxx / Txxx / Q9xx / K.
    // AJ9x / Txxx / Q8xx / K.
    // AJ98 / Txxx / Qxxx / K.
    if (pickFlag) holdCtr[116]++;
    int r = (htop.N == QT_PARD ? SDS_VOID-6 : SDS_VOID-7);
    return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, r, 4);
  }
  else if (length[QT_ACE] >= 3 && length[QT_LHO] >= 4 &&
    length[QT_PARD] >= 3 && htop.T == QT_LHO &&
    (htop.N == QT_LHO || (htop.N == QT_ACE && htop.E == QT_LHO)))
  {
    // AHxx / Txxx / Hxx / K.
    if (pickFlag) holdCtr[117]++;
    return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-4, 3);
  }
  return false;
}


bool LoopHold::SolveStopped12(HoldingSimpleMove& move)
{
  // ==== G12 ==========================================================
  //      AKJ+    
  // Q+         +
  //      +     
  // ==== G12 ==========================================================

  if (length[QT_RHO] >= 3 || length[QT_LHO] <= 3)
    return false;

  if (length[QT_ACE] == 3)
  {
    if (length[QT_PARD] <= 3 ||
      completeList[QT_LHO][0] > completeList[QT_PARD][0])
    {
      // AKJ / xxxx+ / ? / Q+.
      if (pickFlag) holdCtr[120]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_ACE, SDS_VOID-4, 3);
    }
  }
  else if (completeList[QT_LHO][0] > completeList[QT_ACE][3])
  {
    if (length[QT_PARD] == 1 ||
      completeList[QT_LHO][0] > completeList[QT_PARD][0])
    {
      // AKJx+ / Txxx+ / ? / Q(x).
      if (pickFlag) holdCtr[121]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_ACE, SDS_VOID-4, 3);
    }
    else if (length[QT_PARD] == 2 && length[QT_RHO] == 2 &&
      (hopp.T || hopp.N))
    {
      // AKJx+ / Txxx+ / xx / Qx.
      if (pickFlag) holdCtr[122]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_ACE, SDS_VOID-4, 3);
    }
  }
  else if (completeList[QT_LHO][0] > completeList[QT_ACE][4])
  {
    if (length[QT_LHO] >= 5 &&
      (length[QT_RHO] == 2 || (length[QT_PARD] == 1 ||
        completeList[QT_PARD][0] < completeList[QT_LHO][0])))
    {
      // AKJxx / xxxxx / x / Q(x).
      if (pickFlag) holdCtr[123]++;
      int r = static_cast<int>(Holding::ListToRank(completeList[QT_ACE][3]));
      return LoopHold::SetMove(move, QT_BOTH, QT_ACE, r, 4);
    }
  }
  return false;
}


bool LoopHold::SolveStopped14(HoldingSimpleMove& move)
{
  // ==== G14 ================ G38 =====================================
  //      AJ+        |         A+
  // Q+         +    |  +             Q+
  //      K+         |         KJ+
  // ==== G14 ================ G38 =====================================

  posType pa, pl, pp, pr;
  if (htop.Q == QT_RHO)
  {
    pa = QT_ACE;
    pl = QT_LHO;
    pp = QT_PARD;
    pr = QT_RHO;
  }
  else
  {
    pa = QT_PARD;
    pl = QT_RHO;
    pp = QT_ACE;
    pr = QT_LHO;
  }

  if (length[pa] == 4 && length[pp] == 4 && length[pr] >= 4 && 
    htop.T == pp && htop.N == pp)
  {
    // We're going to limit this one for now.
    // AJxx / + / KT9x / Q+.
    // if (pickFlag) holdCtr[146]++;
    // return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-6, 4);
    return false;
  }
  else if (length[pa] == 4 && length[pl] == 1 && length[pp] == 4 &&
    length[pr] == 4 && htop.T == pl && htop.N == pp && htop.E == pp)
  {
    return false;
    // AJxx / T / K98x / Qxxx.
    if (pickFlag) holdCtr[148]++;
    return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-7, 4);
  }
  else if (length[pa] == 4 && length[pl] == 1 && length[pp] == 4 &&
    length[pr] == 4 && htop.T == pp && htop.N == pl && htop.E == pp)
  {
    return false;
    // AT8x / Qxxx / KJxx / 9.
    // AJxx / 9 / KT8x / Qxxx.
    if (pickFlag) holdCtr[381]++;
    return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-7, 4);
  }

  if (length[pa] <= 2 || length[pr] >= 3 || length[pl] <= 3)
    return false;

  if (length[pr] == 1)
  {
    if (htop.T == pl)
    {
      if (length[pp] == 2)
      {
        // AJx+ / Q / Kx / Txxx+.
        if (pickFlag) holdCtr[140]++;
        return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-4, 3);
      }
      else if (htop.T == pl &&
        (htop.N == pl || (htop.N == pa && htop.E == pl)))
      {
         // AJ9x / T8xx / Kxx / Q.
        if (pickFlag) holdCtr[141]++;
        return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-4, 3);
      }
      else if (length[pa] == 4 && length[pp] == 4 && length[pl] == 4 &&
        ! hopp.N)
      {
        if (htop.N == pp)
        {
          // AJxx / T8xx / K9xx / Q.
          if (pickFlag) holdCtr[380]++;
          return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-6, 4);
        }
        else if (! hopp.E)
        {
          // AJ9x / Txxx / K8xx / Q.
          // AJ8x / Txxx / K9xx / Q.
          if (pickFlag) holdCtr[147]++;
          return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-7, 4);
        }
      }
    }
    else if (length[pa] >= 5 && length[pl] >= 5 && htop.T == pa && hopp.N)
    {
      // AJTxx / 9xxxx / Kx / Q.
      if (pickFlag) holdCtr[142]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-5, 4);
    }
  }
  else if (htop.T == pl || (htop.T == pr && htop.N == pl))
  {
    if (length[pp] <= 3)
    {
      // AJ65 / T743 / K98 / Q2.
      if (pickFlag) holdCtr[143]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-4, 3);
    }
    else if (length[pa] == 3 && completeList[pl][1] > completeList[pp][1])
    {
      // AJ9 / T654 / K732 / Q8.
      if (pickFlag) holdCtr[144]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-4, 3);
    }
  }
  else if (length[pa] >= 4 && length[pp] == 2 &&
    completeList[pl][0] > completeList[pa][2])
  {
    // AJxx / 9xxx / KT / Qx, etc.
    if (pickFlag) holdCtr[145]++;
    return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-4, 3);
  }
  return false;
}


bool LoopHold::SolveStopped15(HoldingSimpleMove& move)
{
  // ==== G15 ================ G17 ================ G23 ================
  //      AJ+        |         AQ+        |         A+  
  // KQ+        +    |   +           KJ+  |   K+          QJ+
  //      +          |         +          |         + 
  // ==== G15 ================ G17 ================ G23 ================

  // ==== G25 ================ G29 ================ G31 ================
  //      A+         |         A+         |         A+  
  // +          KJ+  |   Q+          KJ+  |   KQ+         J+ 
  //      Q+         |         +          |         + 
  // ==== G25 ================ G29 ================ G31 ================

  // ==== G47 ================ G51 ================ G55 ================
  //      A+         |         AQ+        |         A+  
  // KQ+        +    |   KJ+         +    |   KJ+         Q+ 
  //      J+         |         +          |         + 
  // ==== G47 ================ G51 ================ G55 ================

  // ==== G59 ================ G61 ================ G63 ================
  //      A+         |         A+         |         A+  
  // KJ+        +    |   QJ+         K+   |   KQJ+        +  
  //      Q+         |         +          |         + 
  // ==== G59 ================ G61 ================ G63 ================

  int fourTops = 1;
  if (! hopp.K) fourTops++;
  if (! hopp.Q) fourTops++;
  if (! hopp.J) fourTops++;

  if (fourTops == 1)
  {
    if (pickFlag) holdCtr[150]++;
    return LoopHold::SetMove(move, QT_BOTH, QT_ACE, SDS_VOID-1, 1);
  }
  else if (fourTops == 2 && hopp.K)
  {
    if (htop.K == QT_LHO && htop.J == QT_LHO)
    {
      if (pickFlag) holdCtr[151]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_ACE, SDS_VOID-1, 1);
    }
    else if (htop.K == QT_RHO)
    {
      if (htop.Q == QT_RHO || (htop.Q == QT_PARD && htop.J == QT_RHO))
      {
        if (pickFlag) holdCtr[152]++;
        return LoopHold::SetMove(move, QT_BOTH, QT_ACE, SDS_VOID-1, 1);
      }
    }
  }

  return false;
}


bool LoopHold::SolveStopped18(HoldingSimpleMove& move)
{
  // ==== G18 ================ G24 ================ G58 ================
  //      AQ+        |         AK+        |         A+
  // +          J+   |   +           J+   |  J+            +
  //      K+         |         Q+         |         KQ+
  // ==== G18 ================ G24 ================ G58 ================

  if ((length[QT_ACE] <= 3 && length[QT_PARD] <= 3) ||
      (length[QT_LHO] <= 3 && length[QT_RHO] <= 3))
    return false;

  posType pa, pl, pp, pr;
  if (htop.K == QT_ACE || htop.Q == QT_ACE)
  {
    pa = QT_ACE;
    pl = QT_LHO;
    pp = QT_PARD;
    pr = QT_RHO;
  }
  else
  {
    pa = QT_PARD;
    pl = QT_RHO;
    pp = QT_ACE;
    pr = QT_LHO;
  }

  int rnum = 0;
  if (htop.J == pr) rnum++;
  if (htop.T == pr) rnum++;
  if (htop.N == pr) rnum++;

  if (length[pl] == 1)
  {
    if (length[pp] == 2)
    {
      if (htop.T == pa)
      {
        if (((pa == QT_ACE && distHex == 0x5125) ||
             (pa == QT_PARD && distHex == 0x2551)) && 
          completeList[pr][1] > completeList[pa][3] &&
          completeList[pr][0] > completeList[pp][1])
        {
          // AQTxx / J / Kx / 9xxxx.
          if (pickFlag) holdCtr[180]++;
          return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-5, 4);
        }
      }
      else if (hopp.T && 
        (rnum == 2 || (htop.N == pp && htop.E == pr)))
      {
        // AHxx+ / h / Hx / hxxx+ with enough middle cards.
        if (pickFlag) holdCtr[181]++;
        return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-3, 3);
      }
    }
    else if (length[pa] == 3)
    {
      if (hopp.T && LoopHold::StopFinesse(1, 2, 1, true, QT_ACE))
      {
        // AHx / h / Hxxx+ / hxxx+.
        if (pickFlag) holdCtr[182]++;
        return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-3, 3);
      }
    }
    else if (length[pa] == 4 && length[pl] == 1 && length[pp] == 4 &&
      length[pr] == 4 && htop.T == pr && htop.N == pp && htop.E == pp)
    {
      // AKxx / J / Q98x / Txxx.
      if (pickFlag) holdCtr[244]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-7, 4);
    }
    else if (hopp.T && LoopHold::StopFinesse(2, 2, 1, true, QT_ACE))
    {
      // AHxx+ / h / Hxx+ / hxxx+.
      if (pickFlag) holdCtr[183]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-3, 3);
    }
  }
  else if (length[pl] == 2)
  {
    if (hopp.T && rnum >= 1)
    {
      if (length[pp] == 2 || length[pa] == 3 ||
        (((pa == QT_ACE && distHex == 0x4234) ||
          (pa == QT_PARD && distHex == 0x3442)) &&
        completeList[pr][1] > completeList[pa][2]))
      {
        // AHxx / hx / Hxx / hxxx.
        // AHx / hx / Hxxx / hxxx.
        // AHx+ / hx / Hx / hxxx.
        if (pickFlag) holdCtr[184]++;
        return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-3, 3);
      }
    }
    else if (length[pp] == 2 && length[pr] >= 4 &&
      htop.T == pl && htop.N == pp && htop.E == pr)
    {
      // AKxx / JT / Q9 / 8xxx.
      if (pickFlag) holdCtr[185]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-3, 3);
    }
  }
  else if (length[pa] == 4 && length[pp] == 4 && length[pl] >= 4 && 
    htop.J == pl && ! hopp.T && (htop.T == pp || ! hopp.N))
  {
    // AKT9 / Jxxx+ / Qxxx / +.
    // AKTx / Jxxx+ / Q9xx / +.
    // AK9x / Jxxx+ / QTxx / +.
    // Reset numbers.
    if (pickFlag) holdCtr[249]++;
    int r;
    if (htop.T == pa && htop.N == pa)
      r = SDS_VOID - 6;
    else
      r = static_cast<int>(Holding::ListToRank(completeList[pp][1]));

    return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, r, 4);
  }
  else if (length[pa] == 4 && length[pl] == 4 && length[pp] == 4 &&
    length[pr] == 1 && ! hopp.T && htop.N == pr && ! hopp.E)
  {
    // AKTx / Jxxx / Q8xx / 9.
    // AKT8 / Jxxx / Qxxx / 9.
    if (pickFlag) holdCtr[248]++;
    return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-7, 4);
  }
  else if (length[pl] >= 4 && htop.J == pl &&
    (htop.T == pl || (htop.T == pa && htop.N == pl)))
  {
    // AHx+ / JTxx+ / Hx+ / ?.
    // AHT+ / J9xx+ / Hx+ / ?.
    if (pickFlag) holdCtr[186]++;
    return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-3, 3);
  }
  else if (length[pa] >= 4 && length[pl] >= 4 &&
    length[pp] == 2 && length[pr] >= 2 &&
    htop.J == pl && htop.T == pa &&
    completeList[pl][1] > completeList[pa][3] &&
    completeList[pr][0] > completeList[pp][1])
  {
    // AKTx / Jxxx / Qx / xx.
    if (pickFlag) holdCtr[187]++;
    return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-3, 3);
  }
  else if (length[pr] == 1)
  {
    if (length[pl] >= 4 && hopp.T && rnum >= 1)
    {
      if (length[pp] == 2)
      {
        // AHxx / hxxx+ / Hx / h.
        if (pickFlag) holdCtr[188]++;
        return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-3, 3);
      }
      else if ((htop.N == pl || 
        (htop.N == pa && htop.E == pl)))
      {
        // AHxx / hxxx+ / Hxx+ / h.
        if (pickFlag) holdCtr[189]++;
        return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-3, 3);
      }
    }
  }
  else if (hopp.T && rnum >= 1)
  {
    if (length[pr] >= 3)
    {
      // AHx+ / hxx+ / Hx+ / hxx+.
      if (pickFlag) holdCtr[240]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-3, 3);
    }
    else if (length[pp] <= 3)
    {
      // AHxx / hxxx / Hx / hx.
      // AHxx / hxxx / Hxx / hx.
      if (pickFlag) holdCtr[241]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-3, 3);
    }
    else if (completeList[pl][1] > completeList[pp][1])
    {
      // AHx / hxxx / Hxxx / hx.
      if (pickFlag) holdCtr[242]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-3, 3);
    }
  }
  else if (length[pp] == 2)
  {
    if (length[pr] == 4 &&
      completeList[pl][1] > completeList[pp][1] &&
      completeList[pr][0] > completeList[pa][2])
    {
      // AKxx / Jxx / Qx / xxxx.
      if (pickFlag) holdCtr[243]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-3, 3);
    }
  }
  return false;
}


bool LoopHold::SolveStopped19(HoldingSimpleMove& move)
{
  // ==== G19 ==========================================================
  //      AQ+    
  // K+         J+
  //      +      
  // ==== G19 ==========================================================

  if (length[QT_RHO] >= 2)
    return false;

  if (length[QT_LHO] >= 3)
  {
    // AQ+ / Jxx / ? / K.
    if (pickFlag) holdCtr[190]++;
    return LoopHold::SetMove(move, QT_BOTH, QT_ACE, SDS_VOID-3, 2);
  }
  else if (length[QT_PARD] <= 1)
  {
    if (pickFlag) holdCtr[191]++;
    int r = static_cast<int>(length[QT_LHO] <= 1 ? SDS_VOID-1 : SDS_VOID-3);
    return LoopHold::SetMove(move, QT_BOTH, QT_ACE, r, 
      static_cast<int>(length[QT_ACE]));
  }
  else if (length[QT_PARD] == 2 && length[QT_ACE] == 2)
  {
    if (pickFlag) holdCtr[192]++;
    int r = static_cast<int>(Holding::ListToRank(completeList[QT_ACE][length[QT_LHO]-1]));
    return LoopHold::SetMove(move, QT_BOTH, QT_ACE, r, 
      static_cast<int>(length[QT_ACE]));
  }
  return false;
}


bool LoopHold::SolveStopped20(HoldingSimpleMove& move)
{
  // ==== G20 ============= G22 ========================================
  //      AK+        |      A+     
  // +          QJ+  |  +       QJ+
  //      +          |      K+     
  // ==== G20 ============= G22 ========================================

  posType pend = (htop.K == QT_ACE ? QT_ACE : QT_BOTH);
  int x = (htop.K == QT_ACE ? 2 : 1);

  if (length[QT_LHO] >= 3 || (length[QT_RHO] >= 3 &&
    (htop.T == QT_RHO || ((length[QT_PARD] <= 2 &&
      completeList[QT_RHO][0] > completeList[QT_ACE][x]) ||
        (length[QT_ACE] == 2 && 
          completeList[QT_RHO][0] > completeList[QT_PARD][1])))))
  {
    if (pickFlag) holdCtr[200]++;
    return LoopHold::SetMove(move, QT_BOTH, pend, SDS_VOID-2, 2);
  }
  else if (length[QT_ACE] >= 3 &&
    length[QT_PARD] <= 2 && length[QT_RHO] >= 4 && 
    completeList[QT_RHO][0] > completeList[QT_ACE][x+1] &&
    completeList[QT_RHO][0] < completeList[QT_ACE][x])
  {
    // AKxx / QJ / xx / xxxx.
    // Axxx / QJ / Kx / xxxx.
    if (pickFlag) holdCtr[201]++;
    int r = static_cast<int>(Holding::ListToRank(completeList[QT_ACE][x]));
    return LoopHold::SetMove(move, QT_BOTH, pend, r, 3);
  }
  else if (length[QT_ACE] >= 4 &&
    length[QT_PARD] <= 2 && length[QT_RHO] >= 5 &&
    completeList[QT_RHO][0] > completeList[QT_ACE][x+2] &&
    completeList[QT_RHO][0] < completeList[QT_ACE][x+1])
  {
    // AKT9x / QJ / xx / 8xxxx.
    // AT9xx / QJ / Kx / 8xxxx.
    if (pickFlag) holdCtr[202]++;
    int r = static_cast<int>(Holding::ListToRank(completeList[QT_ACE][x+1]));
    return LoopHold::SetMove(move, QT_BOTH, pend, r, 4);
  }

  if (length[QT_RHO] <= 3)
    return false;

  if (distHex == 0x4234)
  {
    if (htop.T == QT_PARD &&
         ((htop.K == QT_ACE &&
           completeList[QT_RHO][0] > completeList[QT_ACE][2]) ||
          (htop.K == QT_PARD &&
           completeList[QT_RHO][0] > completeList[QT_ACE][1])))
    {
      // AKxx / QJ / Txx / xxxx.
      // Axxx / QJ / KTx / xxxx.
      if (pickFlag) holdCtr[203]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-5, 3);
    }
    else if (htop.T == QT_ACE && htop.N == QT_RHO &&
      ((htop.K == QT_ACE &&
        completeList[QT_RHO][1] > completeList[QT_ACE][3]) ||
       (htop.K == QT_PARD &&
        completeList[QT_RHO][1] > completeList[QT_ACE][2])))
    {
      // AKTx / QJ / xxx / 9xxx.
      // ATxx / QJ / Kxx / 9xxx.
      if (pickFlag) holdCtr[204]++;
      return LoopHold::SetMove(move, QT_BOTH, pend, SDS_VOID-5, 3);
    }
  }
  else if (length[QT_PARD] >= 4 && htop.K == QT_PARD)
  {
    if (completeList[QT_RHO][0] < completeList[QT_PARD][1] &&
        completeList[QT_RHO][0] > completeList[QT_PARD][2] &&
        (length[QT_ACE] == 2 ||
          completeList[QT_RHO][0] > completeList[QT_ACE][1]))
    {
      // Ax / QJ / KTxx / 9xxx.
      // Axx / QJ / KTxx / 9xxx.
      if (pickFlag) holdCtr[205]++;
      int r = static_cast<int>(Holding::ListToRank(completeList[QT_PARD][1]));
      return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, r, 3);
    }
    else if (length[QT_ACE] == 3 && htop.T == QT_ACE &&
      completeList[QT_RHO][0] > completeList[QT_PARD][1])
    {
      // ATx / QJ / Kxxx / 9xxx.
      if (pickFlag) holdCtr[206]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-5, 3);
    }
  }
  else if (distHex == 0x3244 && htop.K == QT_ACE && ! hopp.T && hopp.N)
  {
    // AKT / QJ / xxxx / 9xxx.
    // AKx / QJ / Txxx / 9xxx.
    if (pickFlag) holdCtr[207]++;
    posType e = (htop.T == QT_ACE ? QT_ACE : QT_BOTH);
    return LoopHold::SetMove(move, QT_BOTH, e, SDS_VOID-5, 3);
  }
  return false;
}


bool LoopHold::SolveStopped26(HoldingSimpleMove& move)
{
  // ==== G26 ================ G50 ================ G56 ================
  //      A+         |         AQ+        |         AK+ 
  // +          J+   |   J+          +    |   J+          +  
  //      KQ+        |         K+         |         Q+
  // ==== G26 ================ G50 ================ G56 ================

  posType pa, pl, pp, pr;
  if (htop.K == QT_PARD && htop.Q == QT_PARD)
  {
    pa = QT_ACE;
    pl = QT_LHO;
    pp = QT_PARD;
    pr = QT_RHO;
  }
  else
  {
    pa = QT_PARD;
    pl = QT_RHO;
    pp = QT_ACE;
    pr = QT_LHO;
  }

  if (length[pa] == 2 && length[pl] >= 1 && 
      length[pp] >= 4 && length[pr] >= 5 &&
      htop.T == pp && 
      completeList[pr][0] > completeList[pa][1] &&
      completeList[pr][0] > completeList[pp][3])
  {
    // AQTxx / 9xxxx / Kx / J.
    // AKTxx / 9xxxx / Qx / J.
    // Ax / J / KQTxx / 9xxxx.
    if (pickFlag) holdCtr[260]++;
    return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-5, 4);
  }

  if (length[pa] == 2)
  {
    // Jack must be left.
    if (length[pp] <= 2)
      return false;

    if (length[pl] >= 4 && completeList[pl][1] > completeList[pp][2])
    {
      // Single defender, e.g. JTxx, J9xx over T.
      if (pickFlag) holdCtr[261]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-3, 3);
    }
    else if (length[pr] >= 4 &&
      completeList[pr][0] > completeList[pp][2] &&
      (htop.T == pr || length[pl] >= 2))
    {
      // AT / Jx+ / KQxx / 9xxx.
      if (pickFlag) holdCtr[262]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-3, 3);
    }
  }
  else if (length[pa] == 4 && length[pl] == 4 &&
    length[pp] == 3 && length[pr] == 2 &&
    htop.J == pl && htop.T == pa &&
    completeList[pr][0] > completeList[pa][2] &&
    completeList[pr][0] > completeList[pp][2] &&
    completeList[pl][1] > completeList[pa][2] &&
    completeList[pl][1] > completeList[pp][2])
  {
    // AKx / 9x / QTxx / Jxxx.
    if (pickFlag) holdCtr[263]++;
    return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-3, 3);
  }
  else if (length[pl] >= 4 && length[pr] <= 2 && htop.J == pl && 
    (((pp == QT_ACE && LoopHold::StopFinesse(2, 2, 1, true, pp)) ||
      (pp == QT_PARD && LoopHold::StopFinesse(2, 1, 2, true, pp)))))
  {
    // AT8x / J9xx / KQx / xx.
    if (pickFlag) holdCtr[264]++;
    return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-3, 3);
  }
  else if (length[pp] == 3)
  {
    if (completeList[SDS_PARTNER[htop.J]][0] > completeList[pa][1] &&
        completeList[SDS_PARTNER[htop.J]][0] > completeList[pp][2])
    {
      // Split honors.
      if (length[pl] >= 4 && (length[pr] >= 2 ||
        ((htop.N == pl || (htop.N == pa && htop.E == pl)))))
      {
        // AKxx / hxxx / KQx / h.
        if (pickFlag) holdCtr[265]++;
        return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-3, 3);
      }
    }
    else if (length[pl] >= 4 && length[pr] >= 2 && 
      htop.J == pl && htop.T == pl && htop.N == pa && hopp.E)
    {
      // Axx+ / JTxx / KQx / xx.
      if (pickFlag) holdCtr[266]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-3, 3);
    }
  }
  else if (length[pa] == 4 && length[pp] == 4 && length[pl] >= 4 && 
    htop.T == pa && (htop.N == pa || (htop.N == pr && length[pr] == 1 &&
      htop.E == pa)))
  {
    // AT9x / J+ / KQxx / +.
    // AT8x / J+ / KQxx / 9.
    if (pickFlag) holdCtr[267]++;
    int r = static_cast<int>(Holding::ListToRank(completeList[pa][2]));
    return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, r, 4);
  }
  else if (length[pa] == 4 && length[pp] == 4 && 
    length[pr] == 4 && length[pl] == 1 && 
    htop.T == pr &&
      (htop.N == pa || (! hopp.N && ! hopp.E)))
  {
    // Axxx / h / KQ9x / h+.
    // A9xx / h / KQ8x / h+.
    // A98x / h / KQxx / h+.
    if (pickFlag) holdCtr[268]++;
    int r = (htop.N == pa ? SDS_VOID - 6 : SDS_VOID - 7);
    return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, r, 4);
  }
  return false;
}


bool LoopHold::SolveStopped27(HoldingSimpleMove& move)
{
  // ==== G27 ==========================================================
  //      A+     
  // K+         J+
  //      Q+     
  // ==== G27 ==========================================================

  if (length[QT_ACE] >= 3 && length[QT_LHO] >= 4 &&
    length[QT_PARD] >= 3 && length[QT_RHO] == 1 &&
    htop.T == QT_ACE && htop.N == QT_ACE && htop.E == QT_LHO)
  {
    // AT9+ / J8xx+ / Qxx / K.
    if (pickFlag) holdCtr[270]++;
    return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-6, 3);
  }
  else if (length[QT_ACE] == 3 && length[QT_LHO] >= 3 &&
    length[QT_PARD] == 3 && length[QT_RHO] == 1 &&
    htop.T == QT_ACE && htop.N == QT_ACE)
  {
    // AT9 / Jxx+ / Qxx / K.
    if (pickFlag) holdCtr[278]++;
    return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-6, 3);
  }
  else if (distHex == 0x4341 && htop.T == QT_ACE && htop.N == QT_ACE)
  {
    // AT9x / Jxx / Qxxxx / K.
    if (pickFlag) holdCtr[277]++;
    return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-6, 4);
  }
  else if (distHex == 0x4441 && ! hopp.T && ! hopp.N && ! hopp.E)
  {
    int rnum = 0;
    if (htop.T == QT_ACE) rnum++;
    if (htop.N == QT_ACE) rnum++;
    if (htop.E == QT_ACE) rnum++;

    // if (rnum >= 2 || (rnum == 1 && htop.E == QT_PARD))
    // if (rnum >= 2)
    if (rnum == 3)
    {
      // AT98 / Jxxx / Qxxx / K.
      // Ahhx / Jxxx / Qhxx / K.
      if (pickFlag) holdCtr[279]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-7, 4);
    }
    else if (rnum == 1 && htop.E == QT_ACE &&
      completeList[QT_ACE][2] > completeList[QT_LHO][1] &&
      completeList[QT_ACE][2] > completeList[QT_PARD][3])
    {
      return false;
      // A87x / Jxxx / QT9x / K.
      if (pickFlag) holdCtr[276]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-8, 4);
    }
    // Ahxx / Jxxx / Qh8x / K.
    // A87x / Jxxx / QT9x / K.
  }


  if (length[QT_RHO] >= 2)
  {
    if (length[QT_LHO] >= 2 || hopp.T)
    {
      // K not finessable.
      if (pickFlag) holdCtr[271]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_ACE, SDS_ACE, 1);
    }
  }
  else if (length[QT_PARD] == 1 && length[QT_LHO] >= 2)
  {
    // K not finessable.
    if (pickFlag) holdCtr[272]++;
    return LoopHold::SetMove(move, QT_BOTH, QT_ACE, SDS_ACE, 1);
  }
  else if (length[QT_LHO] == 1 && length[QT_PARD] == 1)
  {
    // Ax+ / K / x / J.
    if (pickFlag) holdCtr[273]++;
    return LoopHold::SetMove(move, QT_BOTH, QT_ACE, SDS_ACE, 
      static_cast<int>(length[QT_ACE]));
  }
  else if (length[QT_ACE] == 2 && length[QT_PARD] == 2 &&
    length[QT_LHO] >= 2)
    // (htop.T == QT_PARD || length[QT_LHO] >= 2))
  {
    // Ax / J / QT / K.
    // Ax / Jx+ / Qx / K.
    if (pickFlag) holdCtr[274]++;
    return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-3, 2);
  }
  else if (length[QT_LHO] >= 3 && (length[QT_PARD] == 2 ||
    ((completeList[QT_LHO][1] > completeList[QT_PARD][1]) &&
      completeList[QT_LHO][1] > completeList[QT_ACE][2])))
  {
    // Ax+ / Jxx+ / Qx+ / K.
    if (pickFlag) holdCtr[275]++;
    return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-3, 2);
  }
  return false;
}


bool LoopHold::SolveStopped28(HoldingSimpleMove& move)
{
  // ==== G28 ============= G30 =========== G52 =========== G54 ========
  //      AK+        |      A+       |      AK+      |      A+
  // Q+         J+   |  Q+      J+   |  J+      Q+   |  J+      Q+ 
  //      +          |      K+       |      +        |      +
  // ==== G28 ============= G30 =========== G52 =========== G54 ========

  if (length[QT_LHO] <= 2 && length[QT_RHO] <= 2)
    return false;

  //
  // Should rotate to have the stiff opposing honor with pr.
  //

  posType pend = (htop.K == QT_ACE ? QT_ACE : QT_BOTH);

  if (pend == QT_BOTH && distHex == 0x4441 && 
    ! hopp.T && ! hopp.N && ! hopp.E &&
    (htop.T == QT_ACE && htop.N == QT_ACE && htop.E == QT_ACE))
    // (htop.T == QT_ACE || htop.N == QT_ACE))
  {
    // Axxx / Hxxx / Kxxx / H with T98, T9, T8, 98, T, 9 with ace.
    if (pickFlag) holdCtr[302]++;
    return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-7, 4);
  }
  else if (pend == QT_BOTH && ! hopp.T && htop.T == htop.N &&
    length[QT_ACE] >= 3)
  {
    if (htop.T == QT_ACE && htop.E == QT_LHO &&
      length[QT_LHO] >= 4 && length[QT_PARD] >= 3 && length[QT_RHO] == 1)
    {
      // AT9+ / H8xx+ / Kxx+ / H.
      if (pickFlag) holdCtr[280]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-6, 3);
    }
    else if (htop.T == QT_PARD && htop.E == QT_RHO &&
      length[QT_RHO] >= 4 && length[QT_PARD] >= 3 && length[QT_LHO] == 1)
    {
      // Axx+ / H / KT9+ / H8xx+.
      if (pickFlag) holdCtr[281]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-6, 3);
    }
    else if (length[QT_ACE] == 3 && length[QT_PARD] == 3 &&
      ((htop.T == QT_ACE && length[QT_RHO] == 1 && length[QT_LHO] >= 3) ||
       (htop.T == QT_PARD && length[QT_LHO] == 1 && length[QT_RHO] >= 3)))
    {
      // AT9 / Hxx+ / Kxx / H.
      // Axx / H / KT9 / Hxx+.
      if (pickFlag) holdCtr[282]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-6, 3);
    }
    else if (length[QT_ACE] == 4 && length[QT_PARD] == 4 &&
      ((htop.T == QT_ACE && length[QT_RHO] == 1 && length[QT_LHO] == 3) ||
       (htop.T == QT_PARD && length[QT_LHO] == 1 && length[QT_RHO] == 3)))
    {
      // AT9x / Hxx / Kxxx / H.
      // Axxx / H / KT9x / Hxx.
      if (pickFlag) holdCtr[301]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-6, 4);
    }
    else if (distHex == 0x4144 && 
      htop.T == QT_PARD && htop.N == QT_PARD && htop.E == QT_PARD)
    {
      // Axxx / H / KT98 / Hxxx.
      if (pickFlag) holdCtr[302]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-7, 4);
    }
    else if (distHex == 0x4144 &&
      htop.T == QT_ACE && htop.N == QT_ACE && htop.E == QT_PARD &&
      completeList[QT_PARD][2] > completeList[QT_ACE][3] &&
      completeList[QT_PARD][2] > completeList[QT_RHO][1])
    {
      return false;
      // AT9x / H / K87x / Hxxx.
      if (pickFlag) holdCtr[303]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-8, 4);
    }
    else if (distHex == 0x4441 &&
      htop.T == QT_PARD && htop.N == QT_PARD && htop.E == QT_ACE &&
      completeList[QT_ACE][2] > completeList[QT_PARD][3] &&
      completeList[QT_ACE][2] > completeList[QT_LHO][1])
    {
      return false;
      // A87x / Hxxx / KT9x / H.
      if (pickFlag) holdCtr[305]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-8, 4);
    }
  }
  else if (pend == QT_BOTH && 
    length[QT_ACE] == 4 && length[QT_PARD] == 4 && length[QT_LHO] >= 4 &&
    ! hopp.T && ! hopp.N && ! hopp.E)
  {
    if (length[QT_RHO] == 1 && htop.T != htop.E && htop.E == QT_ACE)
    {
      return false;
      // AT8x / H+ / K9xx / H.
      // A98x / H+ / KTxx / H.
      if (pickFlag) holdCtr[302]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-7, 4);
    }
    else if (length[QT_LHO] == 1 && htop.T != htop.E && htop.E == QT_PARD)
    {
      return false;
      // A9xx / H / KT8x / H+.
      // ATxx / H / K98x / H+.
      if (pickFlag) holdCtr[303]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-7, 4);
    }
  }
  else if (pend == QT_BOTH && distHex == 0x4144 &&
    ! hopp.T && ! hopp.N && ! hopp.E && 
    (htop.T == QT_PARD && htop.N == QT_PARD && htop.E == QT_PARD))
    // htop.T != htop.N)
  {
    // ATxx / H / K98x / Jxxx.
    // A9xx / H / KT8x / Jxxx.
    if (pickFlag) holdCtr[304]++;
    return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-7, 4);
  }

  if (length[QT_LHO] >= 2 && length[QT_RHO] >= 2 &&
    length[QT_LHO] + length[QT_RHO] >= 5)
  {
    // Axx / Hxx / Kxx / Hx.
    // Axx / Hx / Kxx / Hxx.
    if (pickFlag) holdCtr[283]++;
    return LoopHold::SetMove(move, QT_BOTH, pend, SDS_VOID-2, 2);
  }
  else if (hopp.T && (length[QT_LHO] >= 3 || length[QT_RHO] >= 3))
  {
    // Axx / H / Kxx / HHx.
    // Axx / HHx / Kxx / H.
    if (pickFlag) holdCtr[284]++;
    return LoopHold::SetMove(move, QT_BOTH, pend, SDS_VOID-2, 2);
  }
  else if (htop.K == QT_ACE && length[QT_LHO] >= 3)
  {
    // AKx / Hxx / xxx / H.
    if (pickFlag) holdCtr[285]++;
    return LoopHold::SetMove(move, QT_BOTH, pend, SDS_VOID-2, 2);
  }
  else if (htop.K == QT_ACE && length[QT_RHO] >= 3 &&
    (length[QT_PARD] == 1 ||
      (completeList[QT_RHO][1] > completeList[QT_ACE][2] &&
         (length[QT_PARD] == 2 ||
           completeList[QT_RHO][1] > completeList[QT_PARD][1]))))
  {
    if (pickFlag) holdCtr[286]++;
    return LoopHold::SetMove(move, QT_BOTH, pend, SDS_VOID-2, 2);
  }
  else if (pend == QT_BOTH)
  {
    if (htop.T == QT_PARD && htop.N == QT_RHO && length[QT_LHO] == 1)
    {
      // A+ / H / KT+ / H9x+.
      if (pickFlag) holdCtr[287]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-2, 2);
    }
    else if (htop.T == QT_ACE && htop.N == QT_LHO && length[QT_RHO] == 1)
    {
      // AT+ / H9x+ / K+ / H.
      if (pickFlag) holdCtr[288]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-2, 2);
    }
    else if (length[QT_ACE] == 2 && length[QT_RHO] >= 3)
    {
      // Ax / H / Kx+ / Hxx+.
      if (pickFlag) holdCtr[289]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-2, 2);
    }
    else if (length[QT_PARD] == 2 && length[QT_LHO] >= 3)
    {
      // Ax+ / Hxx+ / Kx / H.
      if (pickFlag) holdCtr[300]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-2, 2);
    }
  }
  return false;
}


bool LoopHold::SolveStopped36(HoldingSimpleMove& move)
{
  // ==== G36 ==========================================================
  //      AK+     
  // +          Q+
  //      J+     
  // ==== G36 ==========================================================

  if (length[QT_LHO] >= 3)
  {
    // AKx / Qxx+ / J+ / ?.
    if (pickFlag) holdCtr[360]++;
    return LoopHold::SetMove(move, QT_BOTH, QT_ACE, SDS_VOID-2, 2);
  }
  else if (length[QT_PARD] <= length[QT_LHO] &&
    length[QT_ACE] >= 4 && length[QT_RHO] >= 5 &&
    completeList[QT_RHO][0] > completeList[QT_ACE][4] &&
    completeList[QT_RHO][0] < completeList[QT_ACE][3])
  {
    // AKxxx / xxxxx / x / Q(x).
    if (pickFlag) holdCtr[361]++;
    int r = static_cast<int>(Holding::ListToRank(completeList[QT_ACE][3]));
    return LoopHold::SetMove(move, QT_BOTH, QT_ACE, r, 4);
  }
  else if (length[QT_LHO] == 2)
  {
    if (length[QT_PARD] <= 2)
    {
      if (length[QT_RHO] >= 3 &&
        completeList[QT_RHO][0] > completeList[QT_ACE][2])
      {
        // With enough middle cards:
        // AKx / Qx / Jx / xxx.
        // AKx / QJ / xx / xxx.
        if (pickFlag) holdCtr[362]++;
        return LoopHold::SetMove(move, QT_BOTH, QT_ACE, SDS_VOID-2, 2);
      }
      else if (length[QT_RHO] >= 4 &&
        completeList[QT_RHO][0] < completeList[QT_ACE][2] &&
        completeList[QT_RHO][0] > completeList[QT_ACE][3])
      {
        // AKx+ / Qx / J(x) / xxxx+.
        if (pickFlag) holdCtr[363]++;
        int r = static_cast<int>(Holding::ListToRank(completeList[QT_ACE][2]));
        return LoopHold::SetMove(move, QT_BOTH, QT_ACE, r, 3);
      }
    }
    else if (length[QT_RHO] >= 4 &&
      completeList[QT_RHO][0] > completeList[QT_ACE][2] &&
      (length[QT_PARD] == 3 ||
        completeList[QT_RHO][0] > completeList[QT_PARD][1]))
    {
      // AKx+ / Qx / Jxx+ / Txxx.
      if (pickFlag) holdCtr[364]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-4, 3);
    }
  }
  else if (length[QT_PARD] == 1)
  {
    if (htop.T == QT_RHO && length[QT_RHO] >= 3)
    {
      // AKx / Q / J / Txx+.
      if (pickFlag) holdCtr[365]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_ACE, SDS_VOID-2, 2);
    }
    else if (htop.T == QT_ACE && htop.N == QT_RHO && length[QT_RHO] >= 4)
    {
      // AKT+ / 9xxx+ / J / Q.
      if (pickFlag) holdCtr[366]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_ACE, SDS_VOID-5, 3);
    }
  }
  else if (length[QT_PARD] >= 3 && length[QT_RHO] >= 4 &&
    htop.T == QT_RHO &&
    completeList[QT_RHO][1] > completeList[QT_ACE][2] &&
    (length[QT_PARD] == 3 ||
      completeList[QT_RHO][1] > completeList[QT_PARD][2]))
  {
    // AKx+ / Q / Jxx+ / Txxx+.
    if (pickFlag) holdCtr[367]++;
    return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-4, 3);
  }
  else if (distHex == 0x4144 && 
    htop.T == QT_RHO && htop.N == QT_PARD && htop.E == QT_PARD)
  {
    // AKxx / Q / J98x / Txxx.
    if (pickFlag) holdCtr[368]++;
    return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-7, 4);
  }
  return false;
}


bool LoopHold::SolveStopped41(HoldingSimpleMove& move)
{
  // ==== G41 ==========================================================
  //      A+      
  // +          K+
  //      QJ+    
  // ==== G41 ==========================================================

  if (length[QT_LHO] >= 2)
  {
    // Protected K behind the ace.
    if (pickFlag) holdCtr[410]++;
    return LoopHold::SetMove(move, QT_BOTH, QT_ACE, SDS_ACE, 1);
  }
  else if (length[QT_ACE] == 2)
  {
    if (length[QT_PARD] >= 4 && length[QT_RHO] >= 5 && 
      htop.T == QT_PARD && htop.N == QT_RHO)
    {
      // Ax / 9xxxx / QJTxx / K.
      if (pickFlag) holdCtr[411]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-5, 4);
    }
    else if (length[QT_PARD] == 2)
    {
      if (length[QT_RHO] >= 2)
      {
        if (pickFlag) holdCtr[412]++;
        return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-3, 2);
      }
      else
      {
        return false;
        if (pickFlag) holdCtr[413]++;
        return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-1, 2);
      }
    }
    else if (length[QT_RHO] <= 2)
    {
      // Ax / K / QJx+ / xx.
      // Ax / K / QJ+ / xx+.
      if (pickFlag) holdCtr[414]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-3, 
        static_cast<int>(length[QT_PARD]));
    }
    else if (length[QT_PARD] == 3 || 
      (length[QT_RHO] >= 4 && htop.T == QT_RHO))
    {
      // Ax / K / QJ+ / Txx+.
      if (pickFlag) holdCtr[415]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-4, 3);
    }
  }
  else if (length[QT_PARD] == 2)
  {
    if (htop.T == QT_RHO && length[QT_RHO] >= 3)
    {
      // Axx+ / Txx+ / QJ / K.
      if (pickFlag) holdCtr[416]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-3, 2);
    }
  }
  else if (length[QT_RHO] >= 4 &&
    completeList[QT_RHO][0] > completeList[QT_ACE][1] &&
    (length[QT_PARD] == 3 ||
      completeList[QT_RHO][0] > completeList[QT_PARD][2]))
  {
    // Axx / K / QJT / 9xxx.
    // Axxx / K / QJx / Txxx+.
    // Axx+ / K / QJx+ / Txxx+.
    if (pickFlag) holdCtr[417]++;
    return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-4, 3);
  }
  return false;
}


bool LoopHold::SolveStopped43(HoldingSimpleMove& move)
{
  // ==== G43 ==========================================================
  //      A+      
  // K+         + 
  //      QJ+    
  // ==== G43 ==========================================================

  if (length[QT_RHO] >= 2)
    return false;
  else if (length[QT_ACE] == 2)
  {
    if (length[QT_PARD] == 2)
    {
      if (length[QT_LHO] >= 2)
      {
        if (pickFlag) holdCtr[430]++;
        return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-3, 2);
      }
      else
      {
        return false;
        if (pickFlag) holdCtr[431]++;
        return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-1, 2);
      }
    }
    else if (length[QT_PARD] >= 4 && length[QT_LHO] >= 5 && 
      htop.T == QT_PARD && htop.N == QT_LHO &&
      completeList[QT_LHO][1] > completeList[QT_PARD][3])
    {
      // Ax / K / QJTx+ / 9xxxx.
      if (pickFlag) holdCtr[432]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-5, 4);
    }
    // else if (length[QT_LHO] <= 2 || length[QT_PARD] == 2)
    else if (length[QT_LHO] <= 2)
    {
      // Ax / K / QJx+ / xx.
      // Ax / K / QJ+ / xx+.
      if (pickFlag) holdCtr[433]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-3, 
        static_cast<int>(length[QT_PARD]));
    }
    else if (length[QT_PARD] == 3 || 
      (length[QT_LHO] >= 4 && htop.T == QT_LHO &&
        completeList[QT_LHO][1] > completeList[QT_PARD][2]))
    {
      // Ax / K / QJ+ / Txx+.
      if (pickFlag) holdCtr[434]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-4, 3);
    }
  }
  else if (length[QT_PARD] == 2)
  {
    if (htop.T == QT_LHO && length[QT_LHO] >= 3)
    {
      // Axx+ / K / QJ / Txx+.
      if (pickFlag) holdCtr[435]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-3, 2);
    }
  }
  else if (length[QT_PARD] == 3)
  {
    if (length[QT_ACE] == 3)
      return false;
    else if (length[QT_LHO] >= 4 && 
      completeList[QT_LHO][0] > completeList[QT_ACE][1])
    {
      // Axx / ? / QJx / K.
      if (pickFlag) holdCtr[436]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-4, 3);
    }
  }
  else if (length[QT_ACE] == 4 && length[QT_LHO] >= 4 &&
    length[QT_PARD] == 4 && 
    htop.T == QT_LHO && htop.N == QT_ACE && htop.E == QT_ACE)
  {
    // A98x / Txxx / QJxx / K.
    if (pickFlag) holdCtr[437]++;
    return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-7, 4);
  }
  else if (length[QT_LHO] >= 4 && htop.T == QT_LHO &&
    LoopHold::StopFinesse(2, 1, 2, true, QT_PARD))
  {
    // Axx / Txxx / QJx(x) / K.
    if (pickFlag) holdCtr[438]++;
    return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-4, 3);
  }
  return false;
}


bool LoopHold::SolveStopped44(HoldingSimpleMove& move)
{
  // ==== G44 ==========================================================
  //      AK+     
  // Q+         + 
  //      J+    
  // ==== G44 ==========================================================

  if (length[QT_PARD] == 1)
  {
    if (length[QT_RHO] <= 2 &&
      length[QT_ACE] >= 4 && length[QT_LHO] >= 5 &&
      completeList[QT_LHO][0] > completeList[QT_ACE][4] &&
      completeList[QT_LHO][0] < completeList[QT_ACE][3])
    {
      // AKxxx / xxxxx / x / Q(x).
      if (pickFlag) holdCtr[440]++;
      int r = static_cast<int>(Holding::ListToRank(completeList[QT_ACE][3]));
      return LoopHold::SetMove(move, QT_BOTH, QT_ACE, r, 4);
    }
    else if (htop.T == QT_ACE && htop.N == QT_LHO &&
      length[QT_RHO] == 1 && length[QT_LHO] >= 4)
    {
      // AKT+ / 9xxx+ / J / Q.
      if (pickFlag) holdCtr[441]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_ACE, SDS_VOID-5, 3);
    }
    else if ((length[QT_RHO] >= 3 &&
      completeList[QT_RHO][1] > completeList[QT_ACE][2]) ||
      (hopp.T && length[htop.T] >= 3))
    {
      // AK+ / Txx+ / J / Q+.
      // AK+ / QTx+ / J / +.
      // AK+ / Q9x+ / J / Tx.
      if (pickFlag) holdCtr[442]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_ACE, SDS_VOID-2, 2);
    }
  }

  if (length[QT_RHO] == 1)
  {
    if (length[QT_LHO] >= 4 && length[QT_PARD] >= 3 && 
      htop.T == QT_LHO && 
      completeList[QT_LHO][1] > completeList[QT_PARD][1] &&
      completeList[QT_LHO][1] > completeList[QT_ACE][3])
    {
      // AKx / Txxx / Jxx / Q.
      if (pickFlag) holdCtr[443]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-4, 3);
    }
    else if (distHex == 0x4441 && htop.T == QT_LHO &&
      (htop.N == QT_PARD || (htop.N == QT_ACE && ! hopp.E)))
    {
      // AKxx / Txxx / J9xx / Q.
      // AK98 / Txxx / Jxxx / Q.
      // AK9x / Txxx / J8xx / Q.
      if (pickFlag) holdCtr[459]++;
      int r = (htop.N == QT_PARD ? SDS_VOID-6 : SDS_VOID-7);
      return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, r, 4);
    }
  }
  else if (htop.T == QT_ACE)
  {
    if (length[QT_LHO] >= 4 && length[QT_PARD] <= 2 && 
      length[QT_RHO] == 2 && 
      completeList[QT_LHO][0] > completeList[QT_ACE][3] &&
      (completeList[QT_LHO][0] > completeList[QT_PARD][1] ||
       completeList[QT_RHO][1] > completeList[QT_PARD][1]))
    {
      // AKT+ / xxxx+ / J(x) / Qx with enough middle cards.
      if (pickFlag) holdCtr[444]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_ACE, SDS_VOID-5, 3);
    }
  }
  else if (htop.T == QT_PARD)
  {
    if (distHex == 0x4432 && htop.N == QT_LHO && (hopp.E || 
      (completeList[QT_LHO][1] > completeList[QT_PARD][2] &&
       completeList[QT_LHO][1] > completeList[QT_ACE][3])))
    {
      // AKxx / 9xxx / / JTx / Qx.
      if (pickFlag) holdCtr[445]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-4, 3);
    }
  }
  else if (length[QT_ACE] == 3 && length[QT_PARD] == 3 &&
    length[QT_RHO] <= 2)
  {
    if (length[QT_LHO] <= 2)
      return false;

    // AKx / + / Jxx / Q(x).
    if (pickFlag) holdCtr[446]++;
    return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-4, 3);
  }
  else if (length[QT_ACE] >= 4 && length[QT_LHO] >= 4 &&
    length[QT_PARD] == 3)
  {
    if (completeList[QT_LHO][0] > completeList[QT_ACE][2])
    {
      // AKxx / xxxx / Jxx / QT.
      if (pickFlag) holdCtr[447]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-4, 3);
    }
  }
  else if (distHex == 0x3442 && 
    (htop.T == QT_LHO || (htop.T == QT_RHO && htop.N == QT_LHO)) &&
    LoopHold::StopFinesse(2, 2, 1, true, QT_PARD))
  {
    // AKx / Txxx / Jxxx / Qx.
    if (pickFlag) holdCtr[448]++;
    return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-4, 3);
  }
  else if (length[QT_LHO] >= 3 && length[QT_RHO] >= 2 && 
    htop.T == QT_LHO)
  {
    if (length[QT_PARD] <= 2 || (length[QT_RHO] >= 3))
      // && completeList[QT_RHO][2] > completeList[QT_PARD][2]))
    {
      // AKx / Txx+ / J(x) / Qx.
      if (pickFlag) holdCtr[449]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_ACE, SDS_VOID-2, 2);
    }
  }
  else if (length[QT_LHO] >= 3 && length[QT_RHO] >= 2 && 
    htop.T == QT_RHO && length[QT_PARD] <= 2 &&
    ((completeList[QT_LHO][0] > completeList[QT_ACE][2] &&
       completeList[QT_LHO][0] > completeList[QT_PARD][2]) || // del
      (completeList[QT_RHO][2] > completeList[QT_ACE][2] &&
       completeList[QT_RHO][2] > completeList[QT_PARD][2]))) // del
  {
    // AKx / 9xx+ / J(x) / QT.
    if (pickFlag) holdCtr[450]++;
    return LoopHold::SetMove(move, QT_BOTH, QT_ACE, SDS_VOID-2, 2);
  }
  else if (length[QT_RHO] >= 3)
  {
    if (length[QT_LHO] == 2 &&
      (htop.T == QT_LHO || htop.N == QT_LHO) &&
      completeList[QT_RHO][1] > completeList[QT_ACE][2] &&
      completeList[QT_RHO][1] > completeList[QT_PARD][1])
    {
      // AKx / Tx / Jxx / Qxx.
      if (pickFlag) holdCtr[451]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_ACE, SDS_VOID-2, 2);
    }
    else if (length[QT_LHO] <= 2 && length[QT_PARD] >= 2 &&
      LoopHold::StopFinesse(2, 2, 0, false, QT_ACE))
    {
      // AKx+ / h / Jx+ / Qhx+.
      // AKx+ / ? / Jx+ / Qhx+.
      if (pickFlag) holdCtr[452]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_ACE, SDS_VOID-2, 2);
    }
    else if (length[QT_LHO] >= 3 && 
      (hopp.N || (htop.N == QT_PARD && hopp.E)))
    {
      // AKx / 8xx / J9+ / QTx.
      // AKx / Txx / J9+ / Q8x.
      // AKx / T8x / J9+ / Qxx.
      if (pickFlag) holdCtr[453]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_ACE, SDS_VOID-2, 2);
    }
  }
  else if (length[QT_LHO] <= 3)
    return false;
  else if (length[QT_PARD] <= 2)
  {
    if (completeList[QT_LHO][0] < completeList[QT_ACE][2] &&
        completeList[QT_LHO][0] > completeList[QT_ACE][3])
    {
      // AK9x / 8xxx / J(x) / QT.
      // AK9x / 7xxx / J8 / QT.
      // AK8x / 7xxx / J9 / QT.
      if (pickFlag) holdCtr[454]++;
      int r = static_cast<int>(Holding::ListToRank(completeList[QT_ACE][2]));
      return LoopHold::SetMove(move, QT_BOTH, QT_ACE, r, 3);
    }
  }
  return false;
}


bool LoopHold::SolveStopped48(HoldingSimpleMove& move)
{
  // ==== G48 ==========================================================
  //      AKQ+    
  // J+         + 
  //      +     
  // ==== G48 ==========================================================

  if ((length[QT_LHO] >= 4 || length[QT_RHO] >= 4) &&
    hopp.T && hopp.N && hopp.E)
  {
    if (pickFlag) holdCtr[480]++;
    return LoopHold::SetMove(move, QT_BOTH, QT_ACE, SDS_VOID-3, 3);
  }
  else if (length[QT_LHO] >= 5 && length[QT_PARD] <= 2 && 
    completeList[QT_LHO][0] < completeList[QT_ACE][3] &&
    completeList[QT_LHO][0] > completeList[QT_ACE][4] &&
    (length[QT_PARD] == 1 ||
    completeList[QT_LHO][0] > completeList[QT_PARD][0]))
  {
    // AHHxx / xxxxx / Hx / H.
    if (pickFlag) holdCtr[481]++;
    int r = static_cast<int>(Holding::ListToRank(completeList[QT_ACE][3]));
    return LoopHold::SetMove(move, QT_BOTH, QT_ACE, r, 4);
  }
  else if (length[QT_PARD] == 1)
  {
    if ((length[QT_RHO] >= 4 &&
        completeList[QT_RHO][1] > completeList[QT_ACE][3]) ||
      (length[QT_LHO] >= 4 &&
        completeList[QT_LHO][0] > completeList[QT_ACE][3]))
    {
      // AKQ+ / ? / x / JTxx.
      // AKQ+ / ? / T / J9xx.
      // AKQ+ / xxxx / x / J+ with enough middle cards.
      if (pickFlag) holdCtr[482]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_ACE, SDS_VOID-3, 3);
    }
  }
  else if (length[QT_RHO] >= 4 && 
    LoopHold::StopFinesse(3, 3, 0, false, QT_ACE))
  {
    // All kinds of combinations such as
    // AKQ3 / 2 / T864 / J975.
    if (pickFlag) holdCtr[483]++;
    return LoopHold::SetMove(move, QT_BOTH, QT_ACE, SDS_VOID-3, 3);
  }
  else if (length[QT_LHO] >= 4 && length[QT_PARD] == 2 && 
    length[QT_RHO] >= 2 &&
    completeList[QT_LHO][0] > completeList[QT_ACE][3] &&
    (completeList[QT_RHO][1] > completeList[QT_PARD][1] ||
     completeList[QT_LHO][0] > completeList[QT_PARD][1]))
  {
    if (pickFlag) holdCtr[484]++;
    // AKQ+ / xxxx / xx / Jx with enough middle cards.
    return LoopHold::SetMove(move, QT_BOTH, QT_ACE, SDS_VOID-3, 3);
  }
  return false;
}


bool LoopHold::SolveStopped49(HoldingSimpleMove& move)
{
  // ==== G49 ==========================================================
  //      AQ+     
  // J+         K+
  //      +     
  // ==== G49 ==========================================================

  if (length[QT_LHO] >= 2)
  {
    // Protected K behind the ace.
    if (pickFlag) holdCtr[490]++;
    return LoopHold::SetMove(move, QT_BOTH, QT_ACE, SDS_ACE, 1);
  }
  else if (length[QT_RHO] == 1 && length[QT_PARD] == 1)
  {
    // Ax / K / x / J.
    if (pickFlag) holdCtr[491]++;
    return LoopHold::SetMove(move, QT_BOTH, QT_ACE, SDS_ACE,
      static_cast<int>(length[QT_ACE]));
  }
  else if (length[QT_ACE] == 2 && length[QT_PARD] <= 2 &&
    length[QT_RHO] <= 2)
  {
    // AQ / K / x(x) / J(x).
    if (pickFlag) holdCtr[492]++;
    int r = static_cast<int>(Holding::ListToRank(completeList[QT_ACE][length[QT_RHO]-1]));
    return LoopHold::SetMove(move, QT_BOTH, QT_ACE, r, 2);
    // return LoopHold::SetMove(move, QT_BOTH, QT_ACE, SDS_VOID-3, 2);
  }
  else if (length[QT_RHO] >= 3 &&
    (length[QT_ACE] == 2 || 
     length[QT_PARD] == 1 ||
     (completeList[QT_RHO][1] > completeList[QT_ACE][2] && 
       (length[QT_PARD] == 2 ||
          completeList[QT_RHO][1] > completeList[QT_PARD][1]))))
  {
    // AQ+  / Jxx+ / x+ / K.
    // AQ   / K    / x+ / Jxx+.
    // AQx+ / K    / x  / Jxx+.
    // AQx+ / K    / xx / Jxx with middle cards.
    if (pickFlag) holdCtr[493]++;
    return LoopHold::SetMove(move, QT_BOTH, QT_ACE, SDS_VOID-3, 2);
  }
  return false;
}


bool LoopHold::SolveStopped57(HoldingSimpleMove& move)
{
  // ==== G57 ==========================================================
  //      A+      
  // J+         K+
  //      Q+    
  // ==== G57 ==========================================================

  if (length[QT_LHO] >= 2)
  {
    // A+ / Kx+ / Q+ / J+.
    if (pickFlag) holdCtr[570]++;
    return LoopHold::SetMove(move, QT_BOTH, QT_ACE, SDS_VOID-1, 1);
  }
  else if (length[QT_PARD] == 1)
  {
    if (length[QT_RHO] == 1)
    {
      // A+ / K / Q / J.
      if (pickFlag) holdCtr[571]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_ACE, SDS_VOID-1, 
        static_cast<int>(length[QT_ACE]));
    }
    else
    {
      // A+ / K / Q / Jx+.
      if (pickFlag) holdCtr[572]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_ACE, SDS_VOID-1, 1);
    }
  }
  else if (length[QT_RHO] >= 3)
  {
    // A+ / K / Q+ / Jxx+.
    if (pickFlag) holdCtr[573]++;
    return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-3, 2);
  }
  else if (length[QT_ACE] == 2 && length[QT_PARD] == 2 &&
    length[QT_RHO] == 2)
    // (length[QT_RHO] == 2 || htop.T == QT_PARD))
  {
    // Ax / K / Qx / Jx.
    if (pickFlag) holdCtr[574]++;
    return LoopHold::SetMove(move, QT_BOTH, QT_BOTH, SDS_VOID-3, 2);
  }
  return false;
}


bool LoopHold::SolveStopped60(HoldingSimpleMove& move)
{
  // ==== G60 ============= G62 ========================================
  //      AK+        |      A+     
  // QJ+        +    | QJ+      +  
  //      +          |      K+     
  // ==== G60 ============= G62 ========================================

  posType pend = (htop.K == QT_ACE ? QT_ACE : QT_BOTH);
  int x = (htop.K == QT_ACE ? 2 : 1);

  if (htop.K == QT_ACE && length[QT_PARD] >= 2 && length[QT_RHO] >= 3)
  {
    if (! hopp.T && ! hopp.N && ! hopp.E)
    {
      // AKx / xxx / T98 / QJx.
      return false;
    }
    else if (completeList[QT_ACE][2] > completeList[QT_RHO][2] &&
      completeList[QT_ACE][2] > completeList[QT_LHO][0])
    {
      // QJ directly finessable.
      return false;
    }
    else if (length[QT_LHO] >= 3 &&
      completeList[QT_LHO][0] > completeList[QT_ACE][2] &&
      completeList[QT_LHO][0] > completeList[QT_PARD][2])
    {
      if (pickFlag) holdCtr[600]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_ACE, SDS_VOID-2, 2);
    }
    else if (length[QT_LHO] >= 2 && htop.T == QT_PARD &&
      htop.N == QT_LHO)
    {
      // AKx+ / 9x+ / Tx+ / QJx+.
      if (pickFlag) holdCtr[601]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_ACE, SDS_VOID-2, 2);
    }
    else if (LoopHold::StopFinesse(2, 2, 0, true, QT_ACE))
    {
      if (pickFlag) holdCtr[602]++;
      return LoopHold::SetMove(move, QT_BOTH, QT_ACE, SDS_VOID-2, 2);
    }
  }
  else if (length[QT_RHO] >= 3 || (length[QT_LHO] >= 3 &&
    (htop.T == QT_LHO || ((length[QT_PARD] <= 2 &&
      completeList[QT_LHO][0] > completeList[QT_ACE][x]) ||
        (length[QT_ACE] == 2 && 
          completeList[QT_LHO][0] > completeList[QT_PARD][1])))))
  {
    if (pickFlag) holdCtr[603]++;
    return LoopHold::SetMove(move, QT_BOTH, pend, SDS_VOID-2, 2);
  }
  else if (length[QT_ACE] >= 3 &&
    length[QT_PARD] <= 2 && length[QT_LHO] >= 4 && 
    completeList[QT_LHO][0] > completeList[QT_ACE][x+1] &&
    completeList[QT_LHO][0] < completeList[QT_ACE][x])
  {
    // AKxx / xxxx / xx / QJ.
    // Axxx / xxxx / Kx / QJ.
    if (pickFlag) holdCtr[604]++;
    int r = static_cast<int>(Holding::ListToRank(completeList[QT_ACE][x]));
    return LoopHold::SetMove(move, QT_BOTH, pend, r, 3);
  }
  else if (length[QT_PARD] >= 3 &&
    length[QT_ACE] <= 2 && length[QT_LHO] >= 4 && 
    completeList[QT_LHO][0] > completeList[QT_PARD][2] &&
    completeList[QT_LHO][0] < completeList[QT_PARD][1])
  {
    // Ax / xxxx / Kxxx / QJ.
    if (pickFlag) holdCtr[605]++;
    int r = static_cast<int>(Holding::ListToRank(completeList[QT_PARD][1]));
    return LoopHold::SetMove(move, QT_BOTH, pend, r, 3);
  }
  else if (length[QT_ACE] >= 4 &&
    length[QT_PARD] <= 2 && length[QT_LHO] >= 5 &&
    completeList[QT_LHO][0] > completeList[QT_ACE][x+2] &&
    completeList[QT_LHO][0] < completeList[QT_ACE][x+1])
  {
    // AKT9x / 8xxxx / xx / QJ.
    // AT9xx / 8xxxx / Kx / QJ.
    if (pickFlag) holdCtr[606]++;
    int r = static_cast<int>(Holding::ListToRank(completeList[QT_ACE][x+1]));
    return LoopHold::SetMove(move, QT_BOTH, pend, r, 4);
  }

  if (length[QT_LHO] <= 3)
    return false;

  if (distHex == 0x4432)
  {
    int xx = 1;
    if (htop.K == QT_ACE) xx++;
    if (htop.T == QT_ACE) xx++;

    if (completeList[QT_LHO][0] > completeList[QT_ACE][xx] &&
      (htop.T == QT_PARD || (htop.T == QT_ACE && 
        completeList[QT_LHO][0] > completeList[QT_PARD][3-xx])))
    {
      // AKTx / 9xxx / xxx / QJ.
      // ATxx / 9xxx / Kxx / QJ.
      // AKxx / 9xxx / Txx / QJ.
      // Axxx / 9xxx / KTx / QJ.
      if (pickFlag) holdCtr[607]++;
      posType pe = (htop.K == QT_PARD || htop.T == QT_PARD ?
        QT_BOTH : QT_ACE);
      return LoopHold::SetMove(move, QT_BOTH, pe, SDS_VOID-5, 3);
    }
  }
  else if (distHex == 0x3442 && ! hopp.T && 
    ((htop.K == QT_PARD && htop.T == QT_ACE && htop.N != QT_PARD) ||
     ((htop.K == QT_ACE || htop.T == QT_PARD) && hopp.N)))
  {
    int xx = 0;
    if (htop.K == QT_PARD) xx++;
    if (htop.T == QT_PARD) xx++;

    if (completeList[QT_LHO][0] > completeList[QT_PARD][xx] &&
      (htop.T == QT_ACE ||
        completeList[QT_LHO][1] > completeList[QT_PARD][xx]))
    {
      // AKT / 9xxx / xxxx / QJ.
      // ATx / 9xxx / Kxxx / QJ.
      // AKx / 9xxx / Txxx / QJ.
      // Axx / 9xxx / KTxx / QJ.
      if (pickFlag) holdCtr[608]++;
      posType e = (xx == 0 ? QT_ACE : QT_BOTH);
      return LoopHold::SetMove(move, QT_BOTH, e, SDS_VOID-5, 3);
    }
  }
  return false;
}


