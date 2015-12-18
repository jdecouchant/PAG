package rrb.simulations;
import inet.nodes.inet.Router;
import inet.nodes.inet.StandardHost;
import rrb.Node;
channel host2edge extends ned.DatarateChannel
{
	parameters:
		delay = 1us;
		datarate = 1Mbps;
}
channel edge2host extends ned.DatarateChannel
{
	parameters:
		delay = 1us;
		datarate = 1Mbps;
}
channel edge2gateway extends ned.DatarateChannel
{
	parameters:
		delay = 1us;
		datarate = 155Mbps;
}
channel gateway2core extends ned.DatarateChannel
{
	parameters:
		delay = 1us;
		datarate = 622Mbps;
}
channel core2core extends ned.DatarateChannel
{
	parameters:
		delay = 1us;
		datarate = 1000Mbps;
}
channel stub2stub extends ned.DatarateChannel
{
	parameters:
		delay = 1us;
		datarate = 1000Mbps;
}
channel stub2transit extends ned.DatarateChannel
{
	parameters:
		delay = 1us;
		datarate = 5000Mbps;
}
channel transit2transit extends ned.DatarateChannel
{
	parameters:
		delay = 1us;
		datarate = 10000Mbps;
}

module Internet
{
	parameters:
		@Internet();
	submodules:
		sas0: SAS0 {
			parameters:
				@AS();
				@display("i=misc/cloud_s");
		}
		tas1: TAS1 {
			parameters:
				@AS();
				@display("i=misc/globe_s");
		}
		sas2: SAS2 {
			parameters:
				@AS();
				@display("i=misc/cloud_s");
		}
		tas3: TAS3 {
			parameters:
				@AS();
				@display("i=misc/globe_s");
		}
		sas4: SAS4 {
			parameters:
				@AS();
				@display("i=misc/cloud_s");
		}
		tas5: TAS5 {
			parameters:
				@AS();
				@display("i=misc/globe_s");
		}
		sas6: SAS6 {
			parameters:
				@AS();
				@display("i=misc/cloud_s");
		}
		sas7: SAS7 {
			parameters:
				@AS();
				@display("i=misc/cloud_s");
		}
		sas8: SAS8 {
			parameters:
				@AS();
				@display("i=misc/cloud_s");
		}
		sas9: SAS9 {
			parameters:
				@AS();
				@display("i=misc/cloud_s");
		}

	connections:
		sas0.pppg++ <--> stub2transit <--> tas1.pppg++;
		sas0.pppg++ <--> stub2stub <--> sas2.pppg++;
		sas0.pppg++ <--> stub2transit <--> tas3.pppg++;
		sas0.pppg++ <--> stub2stub <--> sas9.pppg++;
		sas0.pppg++ <--> stub2stub <--> sas8.pppg++;
		tas1.pppg++ <--> stub2transit <--> sas2.pppg++;
		tas1.pppg++ <--> stub2transit <--> sas4.pppg++;
		tas1.pppg++ <--> transit2transit <--> tas5.pppg++;
		tas1.pppg++ <--> transit2transit <--> tas3.pppg++;
		tas1.pppg++ <--> stub2transit <--> sas7.pppg++;
		tas1.pppg++ <--> stub2transit <--> sas8.pppg++;
		sas2.pppg++ <--> stub2transit <--> tas3.pppg++;
		sas2.pppg++ <--> stub2transit <--> tas5.pppg++;
		tas3.pppg++ <--> stub2transit <--> sas4.pppg++;
		tas3.pppg++ <--> transit2transit <--> tas5.pppg++;
		tas3.pppg++ <--> stub2transit <--> sas9.pppg++;
		sas4.pppg++ <--> stub2transit <--> tas5.pppg++;
		tas5.pppg++ <--> stub2transit <--> sas6.pppg++;
		tas5.pppg++ <--> stub2transit <--> sas7.pppg++;

}

network Inet extends Internet
{
	parameters:
}

module SAS0
{
	gates:
		inout pppg[];
	submodules:
		core0: Router {
			parameters:
				@RL();
				@CoreRouter();
				@display("i=abstract/switch");
		}
		gw1: Router {
			parameters:
				@RL();
				@display("i=abstract/router2");
		}
		edge2: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		edge3: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		edge4: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		edge5: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		edge6: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		host7: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host8: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host9: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host10: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host11: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host12: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host13: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host14: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host15: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host16: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host17: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host18: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host19: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host20: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host21: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host22: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}

	connections:
		pppg++ <--> core0.pppg++;
		pppg++ <--> core0.pppg++;
		pppg++ <--> core0.pppg++;
		pppg++ <--> core0.pppg++;
		pppg++ <--> core0.pppg++;

		core0.pppg++ <--> gateway2core <--> gw1.pppg++;
		gw1.pppg++ <--> edge2gateway <--> edge2.pppg++;
		gw1.pppg++ <--> edge2gateway <--> edge3.pppg++;
		gw1.pppg++ <--> edge2gateway <--> edge4.pppg++;
		gw1.pppg++ <--> edge2gateway <--> edge5.pppg++;
		gw1.pppg++ <--> edge2gateway <--> edge6.pppg++;
		host7.pppg$o++ --> host2edge --> edge2.pppg$i++;
		edge2.pppg$o++ --> edge2host --> host7.pppg$i++;
		host8.pppg$o++ --> host2edge --> edge2.pppg$i++;
		edge2.pppg$o++ --> edge2host --> host8.pppg$i++;
		host9.pppg$o++ --> host2edge --> edge3.pppg$i++;
		edge3.pppg$o++ --> edge2host --> host9.pppg$i++;
		host10.pppg$o++ --> host2edge --> edge3.pppg$i++;
		edge3.pppg$o++ --> edge2host --> host10.pppg$i++;
		host11.pppg$o++ --> host2edge --> edge3.pppg$i++;
		edge3.pppg$o++ --> edge2host --> host11.pppg$i++;
		host12.pppg$o++ --> host2edge --> edge3.pppg$i++;
		edge3.pppg$o++ --> edge2host --> host12.pppg$i++;
		host13.pppg$o++ --> host2edge --> edge3.pppg$i++;
		edge3.pppg$o++ --> edge2host --> host13.pppg$i++;
		host14.pppg$o++ --> host2edge --> edge4.pppg$i++;
		edge4.pppg$o++ --> edge2host --> host14.pppg$i++;
		host15.pppg$o++ --> host2edge --> edge4.pppg$i++;
		edge4.pppg$o++ --> edge2host --> host15.pppg$i++;
		host16.pppg$o++ --> host2edge --> edge5.pppg$i++;
		edge5.pppg$o++ --> edge2host --> host16.pppg$i++;
		host17.pppg$o++ --> host2edge --> edge5.pppg$i++;
		edge5.pppg$o++ --> edge2host --> host17.pppg$i++;
		host18.pppg$o++ --> host2edge --> edge5.pppg$i++;
		edge5.pppg$o++ --> edge2host --> host18.pppg$i++;
		host19.pppg$o++ --> host2edge --> edge6.pppg$i++;
		edge6.pppg$o++ --> edge2host --> host19.pppg$i++;
		host20.pppg$o++ --> host2edge --> edge6.pppg$i++;
		edge6.pppg$o++ --> edge2host --> host20.pppg$i++;
		host21.pppg$o++ --> host2edge --> edge6.pppg$i++;
		edge6.pppg$o++ --> edge2host --> host21.pppg$i++;
		host22.pppg$o++ --> host2edge --> edge6.pppg$i++;
		edge6.pppg$o++ --> edge2host --> host22.pppg$i++;
}

