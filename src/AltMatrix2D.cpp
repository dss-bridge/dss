/* 
   SDS, a bridge single-suit double-dummy quick-trick solver.

   Copyright (C) 2015 by Soren Hein.

   See LICENSE and README.
*/


#include <iomanip>
#include <assert.h>

#include "AltMatrix2D.h"

using namespace std;


AltMatrix2D::AltMatrix2D()
{
  AltMatrix2D::Reset();
}


AltMatrix2D::~AltMatrix2D()
{
}


void AltMatrix2D::Reset()
{
  numX = 0;
  numY = 0;
}


void AltMatrix2D::SetDimensions(
  const unsigned x,
  const unsigned y)
{
  assert(x <= SDS_MAX_ALT);
  assert(y <= SDS_MAX_ALT);

  numX = x;
  numY = y;

  for (unsigned i = 0; i < numX; i++)
    activeX[i] = true;

  for (unsigned j = 0; j < numY; j++)
    activeY[j] = true;

  for (unsigned i = 0; i < numX; i++)
    for (unsigned j = 0; j < numY; j++)
      matrix[i][j] = SDS_HEADER_SAME;
}


bool AltMatrix2D::IsPurgedX(
  const unsigned x) const
{
  return ! activeX[x];
}


bool AltMatrix2D::IsPurgedY(
  const unsigned y) const
{
  return ! activeY[y];
}


void AltMatrix2D::SetValue(
  const unsigned x,
  const unsigned y,
  const cmpDetailType c)
{
  assert(x < numX);
  assert(y < numY);

  matrix[x][y] = c;
}


void AltMatrix2D::PurgeX(
  const unsigned x)
{
  assert(x < numX);
  assert(activeX[x]);
  activeX[x] = false;
}


void AltMatrix2D::PurgeY(
  const unsigned y)
{
  assert(y < numY);
  assert(activeY[y]);
  activeY[y] = false;
}


void AltMatrix2D::ResetVectors(
  bool has[SDS_MAX_ALT][SDS_HEADER_CMP_SIZE],
  const bool active[],
  const unsigned len,
  bool hasSum[])
{
  for (unsigned i = 0; i < len; i++)
  {
    if (! active[i])
      continue;

    for (unsigned c = 0; c < SDS_HEADER_CMP_SIZE; c++)
      has[i][c] = false;
  }

  for (unsigned c = 0; c < SDS_HEADER_CMP_SIZE; c++)
    hasSum[c] = false;
}


void AltMatrix2D::VerifyVector(
  const bool has[SDS_MAX_ALT][SDS_HEADER_CMP_SIZE],
  const bool active[],
  const unsigned len,
  bool hasSum[],
  bool verifyFlag)
{
  for (unsigned i = 0; i < len; i++)
  {
    if (! active[i])
      continue;

    // This could be one table look-up, one bit per value.

    if (has[i][SDS_HEADER_PLAY_OLD_BETTER] ||
        has[i][SDS_HEADER_RANK_OLD_BETTER])
    {
      if (has[i][SDS_HEADER_PLAY_NEW_BETTER] ||
          has[i][SDS_HEADER_RANK_NEW_BETTER] ||
          has[i][SDS_HEADER_SAME])
      {
        if (verifyFlag)
        {
          AltMatrix2D::Print(cerr, "Verify error I");
	  assert(false);
        }
      }

      if (has[i][SDS_HEADER_PLAY_OLD_BETTER])
        hasSum[SDS_HEADER_PLAY_OLD_BETTER] = true;

      if (has[i][SDS_HEADER_RANK_OLD_BETTER])
        hasSum[SDS_HEADER_RANK_OLD_BETTER] = true;
    }
    else if (has[i][SDS_HEADER_PLAY_NEW_BETTER] ||
        has[i][SDS_HEADER_RANK_NEW_BETTER])
    {
      if (has[i][SDS_HEADER_SAME])
      {
        if (verifyFlag)
        {
          AltMatrix2D::Print(cerr, "Verify error II");
	  assert(false);
        }
      }

      if (has[i][SDS_HEADER_PLAY_NEW_BETTER])
        hasSum[SDS_HEADER_PLAY_NEW_BETTER] = true;

      if (has[i][SDS_HEADER_RANK_NEW_BETTER])
        hasSum[SDS_HEADER_RANK_NEW_BETTER] = true;
    }
    else if (has[i][SDS_HEADER_SAME])
      hasSum[SDS_HEADER_SAME] = true;
    else
    {
      if (has[i][SDS_HEADER_PLAY_DIFFERENT])
        hasSum[SDS_HEADER_PLAY_DIFFERENT] = true;

      if (has[i][SDS_HEADER_RANK_DIFFERENT])
        hasSum[SDS_HEADER_RANK_DIFFERENT] = true;
    }
  }
}


