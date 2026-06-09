#include <bits/stdc++.h>
#include "Dijkstra.h"

using namespace std;

// Fungsi Utama Dijkstra
// Algoritma ini mencari lintasan terpendek dari satu titik awal (Start) ke seluruh node lainnya di dalam Graf.
// Cara kerja algoritma:
// 1. Inisialisasi jarak semua node dari node awal dengan nilai Tak Hingga (INF).
// 2. Jarak dari node awal ke dirinya sendiri diset = 0.
// 3. Pada setiap langkah, cari node dengan nilai jarak terkecil yang belum pernah dikunjungi.
// 4. Perbarui jarak ke seluruh node tetangganya yang belum dikunjungi jika melewati node terpilih ini menghasilkan jarak yang lebih pendek.
// 5. Ulangi proses ini hingga semua node telah dikunjungi.
vector<int> Dijkstra(
    const vector<vector<int>>& Graf, int Start) {
        int JumlahNode = Graf.size(); // Menyimpan jumlah simpul/node pada graf

        vector<int> Jarak(JumlahNode, INF); // Inisialisasi jarak ke semua node sebagai INF
        vector<int> Prev(JumlahNode, -1); // Array penyimpan node asal/sebelumnya untuk melacak jalur
        vector<bool> Dikunjungi(JumlahNode, false); // Array penanda apakah suatu node sudah diproses

        Jarak[Start] = 0; // Jarak ke node asal diatur ke 0
        for (int Langkah = 0; Langkah < JumlahNode; Langkah++) {

            // Langkah A: Mencari node dengan jarak terkecil yang belum dikunjungi
            int NodeTerdekat = -1;
            for (int i = 0; i < JumlahNode; i++) {
                if (!Dikunjungi[i]) {
                    if (NodeTerdekat == -1 || Jarak[i] < Jarak[NodeTerdekat]) {
                        NodeTerdekat = i;
                    }
                }
            }

            // Jika node terdekat memiliki jarak INF, berarti node-node yang tersisa tidak terhubung
            if (NodeTerdekat == -1 || Jarak[NodeTerdekat] == INF) break;

            Dikunjungi[NodeTerdekat] = true; // Tandai node ini telah selesai dikunjungi

            // Langkah B: Perbarui (rileksasi) jarak ke semua tetangga dari node terdekat
            for (int Tetangga = 0; Tetangga < JumlahNode; Tetangga++) {
                // Periksa apakah ada jalur penghubung langsung (bukan INF) dan tetangga belum dikunjungi
                if (Graf[NodeTerdekat][Tetangga] != INF && !Dikunjungi[Tetangga]) {
                    int JarakBaru = Jarak[NodeTerdekat] + Graf[NodeTerdekat][Tetangga];

                    // Jika jalur baru lebih pendek, update nilai jarak dan catat pendahulunya
                    if (JarakBaru < Jarak[Tetangga]) {
                        Jarak[Tetangga] = JarakBaru;
                        Prev[Tetangga] = NodeTerdekat; 
                    }
                }
            }
    }
    return Jarak;
}

// Fungsi Rekonstruksi Jalur
// Setelah algoritma Dijkstra berjalan, kita mendapatkan informasi jarak terpendek beserta predecessors.
// Fungsi ini berjalan mundur mulai dari node Tujuan hingga kembali ke node Awal menggunakan array Prev,
// lalu membalikkan urutan array tersebut agar kita mendapatkan urutan perjalanan yang benar (Maju).
vector<int> RekonstruksiJalur(
    const vector<int>& Prev, int Tujuan) {
        vector<int> Jalur;
        int Node = Tujuan;

        // Berjalan mundur sampai menemukan node awal (-1)
        while (Node != -1) {
            Jalur.push_back(Node);
            Node = Prev[Node];
        }

        // Jalur didapatkan secara terbalik (Tujuan -> Awal), sehingga perlu dibalik
        reverse(Jalur.begin(), Jalur.end());
        return Jalur;
    }

// Fungsi Dijkstra Makro
// Fungsi ini menjalankan algoritma Dijkstra untuk tingkat Makro (antar blok/komplek) 
// dan mencetak rute perjalanan serta total jaraknya langsung ke konsol.
void DijkstraMakro(
    const vector<vector<int>>& GrafMakro,
    const vector<string>& NamaBlok,
    int StartBlok,
    const vector<int>& BlokTujuan
) {
 int JumlahBlok = GrafMakro.size();

 // Inisialisasi struktur data Dijkstra
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
    if (BlokTerdekat == -1 || Jarak[BlokTerdekat] == INF) break;
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

  // Tampilkan hasil pencarian rute rincian ke layar
  cout << "\n Rute Antar Blok (Makro)" << endl;
  cout << "Titik Awal: " << NamaBlok[StartBlok] << endl;

  for (int Tujuan : BlokTujuan) {
    cout << "Tujuan: " << NamaBlok[Tujuan] << endl;
    if (Jarak[Tujuan] == INF) {
        cout << "Tidak ada rute yang tersedia." << endl;
        continue;
    }

    // Melakukan rekonstruksi urutan perjalanan dari Depot ke Komplek Tujuan
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
    cout << "\n Total Jarak: " << Jarak[Tujuan] << " meter" << endl;
  }
}