module TAS1
{
	gates:
		inout pppg[];
	submodules:
		core0: Router {
			parameters:
				@RL();
				@CoreRouter();
				@display("i=abstract/switch");
		}
		gw1: Router {
			parameters:
				@RL();
				@display("i=abstract/router2");
		}
		edge2: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		edge3: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		edge4: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		edge5: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		host6: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host7: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host8: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host9: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host10: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host11: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host12: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host13: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host14: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host15: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host16: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host17: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host18: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host19: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host20: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host21: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host22: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}

	connections:
		pppg++ <--> core0.pppg++;
		pppg++ <--> core0.pppg++;
		pppg++ <--> core0.pppg++;
		pppg++ <--> core0.pppg++;
		pppg++ <--> core0.pppg++;
		pppg++ <--> core0.pppg++;
		pppg++ <--> core0.pppg++;

		core0.pppg++ <--> gateway2core <--> gw1.pppg++;
		gw1.pppg++ <--> edge2gateway <--> edge2.pppg++;
		gw1.pppg++ <--> edge2gateway <--> edge3.pppg++;
		gw1.pppg++ <--> edge2gateway <--> edge4.pppg++;
		gw1.pppg++ <--> edge2gateway <--> edge5.pppg++;
		host6.pppg$o++ --> host2edge --> edge2.pppg$i++;
		edge2.pppg$o++ --> edge2host --> host6.pppg$i++;
		host7.pppg$o++ --> host2edge --> edge2.pppg$i++;
		edge2.pppg$o++ --> edge2host --> host7.pppg$i++;
		host8.pppg$o++ --> host2edge --> edge2.pppg$i++;
		edge2.pppg$o++ --> edge2host --> host8.pppg$i++;
		host9.pppg$o++ --> host2edge --> edge2.pppg$i++;
		edge2.pppg$o++ --> edge2host --> host9.pppg$i++;
		host10.pppg$o++ --> host2edge --> edge3.pppg$i++;
		edge3.pppg$o++ --> edge2host --> host10.pppg$i++;
		host11.pppg$o++ --> host2edge --> edge3.pppg$i++;
		edge3.pppg$o++ --> edge2host --> host11.pppg$i++;
		host12.pppg$o++ --> host2edge --> edge3.pppg$i++;
		edge3.pppg$o++ --> edge2host --> host12.pppg$i++;
		host13.pppg$o++ --> host2edge --> edge3.pppg$i++;
		edge3.pppg$o++ --> edge2host --> host13.pppg$i++;
		host14.pppg$o++ --> host2edge --> edge3.pppg$i++;
		edge3.pppg$o++ --> edge2host --> host14.pppg$i++;
		host15.pppg$o++ --> host2edge --> edge4.pppg$i++;
		edge4.pppg$o++ --> edge2host --> host15.pppg$i++;
		host16.pppg$o++ --> host2edge --> edge4.pppg$i++;
		edge4.pppg$o++ --> edge2host --> host16.pppg$i++;
		host17.pppg$o++ --> host2edge --> edge4.pppg$i++;
		edge4.pppg$o++ --> edge2host --> host17.pppg$i++;
		host18.pppg$o++ --> host2edge --> edge5.pppg$i++;
		edge5.pppg$o++ --> edge2host --> host18.pppg$i++;
		host19.pppg$o++ --> host2edge --> edge5.pppg$i++;
		edge5.pppg$o++ --> edge2host --> host19.pppg$i++;
		host20.pppg$o++ --> host2edge --> edge5.pppg$i++;
		edge5.pppg$o++ --> edge2host --> host20.pppg$i++;
		host21.pppg$o++ --> host2edge --> edge5.pppg$i++;
		edge5.pppg$o++ --> edge2host --> host21.pppg$i++;
		host22.pppg$o++ --> host2edge --> edge5.pppg$i++;
		edge5.pppg$o++ --> edge2host --> host22.pppg$i++;
}

module SAS2
{
	gates:
		inout pppg[];
	submodules:
		core0: Router {
			parameters:
				@RL();
				@CoreRouter();
				@display("i=abstract/switch");
		}
		gw1: Router {
			parameters:
				@RL();
				@display("i=abstract/router2");
		}
		edge2: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		edge3: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		edge4: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		edge5: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		edge6: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		host7: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host8: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host9: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host10: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host11: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host12: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host13: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host14: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host15: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host16: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host17: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host18: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host19: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host20: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host21: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host22: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host23: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host24: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host25: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host26: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host27: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}

	connections:
		pppg++ <--> core0.pppg++;
		pppg++ <--> core0.pppg++;
		pppg++ <--> core0.pppg++;
		pppg++ <--> core0.pppg++;

		core0.pppg++ <--> gateway2core <--> gw1.pppg++;
		gw1.pppg++ <--> edge2gateway <--> edge2.pppg++;
		gw1.pppg++ <--> edge2gateway <--> edge3.pppg++;
		gw1.pppg++ <--> edge2gateway <--> edge4.pppg++;
		gw1.pppg++ <--> edge2gateway <--> edge5.pppg++;
		gw1.pppg++ <--> edge2gateway <--> edge6.pppg++;
		host7.pppg$o++ --> host2edge --> edge2.pppg$i++;
		edge2.pppg$o++ --> edge2host --> host7.pppg$i++;
		host8.pppg$o++ --> host2edge --> edge2.pppg$i++;
		edge2.pppg$o++ --> edge2host --> host8.pppg$i++;
		host9.pppg$o++ --> host2edge --> edge2.pppg$i++;
		edge2.pppg$o++ --> edge2host --> host9.pppg$i++;
		host10.pppg$o++ --> host2edge --> edge2.pppg$i++;
		edge2.pppg$o++ --> edge2host --> host10.pppg$i++;
		host11.pppg$o++ --> host2edge --> edge3.pppg$i++;
		edge3.pppg$o++ --> edge2host --> host11.pppg$i++;
		host12.pppg$o++ --> host2edge --> edge3.pppg$i++;
		edge3.pppg$o++ --> edge2host --> host12.pppg$i++;
		host13.pppg$o++ --> host2edge --> edge3.pppg$i++;
		edge3.pppg$o++ --> edge2host --> host13.pppg$i++;
		host14.pppg$o++ --> host2edge --> edge3.pppg$i++;
		edge3.pppg$o++ --> edge2host --> host14.pppg$i++;
		host15.pppg$o++ --> host2edge --> edge3.pppg$i++;
		edge3.pppg$o++ --> edge2host --> host15.pppg$i++;
		host16.pppg$o++ --> host2edge --> edge4.pppg$i++;
		edge4.pppg$o++ --> edge2host --> host16.pppg$i++;
		host17.pppg$o++ --> host2edge --> edge4.pppg$i++;
		edge4.pppg$o++ --> edge2host --> host17.pppg$i++;
		host18.pppg$o++ --> host2edge --> edge4.pppg$i++;
		edge4.pppg$o++ --> edge2host --> host18.pppg$i++;
		host19.pppg$o++ --> host2edge --> edge4.pppg$i++;
		edge4.pppg$o++ --> edge2host --> host19.pppg$i++;
		host20.pppg$o++ --> host2edge --> edge4.pppg$i++;
		edge4.pppg$o++ --> edge2host --> host20.pppg$i++;
		host21.pppg$o++ --> host2edge --> edge5.pppg$i++;
		edge5.pppg$o++ --> edge2host --> host21.pppg$i++;
		host22.pppg$o++ --> host2edge --> edge5.pppg$i++;
		edge5.pppg$o++ --> edge2host --> host22.pppg$i++;
		host23.pppg$o++ --> host2edge --> edge6.pppg$i++;
		edge6.pppg$o++ --> edge2host --> host23.pppg$i++;
		host24.pppg$o++ --> host2edge --> edge6.pppg$i++;
		edge6.pppg$o++ --> edge2host --> host24.pppg$i++;
		host25.pppg$o++ --> host2edge --> edge6.pppg$i++;
		edge6.pppg$o++ --> edge2host --> host25.pppg$i++;
		host26.pppg$o++ --> host2edge --> edge6.pppg$i++;
		edge6.pppg$o++ --> edge2host --> host26.pppg$i++;
		host27.pppg$o++ --> host2edge --> edge6.pppg$i++;
		edge6.pppg$o++ --> edge2host --> host27.pppg$i++;
}

