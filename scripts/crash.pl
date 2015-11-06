#!perl

use strict;
use warnings;

open my $fh, '<', "b2" or die "Can't open b\n";
my @list = <$fh>;
close $fh;

my @c;
for my $i (0 .. $#list)
{
  if ($list[$i] =~ /^Compare/)
  {
    next unless @c;

    if ($#c != 9)
    {
      print "========== $#c\n";
      for my $j (0 .. $#c)
      {
        printf "$c[$j]\n";
      }
    }
    else
    {
      my $l1 = $c[5];
      my $l2 = $c[8];

      my @a1 = split /\s+/, $l1;
      my @a2 = split /\s+/, $l2;

      die "$i: l1 '$l1'\n" unless ($#a1 == 7);
      die "$i: l2 '$l2'\n" unless ($#a2 == 7);

      for my $j (0 .. 6)
      {
        die "$i: l1 '$l1' l2 '$l2'\n" unless ($a1[$j] eq $a2[$j]);
      }
      if ($a1[7] ne 'K' || $a2[7] ne 'Q')
      {
        print "---------- $a1[7] $a2[7]\n";
        for my $j (0 .. $#c)
        {
          printf "$c[$j]\n";
        }
      }
    }

    @c = ();
  }
  else
  {
    chomp $list[$i];
    push @c, $list[$i];
  }
}
