//
// Copyright (C) 2004 Andras Varga
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//

package  rrb.simulations;
import inet.networklayer.autorouting.FlatNetworkConfigurator;
import inet.nodes.inet.Router;
import inet.nodes.inet.StandardHost;
import ned.DatarateChannel;
import rrb.Node;

network NClients
{
    parameters:
        int numRouters;
        int hostsPerRouter;
    types:
        channel ethernetline2 extends DatarateChannel
        {
            delay = 0.1ms;
            datarate = 1Gbps;
        }
        channel gigabitline2 extends DatarateChannel
        {
            delay = 0.1ms;
            datarate = 50Gbps;
        }
    submodules:
        client[numRouters*hostsPerRouter]: Node {
            parameters:
                @display("i=device/laptop_vs");
        }
        r[numRouters]: Router;
        configurator: FlatNetworkConfigurator;
    connections:
        for i=0..numRouters-1, for j=0..hostsPerRouter-1 {
            client[i*hostsPerRouter+j].pppg++ <--> ethernetline2 <--> r[i].pppg++;
        }
//        for i=0..numRouters-2 {
//            r[i].pppg++ <--> gigabitline2 <--> r[i+1].pppg++;
//        }
        for i=0..numRouters-1, for j=0..numRouters-1 {
            r[i].pppg++ <--> gigabitline2 <--> r[j].pppg++ if i!=j;
        }
}

