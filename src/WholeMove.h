/* 
   SDS, a bridge single-suit double-dummy quick-trick solver.

   Copyright (C) 2015 by Soren Hein.

   See LICENSE and README.
*/


#ifndef SDS_WHOLEMOVE_H
#define SDS_WHOLEMOVE_H

#include <iostream>
#include "DefList.h"


class WholeMove
{
  private:

    DefList def1;
    DefList def2;
    DefList defMerged;

  public:

    friend class MoveList;

    WholeMove();

    ~WholeMove();

    void Reset();

    void Add(
      const DefList& res1);

    void Add(
      DefList& res1,
      DefList& res2);

    bool operator == (
      const DefList& def);

    bool operator == (
      const WholeMove& whole2);

    bool operator != (
      const WholeMove& whole2);

    Header& GetHeader();

    DefList& GetCombinedMove();

    unsigned GetKeyNew();

    unsigned GetTrickKey();

    unsigned GetRankKey();

    unsigned GetMaxRank();

    void Print(
      std::ostream& out = std::cout) const;

};

struct singleType
{
  unsigned moveNo;
  unsigned char declLen;
  unsigned char minLen;
  unsigned char oppLen;
};

#endif
