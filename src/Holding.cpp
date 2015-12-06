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
#include <string.h>
// #include <stdlib.h>
#include <assert.h>

#include "cst.h"
#include "Holding.h"

Holding::Holding()
{
}


Holding::~Holding()
{
}


void Holding::Set(
  const int sl,
  const int c)
{
  this->suitLength = static_cast<unsigned>(sl);
  this->counter = c;

  Holding::MakeRanks();
}


void Holding::SetSide(const int sideVal)
{
  if (sideVal == QT_ACE)
  {
    side = QT_ACE;
    lho = QT_LHO;
    pard = QT_PARD;
    rho = QT_RHO;
  }
  else if (sideVal == QT_LHO)
  {
    side = QT_LHO;
    lho = QT_PARD;
    pard = QT_RHO;
    rho = QT_ACE;
  }
  else if (sideVal == QT_PARD)
  {
    side = QT_PARD;
    lho = QT_RHO;
    pard = QT_ACE;
    rho = QT_LHO;
  }
  else if (sideVal == QT_RHO)
  {
    side = QT_RHO;
    lho = QT_ACE;
    pard = QT_LHO;
    rho = QT_PARD;
  }
  else
    assert(false);
}


const Trick Holding::GetTrick() const
{
  Trick trick;
  trick.Set(side, winSide, Holding::GetPrependRank(), 1);
  return trick;
}


// #define HOLD_DEBUG


void Holding::RewindLead()
{
  numLeads = 0;
  leadCurrIndex = 0;
  int maxPard = cardListHi[pard][0];

  for (unsigned l = 0; l < cardNo[side]; l++)
  {
    int lrHi = cardListHi[side][l];
    int lrLo = cardListLo[side][l];
    int m = Max(maxPard, lrHi);

    multiLead[lrHi] = false;
    multiLead[lrLo] = false;
      
    if (m < maxDef)
    {
      // Will lose trick.
      continue;
    }
    else if (lrHi < cardListHi[rho][0])
    {
      // Just the lowest option.
      leadList[numLeads] = lrLo;
      numLeads++;
    }
    else if (lrHi > maxPard)
    {
      // Highest, will win.
      leadList[numLeads] = lrHi;
      numLeads++;
    }
    else if (lrHi < cardListHi[pard][cardNo[pard]-1])
    {
      // Even partner's lowest card is higher, so play lowest.
      if (l == cardNo[side]-1)
      {
        // Only play the lowest of such ranks.
        leadList[numLeads] = lrLo;
        numLeads++;
      }
    }
    else if (lrHi > cardListHi[lho][0])
    {
      // Order is 1-2-0-3 or 1-3-0-2.  Either leader will win
      // (play highest), or partner will overtake (play lowest).
      leadList[numLeads] = lrLo;
      numLeads++;
      if (lrHi != lrLo)
      {
        leadList[numLeads] = lrHi;
        numLeads++;
      }
    }
    else if (cardListHi[lho][cardNo[lho]-1] > lrHi)
    {
      // Order is 2-1-0-3.  LHO must cover, so start low.
      leadList[numLeads] = lrLo;
      numLeads++;
    }
    else
    {
      // Order is 2-1-0-3 and LHO may or may not cover.
      /*
      leadList[numLeads] = lrHi;
      if (lrLo != lrHi)
        multiLead[lrHi] = true;
      numLeads++;
      */

      // Consider all the options.
      for (int mm = lrLo; mm <= lrHi; mm++)
      {
        leadList[numLeads] = mm;
        numLeads++;
      }
    }
  }

#ifdef HOLD_DEBUG
  cout << "Leads for side " << side << ":\n";
  for (int i = 0; i < numLeads; i++)
    cout << setw(2) << i << "  " << i << "\n";
#endif
}


