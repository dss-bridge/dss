/* 
   SDS, a bridge single-suit double-dummy quick-trick solver.

   Copyright (C) 2015 by Soren Hein.

   See LICENSE and README.
*/


#ifndef SDS_WHOLEMOVE_H
#define SDS_WHOLEMOVE_H

#include "DefList.h"


class WholeMove
{
  private:

    DefList def1;
    DefList def2;

  public:

    WholeMove();

    ~WholeMove();

    void Reset();

};

#endif
