//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2008 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

package  rrb.simulations;
import  rrb.Node;
import inet.networklayer.autorouting.FlatNetworkConfigurator;
import ned.DatarateChannel;


network RandomGraph
{
    parameters:
        int n @prompt("Number of nodes") = default(20);
       
    submodules:
        //view : View;
        client[n]: Node{
             gates:
              pppg[n];
        }; 
    configurator: FlatNetworkConfigurator;
    connections allowunconnected:
        for i=0..n-2, for j=i+1..n-1 {
            client[i].pppg[j] <--> {datarate = 1Gbps; } <--> client[j].pppg[i] if uniform(0,1)<0.15;
        }
}


