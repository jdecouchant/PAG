//
// Copyright (C) 2004 Andras Varga
// Copyright (C) 2000 Institut fuer Telematik, Universitaet Karlsruhe
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//

package fprivacycrypto.Streaming;

import inet.applications.pingapp.PingApp;
import inet.applications.sctpapp.SCTPApp;
import inet.applications.tcpapp.TCPApp;
import inet.applications.udpapp.UDPApp;
import inet.base.NotificationBoard;
import inet.linklayer.ppp.PPPInterface;
import inet.networklayer.common.InterfaceTable;
import inet.networklayer.ipv4.RoutingTable;
import inet.transport.sctp.SCTP;
import inet.transport.tcp.ITCP;
import inet.transport.udp.UDP;
import inet.util.NAMTraceWriter;
import inet.util.TCPDump;
import inet.nodes.inet.NetworkLayer;
//
// \IP host with SCTP, TCP, UDP layers and applications.
//
module ServerNode {
    parameters:
        @node();
        @labels(node,ethernet-node);
        @display("i=device/pc2");
    gates:
        inout pppg[] @labels(PPPFrame-conn);
    submodules:
        notificationBoard: NotificationBoard {
            parameters:
                @display("p=57,54");
        }
        interfaceTable: InterfaceTable {
            parameters:
                @display("p=57,134");
        }
        routingTable: RoutingTable {
            parameters:
                @display("p=57,214");
        }
        source: Source {
            parameters:
                @display("p=186,54");
        }
        udp: UDP {
            parameters:
                @display("p=186,144");
        }
        networkLayer: NetworkLayer {
            parameters:
                proxyARP = false;
                @display("p=186,221;q=queue");
            gates:
                ifIn[sizeof(pppg)];
                ifOut[sizeof(pppg)];
        }
        ppp[sizeof(pppg)]: PPPInterface {
            parameters:
                @display("p=186,390,row,90;q=txQueue");
        }
        tcpdump: TCPDump {
            parameters:
                @display("p=186,302;i=abstract/cache_s");
            gates:
                ifIn[sizeof(pppg)];
                in2[sizeof(pppg)];
                ifOut[sizeof(pppg)];
                out2[sizeof(pppg)];
        }
    connections allowunconnected:
        source.udpOut --> udp.appIn++;
        source.udpIn <-- udp.appOut++;

        udp.ipOut --> networkLayer.udpIn;
        udp.ipIn <-- networkLayer.udpOut;

        // connections to network outside
        for i=0..sizeof(pppg)-1 {
            pppg[i] <--> ppp[i].phys;
            ppp[i].netwOut --> tcpdump.ifIn[i];
            tcpdump.out2[i] --> networkLayer.ifIn[i];
            ppp[i].netwIn <-- tcpdump.ifOut[i];
            tcpdump.in2[i] <-- networkLayer.ifOut[i];
        }

}