void Holding::RewindLho()
{
  numLhos = 0;
  lhoCurrIndex = 0;

  unsigned numLhoGroups = cardNo[lho];

  if (numLhoGroups > 1 && 
      cardListHi[lho][numLhoGroups-1] == leadRank-1 &&
      cardListLo[lho][numLhoGroups-2] == leadRank+1 &&
      cardListHi[rho][0] < leadRank)
  {
    // Rather special case of type AJx / QT / Kxx / -.
    // If the jack is led, never play the T, so skip a group.

    numLhoGroups--;
  }

  for (unsigned l = 0; l < numLhoGroups; l++)
  {
    int c = cardListLo[lho][l];
    if (l == numLhoGroups-1)
    {
      // Lowest card is always an option.
      // Could also be void.
      lhoList[numLhos] = c;
      numLhos++;
#ifdef HOLD_DEBUG
cout << "P0 " << l << "\n";
#endif
      continue;
    }
    else if (c < leadRank)
    {
      // Only the lowest of these cards counts.
#ifdef HOLD_DEBUG
cout << "R1 " << l << "\n";
#endif
      continue;
    }
    else if (c < minRank[pard])
    {
      // No point in going higher.
#ifdef HOLD_DEBUG
cout << "R2 " << l << "\n";
#endif
      continue;
    }
    else if (c < cardListHi[rho][0])
    {
      // RHO has a higher card anyway.
#ifdef HOLD_DEBUG
cout << "R3 " << l << "\n";
#endif
      continue;
    }
    else
    {
      int nextDown = Max(cardListHi[rho][0], cardListLo[lho][l+1]);
      if (nextDown < leadRank)
      {
        // No lower card that still beats the lead
        lhoList[numLhos] = c;
        numLhos++;
#ifdef HOLD_DEBUG
cout << "P4 " << l << "\n";
#endif
        continue;
      }

      for (unsigned i = 0; i < cardNo[pard]; i++)
      {
        int pc = cardListLo[pard][i];
        if (pc < nextDown)
        {
          // Nothing more forthcoming
#ifdef HOLD_DEBUG
cout << "R5 " << l << " i " << i << " nextDown " << nextDown << "\n";
#endif
          break;
        }
        else if (pc < c)
        {
          // Partner has finesse, so the card is useful.
          lhoList[numLhos] = c;
          numLhos++;
#ifdef HOLD_DEBUG
cout << "P6 " << l << "\n";
#endif
          break;
        }
      }
#ifdef HOLD_DEBUG
cout << "R7 " << l << "\n";
#endif
    }
  }

#ifdef HOLD_DEBUG
  cout << "LHO for side " << sie << " lead " << leadRank << ":\n";
  for (int i = 0; i < numLhos; i++)
    cout << setw(2) << i << "  " << lhoList[i] << "\n";
#endif
}


