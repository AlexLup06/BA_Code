package cdnsimulator;

import inet.applications.contract.IApp;

simple OriginServerApp like IApp
{
    parameters:
        string pathToObjectsFile = default("/Users/alexanderlupatsiy/Documents/Uni/Semester_6/Bachelor_Arbeit/Code/CDNsimulator/src/objects.txt");
        double computeTime = default(0.02);
        double sendLoadInterval = default(0.01);
        
        @display("i=block/app");
        @lifecycleSupport;
        @signal[packetSent](type=inet::Packet);
        @signal[packetReceived](type=inet::Packet);
        @statistic[packetReceived](title="packets received"; source=packetReceived; record=count,"sum(packetBytes)","vector(packetBytes)"; interpolationmode=none);
        @statistic[packetSent](title="packets sent"; source=packetSent; record=count,"sum(packetBytes)","vector(packetBytes)"; interpolationmode=none);
        @statistic[endToEndDelay](title="end-to-end delay"; source="dataAge(packetReceived)"; unit=s; record=histogram,weightedHistogram,vector; interpolationmode=none);
        
        @signal[originRequestIn](type=short);
        @statistic[originRequestIn](title="request in"; source=originRequestIn; record=vector);

        @signal[originResponseOut](type=short);
        @statistic[originResponseOut](title="response out"; source=originResponseOut; record=vector);

        @signal[loadInfoOrigin](type=long);
        @statistic[load](title="load of surrogate"; source=loadInfoOrigin; record=vector);
    gates:
        input socketIn @labels(TcpCommand/up);
        output socketOut @labels(TcpCommand/down);
}
