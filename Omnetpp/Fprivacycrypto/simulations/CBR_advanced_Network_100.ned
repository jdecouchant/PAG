package fprivacycrypto.simulations;

import inet.networklayer.autorouting.FlatNetworkConfigurator;
import inet.nodes.inet.Router;
import ned.DatarateChannel;
import fprivacycrypto.Streaming.ServerNode;
import fprivacycrypto.Streaming.ClientNode;

network CBR_advanced_Network_100
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
        rte: Router {
            parameters:
                @display("p=338,208");
        }  
        source: ServerNode {
            parameters:
                @display("i=device/laptop_s;p=538,208;is=l");
        }
        clients[100]: ClientNode {
            parameters:
                @display("i=device/laptop_s;is=l");
        }
    connections:
        for i=0..99 {
                clients[i].pppg++ <--> ethernet <--> rte.pppg++;
        }
        rte.pppg++ <--> ethernet <--> source.pppg++;
}


