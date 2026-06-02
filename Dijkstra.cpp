#include <bits/stdc++.h>
#include "Dijkstra.h"

using namespace std;

//Fungsi utama Dijkstra
// Cara kerjanya:
// 1.Semua jarak dimulai dari INF (blm diketahui)
// 2.Jarak start ke start = 0
// 3.Setiap langkah: pilih node yang blm dikunjungi dengan jarak terkecil
// 4.Update jarak tetangganya jika lewat node ini lebih dekat
// 5.Ulangi sampai semua node dikunjungi

vector<int> Dijkstra(
    const vector<vector<int>>& Graf, int Start) {
        int JumlahNode = Graf.size(); //Jumlah nodenya

        vector<int> Jarak(JumlahNode, INF); //Inisialisasi jarak
        vector<int> Prev(JumlahNode, -1); //Untuk rekonstruksi jalur
        vector<bool> Dikunjungi(JumlahNode, false); //Tandai node yang sudah dikunjungi atau blm

        Jarak[Start] = 0; //Jarak ke diri sendiri = 0
        for (int Langkah = 0; Langkah < JumlahNode; Langkah++) {

            //Cari node dengan jarak terkecil yang blm dikunjungi
            int NodeTerdekat = -1;
            for (int i = 0; i < JumlahNode; i++) {
                if (!Dikunjungi[i]) {
                    if (NodeTerdekat == -1 || Jarak[i] < Jarak[NodeTerdekat]) {
                        NodeTerdekat = i;
                    }
                }
            }

// Kalau jarak terkecil masih INF, berarti node sisanya
// Tidak terhubung sama sekali, ya berhenti aja
            if (Jarak[NodeTerdekat] == INF) break;

            Dikunjungi[NodeTerdekat] = true; //Tandai node ini sudah dikunjungi

            //Update jarak tetangganya
            for (int Tetangga = 0; Tetangga < JumlahNode; Tetangga++) {
                //Kalau ada jalan dari NodeTerdekat ke Tetangga (bukan INF)
                if (Graf[NodeTerdekat][Tetangga] != INF && !Dikunjungi[Tetangga]) {
                    int JarakBaru = Jarak[NodeTerdekat] + Graf[NodeTerdekat][Tetangga];

                    //Kalau lewat NodeTerdekat lbh dekat, update jaraknya
                    if (JarakBaru < Jarak[Tetangga]) {
                        Jarak[Tetangga] = JarakBaru;
                        Prev[Tetangga] = NodeTerdekat; //Simpan jalur untuk rekonstruksi
                    }
                }
            }
    }
    //Prev disimpan sebagai variabel globall sementara
    // (atau bisa dijadikan parameter output juga kalau mau lebih rapi)
    return Jarak;
    }

//Rekonstruksi jalur: dari mana aja lewatnya
// Setelah Dijkstra jalan, kita tau jarak terpendek
// Tapi kita juga pengen tau rutenya (lewat mana aja)
// Fungsi ini mundur dari tujuan ke start make array Prev yang disimpan selama Dijkstra

vector<int> RekonstruksiJalur(
    const vector<int>& Prev, int Tujuan) {
        vector<int> Jalur;
        int Node = Tujuan;

        while (Node != -1) {
            Jalur.push_back(Node);
            Node = Prev[Node];
        }

        //Jalur Rekonstruksi dari belakang, jadi dibalik dulu
        reverse(Jalur.begin(), Jalur.end());
        return Jalur;
    }

