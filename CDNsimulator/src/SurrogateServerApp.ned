package cdnsimulator;

import inet.applications.contract.IApp;

simple SurrogateServerApp like IApp
{
    parameters:
        double computeTime = default(0.04);
        int maxCacheSize = default(4);
        int maxNumberConnections = default(2);
        double sendLoadInterval = default(0.01);
        double priorityTime = default(0.01);

        @display("i=block/app");
        @lifecycleSupport;
        @signal[packetSent](type=inet::Packet);
        @signal[packetReceived](type=inet::Packet);
        @statistic[packetReceived](title="packets received"; source=packetReceived; record=count,"sum(packetBytes)","vector(packetBytes)"; interpolationmode=none);
        @statistic[packetSent](title="packets sent"; source=packetSent; record=count,"sum(packetBytes)","vector(packetBytes)"; interpolationmode=none);
        @statistic[endToEndDelay](title="end-to-end delay"; source="dataAge(packetReceived)"; unit=s; record=histogram,weightedHistogram,vector; interpolationmode=none);

        @signal[surrogateLoadObjects](type=short);
        @statistic[surrogateLoadObjects](title="load"; source=surrogateLoadObjects; record=vector);

        @signal[loadSurrogate](type=short);
        @statistic[loadSurrogate](title="load"; source=loadSurrogate; record=vector);

        @signal[surrogateObjectRequestIn](type=short);
        @statistic[surrogateObjectRequestIn](title="request in"; source=surrogateObjectRequestIn; record=vector);

        @signal[surrogateObjectResponseOut](type=short);
        @statistic[surrogateObjectResponseOut](title="response out"; source=surrogateObjectResponseOut; record=vector);

        @signal[surrogateClientRequestIn](type=int);
        @statistic[surrogateClientRequestIn](title="load of surrogate"; source=surrogateClientRequestIn; record=vector);

        @signal[surrogateClientResponseOut](type=int);
        @statistic[surrogateClientResponseOut](title="load of surrogate"; source=surrogateClientResponseOut; record=vector);
    gates:
        input socketIn @labels(TcpCommand/up);
        output socketOut @labels(TcpCommand/down);

        input finishIn;

}
