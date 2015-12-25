/* 
   SDS, a bridge single-suit double-dummy quick-trick solver.

   Copyright (C) 2015 by Soren Hein.

   See LICENSE and README.
*/


#include <stdio.h>
#include <assert.h>

#include "cst.h"
#include "DefList.h"
#include "MoveList.h"
#include "SimpleMoves.h"
#include "ComplexMoves.h"
#include "Twiddle.h"
#include "Dist.h"
#include "LoopHold.h"

using namespace std;

#define DEBUG_SIMPLE


extern singleType * singles[14];

extern MoveList moveList;

extern int histCount[HIST_SIZE][14], histRank[HIST_SIZE][14];
extern int histMoveCount[HIST_SIZE][14], histMoveRank[HIST_SIZE][14];
extern bool debugComplex;


extern vector<unsigned> holdCtr;

void MakeMovesSimple()
{
  DefList def;
  LoopHold holding, hNew;
  HoldingDetails hdetNew;
  bool newFlag;
  posType start, end;
  int r, t;
  int brank, rrank, crank, crank2, btricks, rtricks, ctricks;
  HoldingSimpleMove move;
  Trick trick, trick2, trick3;

  for (int sl = 1; sl <= 13; sl++)
  {
    cout << "\nSimple moves: " << sl << "\n";

    for (int c = 0; c < SDS_NUMSINGLES[sl]; c++)
    {
      if ((c & 0x3ffff) == 0)
      {
        cout << "Count " <<
          setw(8) << c << " (" <<
          setw(5) << std::fixed << std::setprecision(2) <<
            100. * c / SDS_NUMSINGLES[sl] << "%)" << endl;
      }

      if (singles[sl][c].moveNo)
        continue;

      holding.Set(sl, c);

      unsigned lenAce = holding.GetLength(QT_ACE);
      unsigned lenPard = holding.GetLength(QT_PARD);

      if (lenAce == holding.GetNumTops())
      {
        if (lenPard <= lenAce || ! holding.CashoutPard(t, r))
        {
          // Tops opposite partner who is either cashed out,
          // or whose cards are too low to yield more tricks.
          // Examples:  AK / xx / x / - or AK / Qxx / JT9 / -.
          //
	  // Sum (3.6% of cases)			97	810,075
          // -, AA    1-13 -                            13           13
          // A, AA/BA 1-12 A                            24      531,592
          // K, AA/BA 2-11 K                            20      178,760
          // Q, AA/BA 3-10 Q                            16       71,162
          // J, AA/BA 4- 9 J                            12       26,240
          // T, AA/BA 5- 8 T                             8        2,258
          // 9, AA/BA 6- 7 9                             4           50

          holding.SolveShortAce(r);

          start = (lenPard ? QT_BOTH : QT_ACE);
          trick.Set(start, QT_ACE, r, lenAce);
          def.Set1(trick);
          unsigned mno = moveList.AddMoves(def, holding, newFlag);
          SetAllPermutations(sl, c, mno, holding, r, 
            HIST_ACE_SHORT, newFlag);
        }
        else
        {
          // Blocked tops opposite long, non-void partner.
          //
	  // Sum (1.7% of cases)			93	385,660
          // A, BA1A + PP 1-11 -                        11          725
          // K, BA1A + PP 1-9 K, BA2K + PP 1-9 -        18      179,789
          // Q, BA1A + PP 1-8 Q, BA2K + PP 1-6 Q,       21      140,363
          //    BA3Q + PP 1-7 -
          // J, BA1A + PP 2-7 J, BA2K + PP 1-5 J,       19       49,403
          //    BA3Q + PP 1-3 J, BA4J + PP 1-5 J
          // T, BA1A + PP 2-6 T, BA2K + BB 1-4 T,       14       13,003
          //    BA3Q + PP 1-2 T, BA5T + PP 1-3 -
          // 9, BA1A + PP 3-5 9, BA2K + PP 2-3 9,        7        2,223
          //    BA3Q + PP 1-1 9, BA69 + PP 1-1 -
          // 8, BA1A + PP 3-4 8, BA3Q + PP 1-1 8         3          154

          assert(t > 0);

          brank = SDS_VOID - static_cast<int>(lenAce);
          int rmin = Min(brank, r);
          
          trick.Set(QT_BOTH, QT_ACE, brank, lenAce);
          trick2.Set(QT_PARD, QT_PARD, r, static_cast<unsigned>(t));
          def.Set2(trick, trick2);

          unsigned mno = moveList.AddMoves(def, holding, newFlag);
          SetAllPermutations(sl, c, mno, holding, rmin, 
            HIST_BLOCKED, newFlag);
        }
      }

      if (singles[sl][c].minLen == 0)
      {
        if (singles[sl][c].moveNo == 0)
        {
          // Partner is void.  Cash maximum from aceholder.
          //
	  // Sum (3.5% of cases)			10	780,792
          // A, AA 2-12 A                                0      519,818
          // K, AA 2-11 K                                0      169,074
          // Q, AA 2    Q                                1       61,826
          // J, AA 3    J                                1       21,328
          // T, AA 3-4  T                                2        6,940
          // 9, AA 4-5  9                                2        1,600
          // 8, AA 4-6  8                                3          198
          // 7, AA 5    7                                3            8

          holding.CashoutAce(t, r);

          trick.Set(QT_ACE, QT_ACE, r, static_cast<unsigned>(t));
          def.Set1(trick);
          unsigned mno = moveList.AddMoves(def, holding, newFlag);
          SetAllPermutations(sl, c, mno, holding, r, 
            HIST_PARD_VOID, newFlag);
        }

        // Can also make some crash positions out of this, e.g. AQ / K.
        //
        // Sum (4.9% of cases)                         573    1,097,430
        // K                                            30      264,949
        // BB 2-11 K                            10
        // BA(n+1)A/AP1K + AA 1-11 -            11
        // BA1A/BP1K + AA 1-9 -                  9
        // Q                                            70      455,512
        // BB 3-10 Q, BAnK/AB 3-11 Q            17
        // BA(n+1)A/AP1Q + AA 3-12 -            10
        // BA(n+1)K/AP1Q + AA 2-9 -              8
        // BA(n+1)Q/AP1K + AA 1-9 -              9
        // BA2K/BP1Q + AA 2-8 -                  7
        // BB2K/BP2Q + AA 1-6 -                  6
        // BB2K/BA2Q + PP 1-6 -                  6
        // BA2Q/BP1K + AA 2-8 Q                  7
        // J                                           108      267,521
        // BB3J, BAn(K or Q)/AB 4-(11 or 10) J  16
        // BA(n+1)A/AP1J + AA 3-11 -             9
        // BA(n+1)K/AP1J + AA 3-9 -              7
        // BA(n+1)Q/AP1J + AA 3-8 -              6
        // BA(n+1)J/AP1K + AA 2-8 -              7
        // BA(n+1)J/AP1K + AA 2-8 Q              7
        // BA(n+1)J/AP1Q + AA 1-8 -              8
        // BA1A/BP1K + AA 2-6 J                  5
        // BA3Q/BP1J + AA 3-7 -                  5
        // BB3Q/BP2J + AA 2-5 - or PP 2-5 -      8
        // BB3Q/BP3J + AA 1-3 - or PP 1-3 -      6
        // BA(2 or 3)J/BP1K + AA (2 or 3)-7 J   11
        // BA3J/BP1Q + AA 3-7 J                  5
        // BB3J/BP2Q + AA 2-5 J or PP 2-5 J      8
        // T                                           123       81,918
        // BB3T, BAn(J|Q|K)/AB 5-(9|10|11) T,   19
        // BA(n+1)A/AP1T + AA 4-11 -             8
        // BA(n+1)K/AP1T + AA 4-9 -              6
        // BA(n+1)Q/AP1T + AA 4-8 -              5
        // BA(n+1)J/AP1T + AA 4-7 -              4
        // BA(n+1)T/AP1K + AA 2-7 (Q or J)      12
        // BA(n+1)T/AP1Q + AA 2-7 -              6
        // BA(n+1)T/AP1Q + AA 3-7 J              5
        // BA(n+1)T/AP1J + AA 3-7 -              5
        // BA1A/BP1K + AA 2-6 T                  5
        // BA2K/BP1(K|Q) + AA 3-4 T              4
        // BB2K/BA2Q + PP2T or /BP2Q + AA2T      2
        // BA4J/BP1T + AA 4-6 -                  3
        // BB4J/BP2T + AA3-4 - or BA2T + PP3-4 - 4
        // BB4J/BA3T + PP2- or BP3T + AA2-       2
        // BA(3 or 4)T/BP1(Q|K) +AA(3 or 4)-6 T 14
        // BB3T/BP2Q + AA 2-4 T or PP 2-4 -      6
        // BA4T/BP1J + AA 4-6 T                  3
        // BB4T/BP2(Q|J) + AA 3-4 T or PP 3-4 T  8
        // BB4T/BA3J + AA2T or /BP3J + PP2T      2
        // 9                                           110       22,137
        // BB39,BBn(T|J|Q|K)/AB 6-(8|9|10|11) T 19
        // BA(n+1)A/AP19 + AA 5-11 -             7
        // BA(n+1)K/AP19 + AA 5-9 -              5
        // BA(n+1)Q/AP19 + AA 5-8 -              4
        // BA(n+1)J/AP19 + AA 5-7 -              3
        // BA(n+1)9/AP1(K|Q) + AA 2-6 (Q|-)     10
        // BA(n+1)9/AP1(K|Q) + AA 3-6 (J|T)     16
        // BA(n+1)9/AP1(J|T) + AA 4-6 (T|-)      6
        // BA(n+1)9/AP1J + AA 3-6 -              4
        // BA(n+1)9/AP1T + AA 5-6 -              2
        // BA1A/BP1K + AA 3-5 9                  3
        // BA(n+1..4)9/AA or PP 2-3 9            6
        // BB(4..n)9/BP1(K|Q) + AA 3-5 9        12
        // BB(4..n)9/BP1J + AA 4-5 9             3
        // BB49/BP2J + AA or PP 39               2
        // BA2(K|Q)/BP1(Q|K) + AA 3-4 9          4
        // BB2K/BP2Q + AA or PP 29               2
        // BA59/BP1T + AA59                      1
        // BA5T/BP19 + AA5-                      1
        // 8                                            70        4,402
        // BAn(9..K)/AB 7-(7..11) 8             15
        // BA(n+1)A/AP18 + AA 6-11 -             6
        // BA(n+1)K/AP18 + AA 6-9 -              4
        // BA(n+1)Q/AP18 + AA 6..8 -             3
        // BA(n+1)8/AP1(K|Q) + AA 3-5 (J|T|9)   18
        // BA(n+1)8/AP1J + AA 3-5 -              3
        // BA(n+1)8/AP1J + AA 4-5 (T|9)          4
        // BA2(K|Q)/BP1(Q|K) + AA 3-4 8          4
        // BA(n+1)J/AP18 + AA 6-7 -              2
        // BA(n+1)8/AP1T + AA 4-5 -              2
        // BA7T/AP18 + AA6-                      1
        // BA68/AP1(9|T) + AA5(-|9)              2
        // BA48/BP1(J|Q|K) + AA48                3
        // BB2K/B(P|A)2Q + (AA|PP)28             2
        // BA1A/BP1K + AA48                      1
        // 7                                            38          769
        // BB47, BAn(T..K)/AB 8-(8..11) 7       11
        // BA(n+1)A/AP17 + AA 7..11 A            5
        // BA(n+1)K/AP17 + AA 7..9 A             3
        // BA(n+1)Q/AP17 + AA 7..8 A             2
        // BA8J/AP17 + AA7-                      1
        // BA(n+1)7/AP1J + AA 3-4 -              2
        // BA(n+1)7/AP1(Q|K) + AA 3-4 J          4
        // BA57/AP1(T|J|J|Q) + AA4(-|-|T|J)      4
        // BA57/AP1(J|Q|K) + AA4(8|9)            6
        // 6                                            13          172
        // BAn(J..K)/AB 9-(9..11)                6
        // BA(n+1)A/AP16 + AA 8..11 -            4
        // BA(n+1)K/AP16 + AA 8..9 -             2
        // BA9Q/AP16 + AA8-                      1
        // 5                                             7           41
        // BAn(K..A)/AB 10-(10..11)              3
        // BA(n+1)A/AP15 + AA 9-11 -             3
        // BA10K/AP15 + AA9-                     1
        // 4                                             3            8
        // BA11K/AB11 4                          1
        // BA(n+1)A/AA14 + AA 10-11 -            2
        // 3                                             1            1
        // BA12A/AP13 + AA11-                    1

        int numTops = static_cast<int>(holding.GetNumTops());
        posType oppBest = holding.GetOppBest();
/*
bool pl = false;
if (! singles[8][0x8af].defp)
  pl = true;
  */

        for (int nMask = 1; nMask < (1 << numTops)-1; nMask++)
        {
          // This also takes care of the surely-blocked flips from above.
          int cNew = holding.FlipTops(numTops, nMask);
          if (singles[sl][cNew].moveNo)
            continue;

          hNew.Set(sl, cNew);
          // No complete blocks.
          if (hNew.GetNumTops() == hNew.GetLength(QT_ACE))
            continue;

          hNew.SetDetails(hdetNew);

          posType blocked, bend, cend;
          hNew.SolveCrashTricks(hdetNew, oppBest, bend, cend,
            brank, rrank, crank, crank2, btricks, rtricks, ctricks);

          if (rtricks == 0 && crank2 != SDS_VOID && crank != crank2)
	  {
            assert(crank < crank2);

            trick.Set(QT_BOTH, QT_ACE, crank2, static_cast<unsigned>(ctricks));
            trick2.Set(QT_ACE, QT_PARD, crank, static_cast<unsigned>(ctricks));
            def.Set11(trick, trick2);

            r = Min(brank, rrank);
            r = Min(r, crank);
            unsigned mno = moveList.AddMoves(def, hNew, newFlag);

            SetAllLowCards(sl, cNew, mno, hNew, r, HIST_CRASH, newFlag);
	  }
	  else if (rtricks <= 0)
	  {
            trick.Set(QT_BOTH, cend, crank, static_cast<unsigned>(ctricks));
            def.Set1(trick);
        
            r = Min(brank, rrank);
            r = Min(r, crank);
            unsigned mno = moveList.AddMoves(def, hNew, newFlag);

            SetAllLowCards(sl, cNew, mno, hNew, r, HIST_CRASH, newFlag);
	  }
          else
          {
            blocked = (bend == QT_ACE ? QT_PARD : QT_ACE);
            posType bstart = (btricks + rtricks > ctricks ? QT_BOTH : bend);

            if (bstart == QT_BOTH &&
                ctricks <= btricks &&
                ((cend == QT_ACE && blocked == QT_PARD) ||
                 (cend == QT_BOTH && blocked == QT_PARD) ||
                 (cend == QT_BOTH && blocked == QT_ACE)))
            {
              if (hNew.GetMinDeclLength() == 1)
                trick.Set(QT_PARD, QT_ACE, crank, 
                  static_cast<unsigned>(ctricks));
              else
                trick.Set(QT_BOTH, QT_BOTH, crank, 
                  static_cast<unsigned>(ctricks));
            }
            else
              trick.Set(QT_BOTH, cend, crank, static_cast<unsigned>(ctricks));

            trick2.Set(bstart, blocked, brank, static_cast<unsigned>(btricks));
            trick3.Set(bend, bend, rrank, static_cast<unsigned>(rtricks));

            def.Set12(trick, trick2, trick3);
        
            r = Min(brank, rrank);
            r = Min(r, crank);
            unsigned mno = moveList.AddMoves(def, hNew, newFlag);
            SetAllPermutations(sl, cNew, mno, hNew, r, 
              HIST_CRASH, newFlag);
          }
        }
/*
if (pl && singles[8][0x8af].defp)
{
  cout << "Pos2 r " << r <<
    " sl " << sl << "
    " c " << hex << c << dec << "\n";
  holding.PrintNew(cout);
  hNew.PrintNew(cout);
  singles[8][0x8af].defp->PrintNew(cout);
  cout.flush();
  assert(false);
}
*/
      }

      if (singles[sl][c].moveNo)
        continue;

      if (holding.SolveStopped(move))
      {
        // Opponents have enough tops and length to hold declarer to
        // a number of tricks, often 1.  (For instance LHO has Kx.)
        //
        // Sum (47.7% of cases)                          0   10,677,984
        // May change when we improve SolveSparseTops().

        // >>>>> Need to turn move into a trick to begin with!
        trick.Set(move.start, move.end, move.rank, 
          static_cast<unsigned>(move.tricks));
        def.Set1(trick);

        // def.Set(move.start, move.end, move.rank, move.tricks);
        unsigned mno = moveList.AddMoves(def, holding, newFlag);
        SetAllPermutations(sl, c, mno, holding, move.rank, 
          HIST_SINGLE, newFlag);
      }
      else if (holding.CashoutBoth(def, r))
      {
// cout << "--- new " << sl << " " << hex << c << dec << "\n";
// holding.PrintNew();
// cout << "--- end ---\n";
        // Suits such as AQx / Kx.

        unsigned mno = moveList.AddMoves(def, holding, newFlag);
/*
if (pl && singles[8][0x3a0a].defp)
{
  cout << "Pos1\n";
  holding.PrintNew();
  cout.flush();
  assert(false);
}
*/
        SetAllPermutations(sl, c, mno, holding, r, 
          HIST_CASHES, newFlag);
      }
    }
  }

  PrintHistLowCards();

  cout << "\nHolding hits\n";
  for (unsigned i = 0; i < 1000; i++)
  {
    if (holdCtr[i] == 0) continue;
    cout << setw(4) << i << setw(10) << holdCtr[i] << "\n";
  }

  // cout.flush();
  // assert(false);


}