// Fungsi Dijkstra Mikro
// Menghitung rute distribusi air di dalam satu komplek perumahan tertentu (antar pelanggan)
// dan mencetak rute mikro serta total jaraknya langsung ke konsol.
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
        if (PelangganTerdekat == -1 || Jarak[PelangganTerdekat] == INF) break;
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

    // Tampilkan rute ke setiap Pelanggan Tujuannya di konsol
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

// Fungsi pembantu internal untuk menjalankan Dijkstra dan mengembalikan hasil jarak beserta silsilah jalur (Prev)
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

// Algoritma Heuristik Nearest-Neighbor TSP untuk tingkat Makro (Antar Komplek)
// Membantu merencanakan urutan kunjungan komplek terdekat secara berurutan:
// Depot -> Komplek Terdekat 1 -> Komplek Terdekat 2 -> ... -> Kembali ke Depot
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
        // Cari jalur terpendek dari simpul aktif ke seluruh node lain menggunakan Dijkstra
        auto detail = DijkstraDetail(GrafMakro, NodeSekarang);
        const vector<int>& Jarak = detail.first;
        const vector<int>& Prev = detail.second;
        
        int IndeksTerdekat = -1;
        int JarakTerdekat = INF;
        
        // Cari dari daftar target kunjungan mana yang memiliki jarak paling dekat dari node sekarang
        for (size_t i = 0; i < BelumDikunjungi.size(); i++) {
            int Target = BelumDikunjungi[i];
            if (Jarak[Target] < JarakTerdekat) {
                JarakTerdekat = Jarak[Target];
                IndeksTerdekat = i;
            }
        }
        
        // Jika sisa target tidak dapat dijangkau dari node sekarang, hentikan pencarian
        if (IndeksTerdekat == -1 || JarakTerdekat == INF) break;
        
        int TargetTerdekat = BelumDikunjungi[IndeksTerdekat];
        
        // Melakukan rekonstruksi rute segmen perjalanan aktif
        vector<int> SegmenJalur;
        int TempNode = TargetTerdekat;
        while (TempNode != -1) {
            SegmenJalur.push_back(TempNode);
            TempNode = Prev[TempNode];
        }
        reverse(SegmenJalur.begin(), SegmenJalur.end());
        
        // Simpan informasi hasil rute ke struktur output
        Hasil.DetailJalur.push_back(SegmenJalur);
        Hasil.TotalJarak += JarakTerdekat;
        Hasil.RuteNode.push_back(TargetTerdekat);
        
        // Berpindah ke node tujuan terdekat tersebut
        NodeSekarang = TargetTerdekat;
        BelumDikunjungi.erase(BelumDikunjungi.begin() + IndeksTerdekat);
    }
    
    // Langkah Akhir: Kembali lagi ke Depot (StartBlok)
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

// Algoritma Heuristik Nearest-Neighbor TSP untuk tingkat Mikro (Antar Pelanggan dalam Komplek)
// Membantu merencanakan urutan pengiriman air galon ke rumah-rumah pelanggan di dalam satu blok perumahan:
// Masuk Komplek -> Pelanggan Terdekat 1 -> Pelanggan Terdekat 2 -> ... -> Kembali ke Pintu Masuk
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
        // Cari jalur terpendek dari simpul aktif ke seluruh pelanggan lain
        auto detail = DijkstraDetail(GrafMikro, NodeSekarang);
        const vector<int>& Jarak = detail.first;
        const vector<int>& Prev = detail.second;
        
        int IndeksTerdekat = -1;
        int JarakTerdekat = INF;
        
        // Cari pelanggan tujuan berikutnya yang terdekat dari posisi kurir saat ini
        for (size_t i = 0; i < BelumDikunjungi.size(); i++) {
            int Target = BelumDikunjungi[i];
            if (Jarak[Target] < JarakTerdekat) {
                JarakTerdekat = Jarak[Target];
                IndeksTerdekat = i;
            }
        }
        
        if (IndeksTerdekat == -1 || JarakTerdekat == INF) break;
        
        int TargetTerdekat = BelumDikunjungi[IndeksTerdekat];
        
        // Rekonstruksi segmen perjalanan mikro
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
    
    // Langkah Akhir: Kurir kembali ke pintu masuk gerbang utama komplek (StartPelanggan)
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