#!perl

use strict;
use warnings;

open my $fh, '<', "bb" or die "Can't open bb\n";
my @list = <$fh>;
close $fh;

# Skip initial lines down to first combination.
my $j = 0;
while ($j < $#list && $list[$j] !~ /^sl /)
{
  $j++;
}

my @c = ();
for my $i ($j .. $#list)
{
  next if ($list[$i] =~ /^\s*$/);
  next if ($list[$i] =~ /^Count/);

  if ($list[$i] =~ /^sl /)
  {
    if ($#c == 5)
    {
      print "==========\n";
      # chomp $list[$i];
      # $list[$i] =~ s///g;
      # print "$list[$i]\n";
      for my $k (0 .. $#c)
      {
        printf "$c[$k]\n";
      }
    }

    @c = ();
  }
  # else
  # {
    chomp $list[$i];
    $list[$i] =~ s///g;
    push @c, $list[$i];
  # }
}