//Dijkstra Makro: antar blok
// Fungsinya mirip Dijkstra biasa, tapi kita juga print nama bloknya
void DijkstraMakro(
    const vector<vector<int>>& GrafMakro,
    const vector<string>& NamaBlok,
    int StartBlok,
    const vector<int>& BlokTujuan
) {
 int JumlahBlok = GrafMakro.size();

 //Jalankan Dijkstra dari depot (start blok)
 vector<int> Jarak(JumlahBlok, INF);
 vector<int> Prev(JumlahBlok, -1);
 vector<bool> Dikunjungi(JumlahBlok, false);

 Jarak[StartBlok] = 0;
 
 for (int Langkah = 0; Langkah < JumlahBlok; Langkah++) {
    int BlokTerdekat = -1;
    for (int i = 0; i < JumlahBlok; i++) {
        if (!Dikunjungi[i] && (BlokTerdekat == -1 || Jarak[i] < Jarak[BlokTerdekat])) {
            BlokTerdekat = i;
        }
    }
        if (Jarak[BlokTerdekat] == INF) break;
        Dikunjungi[BlokTerdekat] = true;

        for (int Tetangga = 0; Tetangga < JumlahBlok; Tetangga++) {
            if (GrafMakro[BlokTerdekat][Tetangga] != INF && !Dikunjungi[Tetangga]) {
                int JarakBaru = Jarak[BlokTerdekat] + GrafMakro[BlokTerdekat][Tetangga];
                if (JarakBaru < Jarak[Tetangga]) {
                    Jarak[Tetangga] = JarakBaru;
                    Prev[Tetangga] = BlokTerdekat;
                }
            }
        }
    }

// Tampilkan rute ke setiap Blok Tujuannya
  cout << "\n Rute Antar Blok (Makro)" << endl;
  cout << "Titik Awal: " << NamaBlok[StartBlok] << endl;

  for (int Tujuan : BlokTujuan) {
    cout << "Tujuan: " << NamaBlok[Tujuan] << endl;
    if (Jarak[Tujuan] == INF) {
        cout << "Tidak ada rute yang tersedia." << endl;
        continue;
    }

   // Rekonstruksi Jalur
   vector<int> Jalur;
   int Node = Tujuan;
   while (Node != -1) {
    Jalur.push_back(Node);
    Node = Prev[Node];
   }
   reverse(Jalur.begin(), Jalur.end());

    cout << "Rute: ";
    for (int i = 0; i < Jalur.size(); i++) {
        cout << NamaBlok[Jalur[i]];
        if (i < Jalur.size() - 1) cout << " -> ";
    }
    cout << "\n Total Jarak: " << Jarak[Tujuan] << " meter" <<endl;
  }
}

// Dijkstra Mikro: antar pelanggan dalam satu blok
void DijkstraMikro(
    const vector<vector<int>>& GrafMikro,
    const vector<string>& NamaPelanggan,
    int StartPelanggan,
    const vector<int>& PelangganTujuan
) {
    int JumlahNode = GrafMikro.size();

    vector<int> Jarak(JumlahNode, INF);
    vector<int> Prev(JumlahNode, -1);
    vector<bool> Dikunjungi(JumlahNode, false);

    Jarak[StartPelanggan] = 0;

    for (int Langkah = 0; Langkah < JumlahNode; Langkah++) {
        int PelangganTerdekat = -1;
        for (int i = 0; i < JumlahNode; i++) {
            if (!Dikunjungi[i] && (PelangganTerdekat == -1 || Jarak[i] < Jarak[PelangganTerdekat])) {
                PelangganTerdekat = i;
            }
        }
            if (Jarak[PelangganTerdekat] == INF) break;
            Dikunjungi[PelangganTerdekat] = true;

            for (int Tetangga = 0; Tetangga < JumlahNode; Tetangga++) {
                if (GrafMikro[PelangganTerdekat][Tetangga] != INF && !Dikunjungi[Tetangga]) {
                    int JarakBaru = Jarak[PelangganTerdekat] + GrafMikro[PelangganTerdekat][Tetangga];
                    if (JarakBaru < Jarak[Tetangga]) {
                        Jarak[Tetangga] = JarakBaru;
                        Prev[Tetangga] = PelangganTerdekat;
                     }
                 }
             }
        }

    // Tampilkan rute ke setiap Pelanggan Tujuannya
    cout << "\n Rute Antar Dalam Blok (Mikro)" << endl;
    cout << "Titik Awal: " << NamaPelanggan[StartPelanggan] << endl;

    for (int Tujuan : PelangganTujuan) {
        cout << "\n Pelanggan Tujuan: " << NamaPelanggan[Tujuan] << endl;
        if (Jarak[Tujuan] == INF) {
            cout << "Tidak ada rute yang tersedia." << endl;
            continue;
        }

        vector<int> Jalur;
        int Node = Tujuan;
        while (Node != -1) {
            Jalur.push_back(Node);
            Node = Prev[Node];
        }
    
    reverse(Jalur.begin(), Jalur.end());
    cout << "Rute: ";
    for (int i = 0; i < Jalur.size(); i++) {
        cout << NamaPelanggan[Jalur[i]];
        if (i < Jalur.size() - 1) cout << " -> ";
    }
    cout << "\n Total Jarak: " << Jarak[Tujuan] << " meter" << endl;
    }
}

