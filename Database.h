#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>
#include <map>

// Data pelanggan
struct PelangganData {
    std::string Nama;
    std::string Komplek;
};

// Jarak antar komplek (makro)
struct JarakData {
    std::string Dari;
    std::string Ke;
    int Jarak;
};

// Jarak antar pelanggan / masuk komplek (mikro)
struct JarakPelangganData {
    std::string Komplek;
    std::string Dari;
    std::string Ke;
    int Jarak;
};

class Database {
public:
    std::vector<std::string> KomplekList;
    std::vector<PelangganData> PelangganList;
    std::vector<JarakData> JarakKomplekList;
    std::vector<JarakPelangganData> JarakPelangganList;

    Database();
    
    // Inisialisasi file CSV dengan data default jika tidak ditemukan
    void InisialisasiDefault();
    
    // Load & Save data dari/ke file CSV
    void LoadSemua();
    void SaveSemua();
    
    // Mengambil matriks graf makro untuk dijalankan Dijkstra
    std::vector<std::vector<int>> AmbilGrafMakro();
    
    // Mengambil matriks graf mikro untuk komplek tertentu
    // outNamaNodes akan diisi dengan urutan nama node (Index 0 = "Masuk [Komplek]", 1..N = Nama Pelanggan)
    std::vector<std::vector<int>> AmbilGrafMikro(const std::string& namaKomplek, std::vector<std::string>& outNamaNodes);
    
    // CRUD Pelanggan
    bool TambahPelanggan(const std::string& nama, const std::string& komplek, int jarakKeMasuk, const std::map<std::string, int>& jarakKePelangganLain);
    bool EditPelanggan(const std::string& namaLama, const std::string& namaBaru, const std::string& komplek, int jarakKeMasuk, const std::map<std::string, int>& jarakKePelangganLain);
    bool HapusPelanggan(const std::string& nama, const std::string& komplek);

private:
    void LoadKomplek();
    void LoadJarakKomplek();
    void LoadPelanggan();
    void LoadJarakPelanggan();
    
    void SaveKomplek();
    void SaveJarakKomplek();
    void SavePelanggan();
    void SaveJarakPelanggan();
};

#endif
