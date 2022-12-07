// Task 1: Wireless Local Area Network – Ad-hoc Mode

// simulare una Wireless Local Area Network (WLAN) che opera in modalità Ad-hoc con 5 nodi. 
// I nodi si muovono seguendo come modello di mobilità il 2D Random Walk in un’area rettangolare 
// definita dal suo angolo in basso a sinistra (coordinate x= -90 m, y= -90 m) 
// e dal suo angolo in alto a destra (x = 90 m, y = 90 m).

// • Canale: canale wireless di default su ns-3 

// • Physical Layer: 
//     o Parametri di default definiti dall’IEEE 802.11G standard; 
//     o Adaptive rate control stabilito dall’algoritmo AARF (default)

// • Link Layer: 
//     o Standard MAC senza nessun controllo sulla Quality of Service; 
//     o Ricorda: la rete opera in ad-hoc mode 

// • Network Layer: 
//     o Standard IPv4 
//     o Address range: 192.168.1.0/24 
//     o Assumere che ogni nodo si comporta come un router ideale e scambia la sua routing table in background 

// • Transport Layer: 
//     o UDP 

// • Application Layer: 
//     o UDP Echo Server sul Nodo 0: 
//          Porta 20 

//     o UDP Echo Client sul Nodo 4 
//          Invia 2 pacchetti UDP Echo al server ai tempi 1s e 2s 

//     o UDP Echo Client sul Nodo 3 
//          Invia 2 pacchetti UDP Echo al server ai tempi 2s e 4s 

//     o Packet size: 512 bytes 

// • Informazioni addizionali: 
//     o Il packet tracer deve essere inserito esclusivamente sul Nodo 2 

//     o NetAnim: se abilitato, la simulazione deve poter generare un file “wireless-task1-rts-<state>.xml” 
//         (dove <state> è “on” se il parametro useRtsCts è vero oppure in caso contrario “off”)
//         e deve abilitare i metadati dei pacchetti ed il tracing dei PHY e MAC counters. 
//         I nodi devono essere marcati nel seguente modo: 
//              Rosso per il nodo con l’UDP Echo Server con descrizione “SRV-<id>“ 
//              Verde per i nodi con gli UDP Echo Clients con descrizione “CLI-<id>” 
//              Blu per gli altri nodi con descrizione “HOC-<id>” 
//              <id> rappresenta il Node ID del singolo nodo (e.g., “1”, “2”, etc.). 

//     o Alla simulazione deve essere possibile passare tre diversi parametri da riga di comando: 
//          useRtsCts: booleano (valore di default: false), se vero viene forzato l’utilizzo 
//             dell’handshake RTS/CTS da parte della rete. 
//          verbose: booleano (valore di default: false), se vero viene abilitato l’uso dei logs 
//             per il server e per i clients della UDP Echo Application 
//          useNetAnim: booleano (valore di default: false), se è vero vengono generati tutti i 
//             file relativi per NetAnim (vedi sopra)

// Infine, per distinguere il vostro compito, definire la seguente riga nel file task1.cc dopo l’inclusione dei 
// moduli e del namespace: 
// • NS_LOG_COMPONENT_DEFINE(“HW2_Task1_Team_<numero_team>”); 