void AltMatrix2D::Verify(
  const bool verifyFlag)
{
  AltMatrix2D::ResetVectors(hasX, activeX, numX, hasXsum);
  AltMatrix2D::ResetVectors(hasY, activeY, numY, hasYsum);

  for (unsigned i = 0; i < numX; i++)
  {
    if (! activeX[i])
      continue;

    for (unsigned j = 0; j < numY; j++)
    {
      if (! activeY[j])
        continue;

      hasX[i][matrix[i][j]] = true;
      hasY[j][matrix[i][j]] = true;
    }
  }

  AltMatrix2D::VerifyVector(hasX, activeX, numX, hasXsum, verifyFlag);
  AltMatrix2D::VerifyVector(hasY, activeY, numY, hasYsum, verifyFlag);
}


cmpDetailType AltMatrix2D::ComparePartial(
  const cmpDetailType diff,
  const cmpDetailType winX,
  const cmpDetailType winY)
{
  if (hasYsum[winX] && hasYsum[winY])
  {
    if (! (hasXsum[winX] && hasXsum[winY]))
    {
      cerr << "Error 1: " << 
        static_cast<int>(diff) << " " << 
        static_cast<int>(winX) << " " << 
        static_cast<int>(winY) << "\n";
      for (int c = 0; c < SDS_HEADER_CMP_SIZE; c++)
        cerr << setw(2) << c << 
          setw(5) << hasXsum[c] <<
          setw(5) << hasYsum[c];
      AltMatrix2D::Print(cerr, "Error 1");
      assert(false);
    }
  }

  if (hasXsum[winX] && hasXsum[winY])
  {
    if (! (hasYsum[winX] && hasYsum[winY]))
    {
      cerr << "Error 2: " << 
        static_cast<int>(diff) << " " << 
        static_cast<int>(winX) << " " << 
        static_cast<int>(winY) << "\n";
      for (int c = 0; c < SDS_HEADER_CMP_SIZE; c++)
        cerr << setw(2) << c << 
          setw(5) << hasXsum[c] <<
          setw(5) << hasYsum[c];
      AltMatrix2D::Print(cerr, "Error 2");
      assert(false);
    }
  }

  if (hasXsum[diff] && hasYsum[diff])
    return diff;
  else if (hasYsum[diff])
    return (hasYsum[winX] ? diff : winY);
  else if (hasXsum[diff])
    return (hasXsum[winY] ? diff : winX);
  else if (hasYsum[winY])
    return (hasYsum[winX] ? diff : winY);
  else if (hasXsum[winX])
    return (hasXsum[winY] ? diff : winX);
  else
    return SDS_HEADER_SAME;
}


cmpDetailType AltMatrix2D::ComparePartialDeclarer(
  const cmpDetailType diff,
  const cmpDetailType winX,
  const cmpDetailType winY)
{
  if (hasXsum[diff] && hasYsum[diff])
    return diff;
  else if (hasYsum[diff])
    return (hasYsum[winY] ? diff : winX);
  else if (hasXsum[diff])
    return (hasXsum[winX] ? diff : winY);
  else if (hasYsum[winY])
    return (hasYsum[winX] ? diff : winY);
  else if (hasXsum[winX])
    return (hasXsum[winY] ? diff : winX);
  else
    return SDS_HEADER_SAME;
}


