#!/usr/bin/perl
# boot.pl - based on rcconf

my $DEFAULT_RCNUM = 20;
my @unselects = ("\^\\\.\$", "\^\\\.\\\.\$", "\^rc\$", "\^rcS\$", "\^README\$",
                  "\^skeleton\$", ".*\\\.dpkg-dist\$", ".*\\\.dpkg-old\$",
                  ".*\\\.sh\$");
my $ETC_DIR = "/etc";
my $DATA_DIR = "/var/lib/rcconf/";
my $NO_EXECUTE = "";
if ( $NO_EXECUTE ne "" ) {
  $DATA_DIR = "/tmp/";
}
my $DATA_FILE = $DATA_DIR."services";


my $rcdf = &read_rcd_default(root_dir=>$ETC_DIR);
my $data = &read_data(file=>$DATA_FILE);
my $default = &select_default(rcdf=>$rcdf, data=>$data);
my $initd = &read_initd_dir(root_dir=>$ETC_DIR);
$initd = &select_unlinked_initd(initd=>$initd, rcdf=>$rcdf,
                                unselects=>\@unselects);

for my $key ( sort(@{$default}) ) {
    $key =~ s/\'/\"/;
    print "active $key\n";
}
for my $key ( sort(@{$initd}) ) {
    $key =~ s/\'/\"/;
    print "disabled $key\n";
}

#if ( $ret == 0 ) {
  #my($res_on, $res_off) = &diff_result(on=>$default, off=>$initd, res=>$res);
  #&exec_update(on=>$res_on, off=>$res_off, data=>$data);
  #&write_data(file=>$DATA_FILE, data=>$data);
#}

exit;


#######################################################################
#######################################################################
#######################################################################

#######################################################################
## MODULE: read_rcd_default
## DESC: Read files in rc?.d(?:=0..6) directory and generate %rcdf.
##         %rcdf->{'package'} -> [0]  service num in rc0.d/S??package
##                               [1]
##                               [2]
##                               [3]
##                               [4]
##                               [5]
##                               [6]
##                               [7]    rcS.d/S??package
##                               [10] service num in rc0.d/K??package
##                               [11]   rc1.d/K??package
##                               [12]   rc2.d/K??package
##                               [13]   rc3.d/K??package
##                               [14]   rc4.d/K??package
##                               [15]   rc5.d/K??package
##                               [16]   rc6.d/K??package
##                               [17]   rcS.d/K??package
## IN:
## OUT:
## OP:
## STATUS:
## END:
sub read_rcd_default {
  my($self) = shift if(defined($_[0]) && (ref($_[0]) ne ''));
  my(%ref) = @_;
##
  my $root_dir = $ref{'root_dir'};
  my %rcdf = ();
#  my($i);
  my $dir;
## rc?.d
  my($start, $end);
  for ( my $i = 0; $i <= 6; $i++ ) {
    $dir = $root_dir."/rc".$i.".d";
    ($start, $end) = &read_rc_dir(dir=>$dir);
    &setup_rcd(rcdf=>\%rcdf, rcfile=>$start, dirnum=>$i, margin=>0);
    &setup_rcd(rcdf=>\%rcdf, rcfile=>$end, dirnum=>$i, margin=>10);
  }
## rcS.d
  $dir = $root_dir."/rcS.d";
  ($start, $end) = &read_rc_dir(dir=>$dir);
  &setup_rcd(rcdf=>\%rcdf, rcfile=>$start, dirnum=>7, margin=>0);
  &setup_rcd(rcdf=>\%rcdf, rcfile=>$end, dirnum=>7, margin=>10);
  return \%rcdf;
} ## read_rcd_default

#######################################################################
## MODULE: read_rc_dir
## DESC: Open directory specified in $dir, and list Start/Stop service
##        @start [0] -> {file} -+- 'num'  service number
##               [1]            +- 'name' file name
## IN:
## OUT:
## OP:
## STATUS:
## END:
sub read_rc_dir{
  my($self) = shift if(defined($_[0]) && (ref($_[0]) ne ''));
  my(%ref) = @_;
##
  my $dir = $ref{'dir'};
##
  opendir(DIR, $dir) || die "No such directory: $!";
##
  my @starts = ();
  my @stops = ();
  my @dirs = readdir(DIR);
  foreach ( @dirs ) {
    if(/^S([0-9][0-9])(.*)$/){
      push(@starts, &new_file(num=>$1, name=>$2));
      next;
    } ## if
    if(/^K([0-9][0-9])(.*)$/){
      push(@stops, &new_file(num=>$1, name=>$2));
      next;
    } ## if
  } ## while()
  closedir(DIR);
  return(\@starts, \@stops);
} ## read_rc_dir

