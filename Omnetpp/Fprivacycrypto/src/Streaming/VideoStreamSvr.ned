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
// Video stream server. To be used with UDPVideoStreamCli.
//
// The server will wait for incoming "video streaming requests".
// When a request arrives, it draws a random video stream size
// using the videoSize parameter, and starts streaming to the client.
// During streaming, it will send \UDP packets of size packetLen at every
// waitInterval, until videoSize is reached. The parameters packetLen
// and waitInterval can be set to constant values to create CBR traffic,
// or to random values (e.g. waitInterval=uniform(1e-6, 1.01e-6)) to
// accomodate jitter.
//
// The server can serve several clients, and several streams per client.
//
// @see VideoStreamCli
//
simple Source like UDPApp {
    parameters:
        double startTime @unit("s") = default(1s);
        double roundDuration @unit("s") = default(0.03s); // interval between injecting video stream packets
        volatile int packetLenPar @unit("B");
        int fanoutPar;
        int witnessNbrPar;
        int updNbPerRoundPar;
        volatile int videoSize @unit("B");
        @display("i=block/app");
    gates:
        input udpIn @labels(UDPControlInfo/up);
        output udpOut @labels(UDPControlInfo/down);
}

