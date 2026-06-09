#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include <vector>
#include <string>

// Konstanta penanda nilai tak hingga (infinity) untuk merepresentasikan tidak adanya jalur penghubung
const int INF = 9999999;

// Fungsi utama algoritma Dijkstra
// Graf: Matriks ketetanggaan yang berisi jarak antar simpul/node
// Start: Indeks dari node awal pencarian
// Mengembalikan: Vector berisi daftar jarak terpendek dari node awal ke semua node lainnya
std::vector<int> Dijkstra(
    const std::vector<std::vector<int>>& Graf,
    int Start
);

// Fungsi untuk merekonstruksi jalur rute perjalanan dari node awal ke node tujuan
// Prev: Vector pendahulu (predecessor) hasil dari perhitungan Dijkstra
// Tujuan: Indeks dari node tujuan
// Mengembalikan: Vector berisi urutan indeks node yang dilewati dari awal hingga tujuan
std::vector<int> RekonstruksiJalur(
    const std::vector<int>& Prev,
    int Tujuan
);

// Fungsi pencarian rute Dijkstra tingkat Makro (menghitung rute antar blok/komplek)
// Menampilkan hasil rute makro langsung ke konsol/output standar
void DijkstraMakro(
    const std::vector<std::vector<int>>& GrafMakro,
    const std::vector<std::string>& NamaBlok,
    int StartBlok,
    const std::vector<int>& BlokTujuan
);

// Fungsi pencarian rute Dijkstra tingkat Mikro (menghitung rute antar pelanggan di dalam satu blok)
// Menampilkan hasil rute mikro langsung ke konsol/output standar
void DijkstraMikro(
    const std::vector<std::vector<int>>& GrafMikro,
    const std::vector<std::string>& NamaPelanggan,
    int StartPelanggan,
    const std::vector<int>& PelangganTujuan
);

// Struktur data hasil perhitungan rute gabungan (TSP Nearest-Neighbor)
struct RuteHasil {
    std::vector<int> RuteNode;                  // Urutan simpul/node penting (tujuan) yang wajib dikunjungi
    int TotalJarak;                             // Total akumulasi jarak tempuh perjalanan dalam satuan meter
    std::vector<std::vector<int>> DetailJalur;  // Detail rute per segmen perjalanan berupa jalur node lengkap
};

// Fungsi Heuristik Nearest-Neighbor TSP Makro untuk mencari rute perjalanan antar blok terpendek
// Fungsi ini menjamin kurir mengunjungi seluruh blok tujuan secara efisien dan kembali ke Depot
RuteHasil HitungRuteMakro(
    const std::vector<std::vector<int>>& GrafMakro,
    int StartBlok,
    const std::vector<int>& BlokTujuan
);

// Fungsi Heuristik Nearest-Neighbor TSP Mikro untuk mencari rute perjalanan antar pelanggan terpendek
// Fungsi ini menjamin kurir mengunjungi seluruh pelanggan target di dalam suatu blok dan kembali ke pintu masuk
RuteHasil HitungRuteMikro(
    const std::vector<std::vector<int>>& GrafMikro,
    int StartPelanggan,
    const std::vector<int>& PelangganTujuan
);

#endif