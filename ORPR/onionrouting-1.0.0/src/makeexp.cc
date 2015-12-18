#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <ctype.h>
#include <math.h>

#define panic(a...) do { fprintf(stderr, a); fprintf(stderr, "\n"); exit(1); } while (0)

char nodeid[10000][50];
char ip[10000][20];

int main(int argc, char *argv[]) {
	double pXmit = 1.0;
	bool probabilistic = false;
	__attribute__((unused)) bool isRational = false;
	int num, wit;
	int numRationalNodes;
	int nbRelais;

	if ((argc == 4) && !strcmp(argv[1], "manual")) {
		num = atoi(argv[2]);
		wit = atoi(argv[3]);
	} else if ((argc == 4) && !strcmp(argv[1], "basic")) {
		double fractionFaulty = atof(argv[3]);
		num = atoi(argv[2]);
		wit = (int) (num * fractionFaulty + 0.999);
	} else if ((argc == 5) && !strcmp(argv[1], "sws")) {
		double fractionFaulty = atof(argv[3]);
		double pF = atof(argv[4]);
		num = atoi(argv[2]);
		wit = (int) ceil(log(1 - pow(1 - pF, 1.0 / num)) / log(fractionFaulty));
		if (wit > (int) (num * fractionFaulty + 0.999))
			panic(
					"This configuration does not make sense ('basic' needs fewer witnesses)");
	} else if ((argc == 5) && !strcmp(argv[1], "rcc")) {
		double fractionFaulty = atof(argv[3]);
		double pM = atof(argv[4]);
		num = atoi(argv[2]);
		wit = (int) (num * fractionFaulty + 0.999);
		probabilistic = true;
		pXmit = (1 - pow(pM, 1.0 / (wit * wit)))
				/ ((1 - fractionFaulty) * (1 - fractionFaulty));
	} else if ((argc == 6) && !strcmp(argv[1], "sws+rcc")) {
		double fractionFaulty = atof(argv[3]);
		double pF = atof(argv[4]);
		double pM = atof(argv[5]);
		num = atoi(argv[2]);
		wit = (int) ceil(log(1 - pow(1 - pF, 1.0 / num)) / log(fractionFaulty));
		probabilistic = true;
		pXmit = (1 - pow(pM, 1.0 / (wit * wit)))
				/ ((1 - fractionFaulty) * (1 - fractionFaulty));
		if (wit > (int) (num * fractionFaulty + 0.999))
			panic(
					"This configuration does not make sense ('basic' needs fewer witnesses)");
	} else if ((argc == 6) && !strcmp(argv[1], "rational")) {
		num = atoi(argv[2]);
		wit = atoi(argv[3]);
		numRationalNodes = atoi(argv[4]);
		isRational = true;
		nbRelais = atoi(argv[5]);
	} else {
		fprintf(stderr, "Syntax: makeexp manual <#nodes> <#witnesses>\n");
		fprintf(stderr, "        makeexp basic <#nodes> <fractionFaulty>\n");
		fprintf(stderr,
				"        makeexp sws <#nodes> <fractionFaulty> <probAllFaulty>\n");
		fprintf(stderr,
				"        makeexp rcc <#nodes> <fractionFaulty> <probInstanceMissed>\n");
		fprintf(stderr,
				"        makeexp sws+rcc <#nodes> <fractionFaulty> <probAllFaulty> <probInstanceMissed>\n");
		fprintf(stderr,
				"        makeexp rational <#nodes> <#witnesses> <#rational>\n");
		return 1;
	}

	printf("%d node(s), %d witnesses/node, pXmit=%.6f\n", num, wit,
			(float) pXmit);
	if (probabilistic && (pXmit > 1))
		panic(
				"This configuration is not feasible (increase probInstanceMissed)");
	if (wit > num)
		panic("This configuration is not feasible (increase probAllFaulty)");

	if (numRationalNodes > num)
		panic("This configuration is not feasible (decrease numRationalNodes)");
	struct stat statbuf;
	bool useEsign = (stat("ca/ca_pubkey.esign", &statbuf) == 0);

	for (int i = 0; i < num; i++) {
		char namebuf[200];
		sprintf(namebuf, "ca/node%04d_cert.%s", 1 + i,
				useEsign ? "esign" : "pem");

		FILE *infile = fopen(namebuf, "r");
		if (!infile)
			panic("Cannot open: %s", namebuf);

		if (useEsign) {
			char linebuf[2000]; __attribute__((unused)) char *c;
			for (int j = 0; j < 3; j++)
				c = fgets(linebuf, sizeof(linebuf), infile);
			c = fgets(nodeid[i], sizeof(nodeid[i]), infile);
			strtok(nodeid[i], "\r\n");
		} else {
			char linebuf[2000];
			bool found = false;
			while (fgets(linebuf, sizeof(linebuf), infile)) {
				if (!strstr(linebuf, "Subject:"))
					continue;

				char *cn = strstr(linebuf, "CN=");
				if (!cn)
					panic("Cannot find CN in %s's Subject: line", namebuf);

				strncpy(nodeid[i], &cn[3], 40);
				for (int j = 0; j < 40; j++)
					nodeid[i][j] = toupper(nodeid[i][j]);
				nodeid[i][40] = 0;
				found = true;
			}

			fclose(infile);
			if (!found)
				panic("Cannot find nodeID in %s", namebuf);
		}

		sprintf(ip[i], "10.0.%d.%d", (int) (i / 254), 1 + (i % 254));
		printf("#%d: %s %s\n", i, nodeid[i], ip[i]);
	}

	char paramname[200];
	sprintf(paramname, "onionrouting-%d-%s.param", num, argv[1]);
	FILE *paramfile = fopen(paramname, "w+");
	if (!paramfile)
		panic("Cannot write: %s", paramname);

	FILE *scriptfile = fopen("makedirs.sh", "w+");
	if (!scriptfile)
		panic("Cannot write: makedirs.sh");

	char dirname[200];
	sprintf(dirname, "peerreview-onionrouting-%d-%s", num, argv[1]);

	fprintf(scriptfile, "#!/bin/bash\n\nmkdir -p %s\n", dirname);
	//int z = 7; //par moi
	int z = 0; //par moi
	for (int i = 0; i < num; i++) {
		fprintf(paramfile, "[%s/%s]\n", nodeid[i], ip[i]);
		fprintf(paramfile, "directory=%s/node%04d\n", dirname, 1 + i);
		fprintf(paramfile, "trace=node%04d.trace\n", 1 + i);
		fprintf(paramfile, "storageDir=%s/node%04d/storage\n", dirname, 1 + i);
		fprintf(paramfile, "witnesses=%d\n", wit);


		///decommenter par moi
		for (int j = 0; j < wit; j++)
			fprintf(paramfile, "witness%d=%s/%s\n", 1 + j,
					nodeid[(i + num - wit + j) % num],
					ip[(i + num - wit + j) % num]);
		///
		//if(i==0) fprintf(paramfile, "peerreviewMisbehavior=silent,%d\n",10000000);
		//if((i%3==0 || i%7==0)&& z!=numRationalNodes) {
		if (i > (wit + z) && i <= numRationalNodes + (wit + z)) {
			//if(i>0 && i<numRationalNodes+1) {
			fprintf(paramfile, "rational=%s\n", "true");
			//z++;
			//if(i<=12)
			//fprintf(paramfile, "peerreviewMisbehavior=reluctant,%d\n",10000000);
			//fprintf(paramfile,"multicastRational=%s\n","false");
			//if(i>=wit+1)
			if (i > 2 * wit + z)
				fprintf(paramfile, "onionroutingMisbehavior=10000000,%s\n",
						"silent");
		} else {
			fprintf(paramfile, "rational=%s\n", "false");
			//if(i==numRationalNodes+1)
			/*if(numRationalNodes>0 && i==numRationalNodes+(wit+z)+1)
			 fprintf(paramfile,"multicastMisbehavior=10000000,%s\n","silent");*/
			//fprintf(paramfile, "peerreviewMisbehavior=silent,%d\n",1000000);
		}
		//fprintf(paramfile, "rational=%s\n","false");
		//if(i>50 && i<70)
		//fprintf(paramfile, "peerreviewMisbehavior=silent,%d\n",10000000);
		// fprintf(paramfile, "rational=%s\n","true");
		fprintf(paramfile, "nbRelais=%d\n", nbRelais);
		if (probabilistic)
			fprintf(paramfile, "pTransmit=%.8f\n", pXmit);
		fprintf(paramfile, "\n");

		fprintf(scriptfile, "mkdir -p %s/node%04d/storage\n", dirname, 1 + i);
		fprintf(scriptfile, "mkdir -p %s/node%04d/peers\n", dirname, 1 + i);

		if (useEsign) {
			fprintf(scriptfile, "cp ca/ca_pubkey.esign %s/node%04d/\n", dirname,
					1 + i);
			fprintf(scriptfile,
					"cp ca/node%04d_key.esign %s/node%04d/nodekey.esign\n",
					1 + i, dirname, 1 + i);
			fprintf(scriptfile,
					"cp ca/node%04d_cert.esign %s/node%04d/nodecert.esign\n",
					1 + i, dirname, 1 + i);
		} else {
			fprintf(scriptfile, "cp ca/cacert.pem %s/node%04d/\n", dirname,
					1 + i);
			fprintf(scriptfile,
					"cp ca/node%04d_key.pem %s/node%04d/nodekey.pem\n", 1 + i,
					dirname, 1 + i);
			fprintf(scriptfile,
					"cp ca/node%04d_cert.pem %s/node%04d/nodecert.pem\n", 1 + i,
					dirname, 1 + i);
		}

#if 0
		for (int j=0; j<num; j++)
		fprintf(scriptfile, "cp ca/node%04d_cert.pem %s/node%04d/peers/%s-cert.pem\n", 1+j, dirname, 1+i, nodeid[j]);
#endif
	}

	fclose(paramfile);
	fclose(scriptfile);

	scriptfile = fopen("cleanup.sh", "w+");
	fprintf(scriptfile, "#!/bin/bash\n\n");
	fprintf(scriptfile, "rm -rf %s\n", dirname);
	fprintf(scriptfile, "rm -f sim-*.log\n");
	fprintf(scriptfile, "rm -f snapshots*.data\n");
	fclose(scriptfile);

	chmod("makedirs.sh", 0755);
	chmod("cleanup.sh", 0755);

	return 0;
}
