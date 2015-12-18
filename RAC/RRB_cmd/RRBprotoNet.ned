// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 
//package  rrb.simulations;

import inet.nodes.inet.StandardHost;
import inet.nodes.inet.Router;
import inet.nodes.ethernet.EtherSwitch;
//import inet.nodes.inet.MyRouter;
import ned.DatarateChannel;
import ned.DelayChannel;
import inet.networklayer.autorouting.FlatNetworkConfigurator;
import src.Node;

network Net
{
parameters:
	 int n;
    @display("bgb=232,293");
    types:
        channel C extends DatarateChannel
        {
            datarate = 1Gbps;
            //delay = 0.1us;
        }
        
    submodules:
        client[n]: Node{
            parameters:
                @display("i=device/pc3");
        }
	router: Router {
            parameters:
                @display("p=186,250");
        }
        
//    switch: EtherSwitch {
//          parameters:
//             @display("p=186,250");
//    };
    configurator: FlatNetworkConfigurator;
    connections:
        for i=0..n-1{
            //client[i].ethg++ <--> switch.ethg++;
            client[i].pppg++ <--> C <--> router.pppg++; 
	    }
}