#######################################################################
## MODULE: read_initd_dir
## DESC: Collect files in init.d/ directory.
## IN:
## OUT:
## OP:
## STATUS:
## END:
sub read_initd_dir{
  my($self) = shift if(defined($_[0]) && (ref($_[0]) ne ''));
  my(%ref) = @_;
##
  my $root_dir = $ref{'root_dir'};
##
  my $dir = $root_dir."/init.d";
  opendir(DIR, $dir) || die "No such directory: $!";
  my(@dirs) = readdir(DIR);
  close(DIR);
##
  return \@dirs;
} ## read_initd_dir

#######################################################################
## MODULE: select_unlinked_initd
## DESC: Compare between %rcdf and @initd, and list file in init.d/
##       directory which is not linked to rc?.d.
##       Listed files are not serviced packages.
## IN:
## OUT: \@new_initrd := not serviced packages
## OP:
## STATUS:
## END:
sub select_unlinked_initd{
  my($self) = shift if(defined($_[0]) && (ref($_[0]) ne ''));
  my(%ref) = @_;
##
  my $initd = $ref{'initd'};
  my $rcdf = $ref{'rcdf'};
  my $unselects = $ref{'unselects'};
##
  my @new_initrd = ();
  my $unselect;
##
  foreach my $key (@{$initd}){
    next if ( &check_unselect(file=>$key, unselects=>$unselects) );
    if ( ( ! exists($rcdf->{$key}) ) || 
         ( ( $rcdf->{$key}->[10] != -1 ) &&
           ( $rcdf->{$key}->[11] != -1 ) && 
           ( $rcdf->{$key}->[12] != -1 ) && 
           ( $rcdf->{$key}->[13] != -1 ) && 
           ( $rcdf->{$key}->[14] != -1 ) && 
           ( $rcdf->{$key}->[15] != -1 ) && 
           ( $rcdf->{$key}->[16] != -1 ) ) ) {
      push(@new_initrd, $key);
    }
  }
  return \@new_initrd;
} ## select_unlinked_initd()

#######################################################################
## MODULE: check_unselect
## DESC: Check if 'file' exists in unselects array.
## IN:
## OUT:  results 0 := file is not in the array.
##               1 := file exists in the array.
## OP:
## STATUS:
## END:
sub check_unselect{
  my($self) = shift if(defined($_[0]) && (ref($_[0]) ne ''));
  my(%ref) = @_;
##
  my $file = $ref{'file'};
  my $unselects = $ref{'unselects'};

  return 1 if ( ! -x $ETC_DIR . "/init.d/" . $file );

  foreach my $unselect (@{$unselects}){
    return 1 if($file =~ /$unselect/);
  }
  return 0;
} ## check_unselect()

#######################################################################
## MODULE: new_file
## DESC: Generate new package file
##        'num'  => service number
##        'name' => package name(filename)
## IN:
## OUT:
## OP:
## STATUS:
## END:
sub new_file{
  my($self) = shift if(defined($_[0]) && (ref($_[0]) ne ''));
  my(%ref) = @_;
##
  my $new = {};
  $new->{'num'} = $ref{'num'};
  $new->{'name'} = $ref{'name'};
  return $new;
} ## new_file()

#######################################################################
## MODULE: new_rcd
## DESC:
## IN:
## OUT:
## OP:
## STATUS:
## END:
sub new_rcd{
  my($self) = shift if(defined($_[0]) && (ref($_[0]) ne ''));
  my(%ref) = @_;
##
  my @rcd = ();
  for ( my $i = 0; $i <= 7; $i++ ) {
    $rcd[$i] = -1;       ## start
    $rcd[$i + 10] = -1;  ## end
  }
  return \@rcd;
} ## new_rc()

#######################################################################
## MODULE: setup_rcd
## DESC:
## IN:
## OUT:
## OP:
## STATUS:
## END:
sub setup_rcd{
  my($self) = shift if(defined($_[0]) && (ref($_[0]) ne ''));
  my(%ref) = @_;
##
  my $rcdf = $ref{'rcdf'};
  my $rcfile = $ref{'rcfile'};
  my $dirnum = $ref{'dirnum'};
  my $margin = $ref{'margin'};
##
  my $package; my $num;
  foreach my $file ( @{$rcfile} ) {
    $package = $file->{'name'};
    $num = $file->{'num'};
#    print $package." ".$num." $margin $dirnum\n";
    if(! exists($rcdf->{$package})){
      $rcdf->{$package} = &new_rcd();
#      print "Generate ".$package."\n";
    }
    $rcdf->{$package}->[$dirnum+ $margin] = $num;
  } ## foreach
} ## setup_rcd()

