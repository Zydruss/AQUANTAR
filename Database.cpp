#include "Database.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>

using namespace std;

// Helper untuk memisahkan string berdasarkan koma (CSV)
vector<string> SplitCSV(const string& line) {
    vector<string> result;
    string cell;
    stringstream lineStream(line);
    while (getline(lineStream, cell, ',')) {
        // Hapus karakter whitespace dan carriage return (\r)
        while (!cell.empty() && (cell.back() == '\r' || cell.back() == '\n' || cell.back() == ' ')) {
            cell.pop_back();
        }
        while (!cell.empty() && cell.front() == ' ') {
            cell.erase(cell.begin());
        }
        result.push_back(cell);
    }
    return result;
}

// Helper stoi yang aman
static int SafeStoi(const string& s, int defVal = 9999999) {
    try {
        if (s.empty()) return defVal;
        return stoi(s);
    } catch (...) {
        return defVal;
    }
}

Database::Database() {
    InisialisasiDefault();
    LoadSemua();
}

void Database::InisialisasiDefault() {
    // Cek apakah file database sudah ada
    ifstream checkFile("komplek.csv");
    if (checkFile.good()) {
        checkFile.close();
        return; // File sudah ada, tidak perlu overwrite
    }
    checkFile.close();
    
    // Tulis komplek.csv
    ofstream fKomplek("komplek.csv");
    fKomplek << "NamaKomplek\n";
    fKomplek << "Depot\n";
    fKomplek << "Blok A\n";
    fKomplek << "Blok B\n";
    fKomplek << "Blok C\n";
    fKomplek.close();

    // Tulis komplek_jarak.csv (Makro)
    ofstream fKjarak("komplek_jarak.csv");
    fKjarak << "Dari,Ke,Jarak\n";
    fKjarak << "Depot,Blok A,2\n";
    fKjarak << "Depot,Blok B,5\n";
    fKjarak << "Blok A,Blok B,3\n";
    fKjarak << "Blok A,Blok C,4\n";
    fKjarak << "Blok B,Blok C,1\n";
    fKjarak.close();

    // Tulis pelanggan.csv
    ofstream fPelanggan("pelanggan.csv");
    fPelanggan << "NamaPelanggan,NamaKomplek\n";
    fPelanggan << "Pelanggan X,Blok A\n";
    fPelanggan << "Pelanggan Y,Blok A\n";
    fPelanggan << "Pelanggan Z,Blok A\n";
    fPelanggan << "Pelanggan P,Blok B\n";
    fPelanggan << "Pelanggan Q,Blok B\n";
    fPelanggan << "Pelanggan R,Blok C\n";
    fPelanggan.close();

    // Tulis pelanggan_jarak.csv (Mikro)
    ofstream fPjarak("pelanggan_jarak.csv");
    fPjarak << "NamaKomplek,Dari,Ke,Jarak\n";
    // Jarak di Blok A
    fPjarak << "Blok A,Masuk Blok A,Pelanggan X,1\n";
    fPjarak << "Blok A,Masuk Blok A,Pelanggan Y,4\n";
    fPjarak << "Blok A,Masuk Blok A,Pelanggan Z,9999999\n";
    fPjarak << "Blok A,Pelanggan X,Pelanggan Y,2\n";
    fPjarak << "Blok A,Pelanggan X,Pelanggan Z,5\n";
    fPjarak << "Blok A,Pelanggan Y,Pelanggan Z,1\n";
    // Jarak di Blok B
    fPjarak << "Blok B,Masuk Blok B,Pelanggan P,2\n";
    fPjarak << "Blok B,Masuk Blok B,Pelanggan Q,6\n";
    fPjarak << "Blok B,Pelanggan P,Pelanggan Q,3\n";
    // Jarak di Blok C
    fPjarak << "Blok C,Masuk Blok C,Pelanggan R,3\n";
    fPjarak.close();
}

void Database::LoadSemua() {
    LoadKomplek();
    LoadJarakKomplek();
    LoadPelanggan();
    LoadJarakPelanggan();
}

void Database::SaveSemua() {
    SaveKomplek();
    SaveJarakKomplek();
    SavePelanggan();
    SaveJarakPelanggan();
}

void Database::LoadKomplek() {
    KomplekList.clear();
    ifstream file("komplek.csv");
    if (!file.is_open()) return;
    string line;
    getline(file, line); // Skip header
    while (getline(file, line)) {
        if (line.empty()) continue;
        while (!line.empty() && (line.back() == '\r' || line.back() == '\n')) {
            line.pop_back();
        }
        if (!line.empty()) {
            KomplekList.push_back(line);
        }
    }
    file.close();
}