module TAS3
{
	gates:
		inout pppg[];
	submodules:
		core0: Router {
			parameters:
				@RL();
				@CoreRouter();
				@display("i=abstract/switch");
		}
		gw1: Router {
			parameters:
				@RL();
				@display("i=abstract/router2");
		}
		gw2: Router {
			parameters:
				@RL();
				@display("i=abstract/router2");
		}
		edge3: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		edge4: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		edge5: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		edge6: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		edge7: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		edge8: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		edge9: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		edge10: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		host11: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host12: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host13: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host14: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host15: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host16: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host17: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host18: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host19: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host20: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host21: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host22: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host23: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host24: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host25: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host26: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host27: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host28: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host29: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host30: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host31: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host32: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host33: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host34: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host35: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host36: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host37: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host38: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host39: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}

	connections:
		pppg++ <--> core0.pppg++;
		pppg++ <--> core0.pppg++;
		pppg++ <--> core0.pppg++;
		pppg++ <--> core0.pppg++;
		pppg++ <--> core0.pppg++;
		pppg++ <--> core0.pppg++;

		core0.pppg++ <--> gateway2core <--> gw1.pppg++;
		core0.pppg++ <--> gateway2core <--> gw2.pppg++;
		gw1.pppg++ <--> edge2gateway <--> edge3.pppg++;
		gw1.pppg++ <--> edge2gateway <--> edge4.pppg++;
		gw1.pppg++ <--> edge2gateway <--> edge5.pppg++;
		gw1.pppg++ <--> edge2gateway <--> edge6.pppg++;
		gw1.pppg++ <--> edge2gateway <--> edge7.pppg++;
		gw2.pppg++ <--> edge2gateway <--> edge8.pppg++;
		gw2.pppg++ <--> edge2gateway <--> edge9.pppg++;
		gw2.pppg++ <--> edge2gateway <--> edge10.pppg++;
		host11.pppg$o++ --> host2edge --> edge3.pppg$i++;
		edge3.pppg$o++ --> edge2host --> host11.pppg$i++;
		host12.pppg$o++ --> host2edge --> edge3.pppg$i++;
		edge3.pppg$o++ --> edge2host --> host12.pppg$i++;
		host13.pppg$o++ --> host2edge --> edge3.pppg$i++;
		edge3.pppg$o++ --> edge2host --> host13.pppg$i++;
		host14.pppg$o++ --> host2edge --> edge4.pppg$i++;
		edge4.pppg$o++ --> edge2host --> host14.pppg$i++;
		host15.pppg$o++ --> host2edge --> edge4.pppg$i++;
		edge4.pppg$o++ --> edge2host --> host15.pppg$i++;
		host16.pppg$o++ --> host2edge --> edge4.pppg$i++;
		edge4.pppg$o++ --> edge2host --> host16.pppg$i++;
		host17.pppg$o++ --> host2edge --> edge4.pppg$i++;
		edge4.pppg$o++ --> edge2host --> host17.pppg$i++;
		host18.pppg$o++ --> host2edge --> edge4.pppg$i++;
		edge4.pppg$o++ --> edge2host --> host18.pppg$i++;
		host19.pppg$o++ --> host2edge --> edge5.pppg$i++;
		edge5.pppg$o++ --> edge2host --> host19.pppg$i++;
		host20.pppg$o++ --> host2edge --> edge5.pppg$i++;
		edge5.pppg$o++ --> edge2host --> host20.pppg$i++;
		host21.pppg$o++ --> host2edge --> edge5.pppg$i++;
		edge5.pppg$o++ --> edge2host --> host21.pppg$i++;
		host22.pppg$o++ --> host2edge --> edge5.pppg$i++;
		edge5.pppg$o++ --> edge2host --> host22.pppg$i++;
		host23.pppg$o++ --> host2edge --> edge6.pppg$i++;
		edge6.pppg$o++ --> edge2host --> host23.pppg$i++;
		host24.pppg$o++ --> host2edge --> edge6.pppg$i++;
		edge6.pppg$o++ --> edge2host --> host24.pppg$i++;
		host25.pppg$o++ --> host2edge --> edge7.pppg$i++;
		edge7.pppg$o++ --> edge2host --> host25.pppg$i++;
		host26.pppg$o++ --> host2edge --> edge7.pppg$i++;
		edge7.pppg$o++ --> edge2host --> host26.pppg$i++;
		host27.pppg$o++ --> host2edge --> edge7.pppg$i++;
		edge7.pppg$o++ --> edge2host --> host27.pppg$i++;
		host28.pppg$o++ --> host2edge --> edge8.pppg$i++;
		edge8.pppg$o++ --> edge2host --> host28.pppg$i++;
		host29.pppg$o++ --> host2edge --> edge8.pppg$i++;
		edge8.pppg$o++ --> edge2host --> host29.pppg$i++;
		host30.pppg$o++ --> host2edge --> edge8.pppg$i++;
		edge8.pppg$o++ --> edge2host --> host30.pppg$i++;
		host31.pppg$o++ --> host2edge --> edge9.pppg$i++;
		edge9.pppg$o++ --> edge2host --> host31.pppg$i++;
		host32.pppg$o++ --> host2edge --> edge9.pppg$i++;
		edge9.pppg$o++ --> edge2host --> host32.pppg$i++;
		host33.pppg$o++ --> host2edge --> edge9.pppg$i++;
		edge9.pppg$o++ --> edge2host --> host33.pppg$i++;
		host34.pppg$o++ --> host2edge --> edge9.pppg$i++;
		edge9.pppg$o++ --> edge2host --> host34.pppg$i++;
		host35.pppg$o++ --> host2edge --> edge9.pppg$i++;
		edge9.pppg$o++ --> edge2host --> host35.pppg$i++;
		host36.pppg$o++ --> host2edge --> edge10.pppg$i++;
		edge10.pppg$o++ --> edge2host --> host36.pppg$i++;
		host37.pppg$o++ --> host2edge --> edge10.pppg$i++;
		edge10.pppg$o++ --> edge2host --> host37.pppg$i++;
		host38.pppg$o++ --> host2edge --> edge10.pppg$i++;
		edge10.pppg$o++ --> edge2host --> host38.pppg$i++;
		host39.pppg$o++ --> host2edge --> edge10.pppg$i++;
		edge10.pppg$o++ --> edge2host --> host39.pppg$i++;
}

