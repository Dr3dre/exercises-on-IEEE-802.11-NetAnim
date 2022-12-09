#include "ns3/applications-module.h"
#include "ns3/basic-energy-source.h"
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/netanim-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/simple-device-energy-model.h"
#include "ns3/ssid.h"
#include "ns3/wifi-radio-energy-model.h"
#include "ns3/yans-wifi-helper.h"
// #include presi da ns-3-dev-git/src/netanim/examples/wireless-animation.cc

using namespace ns3;

//     definire la seguente riga nel file task2.cc dopo l’inclusione dei moduli e del namespace: 
NS_LOG_COMPONENT_DEFINE("HW2_Task2_Team_34");

// Task 2: Wireless Local Area Network – Infrastucture Mode 

int
main(int argc, char* argv[]) {


    // simulare una Wireless Local Area Network (WLAN) 
    // che opera in modalità Infrastrutturale con 5 nodi ed un Access Point (AP).
    uint32_t nWifi = 5;
    uint32_t nAp = 1;

    //     o Alla simulazione deve essere possibile passare quattro diversi parametri da riga di 
    //     comando: 
    //          useRtsCts: booleano (valore di default: false), 
    //             se vero viene forzato l’utilizzo dell’handshake RTS/CTS da parte della rete. 
    //          verbose: booleano (valore di default: false), 
    //             se vero viene abilitato l’uso dei logs per il server e per i clients della UDP Echo Application 
    //          useNetAnim: booleano (valore di default: false), 
    //             se è vero vengono generati tutti i file relativi per NetAnim 
    //          ssid: stringa (valore di default: “TLC2022”), SSID della rete che viene creata. 
    //             Notare che il valore da utilizzare per la vostra analisi (con e senza RTS/CTS) 
    //             è quello indicato all’inizio del paragrafo del Task 2.
    CommandLine cmd;
    bool useRtsCts = false;
    bool verbose = false;
    bool useNetAnim = false;
    std::string ssid = "TLC2022";

    cmd.AddValue("useRtsCts", "Enable RTS/CTS", useRtsCts);
    cmd.AddValue("verbose", "Enable verbose", verbose);
    cmd.AddValue("useNetAnim", "Enable NetAnim", useNetAnim);
    cmd.AddValue("ssid", "SSID", ssid);
    cmd.Parse(argc, argv);
    
    if(verbose) {
        LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
        LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }

    if(useRtsCts) {
        Config::SetDefault("ns3::WifiRemoteStationManager::RtsCtsThreshold", StringValue("2200"));
    }

    NodeContainer allNodes;
    NodeContainer wifiStaNodes;
    wifiStaNodes.Create(nWifi);
    allNodes.Add(wifiStaNodes);
    NodeContainer wifiApNode;
    wifiApNode.Create(nAp);
    allNodes.Add(wifiApNode);

    // Il nome della rete (SSID) deve essere passato tramite linea di comando usando il parametro “ssid” 
    // e per le vostre simulazioni deve essere la somma delle vostre matricole in stringa.
    
    ssid = ""; //TODO: inserire la somma delle matricole in stringa

    // • Canale: canale wireless di default su ns-3 
    YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
    YansWifiPhyHelper phy = YansWifiPhyHelper::Default();
    phy.SetChannel(channel.Create());

    // • Physical Layer: 
    //     o Parametri di default definiti dall’IEEE 802.11G standard; 
    //     o Adaptive rate control stabilito dall’algoritmo AARF (default) 
    WifiHelper wifi;
    wifi.SetStandard(WIFI_STANDARD_80211g);
    wifi.SetRemoteStationManager("ns3::AarfWifiManager");

    // • Link Layer: 
    //     o Standard MAC senza nessun controllo sulla Quality of Service; 
    //     o Ricorda: la rete opera in infrastructure mode 
    WifiMacHelper mac = WifiMacHelper::Default();
    mac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid), "ActiveProbing", BooleanValue(false));  //CHECK correttezza

    NetDeviceContainer staDevices;
    staDevices = wifi.Install(phy, mac, wifiStaNodes);
    mac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid));

    NetDeviceContainer apDevices;
    apDevices = wifi.Install(phy, mac, wifiApNode);

    // • Network Layer: 
    //     o Standard IPv4 
    //     o Address range: 192.168.1.0/24 
    InternetStackHelper stack;
    stack.Install(allNodes);

    Ipv4AddressHelper address;
    address.SetBase("192.168.1.0", "255.255.255.0");        // CHECK correttezza
    Ipv4InterfaceContainer staInterfaces;
    staInterfaces = address.Assign(staDevices);
    Ipv4InterfaceContainer apInterface;
    apInterface = address.Assign(apDevices);

    //     o Assumere che ogni nodo si comporta come un router ideale e scambia la sua routing table in background 
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    // • Transport Layer: 
    //     o UDP 

    // • Application Layer: 
    //     o UDP Echo Server sul Nodo 0: 
    //          Porta 21 
    NS_LOG_INFO("Create UdpEchoServer on Node 0");
    UdpEchoServerHelper echoServer(21);
    ApplicationContainer serverApps = echoServer.Install(allNodes.Get(0));
    serverApps.Start(Seconds(0.0));
    serverApps.Stop(Seconds(10.0));

    //     o UDP Echo Client sul Nodo 3 
    //          Invia 2 pacchetti UDP Echo al server ai tempi 2s e 4s 
    NS_LOG_INFO("Create UdpEchoClient on Node 3");
    UdpEchoClientHelper echoClient(interfaces.GetAddress(0), 21);
    echoClient.SetAttribute("MaxPackets", UintegerValue(2));
    echoClient.SetAttribute("Interval", TimeValue(Seconds(2.0)));
    echoClient.SetAttribute("PacketSize", UintegerValue(512));      //  o Packet size: 512 bytes
    ApplicationContainer clientApps = echoClient.Install(allNodes.Get(3));
    clientApps.Start(Seconds(2.0));
    clientApps.Stop(Seconds(10.0));     

    //     o UDP Echo Client sul Nodo 4 
    //          Invia 2 pacchetti UDP Echo al server ai tempi 1s e 4s 
    NS_LOG_INFO("Create UdpEchoClient on Node 4");
    UdpEchoClientHelper echoClient2(interfaces.GetAddress(0), 21);
    echoClient2.SetAttribute("MaxPackets", UintegerValue(2));
    echoClient2.SetAttribute("Interval", TimeValue(Seconds(3.0)));
    echoClient2.SetAttribute("PacketSize", UintegerValue(512));      //  o Packet size: 512 bytes
    ApplicationContainer clientApps2 = echoClient2.Install(allNodes.Get(4));
    clientApps2.Start(Seconds(1.0));
    clientApps2.Stop(Seconds(10.0));

    // I nodi si muovono seguendo come modello di mobilità il 2D Random Walk in un’area rettangolare definita
    // dal suo angolo in basso a sinistra (coordinate x= -90 m, y= -90 m)
    // e dal suo angolo in alto a destra (x = 90 m, y = 90 m). 
    MobilityHelper mobility;
    mobility.SetPositionAllocator("ns3::RandomRectanglePositionAllocator",
                                  "X", StringValue("ns3::UniformRandomVariable[Min=-90.0|Max=90.0]"),
                                  "Y", StringValue("ns3::UniformRandomVariable[Min=-90.0|Max=90.0]"));
    mobility.SetMobilityModel("ns3::RandomWalk2dMobilityModel",
        "Bounds", RectangleValue(Rectangle(-90, 90, -90, 90)));
    mobility.Install(allNodes);

    // • Informazioni addizionali: 
    //     o Il packet tracer deve essere inserito esclusivamente sul Nodo 4 (uno dei due clients) e 
    //     sull’AP 
    UdpTraceClientHelper udpTraceClient(staInterfaces.GetAddress(3), 9, "");
    apps = udpTraceClient.Install(wifiStaNodes.Get(3));
    apps.Start(Seconds(1.0));
    apps.Stop(Seconds(10.0));

    UdpTraceClientHelper udpTraceAp(apInterface.GetAddress(0), 9, "");     // CHECK correttezza
    apps = udpTraceAp.Install(wifiApNode.Get(0));
    apps.Start(Seconds(1.0));
    apps.Stop(Seconds(10.0));

    //     o NetAnim: se abilitato, la simulazione deve poter generare un file “wireless-task2-rts-<state>.xml” 
    //     (dove <state> è “on” se il parametro useRtsCts è vero oppure in caso contrario “off”)
    //     e deve abilitare i metadati dei pacchetti ed il tracing dei PHY e MAC counters. 
    AnimationInterface anim("wireless-task2-rts-" + std::string(useRtsCts ? "on" : "off") + ".xml");
    if(useNetAnim)
    {
        anim.EnablePacketMetadata(true);
        anim.EnableWifiPhyCounters(Seconds(0), Seconds(5));
        anim.EnableWifiMacCounters(Seconds(0), Seconds(5));
    
        //     I nodi devono essere marcati nel seguente modo: 
        //          Rosso per il nodo con l’UDP Echo Server con descrizione “SRV-<id>“ 
        //          Verde per i nodi con gli UDP Echo Clients con descrizione “CLI-<id>” 
        //          Blu per gli altri nodi Station con descrizione “STA-<id>” 
        //          Viola scuro (RGB = (66,49,137)) per il nodo di Access Point con descrizione “AP” 
        //          <id> rappresenta il Node ID del singolo nodo.
        anim.UpdateNodeDescription(allNodes.Get(0), "SRV-" + std::to_string(allNodes.Get(0)->GetId()));
        anim.UpdateNodeColor(allNodes.Get(0), 255, 0, 0);
        anim.UpdateNodeDescription(allNodes.Get(3), "CLI-" + std::to_string(allNodes.Get(3)->GetId()));
        anim.UpdateNodeColor(allNodes.Get(3), 0, 255, 0);
        anim.UpdateNodeDescription(allNodes.Get(4), "CLI-" + std::to_string(allNodes.Get(4)->GetId()));
        anim.UpdateNodeColor(allNodes.Get(4), 0, 255, 0);
        anim.UpdateNodeDescription(allNodes.Get(1), "STA-" + std::to_string(allNodes.Get(1)->GetId()));
        anim.UpdateNodeColor(allNodes.Get(1), 0, 0, 255);
        anim.UpdateNodeDescription(allNodes.Get(2), "STA-" + std::to_string(allNodes.Get(2)->GetId()));
        anim.UpdateNodeColor(allNodes.Get(2), 0, 0, 255);
        anim.UpdateNodeDescription(allNodes.Get(5), "AP");
        anim.UpdateNodeColor(allNodes.Get(5), 66, 49, 137);
    }        

    NS_LOG_INFO("Run Simulation.");
    Simulator::Run();
    Simulator::Stop(Seconds(15.0));
    Simulator::Destroy();
    NS_LOG_INFO("Done.");

    return 0;
}