// Helper untuk menjalankan Dijkstra dan mengembalikan Jarak serta Prev
pair<vector<int>, vector<int>> DijkstraDetail(const vector<vector<int>>& Graf, int Start) {
    int N = Graf.size();
    vector<int> Jarak(N, INF);
    vector<int> Prev(N, -1);
    vector<bool> Dikunjungi(N, false);
    
    Jarak[Start] = 0;
    
    for (int Langkah = 0; Langkah < N; Langkah++) {
        int NodeTerdekat = -1;
        for (int i = 0; i < N; i++) {
            if (!Dikunjungi[i]) {
                if (NodeTerdekat == -1 || Jarak[i] < Jarak[NodeTerdekat]) {
                    NodeTerdekat = i;
                }
            }
        }
        
        if (NodeTerdekat == -1 || Jarak[NodeTerdekat] == INF) break;
        Dikunjungi[NodeTerdekat] = true;
        
        for (int Tetangga = 0; Tetangga < N; Tetangga++) {
            if (Graf[NodeTerdekat][Tetangga] != INF && !Dikunjungi[Tetangga]) {
                int JarakBaru = Jarak[NodeTerdekat] + Graf[NodeTerdekat][Tetangga];
                if (JarakBaru < Jarak[Tetangga]) {
                    Jarak[Tetangga] = JarakBaru;
                    Prev[Tetangga] = NodeTerdekat;
                }
            }
        }
    }
    return {Jarak, Prev};
}

// Algoritma Nearest-Neighbor TSP untuk Makro
RuteHasil HitungRuteMakro(
    const vector<vector<int>>& GrafMakro,
    int StartBlok,
    const vector<int>& BlokTujuan
) {
    RuteHasil Hasil;
    Hasil.TotalJarak = 0;
    
    if (BlokTujuan.empty()) {
        Hasil.RuteNode.push_back(StartBlok);
        return Hasil;
    }
    
    vector<int> BelumDikunjungi = BlokTujuan;
    int NodeSekarang = StartBlok;
    Hasil.RuteNode.push_back(StartBlok);
    
    while (!BelumDikunjungi.empty()) {
        auto detail = DijkstraDetail(GrafMakro, NodeSekarang);
        const vector<int>& Jarak = detail.first;
        const vector<int>& Prev = detail.second;
        
        int IndeksTerdekat = -1;
        int JarakTerdekat = INF;
        
        for (int i = 0; i < BelumDikunjungi.size(); i++) {
            int Target = BelumDikunjungi[i];
            if (Jarak[Target] < JarakTerdekat) {
                JarakTerdekat = Jarak[Target];
                IndeksTerdekat = i;
            }
        }
        
        // Jika tidak ada target yang bisa dijangkau
        if (IndeksTerdekat == -1 || JarakTerdekat == INF) break;
        
        int TargetTerdekat = BelumDikunjungi[IndeksTerdekat];
        
        // Rekonstruksi Jalur segmen ini
        vector<int> SegmenJalur;
        int TempNode = TargetTerdekat;
        while (TempNode != -1) {
            SegmenJalur.push_back(TempNode);
            TempNode = Prev[TempNode];
        }
        reverse(SegmenJalur.begin(), SegmenJalur.end());
        
        Hasil.DetailJalur.push_back(SegmenJalur);
        Hasil.TotalJarak += JarakTerdekat;
        Hasil.RuteNode.push_back(TargetTerdekat);
        
        NodeSekarang = TargetTerdekat;
        BelumDikunjungi.erase(BelumDikunjungi.begin() + IndeksTerdekat);
    }
    
    // Kembali ke Depot (StartBlok)
    if (NodeSekarang != StartBlok) {
        auto detail = DijkstraDetail(GrafMakro, NodeSekarang);
        const vector<int>& Jarak = detail.first;
        const vector<int>& Prev = detail.second;
        if (Jarak[StartBlok] != INF) {
            vector<int> SegmenJalur;
            int TempNode = StartBlok;
            while (TempNode != -1) {
                SegmenJalur.push_back(TempNode);
                TempNode = Prev[TempNode];
            }
            reverse(SegmenJalur.begin(), SegmenJalur.end());
            
            Hasil.DetailJalur.push_back(SegmenJalur);
            Hasil.TotalJarak += Jarak[StartBlok];
            Hasil.RuteNode.push_back(StartBlok);
        }
    }
    
    return Hasil;
}