void Holding::RewindPard()
{
  numPards = 0;
  pardCurrIndex = 0;

  for (unsigned l = 0; l < cardNo[pard]; l++)
  {
    int prLo = cardListLo[pard][l];
    int prHi = cardListHi[pard][l];

    int m = Max(leadRank, prLo);
    if (m < maxDefRun)
      continue;

    pardList[numPards] = (prLo > leadRank ? prHi : prLo);
    numPards++;
  }

#ifdef HOLD_DEBUG
  cout << ("Pard for side " << side << " lead " << leadRank <<
    " LHO " << lhoRank << ":\n";
  for (int i = 0; i < numPards; i++)
    cout << setw(2) << i << "  " << pardList[i];
#endif
}


bool Holding::NextLead()
{
  if (leadCurrIndex >= numLeads)
    return false;

  leadRank = leadList[leadCurrIndex];
  leadCurrIndex++;
#ifdef HOLD_DEBUG
  cout << "Play lead " << leadRank << "\n";
#endif
  return true;
}


bool Holding::NextLho()
{
  if (lhoCurrIndex >= numLhos)
    return false;

  lhoRank = lhoList[lhoCurrIndex];
#ifdef HOLD_DEBUG
  cout << "Play LHO " << lhoRank << "\n";
#endif
  lhoCurrIndex++;
  maxDefRun = Max(lhoRank, cardListHi[rho][0]);
  return true;
}


bool Holding::NextPard()
{
  if (pardCurrIndex >= numPards)
    return false;

  pardRank = pardList[pardCurrIndex];
  pardCurrIndex++;

  if (leadRank > pardRank)
  {
    winSide = side;
    if (maxDef == -1 && length[QT_PARD] == 0)
      winRank = static_cast<int>(suitLength); // This means "void"
    else
      winRank = leadRank;
  }
  else
  {
    winSide = pard;
    winRank = pardRank;
  }

#ifdef HOLD_DEBUG
  cout << "Play pard " << pardRank << "\n";
#endif
  return true;
}


void Holding::SetRhoNo()
{
  // RHO will lose and so will play low.
  rhoRank = cardListLo[rho][cardNo[rho]-1];
}


posType Holding::GetSide() const
{
  return side;
}


posType Holding::GetWinSide()
{
  return winSide;
}


int Holding::GetPrependRank() const
{
  return static_cast<int>
    (SDS_ACE - (suitLength - static_cast<unsigned>(winRank) - 1));
}


int Holding::GetMaxOppRank()
{
  int m = Max(cardListHi[QT_LHO][0], cardListHi[QT_RHO][0]);
  //return m;

  /*
  if (m == -1)
    return 0;
  else
  */
    return SDS_VOID - (static_cast<int>(suitLength) - m);
}


int Holding::GetSuitLength() const
{
  return static_cast<int>(suitLength);
}


int Holding::GetCounter() const
{
  return counter;
}


unsigned Holding::GetLength(const int player) const
{
  assert(player >= 0 && player < DDS_HANDS);
  return length[player];
}


unsigned Holding::GetMinDeclLength() const
{
  return Min(length[QT_ACE], length[QT_PARD]);
}


bool Holding::LHOIsVoid() const
{
  return (length[QT_LHO] == 0);
}


unsigned Holding::GetLHOMaxRank() const
{
  int m = cardListHi[QT_LHO][0];
  if (m == -1)
    return 0;
  else
    return SDS_VOID - (static_cast<int>(suitLength) - m);
}


bool Holding::IsAATrick() const
{
  return (side == QT_ACE && winSide == QT_ACE);
}


void Holding::MakeRanks()
{
  int h, p, r;

  for (h = 0; h < DDS_HANDS; h++)
  {
    cardNo[h] = 0;
    length[h] = 0;
    for (r = 0; r < SDS_MAX_RANKS; r++)
    {
      cardListLo[h][r] = 0;
      cardListHi[h][r] = 0;
      completeList[h][r] = 0;
    }
  }

  cardNo[QT_ACE] = 1;
  cardListLo[QT_ACE][0] = static_cast<int>(suitLength-1);
  cardListHi[QT_ACE][0] = static_cast<int>(suitLength-1);

  length[QT_ACE] = 1;
  completeList[QT_ACE][0] = static_cast<int>(suitLength-1);

  int hlast = QT_ACE;

  for (int m = static_cast<int>(suitLength)-2; m >= 0; m--)
  {
    p = (counter >> (2*m)) & 3;

    // Play lowest of equals, other than the ace.
    if (p == hlast)
    {
      if (cardListLo[p][cardNo[p]-1] != static_cast<int>(suitLength)-1)
        cardListLo[p][cardNo[p]-1] = m;
    }
    else
    {
      cardListLo[p][cardNo[p]] = m;
      cardListHi[p][cardNo[p]] = m;
      cardNo[p]++;
    }

    hlast = p;

    completeList[p][length[p]] = m;
    length[p]++;
  }

  for (h = 0; h < DDS_HANDS; h++)
  {
    if (cardNo[h])
      continue;
    
    cardListLo[h][0] = -1;
    cardListHi[h][0] = -1;

    cardNo[h] = 1;
  }

  for (h = 0; h < DDS_HANDS; h++)
    minRank[h] = cardListLo[h][cardNo[h]-1];

  maxDef = Max(cardListLo[QT_LHO][0], cardListLo[QT_RHO][0]);
}


unsigned Holding::GetNumTops()
{
  unsigned n = 1;
  while (n < length[QT_ACE] &&
      completeList[QT_ACE][n] == completeList[QT_ACE][n-1]-1)
    n++;

  return n;
}


posType Holding::GetOppBest()
{
  if (length[QT_LHO] == 0)
    return QT_RHO;
  else if (length[QT_RHO] == 0)
    return QT_LHO;
  else
  {
    int lhoLo = cardListLo[QT_LHO][0];
    int lhoHi = cardListHi[QT_LHO][0];
    int rhoLo = cardListLo[QT_RHO][0];
    int rhoHi = cardListHi[QT_RHO][0];
    
    if (lhoLo == rhoHi+1 || rhoLo == lhoHi+1)
      return QT_BOTH;
    else
      return (lhoLo > rhoLo ? QT_LHO : QT_RHO);
  }
}


const int loMask[13] =
{
  0x000000, // 2
  0x000003, // 3
  0x00000f, // 4
  0x00003f, // 5
  0x0000ff, // 6
  0x0003ff, // 7
  0x000fff, // 8
  0x003fff, // 9
  0x00ffff, // T
  0x03ffff, // J
  0x0fffff, // Q
  0x3fffff, // K
  0xffffff  // A
};

const int hiMask[13] =
{
  0xfffffc, // 2
  0xfffff0, // 3
  0xffffc0, // 4
  0xffff00, // 5
  0xfffc00, // 6
  0xfff000, // 7
  0xffc000, // 8
  0xff0000, // 9
  0xfc0000, // T
  0xf00000, // J
  0xc00000, // Q
  0x000000, // K
  0x000000, // A
};


void Holding::AdjustWinRank(
  const int used[])
{
  // We only adjust winRank when the lead wins and when the defense
  // has a card higher than the winner (so a finesse).
  if (winRank != leadRank || winRank > maxDef)
    return;

  // We look for the next rank down, skipping over played cards.
  int soughtRank = winRank-1;
  if (soughtRank == lhoRank)
  {
    soughtRank--;
    if (soughtRank == rhoRank)
      soughtRank--;
  }
  else if (soughtRank == rhoRank)
  {
    soughtRank--;
    if (soughtRank == lhoRank)
      soughtRank--;
  }

  if (soughtRank == -1)
    return;
// cout << "AWR soughtRank " << soughtRank << "\n";

  // If declarer holds that card, 
  if (leadCurrIndex < numLeads && leadList[leadCurrIndex] == soughtRank)
  {
// cout << "AWR1 adjusting from " << winRank << " to " << soughtRank << "\n";
    winRank = soughtRank;
  }
  else
  {
    // Could have some kind of table instead.
    for (unsigned p = 0; p < length[pard]; p++)
    {
      if (completeList[pard][p] == soughtRank)
      {
// cout << "AWR2 adjusting from " << winRank << " to " << soughtRank << "\n";
        winRank = soughtRank;
        return;
      }
    }
  }
}


bool Holding::MakePlay(
  int& slNew,
  int& cNew)
{
  // true if a move can be looked up, false if the trick is the
  // last one that can be won by declarer.

  // If the lead ends up winning the trick, we want to play the
  // highest of equals in order to have the most sparing use of ranks.  
  // If not, the lowest of equals for the same reason.

  slNew = static_cast<int>(suitLength);
  cNew = counter;

  int used[13] = {0};
  if (rhoRank != -1)
    used[rhoRank] = 1;

  if (lhoRank != -1)
    used[lhoRank] = 1;

  bool maybeNewAce = false;

  if (leadRank == static_cast<int>(suitLength)-1)
  {
    maybeNewAce = true;
    slNew--;
  }
  else if (leadRank != -1)
  {
    // In certain cases we still have to determine the actual lead
    // from among equals, e.g. Ax / - / QJ / Kx.  If the H was covered,
    // then it was retroactively the Q; if ducked, the J.

    /*
    if (multiLead[leadRank])
    {
      if (leadRank > pardRank)
      {
        used[leadRank-1] = 1;
        winRank--;
      }
      else if (leadRank > lhoRank)
      {
        used[leadRank-1] = 1;
      }
      else
        used[leadRank] = 1;
    }
    else
    */
      used[leadRank] = 1;
  }


  if (pardRank == static_cast<int>(suitLength)-1)
  {
    maybeNewAce = true;
    slNew--;
  }
  else if (pardRank != -1)
    used[pardRank] = 1;

  Holding::AdjustWinRank(used);

  for (int u = static_cast<int>(suitLength)-2; u >= 0; u--)
  {
    if (! used[u])
      continue;
    slNew--;
    cNew = (cNew & loMask[u]) | ((cNew & hiMask[u]) >> 2);
  }

  if (slNew <= 0)
    return false;

  // 0 is 2, ..., 12 is A, 13 is void(!).
  rankMap[13] = 13;

  int revRunRank = 12;
  if (! maybeNewAce)
  {
    // Original ace still in play.
    rankMap[revRunRank--] = 12;
  }

  for (int u = static_cast<int>(suitLength)-2; u >= 0; u--)
  {
    if (used[u])
      continue;

    rankMap[revRunRank--] = 12 - (static_cast<int>(suitLength)-u-1);
  }

  if (maybeNewAce)
  {
    // cNew is in a sense done already, as the "ace" is implicit.
    // But we have to check who has the new "ace".

    posType newTop = static_cast<posType>((cNew >> 2*(slNew-1)) & 0x3);
    if (newTop == QT_LHO || newTop == QT_RHO)
    {
      // This was the last quick trick for declarer.
      return false;
    }
    else if (newTop == QT_PARD)
      aceFlip = true;
    else
      aceFlip = false;
  }
  else
    aceFlip = false;

  if (aceFlip)
  {
    int cFlip = cNew;
    int flipBit;
    for (int s = 0; s < slNew; s++)
    {
      flipBit = 0x2 << (2*s);
      cFlip ^= flipBit;
    }

    cNew = cFlip;
  }

  mergeSpecialFlag = false;
  if ((length[QT_LHO] == 0 && length[QT_RHO] == 0 &&
      length[QT_ACE] >= length[QT_PARD] && length[QT_PARD] >= 2) ||
      // length[QT_ACE] >= 2 && length[QT_PARD] >= 2) ||
     (length[QT_ACE] == length[QT_PARD] && 
      length[QT_LHO] <= 1 && length[QT_RHO] <= 1))
    mergeSpecialFlag = true;

  /*
  if (length[QT_LHO] == 0 && length[QT_RHO] == 0 &&
      length[QT_ACE] >= 2 && length[QT_PARD] >= 2)
    mergeCat = SDS_MERGE_VOID_VOID;
  else if (length[QT_ACE] == length[QT_PARD] && 
      length[QT_LHO] <= 1 && length[QT_RHO] <= 1)
    mergeCat = SDS_MERGE_EQUAL_VOID;
  */

  return true;
}


bool Holding::GetAceFlip() const
{
  return aceFlip;
}


int Holding::GetNewRank(int r) const
{
  // Used when updating a looked-up trick list and appending
  // a new trick.

  int rNew = rankMap[r];

  // Normally PP1Q + BA1A = PP1A + BA1- = PA2Q.
  // But there is a special Prepend case where it becomes PA2A.
  // So we can't throw away that information.

  if (mergeSpecialFlag)
    return rNew;
  else
    return (Holding::GetPrependRank() < rNew ? SDS_VOID : rNew);
}


bool Holding::GetMergeType() const
{
  return mergeSpecialFlag;
}


unsigned Holding::PlayerTopsOverRank(
  const posType& player,
  const int& rank)
{
  // TODO No c???
  unsigned i = 0;
  unsigned c = 0;
  while (i < length[player] && completeList[player][i] > rank)
  {
    c++;
    i++;
  }

  return c;
}


void Holding::FlipAceToPard(
  int& maskFull,
  int& cFlipped)
{
  cFlipped = counter;
  maskFull = (1 << (2*(suitLength-1))) - 1;

  for (int n = 0; n < static_cast<int>(suitLength)-1; n++)
  {
    int mask = 0x3 << (2*n);
    int p = counter & mask;
    // TODO: This look wrong! Only works for bottom bit?
    if (p == QT_ACE)
      cFlipped = (cFlipped & (maskFull ^ mask)) | (QT_PARD << (2*n));
  }
}


int Holding::FlipTops(
  const int numTops,
  const int nMask)
{
  int maskFull, cFlipped;
  Holding::FlipAceToPard(maskFull, cFlipped);

  // Ace is implicit
  int expMask = 0;
  for (int n = 0; n < numTops-1; n++)
  {
    if (nMask & (1 << n))
      expMask |= (0x3 << (2*n));
  }

  expMask = (expMask << 2*(static_cast<int>(suitLength)-numTops));

  if (nMask & (1 << (numTops-1))) // Ace also to be flipped
    return (expMask & counter) | ((maskFull ^ expMask) & cFlipped);
  else
    return (expMask & cFlipped) | ((maskFull ^ expMask) & counter);
}


unsigned Holding::ListToRank(const int listValue)
{
  return (SDS_VOID - suitLength + static_cast<unsigned>(listValue));
  
}

void Holding::Print(
  ostream& out,
  const bool showBinary) const
{
  if (showBinary)
    out << "sl " << suitLength << " c " << hex << counter << dec << "\n";
  Holding::ToText(out);
}


void Holding::ToText(
  ostream& out) const
{
  assert(suitLength < 14);

  char text[4][20];
  char * p[4];
  for (int i = 0; i < 4; i++)
  {
    strcpy(text[i], "");
    p[i] = text[i];
  }

  int r = SDS_ACE;
  * p[QT_ACE] = SDS_RANK_NAMES[r];
  p[QT_ACE]++;
  r--;

  int pl;
  for (int m = static_cast<int>(suitLength)-2; m >= 0; m--)
  {
    pl = (counter >> (2*m)) & 3;
    * p[pl] = SDS_RANK_NAMES[r];
    p[pl]++;
    r--;
  }

  for (pl = 0; pl < 4; pl++)
  {
    if (p[pl] == text[pl])
    {
      * p[pl] = '-';
      p[pl]++;
    }
    * p[pl] = '\0';
  }

  out <<
    setw(8) << "" << setw(12) << left << text[0] << "\n" <<
    setw(12) << left << text[3] << setw(4) << "" << 
      setw(12) << text[1] << "\n" <<
    setw(8) << "" << setw(12) << left << text[2] << "\n";
}


void Holding::PrintRanks() const
{
  cout << "Lowest new ranks\n";
  for (unsigned i = 0; i < DDS_HANDS; i++)
  {
    if (cardNo[i] > 13)
    {
      cout << "i " << i << " cardNo " << cardNo[i] << endl;
      assert(false);
    }

    cout << i << ": " << setw(2) << cardNo[i] << ", ";
    for (unsigned j = 0; j < cardNo[i]; j++)
      cout << cardListLo[i][j] << "-";
    cout << "\n";
  }
  cout << "\n";

  cout << "Highest new ranks\n";
  for (unsigned i = 0; i < DDS_HANDS; i++)
  {
    cout << i << ": " << setw(2) << cardNo[i] << ", ";
    for (unsigned j = 0; j < cardNo[i]; j++)
      cout << cardListHi[i][j] << "-";
    cout << "\n";
  }
  cout << "\n";

  cout << "Complete ranks\n";
  for (unsigned i = 0; i < DDS_HANDS; i++)
  {
    cout << i << ": " << setw(2) << length[i] << ", ";
    for (unsigned j = 0; j < length[i]; j++)
      cout << completeList[i][j] << "-";
    cout << "\n";
  }
  cout << "\n";
}


void Holding::PrintPlay()
{
assert(false);
}


void Holding::PrintPlayNew(
  std::ostream& out)
{
  out << "side " << 
    static_cast<int>(side) << " trick " << leadRank <<
    " (" << lhoRank << ") " << pardRank << " (" << rhoRank << ")\n";
}

