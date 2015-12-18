package fprivacycrypto.simulations;

import inet.networklayer.autorouting.FlatNetworkConfigurator;
import inet.nodes.inet.Router;
import ned.DatarateChannel;
import fprivacycrypto.Streaming.ServerNode;
import fprivacycrypto.Streaming.ClientNode;

network CBR_advanced_Network_10000
{
    parameters:
        @display("bgb=672,335");
        @statistic[latency](title="latency perceived by all nodes"; unit=s; record=histogram);
        volatile double d @unit(ms) = uniform(12ms,16ms);
        
    types:
        channel fiber extends DatarateChannel
        {
                delay = 0.1us;
                datarate = 10Gbps;
        }
        channel ethernet extends DatarateChannel
        {
                delay = 0.1us;
                datarate = 100Mbps;
        }
        channel adsl_up extends DatarateChannel
        {
                delay = 10ms;
                datarate = 1kbps;
        }
        channel adsl_down extends DatarateChannel
        {
                delay = 10ms;
                //            datarate = 1Mbps;
                datarate = 100Gbps;
        }
        channel adsl_down_slow extends DatarateChannel
        {
                delay = d;
                datarate = 100Gbps;
        }
    submodules:
        configurator: FlatNetworkConfigurator {
                @display("p=538,102");
        }
        rte[10]: Router {
            parameters:
                @display("p=338,208");
        }  
        source: ServerNode {
            parameters:
                @display("i=device/laptop_s;p=538,208;is=l");
        }
        clients[10000]: ClientNode {
            parameters:
                @display("i=device/laptop_s;is=l");
        }
    connections:
        for i=0..999 {
                clients[i].pppg++ <--> ethernet <--> rte[0].pppg++;
                rte[0].pppg++ <--> ethernet <--> source.pppg++;
        }
        for i=1000..1999 {
                clients[i].pppg++ <--> ethernet <--> rte[1].pppg++;
                rte[1].pppg++ <--> ethernet <--> source.pppg++;
        }
        for i=2000..2999 {
                clients[i].pppg++ <--> ethernet <--> rte[2].pppg++;
                rte[2].pppg++ <--> ethernet <--> source.pppg++;
        }
        for i=3000..3999 {
                clients[i].pppg++ <--> ethernet <--> rte[3].pppg++;
                rte[3].pppg++ <--> ethernet <--> source.pppg++;
        }
        for i=4000..4999 {
                clients[i].pppg++ <--> ethernet <--> rte[4].pppg++;
                rte[4].pppg++ <--> ethernet <--> source.pppg++;
        }
        for i=5000..5999 {
                clients[i].pppg++ <--> ethernet <--> rte[5].pppg++;
                rte[5].pppg++ <--> ethernet <--> source.pppg++;
        }
        for i=6000..6999 {
                clients[i].pppg++ <--> ethernet <--> rte[6].pppg++;
                rte[6].pppg++ <--> ethernet <--> source.pppg++;
        }
        for i=7000..7999 {
                clients[i].pppg++ <--> ethernet <--> rte[7].pppg++;
                rte[7].pppg++ <--> ethernet <--> source.pppg++;
        }
        for i=8000..8999 {
                clients[i].pppg++ <--> ethernet <--> rte[8].pppg++;
                rte[8].pppg++ <--> ethernet <--> source.pppg++;
        }
        for i=9000..9999 {
                clients[i].pppg++ <--> ethernet <--> rte[9].pppg++;
                rte[9].pppg++ <--> ethernet <--> source.pppg++;
        }
        for i=1..9 {
			rte[0].pppg++ <--> ethernet <--> rte[i].pppg++;
        }
		for i=2..9 {
			rte[1].pppg++ <--> ethernet <--> rte[i].pppg++;
		}
		for i=3..9 {
			rte[2].pppg++ <--> ethernet <--> rte[i].pppg++;
		}
	    for i=4..9 {
			rte[3].pppg++ <--> ethernet <--> rte[i].pppg++;
		}
	    for i=5..9 {
			rte[4].pppg++ <--> ethernet <--> rte[i].pppg++;
		}		
	    for i=6..9 {
			rte[5].pppg++ <--> ethernet <--> rte[i].pppg++;
		}		
	    for i=7..9 {
			rte[6].pppg++ <--> ethernet <--> rte[i].pppg++;
		}		
	    for i=8..9 {
			rte[7].pppg++ <--> ethernet <--> rte[i].pppg++;
		}	
		rte[8].pppg++ <--> ethernet <--> rte[9].pppg++;
}