cmpDetailType AltMatrix2D::Compare()
{
  AltMatrix2D::Verify();

  cmpDetailType c = AltMatrix2D::ComparePartial(
    SDS_HEADER_PLAY_DIFFERENT,
    SDS_HEADER_PLAY_OLD_BETTER,
    SDS_HEADER_PLAY_NEW_BETTER);

  if (c != SDS_HEADER_SAME)
  {
    cval = c;
    return c;
  }
  
  c = AltMatrix2D::ComparePartial(
    SDS_HEADER_RANK_DIFFERENT,
    SDS_HEADER_RANK_OLD_BETTER,
    SDS_HEADER_RANK_NEW_BETTER);

  cval = c;
  return c;
}


cmpDetailType AltMatrix2D::CompareDeclarer()
{
  AltMatrix2D::Verify(false);

  bool Xdiff = (hasXsum[SDS_HEADER_PLAY_DIFFERENT] ||
      hasXsum[SDS_HEADER_RANK_DIFFERENT]);
  bool Ydiff = (hasYsum[SDS_HEADER_PLAY_DIFFERENT] ||
      hasYsum[SDS_HEADER_RANK_DIFFERENT]);
  bool Xwin = (hasXsum[SDS_HEADER_PLAY_OLD_BETTER] ||
        hasXsum[SDS_HEADER_RANK_OLD_BETTER]);
  bool Ywin = (hasYsum[SDS_HEADER_PLAY_NEW_BETTER] ||
        hasYsum[SDS_HEADER_RANK_NEW_BETTER]);

  // Could go deeper with the difference (play or rank).
  if (Xdiff && Ydiff)
    return SDS_HEADER_PLAY_DIFFERENT;
  else if (Ydiff)
    // return (Ywin || ! Xwin ? 
    return (Ywin ?
      SDS_HEADER_PLAY_DIFFERENT : SDS_HEADER_PLAY_OLD_BETTER);
  else if (Xdiff)
    // return (Xwin || ! Ywin ? 
    return (Xwin ?
      SDS_HEADER_PLAY_DIFFERENT : SDS_HEADER_PLAY_NEW_BETTER);
  else if (Ywin)
    return (Xwin ? SDS_HEADER_PLAY_DIFFERENT : SDS_HEADER_PLAY_NEW_BETTER);
  else if (Xwin)
    return (Ywin ? SDS_HEADER_PLAY_DIFFERENT : SDS_HEADER_PLAY_OLD_BETTER);
  else
    return SDS_HEADER_SAME;
}


cmpDetailType AltMatrix2D::CompareHard()
{
  AltMatrix2D::Verify(false);

  bool Xdiff = (hasXsum[SDS_HEADER_PLAY_DIFFERENT] ||
      hasXsum[SDS_HEADER_RANK_DIFFERENT]);
  bool Ydiff = (hasYsum[SDS_HEADER_PLAY_DIFFERENT] ||
      hasYsum[SDS_HEADER_RANK_DIFFERENT]);
  bool Xwin = (hasXsum[SDS_HEADER_PLAY_OLD_BETTER] ||
        hasXsum[SDS_HEADER_RANK_OLD_BETTER]);
  bool Ywin = (hasYsum[SDS_HEADER_PLAY_NEW_BETTER] ||
        hasYsum[SDS_HEADER_RANK_NEW_BETTER]);

  // Could go deeper with the difference (play or rank).
  if (Xdiff && Ydiff)
    return SDS_HEADER_PLAY_DIFFERENT;
  else if (Ydiff)
    return (Xwin ? SDS_HEADER_PLAY_DIFFERENT : SDS_HEADER_PLAY_NEW_BETTER);
  else if (Xdiff)
    return (Ywin ? SDS_HEADER_PLAY_DIFFERENT : SDS_HEADER_PLAY_OLD_BETTER);
  else if (Ywin)
    return (Xwin ? SDS_HEADER_PLAY_DIFFERENT : SDS_HEADER_PLAY_NEW_BETTER);
  else if (Xwin)
    return (Ywin ? SDS_HEADER_PLAY_DIFFERENT : SDS_HEADER_PLAY_OLD_BETTER);
  else
    return SDS_HEADER_SAME;
}


