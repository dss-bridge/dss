/* 
   SDS, a bridge single-suit double-dummy quick-trick solver.

   Copyright (C) 2015 by Soren Hein.

   See LICENSE and README.
*/


#include <iostream>
#include <iomanip>
#include <string>

using namespace std;

#include <assert.h>

#include "cst.h"
#include "AltMatrix2D.h"
#include "portab.h"

extern bool debugComplex;


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
  const unsigned x)
{
  return ! activeX[x];
}


bool AltMatrix2D::IsPurgedY(
  const unsigned y)
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


void AltMatrix2D::Verify()
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

  AltMatrix2D::VerifyVector(hasX, activeX, numX, hasXsum);
  AltMatrix2D::VerifyVector(hasY, activeY, numY, hasYsum);
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
  bool hasSum[])
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
        AltMatrix2D::Print("Verify error I");
        cout.flush();
	assert(false);
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
        AltMatrix2D::Print("Verify error II");
        cout.flush();
	assert(false);
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


cmpDetailType AltMatrix2D::ComparePartial(
  const cmpDetailType diff,
  const cmpDetailType winX,
  const cmpDetailType winY)
{
  if (hasYsum[winX] && hasYsum[winY])
  {
    if (! (hasXsum[winX] && hasXsum[winY]))
    {
      cout << "Error 1: " << 
        static_cast<int>(diff) << " " << 
        static_cast<int>(winX) << " " << 
        static_cast<int>(winY) << "\n";
      for (int c = 0; c < SDS_HEADER_CMP_SIZE; c++)
        cout << setw(2) << c << 
          setw(5) << hasXsum[c] <<
          setw(5) << hasYsum[c];
      AltMatrix2D::Print("Error 1");
      cout.flush();
      assert(false);
    }
  }

  if (hasXsum[winX] && hasXsum[winY])
  {
    if (! (hasYsum[winX] && hasYsum[winY]))
    {
      cout << "Error 2: " << 
        static_cast<int>(diff) << " " << 
        static_cast<int>(winX) << " " << 
        static_cast<int>(winY) << "\n";
      for (int c = 0; c < SDS_HEADER_CMP_SIZE; c++)
        cout << setw(2) << c << 
          setw(5) << hasXsum[c] <<
          setw(5) << hasYsum[c];
      AltMatrix2D::Print("Error 2");
      cout.flush();
      assert(false);
    }
  }

/*
AltMatrix2D::Print();
cout << "hasX diff " << hasXsum[diff] << 
  " winX " << hasXsum[winX] <<
  " winY " << hasXsum[winY] << "\n";
cout << "hasY diff " << hasYsum[diff] <<
  " winX " << hasYsum[winX] <<
  " winY " << hasYsum[winY] << "\n";
*/

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

  // Later expand with length as well.

  cval = c;
  return c;
}


bool AltMatrix2D::CandList(
  const posType sideToLose,
  bool use[]) const
{
  if (sideToLose == QT_ACE)
  {
    if ((cval == SDS_HEADER_PLAY_DIFFERENT &&
        hasXsum[SDS_HEADER_PLAY_NEW_BETTER] &&
        ! hasXsum[SDS_HEADER_PLAY_OLD_BETTER]) ||
        (cval == SDS_HEADER_RANK_DIFFERENT &&
        hasXsum[SDS_HEADER_RANK_NEW_BETTER] &&
        ! hasXsum[SDS_HEADER_RANK_OLD_BETTER]))
    {
      bool usedFlag = false;
      for (unsigned a = 0; a < numX; a++)
      {
        if (hasX[a][SDS_HEADER_RANK_DIFFERENT] ||
            hasX[a][SDS_HEADER_PLAY_DIFFERENT])
        {
          usedFlag = true;
          use[a] = true;
        }
        else
          use[a] = false;
      }
      return usedFlag;
    }
    else
      return false;
  }
  else if (sideToLose == QT_PARD)
  {
    if ((cval == SDS_HEADER_PLAY_DIFFERENT &&
        hasYsum[SDS_HEADER_PLAY_OLD_BETTER] &&
        ! hasYsum[SDS_HEADER_PLAY_NEW_BETTER]) ||
        (cval == SDS_HEADER_RANK_DIFFERENT &&
        hasYsum[SDS_HEADER_RANK_OLD_BETTER] &&
        ! hasYsum[SDS_HEADER_RANK_NEW_BETTER]))
    {
      bool usedFlag = false;
      for (unsigned a = 0; a < numY; a++)
      {
        if (hasY[a][SDS_HEADER_RANK_DIFFERENT] ||
            hasY[a][SDS_HEADER_PLAY_DIFFERENT])
        {
          usedFlag = true;
          use[a] = true;
        }
        else
          use[a] = false;
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


void AltMatrix2D::Print(
  const char text[])
{
  cout << "AltMatrix2D " << text << "\n";
  AltMatrix2D::Print();
}


void AltMatrix2D::Print()
{
  cout << setw(4) << left << "D1" << setw(4) << "D2";
  for (unsigned j = 0; j < numY; j++)
    cout << setw(10) << j;
  cout << "\n  active ";
  for (unsigned j = 0; j < numY; j++)
    cout << setw(10) << (activeY[j] ? "yes" : "-");
  cout << "\n";
  
  cout << setw(9) << "+";
  for (unsigned j = 0; j < numY; j++)
    cout << "----------";
  cout << "\n";


  for (unsigned i = 0; i < numX; i++)
  {
    cout << setw(2) << i << setw(6) << i <<
      (activeX[i] ? "yes" : "-") << "|";

    for (unsigned j = 0; j < numY; j++)
      cout << setw(10) << CMP_DETAIL_NAMES[matrix[i][j]];
    cout << "\n";
  }
  cout << "\n";

  AltMatrix2D::PrintVector("hasX", hasX, numX);
  AltMatrix2D::PrintVector("hasY", hasY, numY);

}


void AltMatrix2D::PrintVector(
  const char text[],
  const bool cvec[SDS_MAX_ALT][SDS_HEADER_CMP_SIZE],
  const unsigned len)
{
  cout << setw(10) << text;
  AltMatrix2D::PrintVector(cvec, len);
}


void AltMatrix2D::PrintVector(
  const bool cvec[SDS_MAX_ALT][SDS_HEADER_CMP_SIZE],
  const unsigned len)
{
  for (unsigned i = 0; i < SDS_HEADER_CMP_SIZE; i++)
    cout << setw(10) << CMP_DETAIL_NAMES[i];
  cout << "\n";

  for (unsigned i = 0; i < len; i++)
  {
    cout << setw(2) << i << setw(8) << "";
    for (unsigned j = 0; j < SDS_HEADER_CMP_SIZE; j++)
      cout << setw(10) << (cvec[i][j] ? "yes" : "-");
    cout << "\n";
  }
  cout << "\n";
}

