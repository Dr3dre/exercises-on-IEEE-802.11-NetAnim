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

// per distinguere il vostro compito, definire la seguente riga nel file task1.cc dopo l’inclusione dei 
// moduli e del namespace: 
NS_LOG_COMPONENT_DEFINE("HW2_Task1_Team_34"); 


// Task 1: Wireless Local Area Network – Ad-hoc Mode

int
main(int argc, char* argv[]) {

    // simulare una Wireless Local Area Network (WLAN) che opera in modalità Ad-hoc con 5 nodi. 
    uint32_t nWifi = 5;
    bool useRtsCts = true;
    bool verbose = true;
    bool useNetAnim = true;
    
    // o Alla simulazione deve essere possibile passare tre diversi parametri da riga di comando: 
    //          useRtsCts: booleano (valore di default: false), se vero viene forzato l’utilizzo 
    //             dell’handshake RTS/CTS da parte della rete. 
    //          verbose: booleano (valore di default: false), se vero viene abilitato l’uso dei logs 
    //             per il server e per i clients della UDP Echo Application 
    //          useNetAnim: booleano (valore di default: false), se è vero vengono generati tutti i 
    //             file relativi per NetAnim (vedi sopra)
    CommandLine cmd;
    cmd.AddValue("nWifi", "Number of wifi STA devices", nWifi);     // default: 5
    cmd.AddValue("useRtsCts", "Enable RTS/CTS", useRtsCts);
    cmd.AddValue("verbose", "Enable verbose logs", verbose);
    cmd.AddValue("useNetAnim", "Enable NetAnim", useNetAnim);
    cmd.Parse(argc, argv);

    if (verbose) {
        LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
        LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }
    
    UintegerValue rtsctsThreshold = UintegerValue(5000);
    if (useRtsCts) {
        rtsctsThreshold=UintegerValue(500);
    }
                
    Config::SetDefault("ns3::WifiRemoteStationManager::RtsCtsThreshold", rtsctsThreshold);

   
    NodeContainer allNodes;
    allNodes.Create(nWifi);
    
    // • Canale: canale wireless di default su ns-3 (YansWifiChannel ?)
    YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
    YansWifiPhyHelper phy ;
    phy.SetChannel(channel.Create());

    // • Physical Layer: 
    //     o Parametri di default definiti dall’IEEE 802.11G standard; 
    //     o Adaptive rate control stabilito dall’algoritmo AARF (default)
    WifiHelper wifi;
    wifi.SetStandard(WIFI_STANDARD_80211g);
    wifi.SetRemoteStationManager("ns3::AarfWifiManager");

    // • Link Layer: 
    //     o Standard MAC senza nessun controllo sulla Quality of Service; 
    //     o Ricorda: la rete opera in ad-hoc mode 
    WifiMacHelper mac;
    mac.SetType("ns3::AdhocWifiMac");

    // • Network Layer: 
    //     o Standard IPv4 
    //     o Address range: 192.168.1.0/24 
    InternetStackHelper stack;
    stack.Install(allNodes);

    NetDeviceContainer devices = wifi.Install(phy, mac, allNodes);

    Ipv4AddressHelper address;
    address.SetBase("192.168.1.0", "255.255.255.0");        //CHECK correttezza
    Ipv4InterfaceContainer interfaces = address.Assign(devices);

    //     o Assumere che ogni nodo si comporta come un router ideale e scambia la sua routing table in background
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    // • Transport Layer: 
    //     o UDP 
    // • Application Layer: 
    //     o UDP Echo Server sul Nodo 0: 
    //          Porta 20 
    NS_LOG_INFO("Create UdpEchoServer application on node 0.");
    uint16_t port = 20;
    UdpEchoServerHelper server(port);
    ApplicationContainer serverapp = server.Install(allNodes.Get(0));
    serverapp.Start(Seconds(1.0));
    serverapp.Stop(Seconds(10.0));

    //     o UDP Echo Client sul Nodo 4 
    //          Invia 2 pacchetti UDP Echo al server ai tempi 1s e 2s 
    NS_LOG_INFO("Create UdpEchoClient application on node 4.");
    UdpEchoClientHelper client1(interfaces.GetAddress(0), port);
    client1.SetAttribute("MaxPackets", UintegerValue(2));
    client1.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    client1.SetAttribute("PacketSize", UintegerValue(512));      // o Packet size: 512 bytes 
    ApplicationContainer client1app = client1.Install(allNodes.Get(4));
    client1app.Start(Seconds(1.0));
    client1app.Stop(Seconds(10.0));        //CHECK correttezza

    //     o UDP Echo Client sul Nodo 3 
    //          Invia 2 pacchetti UDP Echo al server ai tempi 2s e 4s 
    NS_LOG_INFO("Create UdpEchoClient application on node 3.");
    UdpEchoClientHelper client2(interfaces.GetAddress(0), port);
    client2.SetAttribute("MaxPackets", UintegerValue(2));
    client2.SetAttribute("Interval", TimeValue(Seconds(2.0)));
    client2.SetAttribute("PacketSize", UintegerValue(512)); 
    ApplicationContainer client2app = client2.Install(allNodes.Get(3));
    client2app.Start(Seconds(2.0));
    client2app.Stop(Seconds(4.1));

    // I nodi si muovono seguendo come modello di mobilità il 2D Random Walk in un’area rettangolare 
    // definita dal suo angolo in basso a sinistra (coordinate x= -90 m, y= -90 m) 
    // e dal suo angolo in alto a destra (x = 90 m, y = 90 m).
    MobilityHelper mobility;
    // prima configurazione:
    /*mobility.SetPositionAllocator("ns3::RandomRectanglePositionAllocator",
        "X", StringValue("ns3::UniformRandomVariable[Min=-90.0|Max=90.0]"),
        "Y", StringValue("ns3::UniformRandomVariable[Min=-90.0|Max=90.0]"));
    mobility.SetMobilityModel("ns3::RandomWalk2dMobilityModel",
        "Bounds", RectangleValue(Rectangle(-90, 90, -90, 90)));
    mobility.Install(allNodes);*/

    // muova configurazione, da wireless-animation.cc
    mobility.SetPositionAllocator("ns3::GridPositionAllocator",
                                  "MinX",
                                  DoubleValue(10.0),
                                  "MinY",
                                  DoubleValue(10.0),
                                  "DeltaX",
                                  DoubleValue(5.0),
                                  "DeltaY",
                                  DoubleValue(2.0),
                                  "GridWidth",
                                  UintegerValue(5),
                                  "LayoutType",
                                  StringValue("RowFirst"));
    mobility.SetMobilityModel("ns3::RandomWalk2dMobilityModel",
                              "Bounds",
                              RectangleValue(Rectangle(-90, 90, -90, 90)));
    mobility.Install(allNodes);

    // • Informazioni addizionali: 
    //     o Il packet tracer deve essere inserito esclusivamente sul Nodo 2 
    // per ora ci teniamo tutte le catture
    phy.EnablePcapAll("adhoc",true);

    //AnimationInterface anim;
    AnimationInterface anim("wireless-task1-rts-" + std::string(useRtsCts ? "on" : "off") + ".xml");
    if (useNetAnim)
    {    //     o NetAnim: se abilitato, la simulazione deve poter generare un file “wireless-task1-rts-<state>.xml” 
        //         (dove <state> è “on” se il parametro useRtsCts è vero oppure in caso contrario “off”)
        //         e deve abilitare i metadati dei pacchetti ed il tracing dei PHY e MAC counters. 
        anim.EnablePacketMetadata(true);
        anim.EnableWifiPhyCounters(Seconds(0), Seconds(10));
        anim.EnableWifiMacCounters(Seconds(0), Seconds(10));

        //         I nodi devono essere marcati nel seguente modo: 
        //              Rosso per il nodo con l’UDP Echo Server con descrizione “SRV-<id>“ 
        //              Verde per i nodi con gli UDP Echo Clients con descrizione “CLI-<id>” 
        //              Blu per gli altri nodi con descrizione “HOC-<id>” 
        //              <id> rappresenta il Node ID del singolo nodo (e.g., “1”, “2”, etc.). 
        for (uint32_t i = 0; i < allNodes.GetN(); i++) {
            Ptr<Node> node = allNodes.Get(i);
            printf("%d\n",node->GetId());
            if (i == 0) {
                anim.UpdateNodeDescription(node, "SRV-" + std::to_string(node->GetId()));
                anim.UpdateNodeColor(node, 255, 0, 0);
            }
            else if (i == 3 || i == 4) {
                anim.UpdateNodeDescription(node, "CLI-" + std::to_string(node->GetId()));
                anim.UpdateNodeColor(node, 0, 255, 0);
            }
            else {
                anim.UpdateNodeDescription(node, "HOC-" + std::to_string(node->GetId()));
                anim.UpdateNodeColor(node, 0, 0, 255);
            }
        }
    }

    NS_LOG_INFO("Run Simulation.");
    Simulator::Stop(Seconds(15.0));
    Simulator::Run();
    Simulator::Destroy();
    NS_LOG_INFO("Done.");

    return 0;
}