#######################################################################
## MODULE: print_rcd
## DESC:
## IN:
## OUT:
## OP:
## STATUS:
## END:
sub print_rcd{
  my($self) = shift if(defined($_[0]) && (ref($_[0]) ne ''));
  my(%ref) = @_;
##
  my $rcdf = $ref{'rcdf'};
##
  my $i;
  printf "                           Start                     Stop\n";
  printf "%-20s ","Package Name";
  print " 0  1  2  3  4  5  6  S     0  1  2  3  4  5  6  S\n";
  print '-' x 71 ."\n";
  foreach my $package (keys(%{$rcdf})){
    printf "%-20s ", $package;
    for ( $i = 0; $i <= 7; $i++ ) { 
      printf "%2d ", $rcdf->{$package}->[$i];
    }
    print "   ";
    for ( $i = 0; $i <= 7; $i++ ) { 
      printf "%2d ", $rcdf->{$package}->[$i + 10];
    }
    print "\n";
  } ## foreach $package
} ## print_rcd()

#######################################################################
## MODULE: select_default
## DESC:
## IN:
## OUT:
## OP:
## STATUS:
## END:
sub select_default{
  my($self) = shift if(defined($_[0]) && (ref($_[0]) ne ''));
  my(%ref) = @_;
##
  my $rcdf = $ref{'rcdf'};
  my $data = $ref{'data'};
##
  my $link ;
  my @select = ();
  my $start_num; my $stop_num;
  foreach my $package ( keys(%{$rcdf}) ) {
#    print STDERR $package,"\n";
    $link = $rcdf->{$package};
    $start_num = $link->[2];
    $stop_num = $link->[10];
    if ( ( $start_num != -1 ) && ( $stop_num != -1 ) &&
         ( $link->[3] == $start_num ) &&
         ( $link->[4] == $start_num ) &&
         ( $link->[5] == $start_num ) &&
         ( $link->[11] == $stop_num ) &&
         ( $link->[16] == $stop_num ) ) {
      push(@select, $package);
      if ( ( $start_num != $DEFAULT_RCNUM ) ||
           ( $stop_num != $DEFAULT_RCNUM ) ) {
        $data->{$package}->{'start'} = $start_num;
        $data->{$package}->{'stop'} = $stop_num;
      }
#      print STDERR $package,"\n";
    }
  } ## foreach
  return \@select;
} ## select_default()

#######################################################################
## MODULE: output_dialog
## DESC:
## IN:
## OUT:
## OP:
## STATUS:
## END:
sub output_dialog{
  my($self) = shift if(defined($_[0]) && (ref($_[0]) ne ''));
  my(%ref) = @_;
##
  my $on = $ref{'on'};
  my $off = $ref{'off'};
  my $info = $ref{'info'};
##
  my $list = " ".&generate_list(package=>$on, sw=>$DIALOG_SW_ON,
                                 info=>$info);
  $list .= " ".&generate_list(package=>$off, sw=>$DIALOG_SW_OFF,
                              info=>$info);
  my $exec = $DIALOG_BIN." ".$DIALOG_OPT.$list;
  ##
#  print STDERR $exec."\n";
#  exit;
  my $ret  = system($exec." 2>$TMP_FILE");
  ## 'dialog' return 0 if exit by pressing 'OK'
  my @res = ();
  if ( $ret == 0 ) {
    open(RES, $TMP_FILE) || die "Exec error:$!";
    while(<RES>){
      chomp;
      push(@res, $_);
    }
    close(RES);
  }
  unlink $TMP_FILE;
  return($ret, \@res);
} ## output_dialog()

#######################################################################
## MODULE: generate_list
## DESC:
## IN:
## OUT:
## OP:
## STATUS:
## END:
sub generate_list {
  my($self) = shift if(defined($_[0]) && (ref($_[0]) ne ''));
  my(%ref) = @_;
##
  my $package = $ref{'package'};
  my $sw = $ref{'sw'};
  my $info = $ref{'info'};
##
  my $list = "";
  my $str;
  for my $key ( sort(@{$package}) ) {
    $str = "";
    if ( exists($info->{$key}) ) {
      $str = ( $GUIDE_LENGTH > 0 ) ? substr($info->{$key}, 0, $GUIDE_LENGTH)
        : $info->{$key};
    }
    $key =~ s/\'/\"/;
    $str =~ s/\'/\"/;
    $list .= "'".$key."' "." '".$str."' '".$sw."' ";
  }
  return $list;
} ## generate_list()