module SAS4
{
	gates:
		inout pppg[];
	submodules:
		core0: Router {
			parameters:
				@RL();
				@CoreRouter();
				@display("i=abstract/switch");
		}
		gw1: Router {
			parameters:
				@RL();
				@display("i=abstract/router2");
		}
		gw2: Router {
			parameters:
				@RL();
				@display("i=abstract/router2");
		}
		edge3: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		edge4: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		edge5: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		edge6: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		edge7: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		edge8: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		edge9: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		edge10: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		host11: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host12: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host13: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host14: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host15: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host16: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host17: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host18: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host19: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host20: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host21: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host22: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host23: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host24: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host25: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host26: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host27: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host28: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host29: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host30: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host31: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host32: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host33: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host34: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}

	connections:
		pppg++ <--> core0.pppg++;
		pppg++ <--> core0.pppg++;
		pppg++ <--> core0.pppg++;

		core0.pppg++ <--> gateway2core <--> gw1.pppg++;
		core0.pppg++ <--> gateway2core <--> gw2.pppg++;
		gw1.pppg++ <--> edge2gateway <--> edge3.pppg++;
		gw1.pppg++ <--> edge2gateway <--> edge4.pppg++;
		gw1.pppg++ <--> edge2gateway <--> edge5.pppg++;
		gw1.pppg++ <--> edge2gateway <--> edge6.pppg++;
		gw2.pppg++ <--> edge2gateway <--> edge7.pppg++;
		gw2.pppg++ <--> edge2gateway <--> edge8.pppg++;
		gw2.pppg++ <--> edge2gateway <--> edge9.pppg++;
		gw2.pppg++ <--> edge2gateway <--> edge10.pppg++;
		host11.pppg$o++ --> host2edge --> edge3.pppg$i++;
		edge3.pppg$o++ --> edge2host --> host11.pppg$i++;
		host12.pppg$o++ --> host2edge --> edge3.pppg$i++;
		edge3.pppg$o++ --> edge2host --> host12.pppg$i++;
		host13.pppg$o++ --> host2edge --> edge3.pppg$i++;
		edge3.pppg$o++ --> edge2host --> host13.pppg$i++;
		host14.pppg$o++ --> host2edge --> edge3.pppg$i++;
		edge3.pppg$o++ --> edge2host --> host14.pppg$i++;
		host15.pppg$o++ --> host2edge --> edge3.pppg$i++;
		edge3.pppg$o++ --> edge2host --> host15.pppg$i++;
		host16.pppg$o++ --> host2edge --> edge4.pppg$i++;
		edge4.pppg$o++ --> edge2host --> host16.pppg$i++;
		host17.pppg$o++ --> host2edge --> edge4.pppg$i++;
		edge4.pppg$o++ --> edge2host --> host17.pppg$i++;
		host18.pppg$o++ --> host2edge --> edge4.pppg$i++;
		edge4.pppg$o++ --> edge2host --> host18.pppg$i++;
		host19.pppg$o++ --> host2edge --> edge4.pppg$i++;
		edge4.pppg$o++ --> edge2host --> host19.pppg$i++;
		host20.pppg$o++ --> host2edge --> edge5.pppg$i++;
		edge5.pppg$o++ --> edge2host --> host20.pppg$i++;
		host21.pppg$o++ --> host2edge --> edge5.pppg$i++;
		edge5.pppg$o++ --> edge2host --> host21.pppg$i++;
		host22.pppg$o++ --> host2edge --> edge6.pppg$i++;
		edge6.pppg$o++ --> edge2host --> host22.pppg$i++;
		host23.pppg$o++ --> host2edge --> edge6.pppg$i++;
		edge6.pppg$o++ --> edge2host --> host23.pppg$i++;
		host24.pppg$o++ --> host2edge --> edge7.pppg$i++;
		edge7.pppg$o++ --> edge2host --> host24.pppg$i++;
		host25.pppg$o++ --> host2edge --> edge7.pppg$i++;
		edge7.pppg$o++ --> edge2host --> host25.pppg$i++;
		host26.pppg$o++ --> host2edge --> edge7.pppg$i++;
		edge7.pppg$o++ --> edge2host --> host26.pppg$i++;
		host27.pppg$o++ --> host2edge --> edge7.pppg$i++;
		edge7.pppg$o++ --> edge2host --> host27.pppg$i++;
		host28.pppg$o++ --> host2edge --> edge8.pppg$i++;
		edge8.pppg$o++ --> edge2host --> host28.pppg$i++;
		host29.pppg$o++ --> host2edge --> edge8.pppg$i++;
		edge8.pppg$o++ --> edge2host --> host29.pppg$i++;
		host30.pppg$o++ --> host2edge --> edge9.pppg$i++;
		edge9.pppg$o++ --> edge2host --> host30.pppg$i++;
		host31.pppg$o++ --> host2edge --> edge9.pppg$i++;
		edge9.pppg$o++ --> edge2host --> host31.pppg$i++;
		host32.pppg$o++ --> host2edge --> edge9.pppg$i++;
		edge9.pppg$o++ --> edge2host --> host32.pppg$i++;
		host33.pppg$o++ --> host2edge --> edge10.pppg$i++;
		edge10.pppg$o++ --> edge2host --> host33.pppg$i++;
		host34.pppg$o++ --> host2edge --> edge10.pppg$i++;
		edge10.pppg$o++ --> edge2host --> host34.pppg$i++;
}

