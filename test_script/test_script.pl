#!/usr/bin/perl
#use strict

my $pre_cmd = "./sls_cli aaaa::c30c:0:0:";
my $post_cmd = " 3000 sls_led_on";
my $start_time = time();
my $simu_time = 120;
my $time_out = 10;
my $node_id_max = 10;
my $node_id_min = 2;
my $node_id = 1;
my $current_time;
my $cmd;
my $node_failed_count = 0;
my $node_total_count = 0;

$current_time = time();
#print "time now is ", $current_time, "\n";
LABELME: while (($current_time - $start_time) < $simu_time)
{
    eval {
        local $SIG{ALRM} = sub { die "alarm\n" }; # NB: \n required
        
        if($node_id >= $node_id_max)
        {
            $node_id = $node_id_min; 
        }
        else{
            $node_id += 1;
        }
        $node_total_count += 1;
        
        print "sending message to node ID:", $node_id, "\n";
        $cmd = $pre_cmd . $node_id . $post_cmd;
        print "command is ", $cmd, "\n";
        alarm($time_out);
        system($cmd);
        sleep(2);
        alarm(0);
     };

    if ($@) {
        die unless $@ eq "alarm\n";   # propagate unexpected errors
        print "can't send message to node", $node_id, "\n";
        $node_failed_count += 1;
        next LABELME;
    }
    else {
        # didn't
    }
    
    $current_time = time();
    #print "time now is ", $current_time, "\n";
}
    print "total node number is sent = ", $node_total_count, "\n";
    print "node number  can't not sent to =", $node_failed_count, "\n";
    






