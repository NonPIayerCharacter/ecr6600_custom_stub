#!/usr/bin/perl -w


if(@ARGV == 2) {
    $bit_width = 8 ;  #default 
    $source = $ARGV[0];
    $dest = $ARGV[1];
}
else {
    $bit_width = $ARGV[0];
    $source = $ARGV[1];
    $dest = $ARGV[2];
}
    

unless($source and $dest) {
    print "Usage: perl bin2hex [8/16] binfile hexfile\n";
    exit 0;
}

$/=\1;

open(SRC,"$source") || die ("\a Cannot open file $source ");
open(DST,">$dest") || die ("\a Cannot open file $dest \n");

if (defined(SRC) and defined(DST)) {


    #print DST "// -------------------------------------------\n";
    #print DST "//  Verilog readmemh\n";
    #print DST "//     * ASCII file with hexadecimal patterns\n";
    #print DST "// -------------------------------------------\n";
#$n=0;
    binmode(SRC);

    while (!eof(SRC)) {

        #if ($n==8) {$n=0; print DST "\n"; }

        if ($bit_width==32) {

            #if ($n==4) {$n=0; print DST "\n"; }

            $byte0 = ord(getc(SRC));
            $byte1 = ord(getc(SRC));
            $byte2 = ord(getc(SRC));
            $byte3 = ord(getc(SRC));

            print DST sprintf( "%02x", $byte3); 
            #print DST "\n";
            print DST sprintf( "%02x", $byte2); 
            #print DST "\n";
            print DST sprintf( "%02x", $byte1); 
            #print DST "\n";
            print DST sprintf( "%02x", $byte0); 
            print DST "\n";
        }

        elsif ($bit_width==16) {

            #if ($n==2) {$n=0; print DST "\n"; }

            $byte0 = ord(getc(SRC));
            $byte1 = ord(getc(SRC));

            print DST sprintf( "%02x", $byte1); 
            print DST sprintf( "%02x", $byte0); 
            print DST "\n";
        }

        elsif ($bit_width==2) {

            #if ($n==4) {$n=0; print DST "\n"; }

            $byte0 = ord(getc(SRC));
            $byte1 = ord(getc(SRC));
            $byte2 = ord(getc(SRC));
            $byte3 = ord(getc(SRC));

            print DST sprintf( "%08b", $byte3); 
            #print DST "\n";
            print DST sprintf( "%08b", $byte2); 
            #print DST "\n";
            print DST sprintf( "%08b", $byte1); 
            #print DST "\n";
            print DST sprintf( "%08b", $byte0); 
            print DST "\n";
        }
		
        else {
            #if ($n==1) {$n=0; print DST "\n"; }
            $byte0 = ord(getc(SRC));
            print DST sprintf( "%02x", $byte0); 
            print DST "\n";
        }

        #$toprint = ord $_;
        #print DST sprintf( "%02x", $toprint); 
        #$n++;
    }
    #print DST "\n";

    # Rom dummy initial values

    if ($bit_width==32) {
        print DST "00000000\n";
        print DST "00000000\n";
    }

    elsif ($bit_width==16) {
        print DST "0000\n";
        print DST "0000\n";
        print DST "0000\n";
        print DST "0000\n";
    }

    else {
        print DST "00\n";
        print DST "00\n";
        print DST "00\n";
        print DST "00\n";
        print DST "00\n";
        print DST "00\n";
        print DST "00\n";
        print DST "00\n";
    }

    close SRC;
    close DST;
}

print "\n";
print "-------------------------------------------------------------------------\n";
print "  Sucessfully converted bin to ASCII file ( with hexadecimal patterns )\n";

if ($bit_width==32) {print "    -  32-bit Rom file created \n"; }
elsif ($bit_width==16) {print "    -  16-bit Rom file created \n"; }
else { print "    -  8-bit Rom file created \n";}

print "-------------------------------------------------------------------------\n";
print "\n";