// Algoritma Nearest-Neighbor TSP untuk Mikro
RuteHasil HitungRuteMikro(
    const vector<vector<int>>& GrafMikro,
    int StartPelanggan,
    const vector<int>& PelangganTujuan
) {
    RuteHasil Hasil;
    Hasil.TotalJarak = 0;
    
    if (PelangganTujuan.empty()) {
        Hasil.RuteNode.push_back(StartPelanggan);
        return Hasil;
    }
    
    vector<int> BelumDikunjungi = PelangganTujuan;
    int NodeSekarang = StartPelanggan;
    Hasil.RuteNode.push_back(StartPelanggan);
    
    while (!BelumDikunjungi.empty()) {
        auto detail = DijkstraDetail(GrafMikro, NodeSekarang);
        const vector<int>& Jarak = detail.first;
        const vector<int>& Prev = detail.second;
        
        int IndeksTerdekat = -1;
        int JarakTerdekat = INF;
        
        for (int i = 0; i < BelumDikunjungi.size(); i++) {
            int Target = BelumDikunjungi[i];
            if (Jarak[Target] < JarakTerdekat) {
                JarakTerdekat = Jarak[Target];
                IndeksTerdekat = i;
            }
        }
        
        if (IndeksTerdekat == -1 || JarakTerdekat == INF) break;
        
        int TargetTerdekat = BelumDikunjungi[IndeksTerdekat];
        
        vector<int> SegmenJalur;
        int TempNode = TargetTerdekat;
        while (TempNode != -1) {
            SegmenJalur.push_back(TempNode);
            TempNode = Prev[TempNode];
        }
        reverse(SegmenJalur.begin(), SegmenJalur.end());
        
        Hasil.DetailJalur.push_back(SegmenJalur);
        Hasil.TotalJarak += JarakTerdekat;
        Hasil.RuteNode.push_back(TargetTerdekat);
        
        NodeSekarang = TargetTerdekat;
        BelumDikunjungi.erase(BelumDikunjungi.begin() + IndeksTerdekat);
    }
    
    // Kembali ke Masuk Blok (StartPelanggan)
    if (NodeSekarang != StartPelanggan) {
        auto detail = DijkstraDetail(GrafMikro, NodeSekarang);
        const vector<int>& Jarak = detail.first;
        const vector<int>& Prev = detail.second;
        if (Jarak[StartPelanggan] != INF) {
            vector<int> SegmenJalur;
            int TempNode = StartPelanggan;
            while (TempNode != -1) {
                SegmenJalur.push_back(TempNode);
                TempNode = Prev[TempNode];
            }
            reverse(SegmenJalur.begin(), SegmenJalur.end());
            
            Hasil.DetailJalur.push_back(SegmenJalur);
            Hasil.TotalJarak += Jarak[StartPelanggan];
            Hasil.RuteNode.push_back(StartPelanggan);
        }
    }
    
    return Hasil;
}