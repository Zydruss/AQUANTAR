#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include <vector>
#include <string>

const int INF = 9999999; //konstanta buat jadi "tidak ada jalan"

//Fungsi utama Dijkstra
// graf : matriks jarak antar node
// start : titik awal (index)
// kembalikan : vector jarak terpendek dari start ke semua node
std::vector<int> Dijkstra(
    const std::vector<std::vector<int>>& Graf,
    int start
);

//Rekonstruksi jalur: dari mana aja lewatnya
std::vector<int> RekonstruksiJalur(
    const std::vector<int>& Prev,
    int tujuan
);

//Dijkstra Makro: antar blok
// Kembalikan urutan blok yang harus dikunjungi oleh kurir
void DijkstraMakro(
    const std::vector<std::vector<int>>& GrafMakro,
    const std::vector<std::string>& NamaBlok,
    int StartBlok,
    const std::vector<int>& BlokTujuan
);

//Dijkstra Mikro: antar pelanggan dalam satu blok
void DijkstraMikro(
    const std::vector<std::vector<int>>& GrafMikro,
    const std::vector<std::string>& NamaPelanggan,
    int StartPelanggan,
    const std::vector<int>& PelangganTujuan
);

#endif