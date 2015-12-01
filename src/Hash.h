/* 
   SDS, a bridge single-suit double-dummy quick-trick solver.

   Copyright (C) 2015 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SDS_HASH_H
#define SDS_HASH_H

#include <iostream>
#include <vector>

#include "Header.h"


#define SDS_HASH_LENTRICK 4096
#define SDS_HASH_LENRANK 65536
#define SDS_HASH_LENCASE 4096
#define SDS_HASH_LENRANKCASE 131072

#define ML_MAXKEY (1 << 20)


class Hash
{
  private:


    std::vector<unsigned> trickKeyMap;
    std::vector<unsigned> rankKeyMap;
    std::vector<unsigned> caseKeyMap;
    std::vector<unsigned> rankCaseKeyMap;

    unsigned keyMapNo;
    unsigned rankMapNo;
    unsigned caseMapNo;
    unsigned rankCaseMapNo;

  public:

    Hash();

    ~Hash();

    unsigned GetKey(
      const Header& header);

    void PrintCounts() const;
};

#endif