#######################################################################
## MODULE: read_guidefile
## DESC:
## IN:
## OUT:
## OP:
## STATUS:
## END:
sub read_guidefile{
  my($self) = shift if(defined($_[0]) && (ref($_[0]) ne ''));
  my(%ref) = @_;
##
  my $file = $ref{'file'};
##
  my $guide = {};
  my $key; my $guideline;
  open(IN, $file) || return $guide;
  while(<IN>){
    chomp;
    /^(\S+)\s+(.*)/;
    $key = $1;
    $guideline = $2;
    $guide->{$key} = $guideline;
  } ## while(<IN>)
  return $guide;
} # read_guidefile()

#######################################################################
## MODULE: diff_result
## DESC:
## IN:
## OUT:
## OP:
## STATUS:
## END:
sub diff_result{
  my($self) = shift if(defined($_[0]) && (ref($_[0]) ne ''));
  my(%ref) = @_;
##
  my $on = $ref{'on'};
  my $off = $ref{'off'};
  my $res = $ref{'res'};
##
  my $hash = {};
  my @res_on = ();
  my @res_off = ();
  my $key;
  foreach $key (@{$res}){
    $hash->{$key} = "OK";
  }
#  my($hash) = &array2hash(array=>$res, value=>'OK');
  foreach $key ( @{$on} ) {
    if ( ! exists($hash->{$key}) ) {
#      print "OFF ",$key,"\n";
      push(@res_off, $key);
    }
  }
  foreach $key ( @{$off} ) {
    if ( exists($hash->{$key}) ) {
#      print "ON  ",$key,"\n";
      push(@res_on, $key);
    }
  }
  return(\@res_on, \@res_off);
} ## diff_result

#######################################################################
## MODULE: exec_update
## DESC:
## IN:
## OUT:
## OP:
## STATUS:
## END:
sub exec_update{
  my($self) = shift if(defined($_[0]) && (ref($_[0]) ne ''));
  my(%ref) = @_;
##
  my $on = $ref{'on'};
  my $off = $ref{'off'};
  my $data = $ref{'data'};
##
  my $key;
  my $command;
  my $pn;

  foreach $key ( @{$on} ) {
    $command = $UPDATE_RCD_PATH." -f ".$key." remove $DEBUG_STRING ; ";
    $command .= $UPDATE_RCD_PATH." ".$key." defaults";
    if ( exists($data->{$key}) ) {
      $command .= " ".$data->{$key}->{'start'}." ".$data->{$key}->{'stop'};
    }
    $command .= " $DEBUG_STRING";
    if ( $NO_EXECUTE eq '' ) {
      if ( $DEBUG == 1 ) {
        print STDERR $command."\n";
      }
      print SAVE "$key on\n" if($OUTPUT_FILE ne '');
      system($command);
      if ( $RUN_SCRIPTS == 1 ) {
        system($INVOKERC_BIN." ".$key." start");
      }
    }else{
      print STDERR $command."\n";
    }
  }
  foreach $key ( @{$off} ) {
    if ( exists($rcdf->{"mysql"}) ) {
      $pn = sprintf("%02d",
                    ($rcdf->{"mysql"}->[2] >= 0 ? $rcdf->{"mysql"}->[2] :
                     $rcdf->{"mysql"}->[13]));
    } else {
      $pn = "00";
    }
    $command = $UPDATE_RCD_PATH." -f ".$key." remove $DEBUG_STRING ; ";
    $command .= $UPDATE_RCD_PATH." ".$key." stop " .
      $pn . " 0 1 2 3 4 5 6 . $DEBUG_STRING ";
    if ( $NO_EXECUTE eq '' ) {
      if ( $DEBUG == 1 ) {
        print STDERR $command."\n";
      }
      print SAVE "$key off\n" if($OUTPUT_FILE ne '');

      system($command);

      if ( $RUN_SCRIPTS == 1 ) {
        system($INVOKERC_BIN." ".$key." stop");
      }
    }else{
      print STDERR $command."\n";
    }
  }
} ## exec_update()

