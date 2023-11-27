import inet.applications.tcpapp.TcpBasicClientApp;
import inet.applications.contract.IApp;

package cdnsimulator;

simple ClientApp like IApp
{
    parameters:
        string locationOfClientToSurrogateFolder=default("/Users/alexanderlupatsiy/Documents/Uni/Semester_6/Bachelor_Arbeit/Code/CDNsimulator/src/clientToSurrogate.txt");
        string locationOfTrafficFolder = default("/Users/alexanderlupatsiy/Documents/Uni/Semester_6/Bachelor_Arbeit/Code/CDNsimulator/src/requests.txt");
        
        @display("i=block/app");
        @lifecycleSupport;
        @signal[packetSent](type=inet::Packet);
        @signal[packetReceived](type=inet::Packet);
        @signal[connect](type=long);  // 1 for open, -1 for close
        @statistic[packetReceived](title="packets received"; source=packetReceived; record=count,"sum(packetBytes)","vector(packetBytes)"; interpolationmode=none);
        @statistic[packetSent](title="packets sent"; source=packetSent; record=count,"sum(packetBytes)","vector(packetBytes)"; interpolationmode=none);
        @statistic[endToEndDelay](title="end-to-end delay"; source="dataAge(packetReceived)"; unit=s; record=histogram,weightedHistogram,vector; interpolationmode=none);
        @statistic[numActiveSessions](title="number of active sessions"; source=warmup(sum(connect)); record=max,timeavg,vector; interpolationmode=sample-hold; autoWarmupFilter=false);
        @statistic[numSessions](title="total number of sessions"; source="sum(connect+1)/2"; record=last);
        
    gates:
        input socketIn @labels(TcpCommand/up);
        output socketOut @labels(TcpCommand/down);
}