bool AltMatrix2D::CandList(
  const posType sideToLose,
  bool use[],
  cmpDetailType& c) const
{
  c = SDS_HEADER_SAME;

  if (sideToLose == QT_ACE)
  {
    if (cval == SDS_HEADER_PLAY_DIFFERENT ||
        cval == SDS_HEADER_RANK_DIFFERENT)
    {
      bool usedFlag = false;
      for (unsigned a = 0; a < numX; a++)
      {
        use[a] = false;
        if (! activeX[a])
          continue;

        if (hasX[a][SDS_HEADER_SAME] ||
            hasX[a][SDS_HEADER_PLAY_NEW_BETTER] ||
            hasX[a][SDS_HEADER_RANK_NEW_BETTER])
        {
          // We're flipping directions after this.
          if (hasX[a][SDS_HEADER_PLAY_NEW_BETTER])
            c = SDS_HEADER_PLAY_OLD_BETTER;
          else if (hasX[a][SDS_HEADER_RANK_OLD_BETTER])
            c = cmpDetailMatrix[c][SDS_HEADER_RANK_OLD_BETTER];
          continue;
        }
           
        usedFlag = true;
        use[a] = true;
      }
      return usedFlag;
    }
    else
      return false;
  }
  else if (sideToLose == QT_PARD)
  {
    if (cval == SDS_HEADER_PLAY_DIFFERENT ||
        cval == SDS_HEADER_RANK_DIFFERENT)
    {
      bool usedFlag = false;
      for (unsigned a = 0; a < numY; a++)
      {
        use[a] = false;
        if (! activeY[a])
          continue;

        if (hasY[a][SDS_HEADER_SAME] ||
            hasY[a][SDS_HEADER_PLAY_OLD_BETTER] ||
            hasY[a][SDS_HEADER_RANK_OLD_BETTER])
        {
          if (hasY[a][SDS_HEADER_PLAY_OLD_BETTER])
            c = SDS_HEADER_PLAY_OLD_BETTER;
          else if (hasY[a][SDS_HEADER_RANK_OLD_BETTER])
            c = cmpDetailMatrix[c][SDS_HEADER_RANK_OLD_BETTER];
          continue;
        }

        usedFlag = true;
        use[a] = true;
      }
      return usedFlag;
    }
    else
      return false;
  }
  else
  {
    assert(false);
    return false;
  }
}



void AltMatrix2D::PrintVector(
  ostream& out,
  const string text,
  const bool cvec[SDS_MAX_ALT][SDS_HEADER_CMP_SIZE],
  const unsigned len) const
{
  if (text != "")
    out << setw(10) << text;

  for (unsigned i = 0; i < SDS_HEADER_CMP_SIZE; i++)
    out << setw(10) << CMP_DETAIL_NAMES[i];
  out << "\n";

  for (unsigned i = 0; i < len; i++)
  {
    out << setw(2) << i << setw(8) << "";
    for (unsigned j = 0; j < SDS_HEADER_CMP_SIZE; j++)
      out << setw(10) << (cvec[i][j] ? "yes" : "-");
    out << "\n";
  }
  out << endl;
}


void AltMatrix2D::Print(
  ostream& out,
  const string text) const
{
  if (text != "")
    out << "AltMatrix2D " << text << "\n";

  out << setw(4) << left << "D1" << setw(4) << "D2";
  for (unsigned j = 0; j < numY; j++)
    out << setw(10) << j;
  out << "\n  active ";
  for (unsigned j = 0; j < numY; j++)
    out << setw(10) << (activeY[j] ? "yes" : "-");
  out << "\n";
  
  out << setw(9) << "+";
  for (unsigned j = 0; j < numY; j++)
    out << "----------";
  out << "\n";

  for (unsigned i = 0; i < numX; i++)
  {
    out << setw(2) << i << setw(6) << i <<
      (activeX[i] ? "yes" : "-") << "|";

    for (unsigned j = 0; j < numY; j++)
      out << setw(10) << CMP_DETAIL_NAMES[matrix[i][j]];
    out << "\n";
  }
  out << "\n";

  AltMatrix2D::PrintVector(out, "hasX", hasX, numX);
  AltMatrix2D::PrintVector(out, "hasY", hasY, numY);
}