void UpdateHist(
  const int histNo,
  const int suitLength,
  const int counter,
  const int rank,
  const bool newFlag)
{
  histCount[histNo][singles[suitLength][counter].declLen]++;
  histRank[histNo][rank]++;

  if (newFlag)
  {
    histMoveCount[histNo][singles[suitLength][counter].declLen]++;
    histMoveRank[histNo][rank]++;
  }
}


void CompareRecurse(
  unsigned moveNo,
  Holding& holding,
  const int histNo)
{
#ifdef DEBUG_SIMPLE
  DefList& def = moveList.GetCombinedMove(moveNo);

  DefList defCmp1, defCmp2;
  MakeComplexSingleMove(holding, defCmp1, defCmp2);

  DefList defCmp;
  if (! defCmp.MergeSidesSoft(defCmp1, defCmp2))
    defCmp.MergeSidesHard(defCmp1, defCmp2);

  if (defCmp != def)
  {
    cout << "CompareRecurse error: histogram " << histNo <<
      ", " << HIST_NAMES[histNo] << "\n";
    holding.Print(cout);
    def.Print(cout);
    cout << "Actually solved\n";
    defCmp.Print(cout);
    cout.flush();

// debugComplex = true;
// DefList deft;
    // fflush(stdout);
// MakeComplexSingleMove(holding, deft);

    // fflush(stdout);

// assert(false);
  }
#endif
}