void Database::LoadJarakKomplek() {
    JarakKomplekList.clear();
    ifstream file("komplek_jarak.csv");
    if (!file.is_open()) return;
    string line;
    getline(file, line); // Skip header
    while (getline(file, line)) {
        if (line.empty()) continue;
        auto parts = SplitCSV(line);
        if (parts.size() >= 3) {
            JarakData jd;
            jd.Dari = parts[0];
            jd.Ke = parts[1];
            jd.Jarak = SafeStoi(parts[2]);
            JarakKomplekList.push_back(jd);
        }
    }
    file.close();
}

void Database::LoadPelanggan() {
    PelangganList.clear();
    ifstream file("pelanggan.csv");
    if (!file.is_open()) return;
    string line;
    getline(file, line); // Skip header
    while (getline(file, line)) {
        if (line.empty()) continue;
        auto parts = SplitCSV(line);
        if (parts.size() >= 2) {
            PelangganData pd;
            pd.Nama = parts[0];
            pd.Komplek = parts[1];
            PelangganList.push_back(pd);
        }
    }
    file.close();
}

void Database::LoadJarakPelanggan() {
    JarakPelangganList.clear();
    ifstream file("pelanggan_jarak.csv");
    if (!file.is_open()) return;
    string line;
    getline(file, line); // Skip header
    while (getline(file, line)) {
        if (line.empty()) continue;
        auto parts = SplitCSV(line);
        if (parts.size() >= 4) {
            JarakPelangganData jpd;
            jpd.Komplek = parts[0];
            jpd.Dari = parts[1];
            jpd.Ke = parts[2];
            jpd.Jarak = SafeStoi(parts[3]);
            JarakPelangganList.push_back(jpd);
        }
    }
    file.close();
}

void Database::SaveKomplek() {
    ofstream file("komplek.csv");
    file << "NamaKomplek\n";
    for (const auto& k : KomplekList) {
        file << k << "\n";
    }
    file.close();
}

void Database::SaveJarakKomplek() {
    ofstream file("komplek_jarak.csv");
    file << "Dari,Ke,Jarak\n";
    for (const auto& jk : JarakKomplekList) {
        file << jk.Dari << "," << jk.Ke << "," << jk.Jarak << "\n";
    }
    file.close();
}

void Database::SavePelanggan() {
    ofstream file("pelanggan.csv");
    file << "NamaPelanggan,NamaKomplek\n";
    for (const auto& p : PelangganList) {
        file << p.Nama << "," << p.Komplek << "\n";
    }
    file.close();
}

void Database::SaveJarakPelanggan() {
    ofstream file("pelanggan_jarak.csv");
    file << "NamaKomplek,Dari,Ke,Jarak\n";
    for (const auto& jp : JarakPelangganList) {
        file << jp.Komplek << "," << jp.Dari << "," << jp.Ke << "," << jp.Jarak << "\n";
    }
    file.close();
}

vector<vector<int>> Database::AmbilGrafMakro() {
    int N = KomplekList.size();
    vector<vector<int>> Graf(N, vector<int>(N, 9999999));
    for (int i = 0; i < N; i++) {
        Graf[i][i] = 0;
    }
    
    auto AmbilIndex = [&](const string& nama) {
        for (int i = 0; i < N; i++) {
            if (KomplekList[i] == nama) return i;
        }
        return -1;
    };
    
    for (const auto& jk : JarakKomplekList) {
        int u = AmbilIndex(jk.Dari);
        int v = AmbilIndex(jk.Ke);
        if (u != -1 && v != -1) {
            Graf[u][v] = jk.Jarak;
            Graf[v][u] = jk.Jarak;
        }
    }
    
    return Graf;
}

vector<vector<int>> Database::AmbilGrafMikro(const string& namaKomplek, vector<string>& outNamaNodes) {
    outNamaNodes.clear();
    outNamaNodes.push_back("Masuk " + namaKomplek);
    
    for (const auto& p : PelangganList) {
        if (p.Komplek == namaKomplek) {
            outNamaNodes.push_back(p.Nama);
        }
    }
    
    int M = outNamaNodes.size();
    vector<vector<int>> Graf(M, vector<int>(M, 9999999));
    for (int i = 0; i < M; i++) {
        Graf[i][i] = 0;
    }
    
    auto AmbilIndex = [&](const string& nama) {
        for (int i = 0; i < M; i++) {
            if (outNamaNodes[i] == nama) return i;
        }
        return -1;
    };
    
    for (const auto& jp : JarakPelangganList) {
        if (jp.Komplek == namaKomplek) {
            int u = AmbilIndex(jp.Dari);
            int v = AmbilIndex(jp.Ke);
            if (u != -1 && v != -1) {
                Graf[u][v] = jp.Jarak;
                Graf[v][u] = jp.Jarak;
            }
        }
    }
    
    return Graf;
}