module TAS5
{
	gates:
		inout pppg[];
	submodules:
		core0: Router {
			parameters:
				@RL();
				@CoreRouter();
				@display("i=abstract/switch");
		}
		gw1: Router {
			parameters:
				@RL();
				@display("i=abstract/router2");
		}
		gw2: Router {
			parameters:
				@RL();
				@display("i=abstract/router2");
		}
		edge3: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		edge4: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		edge5: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		edge6: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		edge7: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		edge8: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		edge9: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		host10: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host11: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host12: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host13: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host14: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host15: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host16: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host17: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host18: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host19: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host20: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host21: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host22: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host23: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host24: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host25: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host26: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host27: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host28: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host29: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host30: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host31: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host32: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host33: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host34: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}

	connections:
		pppg++ <--> core0.pppg++;
		pppg++ <--> core0.pppg++;
		pppg++ <--> core0.pppg++;
		pppg++ <--> core0.pppg++;
		pppg++ <--> core0.pppg++;
		pppg++ <--> core0.pppg++;

		core0.pppg++ <--> gateway2core <--> gw1.pppg++;
		core0.pppg++ <--> gateway2core <--> gw2.pppg++;
		gw1.pppg++ <--> edge2gateway <--> edge3.pppg++;
		gw1.pppg++ <--> edge2gateway <--> edge4.pppg++;
		gw1.pppg++ <--> edge2gateway <--> edge5.pppg++;
		gw1.pppg++ <--> edge2gateway <--> edge6.pppg++;
		gw2.pppg++ <--> edge2gateway <--> edge7.pppg++;
		gw2.pppg++ <--> edge2gateway <--> edge8.pppg++;
		gw2.pppg++ <--> edge2gateway <--> edge9.pppg++;
		host10.pppg$o++ --> host2edge --> edge3.pppg$i++;
		edge3.pppg$o++ --> edge2host --> host10.pppg$i++;
		host11.pppg$o++ --> host2edge --> edge3.pppg$i++;
		edge3.pppg$o++ --> edge2host --> host11.pppg$i++;
		host12.pppg$o++ --> host2edge --> edge3.pppg$i++;
		edge3.pppg$o++ --> edge2host --> host12.pppg$i++;
		host13.pppg$o++ --> host2edge --> edge4.pppg$i++;
		edge4.pppg$o++ --> edge2host --> host13.pppg$i++;
		host14.pppg$o++ --> host2edge --> edge4.pppg$i++;
		edge4.pppg$o++ --> edge2host --> host14.pppg$i++;
		host15.pppg$o++ --> host2edge --> edge4.pppg$i++;
		edge4.pppg$o++ --> edge2host --> host15.pppg$i++;
		host16.pppg$o++ --> host2edge --> edge5.pppg$i++;
		edge5.pppg$o++ --> edge2host --> host16.pppg$i++;
		host17.pppg$o++ --> host2edge --> edge5.pppg$i++;
		edge5.pppg$o++ --> edge2host --> host17.pppg$i++;
		host18.pppg$o++ --> host2edge --> edge5.pppg$i++;
		edge5.pppg$o++ --> edge2host --> host18.pppg$i++;
		host19.pppg$o++ --> host2edge --> edge5.pppg$i++;
		edge5.pppg$o++ --> edge2host --> host19.pppg$i++;
		host20.pppg$o++ --> host2edge --> edge6.pppg$i++;
		edge6.pppg$o++ --> edge2host --> host20.pppg$i++;
		host21.pppg$o++ --> host2edge --> edge6.pppg$i++;
		edge6.pppg$o++ --> edge2host --> host21.pppg$i++;
		host22.pppg$o++ --> host2edge --> edge6.pppg$i++;
		edge6.pppg$o++ --> edge2host --> host22.pppg$i++;
		host23.pppg$o++ --> host2edge --> edge7.pppg$i++;
		edge7.pppg$o++ --> edge2host --> host23.pppg$i++;
		host24.pppg$o++ --> host2edge --> edge7.pppg$i++;
		edge7.pppg$o++ --> edge2host --> host24.pppg$i++;
		host25.pppg$o++ --> host2edge --> edge7.pppg$i++;
		edge7.pppg$o++ --> edge2host --> host25.pppg$i++;
		host26.pppg$o++ --> host2edge --> edge7.pppg$i++;
		edge7.pppg$o++ --> edge2host --> host26.pppg$i++;
		host27.pppg$o++ --> host2edge --> edge8.pppg$i++;
		edge8.pppg$o++ --> edge2host --> host27.pppg$i++;
		host28.pppg$o++ --> host2edge --> edge8.pppg$i++;
		edge8.pppg$o++ --> edge2host --> host28.pppg$i++;
		host29.pppg$o++ --> host2edge --> edge8.pppg$i++;
		edge8.pppg$o++ --> edge2host --> host29.pppg$i++;
		host30.pppg$o++ --> host2edge --> edge8.pppg$i++;
		edge8.pppg$o++ --> edge2host --> host30.pppg$i++;
		host31.pppg$o++ --> host2edge --> edge9.pppg$i++;
		edge9.pppg$o++ --> edge2host --> host31.pppg$i++;
		host32.pppg$o++ --> host2edge --> edge9.pppg$i++;
		edge9.pppg$o++ --> edge2host --> host32.pppg$i++;
		host33.pppg$o++ --> host2edge --> edge9.pppg$i++;
		edge9.pppg$o++ --> edge2host --> host33.pppg$i++;
		host34.pppg$o++ --> host2edge --> edge9.pppg$i++;
		edge9.pppg$o++ --> edge2host --> host34.pppg$i++;
}

module SAS6
{
	gates:
		inout pppg[];
	submodules:
		core0: Router {
			parameters:
				@RL();
				@CoreRouter();
				@display("i=abstract/switch");
		}
		gw1: Router {
			parameters:
				@RL();
				@display("i=abstract/router2");
		}
		edge2: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		edge3: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		edge4: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		edge5: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		edge6: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		host7: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host8: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host9: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host10: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host11: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host12: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host13: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host14: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host15: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host16: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host17: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host18: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host19: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host20: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host21: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host22: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host23: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host24: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host25: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host26: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}

	connections:
		pppg++ <--> core0.pppg++;

		core0.pppg++ <--> gateway2core <--> gw1.pppg++;
		gw1.pppg++ <--> edge2gateway <--> edge2.pppg++;
		gw1.pppg++ <--> edge2gateway <--> edge3.pppg++;
		gw1.pppg++ <--> edge2gateway <--> edge4.pppg++;
		gw1.pppg++ <--> edge2gateway <--> edge5.pppg++;
		gw1.pppg++ <--> edge2gateway <--> edge6.pppg++;
		host7.pppg$o++ --> host2edge --> edge2.pppg$i++;
		edge2.pppg$o++ --> edge2host --> host7.pppg$i++;
		host8.pppg$o++ --> host2edge --> edge2.pppg$i++;
		edge2.pppg$o++ --> edge2host --> host8.pppg$i++;
		host9.pppg$o++ --> host2edge --> edge2.pppg$i++;
		edge2.pppg$o++ --> edge2host --> host9.pppg$i++;
		host10.pppg$o++ --> host2edge --> edge2.pppg$i++;
		edge2.pppg$o++ --> edge2host --> host10.pppg$i++;
		host11.pppg$o++ --> host2edge --> edge3.pppg$i++;
		edge3.pppg$o++ --> edge2host --> host11.pppg$i++;
		host12.pppg$o++ --> host2edge --> edge3.pppg$i++;
		edge3.pppg$o++ --> edge2host --> host12.pppg$i++;
		host13.pppg$o++ --> host2edge --> edge3.pppg$i++;
		edge3.pppg$o++ --> edge2host --> host13.pppg$i++;
		host14.pppg$o++ --> host2edge --> edge3.pppg$i++;
		edge3.pppg$o++ --> edge2host --> host14.pppg$i++;
		host15.pppg$o++ --> host2edge --> edge4.pppg$i++;
		edge4.pppg$o++ --> edge2host --> host15.pppg$i++;
		host16.pppg$o++ --> host2edge --> edge4.pppg$i++;
		edge4.pppg$o++ --> edge2host --> host16.pppg$i++;
		host17.pppg$o++ --> host2edge --> edge4.pppg$i++;
		edge4.pppg$o++ --> edge2host --> host17.pppg$i++;
		host18.pppg$o++ --> host2edge --> edge4.pppg$i++;
		edge4.pppg$o++ --> edge2host --> host18.pppg$i++;
		host19.pppg$o++ --> host2edge --> edge4.pppg$i++;
		edge4.pppg$o++ --> edge2host --> host19.pppg$i++;
		host20.pppg$o++ --> host2edge --> edge5.pppg$i++;
		edge5.pppg$o++ --> edge2host --> host20.pppg$i++;
		host21.pppg$o++ --> host2edge --> edge5.pppg$i++;
		edge5.pppg$o++ --> edge2host --> host21.pppg$i++;
		host22.pppg$o++ --> host2edge --> edge5.pppg$i++;
		edge5.pppg$o++ --> edge2host --> host22.pppg$i++;
		host23.pppg$o++ --> host2edge --> edge5.pppg$i++;
		edge5.pppg$o++ --> edge2host --> host23.pppg$i++;
		host24.pppg$o++ --> host2edge --> edge6.pppg$i++;
		edge6.pppg$o++ --> edge2host --> host24.pppg$i++;
		host25.pppg$o++ --> host2edge --> edge6.pppg$i++;
		edge6.pppg$o++ --> edge2host --> host25.pppg$i++;
		host26.pppg$o++ --> host2edge --> edge6.pppg$i++;
		edge6.pppg$o++ --> edge2host --> host26.pppg$i++;
}

