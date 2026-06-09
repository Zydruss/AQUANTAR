#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>
#include <map>

// Struktur data untuk menyimpan informasi Pelanggan
struct PelangganData {
    std::string Nama;     // Nama dari pelanggan
    std::string Komplek;  // Nama komplek/blok tempat pelanggan tinggal
};

// Struktur data untuk menyimpan jarak antar komplek (Graf Makro)
struct JarakData {
    std::string Dari;     // Komplek asal
    std::string Ke;       // Komplek tujuan
    int Jarak;            // Jarak dalam satuan meter
};

// Struktur data untuk menyimpan jarak antar pelanggan di dalam suatu komplek (Graf Mikro)
struct JarakPelangganData {
    std::string Komplek;  // Nama komplek lokasi pelanggan
    std::string Dari;     // Node asal (bisa berupa pintu masuk atau nama pelanggan lain)
    std::string Ke;       // Node tujuan (bisa berupa pintu masuk atau nama pelanggan lain)
    int Jarak;            // Jarak dalam satuan meter
};

// Kelas Database untuk mengelola penyimpanan data komplek, pelanggan, dan relasi jarak
class Database {
public:
    std::vector<std::string> KomplekList;               // Daftar nama semua komplek yang terdaftar
    std::vector<PelangganData> PelangganList;           // Daftar data pelanggan
    std::vector<JarakData> JarakKomplekList;            // Daftar relasi jarak antar komplek (Makro)
    std::vector<JarakPelangganData> JarakPelangganList;  // Daftar relasi jarak antar pelanggan (Mikro)

    // Konstruktor kelas Database
    Database();
    
    // Menginisialisasi file CSV dengan data bawaan (default) jika belum ada di sistem
    void InisialisasiDefault();
    
    // Memuat (load) semua data dari berkas-berkas CSV ke dalam memori
    void LoadSemua();

    // Menyimpan (save) seluruh data dari memori kembali ke berkas-berkas CSV
    void SaveSemua();
    
    // Membuat matriks graf makro (jarak antar komplek) untuk algoritma Dijkstra
    std::vector<std::vector<int>> AmbilGrafMakro();
    
    // Membuat matriks graf mikro (jarak antar pelanggan) untuk komplek tertentu
    // outNamaNodes digunakan untuk menyimpan daftar nama node (Index 0 = Pintu Masuk Komplek, 1..N = Nama Pelanggan)
    std::vector<std::vector<int>> AmbilGrafMikro(const std::string& namaKomplek, std::vector<std::string>& outNamaNodes);
    
    // Fungsi CRUD untuk mengelola data Pelanggan
    // Mengembalikan string kosong jika berhasil, atau pesan error jika gagal
    std::string TambahPelanggan(const std::string& nama, const std::string& komplek, int jarakKeMasuk, const std::map<std::string, int>& jarakKePelangganLain);
    std::string EditPelanggan(const std::string& namaLama, const std::string& namaBaru, const std::string& komplek, int jarakKeMasuk, const std::map<std::string, int>& jarakKePelangganLain);
    std::string HapusPelanggan(const std::string& nama, const std::string& komplek);

    // Fungsi CRUD untuk mengelola data Komplek
    // Mengembalikan string kosong jika berhasil, atau pesan error jika gagal
    std::string TambahKomplek(const std::string& nama, const std::map<std::string, int>& jarakKeKomplekLain);
    std::string EditKomplek(const std::string& namaLama, const std::string& namaBaru, const std::map<std::string, int>& jarakKeKomplekLain);
    std::string HapusKomplek(const std::string& nama);

private:
    // Fungsi internal untuk memuat data dari berkas CSV masing-masing
    void LoadKomplek();
    void LoadJarakKomplek();
    void LoadPelanggan();
    void LoadJarakPelanggan();
    
    // Fungsi internal untuk menyimpan data ke berkas CSV masing-masing
    void SaveKomplek();
    void SaveJarakKomplek();
    void SavePelanggan();
    void SaveJarakPelanggan();
};

#endif
