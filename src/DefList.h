/* 
   SDS, a bridge single-suit double-dummy quick-trick solver.

   Copyright (C) 2015 by Soren Hein.

   See LICENSE and README.
*/


#ifndef SDS_DEFLIST_H
#define SDS_DEFLIST_H

#include <iostream>
#include <string>
#include <vector>

#include "AltList.h"
#include "cst.h"


class DefList
{
  private:

    AltList list[SDS_MAX_DEF];
    unsigned len;
    Header header;
    bool headerDirty;

    void operator += (
      AltList& alt);

    void Purge(
      const std::vector<bool>& skip);

    bool Reduce();

    void RegisterSize(
      const std::string& text);

  public:

    DefList();

    ~DefList();

    void Reset();

    bool IsEmpty();

    bool Set1(
      const Trick& trick);

    bool Set2(
      const Trick& trick1,
      const Trick& trick2);

    bool Set11(
      const Trick& trick1,
      const Trick& trick2);

    bool Set12(
      const Trick& trick1,
      const Trick& trick20,
      const Trick& trick21);

    Header& GetHeader();

    bool operator == (
      const DefList& def2) const;

    bool operator != (
      const DefList& def2) const;

    void operator += (
      const Holding& holding);

    void operator += (DefList& def2);

    void operator *= (DefList& def2);

    void MergeSides(
      DefList& def1,
      DefList& def2);

    void Print(
      std::ostream& out = std::cout,
      const std::string& text = "") const;
};

#endif