module SAS7
{
	gates:
		inout pppg[];
	submodules:
		core0: Router {
			parameters:
				@RL();
				@CoreRouter();
				@display("i=abstract/switch");
		}
		gw1: Router {
			parameters:
				@RL();
				@display("i=abstract/router2");
		}
		edge2: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		edge3: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		edge4: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		edge5: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		edge6: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		host7: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host8: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host9: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host10: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host11: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host12: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host13: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host14: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host15: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host16: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host17: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host18: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host19: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host20: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host21: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host22: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host23: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}

	connections:
		pppg++ <--> core0.pppg++;
		pppg++ <--> core0.pppg++;

		core0.pppg++ <--> gateway2core <--> gw1.pppg++;
		gw1.pppg++ <--> edge2gateway <--> edge2.pppg++;
		gw1.pppg++ <--> edge2gateway <--> edge3.pppg++;
		gw1.pppg++ <--> edge2gateway <--> edge4.pppg++;
		gw1.pppg++ <--> edge2gateway <--> edge5.pppg++;
		gw1.pppg++ <--> edge2gateway <--> edge6.pppg++;
		host7.pppg$o++ --> host2edge --> edge2.pppg$i++;
		edge2.pppg$o++ --> edge2host --> host7.pppg$i++;
		host8.pppg$o++ --> host2edge --> edge2.pppg$i++;
		edge2.pppg$o++ --> edge2host --> host8.pppg$i++;
		host9.pppg$o++ --> host2edge --> edge2.pppg$i++;
		edge2.pppg$o++ --> edge2host --> host9.pppg$i++;
		host10.pppg$o++ --> host2edge --> edge2.pppg$i++;
		edge2.pppg$o++ --> edge2host --> host10.pppg$i++;
		host11.pppg$o++ --> host2edge --> edge2.pppg$i++;
		edge2.pppg$o++ --> edge2host --> host11.pppg$i++;
		host12.pppg$o++ --> host2edge --> edge3.pppg$i++;
		edge3.pppg$o++ --> edge2host --> host12.pppg$i++;
		host13.pppg$o++ --> host2edge --> edge3.pppg$i++;
		edge3.pppg$o++ --> edge2host --> host13.pppg$i++;
		host14.pppg$o++ --> host2edge --> edge3.pppg$i++;
		edge3.pppg$o++ --> edge2host --> host14.pppg$i++;
		host15.pppg$o++ --> host2edge --> edge3.pppg$i++;
		edge3.pppg$o++ --> edge2host --> host15.pppg$i++;
		host16.pppg$o++ --> host2edge --> edge3.pppg$i++;
		edge3.pppg$o++ --> edge2host --> host16.pppg$i++;
		host17.pppg$o++ --> host2edge --> edge4.pppg$i++;
		edge4.pppg$o++ --> edge2host --> host17.pppg$i++;
		host18.pppg$o++ --> host2edge --> edge4.pppg$i++;
		edge4.pppg$o++ --> edge2host --> host18.pppg$i++;
		host19.pppg$o++ --> host2edge --> edge5.pppg$i++;
		edge5.pppg$o++ --> edge2host --> host19.pppg$i++;
		host20.pppg$o++ --> host2edge --> edge5.pppg$i++;
		edge5.pppg$o++ --> edge2host --> host20.pppg$i++;
		host21.pppg$o++ --> host2edge --> edge6.pppg$i++;
		edge6.pppg$o++ --> edge2host --> host21.pppg$i++;
		host22.pppg$o++ --> host2edge --> edge6.pppg$i++;
		edge6.pppg$o++ --> edge2host --> host22.pppg$i++;
		host23.pppg$o++ --> host2edge --> edge6.pppg$i++;
		edge6.pppg$o++ --> edge2host --> host23.pppg$i++;
}

