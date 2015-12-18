//
// Copyright (C) 2005 Andras Varga
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//


package fprivacycrypto.Streaming;
import inet.applications.udpapp.UDPApp;
//
// Video streaming client.
//
// @see VideoStreamSvr
//
simple Peer like UDPApp {
    parameters:
        string serverAddress;
        double startTime @unit("s") = default(1s);
        double roundDuration @unit("s") = default(0.03s);
        @display("i=block/app");
        @signal[nbRecvdPackets](type="int");
        @signal[interArrivalTime](type="simtime_t");
        @signal[latency](type="simtime_t");
        @statistic[nbPacketsReceived](title="packets received by streaming client"; record=vector,last);
        //@statistic[interArrivalTime](title="server busy state"; record=vector,timeavg);
        @statistic[interArrivalTime](title="inter arrival time at streaming client"; unit=s; record=vector);
        @statistic[latency](title="latency perceived by streaming client"; unit=s;record=stats);

    gates:
        input udpIn @labels(UDPControlInfo/up);
        output udpOut @labels(UDPControlInfo/down);
}

