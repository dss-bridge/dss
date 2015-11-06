#!perl

use strict;
use warnings;

use List::Permutor;

my $permutor = List::Permutor->new(0, 1, 2, 3, 4, 5, 6, 7);
while (my @pm = $permutor->next())
{
  my $max1 = ($pm[0] > $pm[1] ? $pm[0] : $pm[1]);
  my $max2 = ($pm[2] > $pm[3] ? $pm[2] : $pm[3]);
  my $max3 = ($pm[4] > $pm[5] ? $pm[4] : $pm[5]);
  my $max4 = ($pm[6] > $pm[7] ? $pm[6] : $pm[7]);

  my $min1 = ($max1 < $max2 ? $max1 : $max2);
  my $min2 = ($max3 < $max4 ? $max3 : $max4);

  my $mmax = ($min1 > $min2 ? $min1 : $min2);

  my $amax1 = ($max1 > $max3 ? $max1 : $max3);
  my $amax2 = ($max1 > $max4 ? $max1 : $max4);
  my $amax3 = ($max2 > $max3 ? $max2 : $max3);
  my $amax4 = ($max2 > $max4 ? $max2 : $max4);

  my $amin1 = ($amax1 < $amax2 ? $amax1 : $amax2);
  my $amin2 = ($amax3 < $amax4 ? $amax3 : $amax4);
  my $amin = ($amin1 < $amin2 ? $amin1 : $amin2);

  if ($mmax != $amin)
  {
    print "@pm: $mmax $amin\n";
  }
}
