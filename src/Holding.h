/* 
   SDS, a bridge single-suit double-dummy quick-trick solver.

   Copyright (C) 2015 by Soren Hein.

   See LICENSE and README.
*/


#ifndef SDS_HOLDING_H
#define SDS_HOLDING_H

#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>

#include "cst.h"
#include "Trick.h"

class Trick;

#define SDS_MAX_RANKS 14


class Holding
{
  private:

    posType side;
    posType lho;
    posType pard;
    posType rho;

    int numLeads;
    int numLhos;
    int numPards;
    int leadCurrIndex;
    int lhoCurrIndex;
    int pardCurrIndex;
    int leadList[SDS_MAX_RANKS]; // Too large
    bool multiLead[SDS_MAX_RANKS]; // Set if certain touching honors
    int lhoList[SDS_MAX_RANKS];
    int pardList[SDS_MAX_RANKS];

    int leadRank;
    int lhoRank;
    int pardRank;
    int rhoRank;
    int maxDef;
    int maxDefRun;
    int minRank[DDS_HANDS];

    posType winSide;
    int winRank;

    int rankMap[14];
    bool aceFlip;
    bool mergeSpecialFlag;

    void MakeRanks();

    void FlipAceToPard(
      int& maskFull,
      int& cFlipped);

    void ToText(
      std::ostream&) const;

  protected:

    unsigned suitLength;
    int counter;

    int cardListLo[DDS_HANDS][SDS_MAX_RANKS];
    int cardListHi[DDS_HANDS][SDS_MAX_RANKS];
    unsigned cardNo[DDS_HANDS];
    int completeList[DDS_HANDS][SDS_MAX_RANKS];
    unsigned length[DDS_HANDS];

    unsigned ListToRank(const int listValue);

    unsigned PlayerTopsOverRank(
      const posType& player,
      const int& rank);

    void AdjustWinRank(const int used[]);


  public:

    Holding();

    ~Holding();

    void Set(
      const int suitLength,
      const int counter);

    void RewindLead();
    void RewindLho();
    void RewindPard();

    bool NextLead();
    bool NextLho();
    bool NextPard();
    void SetRhoNo();

    void SetSide(const int side);

    posType GetSide() const;
    posType GetWinSide();

    const Trick GetTrick() const;

    int GetPrependRank() const;
    int GetMaxOppRank();
    int GetSuitLength() const;
    int GetCounter() const;
    unsigned GetLength(const int player) const;
    unsigned GetMinDeclLength() const;
    bool LHOIsVoid() const;
    unsigned GetLHOMaxRank() const;
    bool IsAATrick() const;

    unsigned GetNumTops();
    posType GetOppBest();

    bool MakePlay(
      int& slNew,
      int& cNew);

    bool GetAceFlip() const;

    int GetNewRank(int r) const;

    bool GetMergeType() const;

    int FlipTops(
      const int numTops,
      const int nMask);

    void Print(
      std::ostream& out = std::cout,
      const bool showBinary = true) const;

    void PrintRanks() const;
    
    void PrintPlay();

    void PrintPlayNew(
      std::ostream& pout);
};

#endif
