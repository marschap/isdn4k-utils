package wld;
use strict;
use integer;
use vars qw($VERSION @ISA @EXPORT);
$VERSION=1.1;
require Exporter;
@ISA=qw(Exporter);
@EXPORT=qw(wld);
sub min3 {
    my ($x, $y, $z)=@_;
    $y = $x if ($x < $y);
    $z = $y if ($y < $z);
    $z;
}

sub wld	{	#/* weighted Levenshtein distance */
  # $dmax is the smallest distance already found, only
  # smaller distances are computed, otherwise a number
  # equal or less than the real distance is returned  
  my ($needle, $haystack, $dmax) = @_;
  $dmax = 98 unless (defined($dmax)); 	# default when 3rd arg is missing 
  my($i, $j);
  my $l1 = length($needle);
  my $l2 = length($haystack);
  my @s1 = (0, unpack('A' x $l1, $needle));
  my @s2 = (0, unpack('A' x $l2, $haystack));
  my $ldiff = abs($l1-$l2);
  # the distance can not be less than the length difference
  return $ldiff unless ($ldiff < $dmax);
  my @dw;
  my $imin; 	# minimum value of column $i, also lower limit of result
  my ($P,$Q,$R);
  $P=1;
  $Q=1;
  $R=1;

  $dw[0][0]=0;
  for ($j=1; $j<=$l2; $j++) {
    $dw[0][$j]=$dw[0][$j-1]+$Q;
  }
  for ($i=1; $i<=$l1; $i++) {
    $dw[$i][0]=$dw[$i-1][0]+$R;
  }
  for ($i=1; $i<=$l1; $i++) {
    $imin = $dw[$i][0];
    for($j=1; $j<=$l2; $j++) {
      $dw[$i][$j] = &min3( $dw[$i-1][$j-1] + ( ($s1[$i] eq $s2[$j])?0:$P ),
                           $dw[$i][$j-1]+$Q, $dw[$i-1][$j]+$R );
      $imin = $dw[$i][$j] if ($dw[$i][$j]<$imin);
    }	
    # abort if complete column makes results less than $dmax impossible
    return ($imin) unless ($imin<$dmax);
  }
  return($dw[$l1][$l2]);
}

1;