#######################################################################
## MODULE: read_data
## DESC:
## IN:
## OUT:
## OP:
## STATUS:
## END:
sub read_data{
  my($self) = shift if(defined($_[0]) && (ref($_[0]) ne ''));
  my(%ref) = @_;
##
  my $data = {};
##
  open(IN, $ref{'file'}) || return $data;
##
  while(<IN>){
    next if ( /^\#/ );
    if ( /([0-9][0-9])\s+([0-9][0-9])\s+(\S+)/ ) {
      $data->{$3}->{'start'} = $1;
      $data->{$3}->{'stop'} = $2;
    } elsif ( /([0-9][0-9])\s+(\S+)/ ) {
      $data->{$2}->{'start'} = $1;
      $data->{$2}->{'stop'} = $1;
    }
  } ## while(<IN>)
  close(IN);
  return $data;
} ## read_data()

#######################################################################
## MODULE: read_config_file
## DESC:
## IN:
## OUT:
## OP:
## STATUS:
## END:
sub read_config_file{
  my($self) = shift if(defined($_[0]) && (ref($_[0]) ne ''));
  my(%ref) = @_;
##
  my $tag; my $value;
  my $data = {};
  open(IN, $ref{'file'}) || return(1, 0);
  while(<IN>){
#    s/\r?\n?$//;
    chomp;
    next if(/^$/ || /^\#/);
    ($tag, $value) = split(/\s+/);
    $value = ($value =~ /on/i) ? 1 : 0;
    $data->{$tag} = $value;
  }
  close(IN);
  return(0, $data);
} ## read_config_file()

#######################################################################
## MODULE: set_config
## DESC:
## IN:
## OUT:
## OP:
## STATUS:
## END:
sub set_config{
  my($self) = shift if(defined($_[0]) && (ref($_[0]) ne ''));
  my(%ref) = @_;
##
  my $on = $ref{'on'};
  my $off = $ref{'off'};
  my $set = $ref{'set'};
##
  my $on_hash = &array2hash(array=>$on, value=>'1');
  my $off_hash = &array2hash(array=>$off, value=>'0');
##
  my $key;
  foreach $key ( keys(%{$set}) ) {
    if ( ( $set->{$key} == 1 ) && ( exists($off_hash->{$key}) ) ) {
      $on_hash->{$key} = '1';
    } elsif ( ( $set->{$key} == 0 ) && ( exists($on_hash->{$key}) ) ) {
      $on_hash->{$key} = '0';
    }
  }
  my @res = ();
  foreach $key ( keys(%{$on_hash}) ) {
    push(@res, $key) if($on_hash->{$key} == 1);
  }
  return(0, \@res);
} ## set_config()

#######################################################################
## MODULE: array2hash
## DESC:
## IN:
## OUT:
## OP:
## STATUS:
## END:
sub array2hash{
  my($self) = shift if(defined($_[0]) && (ref($_[0]) ne ''));
  my(%ref) = @_;
##
  my $array = $ref{'array'};
  my $value = $ref{'value'};
##
  my $hash = {};
  foreach my $key (@{$array}){
    $hash->{$key} = $value;
  }
  return $hash;
} ## array2hash()

#######################################################################
## MODULE: write_data
## DESC:
## IN:
## OUT:
## OP:
## STATUS:
## END:
sub write_data{
  my($self) = shift if(defined($_[0]) && (ref($_[0]) ne ''));
  my(%ref) = @_;
##
  my $file = $ref{'file'};
  my $data = $ref{'data'};
  open(OUT, "> ".$file) || die "Cannot write file $file: $!";
  foreach my $key (keys(%{$data})){
    print OUT $data->{$key}->{'start'}." ".
      $data->{$key}->{'stop'}." ".
        $key."\n";
  }
  close(OUT);
} ## write_data()

#######################################################################
## MODULE: update_itme_max_length
## DESC:
## IN:
## OUT:
## OP:
## STATUS:
## END:
sub update_item_max_length {
  my($self) = shift if(defined($_[0]) && (ref($_[0]) ne ''));
  my(%ref) = @_;
##
  my $package = $ref{'data'};
##
  for my $key ( @{$package} ) {
    if ( length($key) > $ITEM_MAX_LENGTH ) {
      $ITEM_MAX_LENGTH = length($key);
    }
  }
} ## update_item_max_length()

#######################################################################
## MODULE: make_lock
## DESC:
## IN:
## OUT:
## OP:
## STATUS:
## END:
sub make_lock{
  my($self) = shift if(defined($_[0]) && (ref($_[0]) ne ''));
  my(%ref) = @_;
##
  if ( -f $LOCK_FILE ) {
    die "Another rcconf is running, or still remain lock file($LOCK_FILE).";
  }
  open(LOCK, "> ".$LOCK_FILE) || die "Can't create lock($LOCK_FILE).";
  close(LOCK);
} ## make_lock()

#######################################################################
## MODULE: remove_lock
## DESC:
## IN:
## OUT:
## OP:
## STATUS:
## END:
sub remove_lock{
  my($self) = shift if(defined($_[0]) && (ref($_[0]) ne ''));
  my(%ref) = @_;
##
  unlink($LOCK_FILE);
} ## remove_lock