module SAS8
{
	gates:
		inout pppg[];
	submodules:
		core0: Router {
			parameters:
				@RL();
				@CoreRouter();
				@display("i=abstract/switch");
		}
		gw1: Router {
			parameters:
				@RL();
				@display("i=abstract/router2");
		}
		gw2: Router {
			parameters:
				@RL();
				@display("i=abstract/router2");
		}
		edge3: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		edge4: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		edge5: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		edge6: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		edge7: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		edge8: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		edge9: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		edge10: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		edge11: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		edge12: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		edge13: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		edge14: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		host15: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host16: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host17: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host18: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host19: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host20: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host21: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host22: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host23: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host24: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host25: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host26: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host27: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host28: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host29: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host30: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host31: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host32: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host33: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host34: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host35: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host36: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host37: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host38: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host39: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host40: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host41: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host42: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host43: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host44: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host45: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host46: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host47: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host48: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host49: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host50: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host51: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host52: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host53: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host54: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}

	connections:
		pppg++ <--> core0.pppg++;
		pppg++ <--> core0.pppg++;

		core0.pppg++ <--> gateway2core <--> gw1.pppg++;
		core0.pppg++ <--> gateway2core <--> gw2.pppg++;
		gw1.pppg++ <--> edge2gateway <--> edge3.pppg++;
		gw1.pppg++ <--> edge2gateway <--> edge4.pppg++;
		gw1.pppg++ <--> edge2gateway <--> edge5.pppg++;
		gw1.pppg++ <--> edge2gateway <--> edge6.pppg++;
		gw1.pppg++ <--> edge2gateway <--> edge7.pppg++;
		gw1.pppg++ <--> edge2gateway <--> edge8.pppg++;
		gw2.pppg++ <--> edge2gateway <--> edge9.pppg++;
		gw2.pppg++ <--> edge2gateway <--> edge10.pppg++;
		gw2.pppg++ <--> edge2gateway <--> edge11.pppg++;
		gw2.pppg++ <--> edge2gateway <--> edge12.pppg++;
		gw2.pppg++ <--> edge2gateway <--> edge13.pppg++;
		gw2.pppg++ <--> edge2gateway <--> edge14.pppg++;
		host15.pppg$o++ --> host2edge --> edge3.pppg$i++;
		edge3.pppg$o++ --> edge2host --> host15.pppg$i++;
		host16.pppg$o++ --> host2edge --> edge3.pppg$i++;
		edge3.pppg$o++ --> edge2host --> host16.pppg$i++;
		host17.pppg$o++ --> host2edge --> edge3.pppg$i++;
		edge3.pppg$o++ --> edge2host --> host17.pppg$i++;
		host18.pppg$o++ --> host2edge --> edge3.pppg$i++;
		edge3.pppg$o++ --> edge2host --> host18.pppg$i++;
		host19.pppg$o++ --> host2edge --> edge3.pppg$i++;
		edge3.pppg$o++ --> edge2host --> host19.pppg$i++;
		host20.pppg$o++ --> host2edge --> edge4.pppg$i++;
		edge4.pppg$o++ --> edge2host --> host20.pppg$i++;
		host21.pppg$o++ --> host2edge --> edge4.pppg$i++;
		edge4.pppg$o++ --> edge2host --> host21.pppg$i++;
		host22.pppg$o++ --> host2edge --> edge4.pppg$i++;
		edge4.pppg$o++ --> edge2host --> host22.pppg$i++;
		host23.pppg$o++ --> host2edge --> edge5.pppg$i++;
		edge5.pppg$o++ --> edge2host --> host23.pppg$i++;
		host24.pppg$o++ --> host2edge --> edge5.pppg$i++;
		edge5.pppg$o++ --> edge2host --> host24.pppg$i++;
		host25.pppg$o++ --> host2edge --> edge6.pppg$i++;
		edge6.pppg$o++ --> edge2host --> host25.pppg$i++;
		host26.pppg$o++ --> host2edge --> edge6.pppg$i++;
		edge6.pppg$o++ --> edge2host --> host26.pppg$i++;
		host27.pppg$o++ --> host2edge --> edge7.pppg$i++;
		edge7.pppg$o++ --> edge2host --> host27.pppg$i++;
		host28.pppg$o++ --> host2edge --> edge7.pppg$i++;
		edge7.pppg$o++ --> edge2host --> host28.pppg$i++;
		host29.pppg$o++ --> host2edge --> edge8.pppg$i++;
		edge8.pppg$o++ --> edge2host --> host29.pppg$i++;
		host30.pppg$o++ --> host2edge --> edge8.pppg$i++;
		edge8.pppg$o++ --> edge2host --> host30.pppg$i++;
		host31.pppg$o++ --> host2edge --> edge8.pppg$i++;
		edge8.pppg$o++ --> edge2host --> host31.pppg$i++;
		host32.pppg$o++ --> host2edge --> edge9.pppg$i++;
		edge9.pppg$o++ --> edge2host --> host32.pppg$i++;
		host33.pppg$o++ --> host2edge --> edge9.pppg$i++;
		edge9.pppg$o++ --> edge2host --> host33.pppg$i++;
		host34.pppg$o++ --> host2edge --> edge9.pppg$i++;
		edge9.pppg$o++ --> edge2host --> host34.pppg$i++;
		host35.pppg$o++ --> host2edge --> edge9.pppg$i++;
		edge9.pppg$o++ --> edge2host --> host35.pppg$i++;
		host36.pppg$o++ --> host2edge --> edge10.pppg$i++;
		edge10.pppg$o++ --> edge2host --> host36.pppg$i++;
		host37.pppg$o++ --> host2edge --> edge10.pppg$i++;
		edge10.pppg$o++ --> edge2host --> host37.pppg$i++;
		host38.pppg$o++ --> host2edge --> edge10.pppg$i++;
		edge10.pppg$o++ --> edge2host --> host38.pppg$i++;
		host39.pppg$o++ --> host2edge --> edge11.pppg$i++;
		edge11.pppg$o++ --> edge2host --> host39.pppg$i++;
		host40.pppg$o++ --> host2edge --> edge11.pppg$i++;
		edge11.pppg$o++ --> edge2host --> host40.pppg$i++;
		host41.pppg$o++ --> host2edge --> edge11.pppg$i++;
		edge11.pppg$o++ --> edge2host --> host41.pppg$i++;
		host42.pppg$o++ --> host2edge --> edge11.pppg$i++;
		edge11.pppg$o++ --> edge2host --> host42.pppg$i++;
		host43.pppg$o++ --> host2edge --> edge11.pppg$i++;
		edge11.pppg$o++ --> edge2host --> host43.pppg$i++;
		host44.pppg$o++ --> host2edge --> edge12.pppg$i++;
		edge12.pppg$o++ --> edge2host --> host44.pppg$i++;
		host45.pppg$o++ --> host2edge --> edge12.pppg$i++;
		edge12.pppg$o++ --> edge2host --> host45.pppg$i++;
		host46.pppg$o++ --> host2edge --> edge12.pppg$i++;
		edge12.pppg$o++ --> edge2host --> host46.pppg$i++;
		host47.pppg$o++ --> host2edge --> edge13.pppg$i++;
		edge13.pppg$o++ --> edge2host --> host47.pppg$i++;
		host48.pppg$o++ --> host2edge --> edge13.pppg$i++;
		edge13.pppg$o++ --> edge2host --> host48.pppg$i++;
		host49.pppg$o++ --> host2edge --> edge13.pppg$i++;
		edge13.pppg$o++ --> edge2host --> host49.pppg$i++;
		host50.pppg$o++ --> host2edge --> edge13.pppg$i++;
		edge13.pppg$o++ --> edge2host --> host50.pppg$i++;
		host51.pppg$o++ --> host2edge --> edge13.pppg$i++;
		edge13.pppg$o++ --> edge2host --> host51.pppg$i++;
		host52.pppg$o++ --> host2edge --> edge14.pppg$i++;
		edge14.pppg$o++ --> edge2host --> host52.pppg$i++;
		host53.pppg$o++ --> host2edge --> edge14.pppg$i++;
		edge14.pppg$o++ --> edge2host --> host53.pppg$i++;
		host54.pppg$o++ --> host2edge --> edge14.pppg$i++;
		edge14.pppg$o++ --> edge2host --> host54.pppg$i++;
}

