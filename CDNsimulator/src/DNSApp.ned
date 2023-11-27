package cdnsimulator;
import inet.applications.contract.IApp;

simple DNSApp like IApp
{
    parameters:
        string pathToSurrogateDistances = default("");
        
        @display("i=block/app");
        @lifecycleSupport;
        @signal[packetSent](type=inet::Packet);
        @signal[packetReceived](type=inet::Packet);
        @statistic[packetReceived](title="packets received"; source=packetReceived; record=count,"sum(packetBytes)","vector(packetBytes)"; interpolationmode=none);
        @statistic[packetSent](title="packets sent"; source=packetSent; record=count,"sum(packetBytes)","vector(packetBytes)"; interpolationmode=none);
        @statistic[endToEndDelay](title="end-to-end delay"; source="dataAge(packetReceived)"; unit=s; record=histogram,weightedHistogram,vector; interpolationmode=none);
    gates:
        input socketIn @labels(TcpCommand/up);
        output socketOut @labels(TcpCommand/down);
}