bool Database::TambahPelanggan(const string& nama, const string& komplek, int jarakKeMasuk, const map<string, int>& jarakKePelangganLain) {
    // Validasi nama tidak boleh kosong atau duplikat
    if (nama.empty()) return false;
    for (const auto& p : PelangganList) {
        if (p.Nama == nama) return false;
    }
    
    // Tambah ke list pelanggan
    PelangganData pd;
    pd.Nama = nama;
    pd.Komplek = komplek;
    PelangganList.push_back(pd);
    
    // Tambah jarak ke pintu masuk komplek
    JarakPelangganData jpd;
    jpd.Komplek = komplek;
    jpd.Dari = "Masuk " + komplek;
    jpd.Ke = nama;
    jpd.Jarak = jarakKeMasuk;
    JarakPelangganList.push_back(jpd);
    
    // Tambah jarak ke pelanggan lain di komplek yang sama
    for (const auto& pair : jarakKePelangganLain) {
        JarakPelangganData jp_lain;
        jp_lain.Komplek = komplek;
        jp_lain.Dari = pair.first;
        jp_lain.Ke = nama;
        jp_lain.Jarak = pair.second;
        JarakPelangganList.push_back(jp_lain);
    }
    
    SaveSemua();
    return true;
}

bool Database::EditPelanggan(const string& namaLama, const string& namaBaru, const string& komplek, int jarakKeMasuk, const map<string, int>& jarakKePelangganLain) {
    if (namaBaru.empty()) return false;
    
    // Jika nama berubah, pastikan tidak duplikat dengan pelanggan lain
    if (namaLama != namaBaru) {
        for (const auto& p : PelangganList) {
            if (p.Nama == namaBaru) return false;
        }
    }
    
    // 1. Update nama di PelangganList
    bool found = false;
    for (auto& p : PelangganList) {
        if (p.Nama == namaLama && p.Komplek == komplek) {
            p.Nama = namaBaru;
            found = true;
            break;
        }
    }
    if (!found) return false;
    
    // 2. Hapus semua data jarak lama yang melibatkan namaLama di komplek ini
    vector<JarakPelangganData> tempJarak;
    for (const auto& jp : JarakPelangganList) {
        if (jp.Komplek == komplek && (jp.Dari == namaLama || jp.Ke == namaLama)) {
            continue;
        }
        tempJarak.push_back(jp);
    }
    JarakPelangganList = tempJarak;
    
    // 3. Tambah kembali jarak yang baru dengan namaBaru
    JarakPelangganData jpd;
    jpd.Komplek = komplek;
    jpd.Dari = "Masuk " + komplek;
    jpd.Ke = namaBaru;
    jpd.Jarak = jarakKeMasuk;
    JarakPelangganList.push_back(jpd);
    
    for (const auto& pair : jarakKePelangganLain) {
        JarakPelangganData jp_lain;
        jp_lain.Komplek = komplek;
        jp_lain.Dari = pair.first;
        jp_lain.Ke = namaBaru;
        jp_lain.Jarak = pair.second;
        JarakPelangganList.push_back(jp_lain);
    }
    
    // 4. Jika ada relasi namaLama di luar komplek ini (seharusnya tidak ada, tapi untuk safety), update namanya
    for (auto& jp : JarakPelangganList) {
        if (jp.Dari == namaLama) jp.Dari = namaBaru;
        if (jp.Ke == namaLama) jp.Ke = namaBaru;
    }
    
    SaveSemua();
    return true;
}

bool Database::HapusPelanggan(const string& nama, const string& komplek) {
    bool found = false;
    for (auto it = PelangganList.begin(); it != PelangganList.end(); ) {
        if (it->Nama == nama && it->Komplek == komplek) {
            it = PelangganList.erase(it);
            found = true;
        } else {
            ++it;
        }
    }
    if (!found) return false;
    
    // Hapus jarak terkait
    for (auto it = JarakPelangganList.begin(); it != JarakPelangganList.end(); ) {
        if (it->Komplek == komplek && (it->Dari == nama || it->Ke == nama)) {
            it = JarakPelangganList.erase(it);
        } else {
            ++it;
        }
    }
    
    SaveSemua();
    return true;
}