module SAS9
{
	gates:
		inout pppg[];
	submodules:
		core0: Router {
			parameters:
				@RL();
				@CoreRouter();
				@display("i=abstract/switch");
		}
		gw1: Router {
			parameters:
				@RL();
				@display("i=abstract/router2");
		}
		gw2: Router {
			parameters:
				@RL();
				@display("i=abstract/router2");
		}
		edge3: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		edge4: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		edge5: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		edge6: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		edge7: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		edge8: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		edge9: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		edge10: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		edge11: Router {
			parameters:
				@RL();
				@display("i=abstract/router");
		}
		host12: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host13: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host14: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host15: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host16: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host17: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host18: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host19: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host20: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host21: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host22: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host23: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host24: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host25: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host26: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host27: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host28: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host29: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host30: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host31: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host32: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host33: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host34: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host35: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host36: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host37: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host38: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host39: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host40: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host41: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host42: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host43: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}
		host44: Node {
			parameters:
				@RL();
				@display("i=device/laptop");
		}

	connections:
		pppg++ <--> core0.pppg++;
		pppg++ <--> core0.pppg++;

		core0.pppg++ <--> gateway2core <--> gw1.pppg++;
		core0.pppg++ <--> gateway2core <--> gw2.pppg++;
		gw1.pppg++ <--> edge2gateway <--> edge3.pppg++;
		gw1.pppg++ <--> edge2gateway <--> edge4.pppg++;
		gw1.pppg++ <--> edge2gateway <--> edge5.pppg++;
		gw1.pppg++ <--> edge2gateway <--> edge6.pppg++;
		gw1.pppg++ <--> edge2gateway <--> edge7.pppg++;
		gw1.pppg++ <--> edge2gateway <--> edge8.pppg++;
		gw2.pppg++ <--> edge2gateway <--> edge9.pppg++;
		gw2.pppg++ <--> edge2gateway <--> edge10.pppg++;
		gw2.pppg++ <--> edge2gateway <--> edge11.pppg++;
		host12.pppg$o++ --> host2edge --> edge3.pppg$i++;
		edge3.pppg$o++ --> edge2host --> host12.pppg$i++;
		host13.pppg$o++ --> host2edge --> edge3.pppg$i++;
		edge3.pppg$o++ --> edge2host --> host13.pppg$i++;
		host14.pppg$o++ --> host2edge --> edge3.pppg$i++;
		edge3.pppg$o++ --> edge2host --> host14.pppg$i++;
		host15.pppg$o++ --> host2edge --> edge4.pppg$i++;
		edge4.pppg$o++ --> edge2host --> host15.pppg$i++;
		host16.pppg$o++ --> host2edge --> edge4.pppg$i++;
		edge4.pppg$o++ --> edge2host --> host16.pppg$i++;
		host17.pppg$o++ --> host2edge --> edge4.pppg$i++;
		edge4.pppg$o++ --> edge2host --> host17.pppg$i++;
		host18.pppg$o++ --> host2edge --> edge4.pppg$i++;
		edge4.pppg$o++ --> edge2host --> host18.pppg$i++;
		host19.pppg$o++ --> host2edge --> edge4.pppg$i++;
		edge4.pppg$o++ --> edge2host --> host19.pppg$i++;
		host20.pppg$o++ --> host2edge --> edge5.pppg$i++;
		edge5.pppg$o++ --> edge2host --> host20.pppg$i++;
		host21.pppg$o++ --> host2edge --> edge5.pppg$i++;
		edge5.pppg$o++ --> edge2host --> host21.pppg$i++;
		host22.pppg$o++ --> host2edge --> edge5.pppg$i++;
		edge5.pppg$o++ --> edge2host --> host22.pppg$i++;
		host23.pppg$o++ --> host2edge --> edge5.pppg$i++;
		edge5.pppg$o++ --> edge2host --> host23.pppg$i++;
		host24.pppg$o++ --> host2edge --> edge5.pppg$i++;
		edge5.pppg$o++ --> edge2host --> host24.pppg$i++;
		host25.pppg$o++ --> host2edge --> edge6.pppg$i++;
		edge6.pppg$o++ --> edge2host --> host25.pppg$i++;
		host26.pppg$o++ --> host2edge --> edge6.pppg$i++;
		edge6.pppg$o++ --> edge2host --> host26.pppg$i++;
		host27.pppg$o++ --> host2edge --> edge6.pppg$i++;
		edge6.pppg$o++ --> edge2host --> host27.pppg$i++;
		host28.pppg$o++ --> host2edge --> edge7.pppg$i++;
		edge7.pppg$o++ --> edge2host --> host28.pppg$i++;
		host29.pppg$o++ --> host2edge --> edge7.pppg$i++;
		edge7.pppg$o++ --> edge2host --> host29.pppg$i++;
		host30.pppg$o++ --> host2edge --> edge7.pppg$i++;
		edge7.pppg$o++ --> edge2host --> host30.pppg$i++;
		host31.pppg$o++ --> host2edge --> edge7.pppg$i++;
		edge7.pppg$o++ --> edge2host --> host31.pppg$i++;
		host32.pppg$o++ --> host2edge --> edge8.pppg$i++;
		edge8.pppg$o++ --> edge2host --> host32.pppg$i++;
		host33.pppg$o++ --> host2edge --> edge8.pppg$i++;
		edge8.pppg$o++ --> edge2host --> host33.pppg$i++;
		host34.pppg$o++ --> host2edge --> edge9.pppg$i++;
		edge9.pppg$o++ --> edge2host --> host34.pppg$i++;
		host35.pppg$o++ --> host2edge --> edge9.pppg$i++;
		edge9.pppg$o++ --> edge2host --> host35.pppg$i++;
		host36.pppg$o++ --> host2edge --> edge9.pppg$i++;
		edge9.pppg$o++ --> edge2host --> host36.pppg$i++;
		host37.pppg$o++ --> host2edge --> edge9.pppg$i++;
		edge9.pppg$o++ --> edge2host --> host37.pppg$i++;
		host38.pppg$o++ --> host2edge --> edge9.pppg$i++;
		edge9.pppg$o++ --> edge2host --> host38.pppg$i++;
		host39.pppg$o++ --> host2edge --> edge10.pppg$i++;
		edge10.pppg$o++ --> edge2host --> host39.pppg$i++;
		host40.pppg$o++ --> host2edge --> edge10.pppg$i++;
		edge10.pppg$o++ --> edge2host --> host40.pppg$i++;
		host41.pppg$o++ --> host2edge --> edge11.pppg$i++;
		edge11.pppg$o++ --> edge2host --> host41.pppg$i++;
		host42.pppg$o++ --> host2edge --> edge11.pppg$i++;
		edge11.pppg$o++ --> edge2host --> host42.pppg$i++;
		host43.pppg$o++ --> host2edge --> edge11.pppg$i++;
		edge11.pppg$o++ --> edge2host --> host43.pppg$i++;
		host44.pppg$o++ --> host2edge --> edge11.pppg$i++;
		edge11.pppg$o++ --> edge2host --> host44.pppg$i++;
}

