#include <iostream>
#include "Dijkstra.h"

using namespace std;

int main() {

    //TEST DIJKSTRA MAKRO
    // 4 Blok: Depot(0), Blok A(1), Blok B(2), Blok C(3)
    vector<string> NamaBlok = {"Depot", "Blok A", "Blok B", "Blok C"};

    vector<vector<int>> GrafMakro = {
        {0, 2, 5, INF}, // Depot ke Blok A, B, C
        {2, 0, 3, 4},   // Blok A ke Depot, B, C
        {5, 3, 0, 1},   // Blok B ke Depot, A, C
        {INF, 4, 1, 0}  // Blok C ke Depot, A, B
    };

//COntoh hari ini ada pesanan di Blok A dan Blok B
vector<int> BlokTujuan = {1, 2}; //Blok A dan Blok B
DijkstraMakro(GrafMakro, NamaBlok, 0, BlokTujuan); //Mulai dari Depot (0)

    //TEST DIJKSTRA MIKRO
    //Misal di Blok A ada 3 pelanggan
    vector<string> NamaPelanggan = {"Masuk Blok A", "Pelanggan X", "Pelanggan Y", "Pelanggan Z"};

    vector<vector<int>> GrafMikro = {
    // Masuk Blok A, Pelanggan X, Y, Z
        {0, 1, 4, INF}, // Masuk Blok A ke Pelanggan X, Y, Z
        {1, 0, 2, 5},   // Pelanggan X ke Masuk Blok A, Y, Z
        {4, 2, 0, 1},   // Pelanggan Y ke Masuk Blok A, X, Z
        {INF, 5, 1, 0}  // Pelanggan Z ke Masuk Blok A, X, Y
    };

//Semua Pelanggan di Blok A perlu dianter
vector<int> PelangganTujuan = {1, 2, 3}; //Pelanggan X, Y, Z
DijkstraMikro(GrafMikro, NamaPelanggan, 0, PelangganTujuan); //Mulai dari Masuk Blok A (0)

    return 0;
}