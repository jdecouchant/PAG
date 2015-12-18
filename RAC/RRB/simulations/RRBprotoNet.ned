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
package  rrb.simulations;

import inet.nodes.inet.StandardHost;
import inet.nodes.inet.Router;
import inet.nodes.ethernet.EtherSwitch;
//import inet.nodes.inet.MyRouter;
import ned.DatarateChannel;
import ned.DelayChannel;
import inet.networklayer.autorouting.FlatNetworkConfigurator;
import rrb.Node;

network Net
{
    parameters:
        //int n;
        @display("bgb=232,293");
    types:
        channel C extends DatarateChannel
        {
                datarate = 1Gbps;
                //delay = 0.1us;
        }
        channel RC extends DatarateChannel
        {
                datarate = 10Gbps;
                //delay = 0.1us;
        }
    submodules:
        client[3]: Node{
            parameters:
                @display("i=device/pc3");
        }
        //router1: Router;
        //router2: Router; 
        //{
        //            parameters:
        //                @display("p=186,250");
        //        }
        
        //switch[1]: EtherSwitch;
        //router[4]: Router;
        //    switch2: EtherSwitch;
        //    switch3: EtherSwitch;
        //    switch4: EtherSwitch;
    configurator: FlatNetworkConfigurator;
    connections:
        //        for i=0..n/4-1{
        //                //client[i].ethg++ <-->{delay=0.1ms;}<--> switch1.ethg++;
        //                //client[i].ethg++ <--> switch[0].ethg++;
        //                client[i].pppg++ <--> C <--> router[0].pppg++; 
        //        }
        //        //switch1.ethg++ <--> switch2.ethg++;
        //        //router1.pppg++<--> C <--> router2.pppg++;
        //        for i=n/4..n/2-1{
        //                //client[i].ethg++ <--> switch[1].ethg++;
        //                client[i].pppg++ <--> C <--> router[1].pppg++;
        //        }
        //        for i=n/2..3*n/4-1{
        //                //client[i].ethg++ <--> switch[2].ethg++;
        //                client[i].pppg++ <--> C <--> router[2].pppg++;
        //        }
        //        for i=3*n/4..n-1{
        //                //client[i].ethg++ <--> switch[3].ethg++;
        //                client[i].pppg++ <--> C <--> router[3].pppg++;
        //        }
//        for i=0..n-1{
//                                client[i].ethg++ <--> switch[0].ethg++;
//                                //client[i].pppg++ <--> C <--> router[3].pppg++;
//                        }
                
                //        for i=0..2, for j=i+1..3 {
                //            //switch[i].ethg[j] <--> switch[j].ethg[i] if uniform(0,1)<0.15;
                //            router[i].pppg++ <--> RC <--> router[j].pppg++ if uniform(0,1)<0.15;
                //        }
//                router[0].pppg++<--> RC <--> router[1].pppg++;
//                router[1].pppg++<--> RC <--> router[2].pppg++;
//                router[2].pppg++<--> RC <--> router[3].pppg++;
//                router[3].pppg++<--> RC <--> router[0].pppg++;
            client[0].pppg++<--> C<--> client[1].pppg++;
            client[1].pppg++<--> C<--> client[2].pppg++;
            client[2].pppg++<--> C<--> client[0].pppg++;
               
        }
        
        
