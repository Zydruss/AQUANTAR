#include "Database.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>

using namespace std;

// Fungsi pembantu untuk memisahkan string berdasarkan tanda koma (format CSV)
// Fungsi ini juga menghapus spasi kosong di awal/akhir dan karakter carriage return (\r)
vector<string> SplitCSV(const string& line) {
    vector<string> result;
    string cell;
    stringstream lineStream(line);
    while (getline(lineStream, cell, ',')) {
        // Menghapus spasi dan karakter baris baru (\r atau \n) di bagian belakang
        while (!cell.empty() && (cell.back() == '\r' || cell.back() == '\n' || cell.back() == ' ')) {
            cell.pop_back();
        }
        // Menghapus spasi di bagian depan
        while (!cell.empty() && cell.front() == ' ') {
            cell.erase(cell.begin());
        }
        result.push_back(cell);
    }
    return result;
}

// Fungsi pembantu untuk mengubah string menjadi integer secara aman
// Jika string kosong atau terjadi kesalahan saat konversi, akan mengembalikan nilai default (defVal)
static int SafeStoi(const string& s, int defVal = 9999999) {
    try {
        if (s.empty()) return defVal;
        return stoi(s);
    } catch (...) {
        return defVal;
    }
}

// Fungsi pembantu untuk membandingkan dua string secara case-insensitive
static bool IsEqualsCaseInsensitive(const string& s1, const string& s2) {
    if (s1.length() != s2.length()) return false;
    for (size_t i = 0; i < s1.length(); ++i) {
        if (tolower(s1[i]) != tolower(s2[i])) return false;
    }
    return true;
}

// Fungsi pembantu untuk memeriksa apakah suatu string diawali oleh prefix tertentu secara case-insensitive
static bool StartsWithCaseInsensitive(const string& str, const string& prefix) {
    if (str.length() < prefix.length()) return false;
    for (size_t i = 0; i < prefix.length(); ++i) {
        if (tolower(str[i]) != tolower(prefix[i])) return false;
    }
    return true;
}

// Fungsi pembantu untuk memvalidasi nama (pelanggan atau komplek)
// Nama dianggap valid jika:
//   1. Tidak kosong
//   2. Mengandung setidaknya satu huruf alfabet (a-z / A-Z)
//   3. Bukan hanya angka, simbol, spasi, tanda minus, dsb.
// Mengembalikan string kosong jika valid, atau pesan error jika tidak valid
static string ValidasiNama(const string& nama) {
    if (nama.empty()) {
        return "Nama tidak boleh kosong.";
    }
    // Periksa apakah ada minimal satu karakter huruf alfabet
    bool adaHuruf = false;
    for (char c : nama) {
        if (isalpha((unsigned char)c)) {
            adaHuruf = true;
            break;
        }
    }
    if (!adaHuruf) {
        return "Nama tidak valid! Nama harus mengandung setidaknya satu huruf (bukan hanya angka atau simbol).";
    }
    return ""; // Nama valid
}

// Konstruktor kelas Database
// Saat objek Database dibuat, otomatis akan membuat file default (jika belum ada) dan memuat semua data
Database::Database() {
    InisialisasiDefault();
    LoadSemua();
}

// Fungsi untuk membuat file CSV default dengan contoh data awal jika file tidak ditemukan
void Database::InisialisasiDefault() {
    // Memeriksa apakah file komplek.csv sudah ada di direktori kerja
    ifstream checkFile("komplek.csv");
    if (checkFile.good()) {
        checkFile.close();
        return; // Jika file sudah ada, tidak perlu menulis ulang data default
    }
    checkFile.close();
    
    // Membuat file komplek.csv (Daftar komplek/blok awal)
    ofstream fKomplek("komplek.csv");
    fKomplek << "NamaKomplek\n";
    fKomplek << "Depot\n";
    fKomplek << "Blok A\n";
    fKomplek << "Blok B\n";
    fKomplek << "Blok C\n";
    fKomplek.close();

    // Membuat file komplek_jarak.csv (Relasi jarak antar komplek / graf makro)
    ofstream fKjarak("komplek_jarak.csv");
    fKjarak << "Dari,Ke,Jarak\n";
    fKjarak << "Depot,Blok A,2\n";
    fKjarak << "Depot,Blok B,5\n";
    fKjarak << "Blok A,Blok B,3\n";
    fKjarak << "Blok A,Blok C,4\n";
    fKjarak << "Blok B,Blok C,1\n";
    fKjarak.close();

    // Membuat file pelanggan.csv (Daftar nama pelanggan beserta kompleknya)
    ofstream fPelanggan("pelanggan.csv");
    fPelanggan << "NamaPelanggan,NamaKomplek\n";
    fPelanggan << "Pelanggan X,Blok A\n";
    fPelanggan << "Pelanggan Y,Blok A\n";
    fPelanggan << "Pelanggan Z,Blok A\n";
    fPelanggan << "Pelanggan P,Blok B\n";
    fPelanggan << "Pelanggan Q,Blok B\n";
    fPelanggan << "Pelanggan R,Blok C\n";
    fPelanggan.close();

    // Membuat file pelanggan_jarak.csv (Relasi jarak antar pelanggan di dalam komplek / graf mikro)
    ofstream fPjarak("pelanggan_jarak.csv");
    fPjarak << "NamaKomplek,Dari,Ke,Jarak\n";
    // Jarak pelanggan di dalam Blok A
    fPjarak << "Blok A,Masuk Blok A,Pelanggan X,1\n";
    fPjarak << "Blok A,Masuk Blok A,Pelanggan Y,4\n";
    fPjarak << "Blok A,Masuk Blok A,Pelanggan Z,9999999\n";
    fPjarak << "Blok A,Pelanggan X,Pelanggan Y,2\n";
    fPjarak << "Blok A,Pelanggan X,Pelanggan Z,5\n";
    fPjarak << "Blok A,Pelanggan Y,Pelanggan Z,1\n";
    // Jarak pelanggan di dalam Blok B
    fPjarak << "Blok B,Masuk Blok B,Pelanggan P,2\n";
    fPjarak << "Blok B,Masuk Blok B,Pelanggan Q,6\n";
    fPjarak << "Blok B,Pelanggan P,Pelanggan Q,3\n";
    // Jarak pelanggan di dalam Blok C
    fPjarak << "Blok C,Masuk Blok C,Pelanggan R,3\n";
    fPjarak.close();
}

// Fungsi utama untuk memuat seluruh data dari semua file CSV ke dalam memori
void Database::LoadSemua() {
    LoadKomplek();
    LoadJarakKomplek();
    LoadPelanggan();
    LoadJarakPelanggan();
}

// Fungsi utama untuk menulis/menyimpan seluruh data dari memori kembali ke file CSV
void Database::SaveSemua() {
    SaveKomplek();
    SaveJarakKomplek();
    SavePelanggan();
    SaveJarakPelanggan();
}

// Memuat data nama komplek dari berkas komplek.csv
void Database::LoadKomplek() {
    KomplekList.clear();
    ifstream file("komplek.csv");
    if (!file.is_open()) return;
    string line;
    getline(file, line); // Melewati baris header (NamaKomplek)
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

// Memuat data jarak antar komplek dari berkas komplek_jarak.csv
void Database::LoadJarakKomplek() {
    JarakKomplekList.clear();
    ifstream file("komplek_jarak.csv");
    if (!file.is_open()) return;
    string line;
    getline(file, line); // Melewati baris header (Dari,Ke,Jarak)
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

// Memuat data pelanggan dari berkas pelanggan.csv
void Database::LoadPelanggan() {
    PelangganList.clear();
    ifstream file("pelanggan.csv");
    if (!file.is_open()) return;
    string line;
    getline(file, line); // Melewati baris header (NamaPelanggan,NamaKomplek)
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

// Memuat data relasi jarak pelanggan dari berkas pelanggan_jarak.csv
void Database::LoadJarakPelanggan() {
    JarakPelangganList.clear();
    ifstream file("pelanggan_jarak.csv");
    if (!file.is_open()) return;
    string line;
    getline(file, line); // Melewati baris header
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

// Menyimpan daftar nama komplek ke komplek.csv
void Database::SaveKomplek() {
    ofstream file("komplek.csv");
    file << "NamaKomplek\n";
    for (const auto& k : KomplekList) {
        file << k << "\n";
    }
    file.close();
}

// Menyimpan daftar jarak makro (antar komplek) ke komplek_jarak.csv
void Database::SaveJarakKomplek() {
    ofstream file("komplek_jarak.csv");
    file << "Dari,Ke,Jarak\n";
    for (const auto& jk : JarakKomplekList) {
        file << jk.Dari << "," << jk.Ke << "," << jk.Jarak << "\n";
    }
    file.close();
}

// Menyimpan daftar pelanggan ke pelanggan.csv
void Database::SavePelanggan() {
    ofstream file("pelanggan.csv");
    file << "NamaPelanggan,NamaKomplek\n";
    for (const auto& p : PelangganList) {
        file << p.Nama << "," << p.Komplek << "\n";
    }
    file.close();
}

// Menyimpan daftar jarak mikro (antar pelanggan) ke pelanggan_jarak.csv
void Database::SaveJarakPelanggan() {
    ofstream file("pelanggan_jarak.csv");
    file << "NamaKomplek,Dari,Ke,Jarak\n";
    for (const auto& jp : JarakPelangganList) {
        file << jp.Komplek << "," << jp.Dari << "," << jp.Ke << "," << jp.Jarak << "\n";
    }
    file.close();
}

// Membuat matriks ketetanggaan graf makro (antar komplek) untuk perhitungan Dijkstra
vector<vector<int>> Database::AmbilGrafMakro() {
    int N = KomplekList.size();
    vector<vector<int>> Graf(N, vector<int>(N, 9999999));
    for (int i = 0; i < N; i++) {
        Graf[i][i] = 0; // Jarak ke diri sendiri adalah 0
    }
    
    // Ekspresi lambda untuk mencari indeks dari nama komplek tertentu
    auto AmbilIndex = [&](const string& nama) {
        for (int i = 0; i < N; i++) {
            if (KomplekList[i] == nama) return i;
        }
        return -1;
    };
    
    // Mengisi matriks graf berdasarkan list jarak makro
    for (const auto& jk : JarakKomplekList) {
        int u = AmbilIndex(jk.Dari);
        int v = AmbilIndex(jk.Ke);
        if (u != -1 && v != -1) {
            Graf[u][v] = jk.Jarak;
            Graf[v][u] = jk.Jarak; // Bersifat undirected (dua arah)
        }
    }
    
    return Graf;
}

// Membuat matriks ketetanggaan graf mikro (antar pelanggan di dalam satu komplek) untuk perhitungan Dijkstra
vector<vector<int>> Database::AmbilGrafMikro(const string& namaKomplek, vector<string>& outNamaNodes) {
    outNamaNodes.clear();
    // Index 0 adalah gerbang masuk komplek
    outNamaNodes.push_back("Masuk " + namaKomplek);
    
    // Mencari semua pelanggan yang berada di komplek target
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
    
    // Ekspresi lambda untuk mencari indeks node (pintu masuk atau nama pelanggan) di graf mikro
    auto AmbilIndex = [&](const string& nama) {
        for (int i = 0; i < M; i++) {
            if (outNamaNodes[i] == nama) return i;
        }
        return -1;
    };
    
    // Mengisi matriks graf berdasarkan list jarak mikro pelanggan
    for (const auto& jp : JarakPelangganList) {
        if (jp.Komplek == namaKomplek) {
            int u = AmbilIndex(jp.Dari);
            int v = AmbilIndex(jp.Ke);
            if (u != -1 && v != -1) {
                Graf[u][v] = jp.Jarak;
                Graf[v][u] = jp.Jarak; // Bersifat undirected (dua arah)
            }
        }
    }
    
    return Graf;
}

// Fungsi untuk menambahkan pelanggan baru ke dalam database
// Mengembalikan string kosong jika berhasil, atau pesan error spesifik jika gagal
string Database::TambahPelanggan(const string& nama, const string& komplek, int jarakKeMasuk, const map<string, int>& jarakKePelangganLain) {
    // Validasi: nama harus mengandung huruf, tidak boleh hanya angka/simbol
    string errNama = ValidasiNama(nama);
    if (!errNama.empty()) return errNama;
    
    // Validasi: nama pelanggan tidak boleh menggunakan kata kunci dilindungi (case-insensitive)
    if (IsEqualsCaseInsensitive(nama, "Depot")) {
        return "Nama 'Depot' adalah nama khusus sistem dan tidak bisa digunakan.";
    }
    if (StartsWithCaseInsensitive(nama, "Masuk")) {
        return "Nama tidak boleh diawali dengan kata 'Masuk' karena merupakan nama sistem internal.";
    }
    
    // Validasi: nama pelanggan tidak boleh duplikat dengan yang sudah ada (case-insensitive)
    for (const auto& p : PelangganList) {
        if (IsEqualsCaseInsensitive(p.Nama, nama)) {
            return "Nama pelanggan '" + nama + "' sudah terdaftar. Gunakan nama yang berbeda.";
        }
    }
    
    // Validasi: jarak ke pintu masuk tidak boleh negatif
    if (jarakKeMasuk < 0) {
        return "Jarak ke pintu masuk tidak boleh bernilai negatif.";
    }
    // Validasi: jarak ke pelanggan lain tidak boleh negatif
    for (const auto& pair : jarakKePelangganLain) {
        if (pair.second < 0) {
            return "Jarak ke pelanggan '" + pair.first + "' tidak boleh bernilai negatif.";
        }
    }
    
    // Menambahkan data ke dalam list pelanggan
    PelangganData pd;
    pd.Nama = nama;
    pd.Komplek = komplek;
    PelangganList.push_back(pd);
    
    // Menambahkan jarak dari pintu masuk komplek ke pelanggan baru
    JarakPelangganData jpd;
    jpd.Komplek = komplek;
    jpd.Dari = "Masuk " + komplek;
    jpd.Ke = nama;
    jpd.Jarak = jarakKeMasuk;
    JarakPelangganList.push_back(jpd);
    
    // Menambahkan jarak ke pelanggan lain di komplek yang sama
    for (const auto& pair : jarakKePelangganLain) {
        JarakPelangganData jp_lain;
        jp_lain.Komplek = komplek;
        jp_lain.Dari = pair.first;
        jp_lain.Ke = nama;
        jp_lain.Jarak = pair.second;
        JarakPelangganList.push_back(jp_lain);
    }
    
    SaveSemua(); // Simpan perubahan ke file CSV
    return ""; // Berhasil
}

// Fungsi untuk mengubah (edit) informasi pelanggan lama
// Mengembalikan string kosong jika berhasil, atau pesan error spesifik jika gagal
string Database::EditPelanggan(const string& namaLama, const string& namaBaru, const string& komplek, int jarakKeMasuk, const map<string, int>& jarakKePelangganLain) {
    // Validasi: nama baru harus mengandung huruf, tidak boleh hanya angka/simbol
    string errNama = ValidasiNama(namaBaru);
    if (!errNama.empty()) return errNama;
    
    // Validasi: nama baru tidak boleh menggunakan kata kunci dilindungi
    if (IsEqualsCaseInsensitive(namaBaru, "Depot")) {
        return "Nama 'Depot' adalah nama khusus sistem dan tidak bisa digunakan.";
    }
    if (StartsWithCaseInsensitive(namaBaru, "Masuk")) {
        return "Nama tidak boleh diawali dengan kata 'Masuk' karena merupakan nama sistem internal.";
    }
    
    // Validasi: jarak ke pintu masuk tidak boleh negatif
    if (jarakKeMasuk < 0) {
        return "Jarak ke pintu masuk tidak boleh bernilai negatif.";
    }
    // Validasi: jarak ke pelanggan lain tidak boleh negatif
    for (const auto& pair : jarakKePelangganLain) {
        if (pair.second < 0) {
            return "Jarak ke pelanggan '" + pair.first + "' tidak boleh bernilai negatif.";
        }
    }
    
    // Jika nama diubah, pastikan nama baru tidak duplikat dengan pelanggan lain
    if (!IsEqualsCaseInsensitive(namaLama, namaBaru)) {
        for (const auto& p : PelangganList) {
            if (IsEqualsCaseInsensitive(p.Nama, namaBaru)) {
                return "Nama pelanggan '" + namaBaru + "' sudah terdaftar. Gunakan nama yang berbeda.";
            }
        }
    }
    
    // 1. Memperbarui nama pelanggan di PelangganList
    bool found = false;
    for (auto& p : PelangganList) {
        if (p.Nama == namaLama && p.Komplek == komplek) {
            p.Nama = namaBaru;
            found = true;
            break;
        }
    }
    if (!found) return "Pelanggan '" + namaLama + "' tidak ditemukan di komplek '" + komplek + "'.";
    
    // 2. Menghapus data jarak lama yang melibatkan pelanggan lama di komplek ini
    vector<JarakPelangganData> tempJarak;
    for (const auto& jp : JarakPelangganList) {
        if (jp.Komplek == komplek && (jp.Dari == namaLama || jp.Ke == namaLama)) {
            continue;
        }
        tempJarak.push_back(jp);
    }
    JarakPelangganList = tempJarak;
    
    // 3. Menambahkan kembali data jarak yang baru dengan nama pelanggan baru
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
    
    // 4. Memastikan jika ada nama lama yang tertinggal di luar komplek ini (safety)
    for (auto& jp : JarakPelangganList) {
        if (jp.Dari == namaLama) jp.Dari = namaBaru;
        if (jp.Ke == namaLama) jp.Ke = namaBaru;
    }
    
    SaveSemua(); // Simpan perubahan ke file CSV
    return ""; // Berhasil
}

// Fungsi untuk menghapus data pelanggan dari database
// Mengembalikan string kosong jika berhasil, atau pesan error spesifik jika gagal
string Database::HapusPelanggan(const string& nama, const string& komplek) {
    bool found = false;
    // Menghapus dari list pelanggan
    for (auto it = PelangganList.begin(); it != PelangganList.end(); ) {
        if (it->Nama == nama && it->Komplek == komplek) {
            it = PelangganList.erase(it);
            found = true;
        } else {
            ++it;
        }
    }
    if (!found) return "Pelanggan '" + nama + "' tidak ditemukan di komplek '" + komplek + "'.";
    
    // Menghapus data jarak terkait pelanggan ini di JarakPelangganList
    for (auto it = JarakPelangganList.begin(); it != JarakPelangganList.end(); ) {
        if (it->Komplek == komplek && (it->Dari == nama || it->Ke == nama)) {
            it = JarakPelangganList.erase(it);
        } else {
            ++it;
        }
    }
    
    SaveSemua(); // Simpan perubahan ke file CSV
    return ""; // Berhasil
}

// Fungsi untuk menambahkan komplek baru beserta jarak ke komplek lain
// Mengembalikan string kosong jika berhasil, atau pesan error spesifik jika gagal
string Database::TambahKomplek(const string& nama, const map<string, int>& jarakKeKomplekLain) {
    // Validasi: nama harus mengandung huruf, tidak boleh hanya angka/simbol
    string errNama = ValidasiNama(nama);
    if (!errNama.empty()) return errNama;
    
    // Validasi: nama komplek tidak boleh menggunakan nama Depot
    if (IsEqualsCaseInsensitive(nama, "Depot")) {
        return "Nama 'Depot' adalah nama khusus sistem dan tidak bisa digunakan.";
    }
    // Validasi: tidak boleh dimulai dengan kata Masuk
    if (StartsWithCaseInsensitive(nama, "Masuk")) {
        return "Nama komplek tidak boleh diawali dengan kata 'Masuk' karena merupakan nama sistem internal.";
    }
    
    // Validasi: nama komplek tidak boleh duplikat (case-insensitive)
    for (const auto& k : KomplekList) {
        if (IsEqualsCaseInsensitive(k, nama)) {
            return "Nama komplek '" + nama + "' sudah terdaftar. Gunakan nama yang berbeda.";
        }
    }
    
    // Validasi: jarak tidak boleh negatif
    for (const auto& pair : jarakKeKomplekLain) {
        if (pair.second < 0) {
            return "Jarak ke komplek '" + pair.first + "' tidak boleh bernilai negatif.";
        }
    }
    
    // Tambahkan ke daftar komplek
    KomplekList.push_back(nama);
    
    // Tambahkan relasi jarak ke komplek lainnya
    for (const auto& pair : jarakKeKomplekLain) {
        JarakData jd;
        jd.Dari = nama;
        jd.Ke = pair.first;
        jd.Jarak = pair.second;
        JarakKomplekList.push_back(jd);
    }
    
    SaveSemua(); // Simpan perubahan ke file CSV
    return ""; // Berhasil
}

// Fungsi untuk mengedit nama komplek dan memperbarui relasi jaraknya
// Perubahan nama akan otomatis kaskade ke data pelanggan dan relasi jarak pelanggan terkait
// Mengembalikan string kosong jika berhasil, atau pesan error spesifik jika gagal
string Database::EditKomplek(const string& namaLama, const string& namaBaru, const map<string, int>& jarakKeKomplekLain) {
    // Validasi: nama baru harus mengandung huruf, tidak boleh hanya angka/simbol
    string errNama = ValidasiNama(namaBaru);
    if (!errNama.empty()) return errNama;
    
    // Depot adalah pangkalan awal wajib, tidak boleh diedit
    if (IsEqualsCaseInsensitive(namaLama, "Depot")) {
        return "Komplek 'Depot' adalah titik awal sistem dan tidak dapat diubah namanya.";
    }
    if (IsEqualsCaseInsensitive(namaBaru, "Depot")) {
        return "Nama 'Depot' adalah nama khusus sistem dan tidak bisa digunakan.";
    }
    
    // Validasi: nama baru tidak boleh diawali dengan "Masuk"
    if (StartsWithCaseInsensitive(namaBaru, "Masuk")) {
        return "Nama komplek tidak boleh diawali dengan kata 'Masuk' karena merupakan nama sistem internal.";
    }
    
    // Validasi: jarak tidak boleh negatif
    for (const auto& pair : jarakKeKomplekLain) {
        if (pair.second < 0) {
            return "Jarak ke komplek '" + pair.first + "' tidak boleh bernilai negatif.";
        }
    }
    
    // Jika nama berubah, pastikan tidak duplikat dengan komplek lainnya
    if (!IsEqualsCaseInsensitive(namaLama, namaBaru)) {
        for (const auto& k : KomplekList) {
            if (IsEqualsCaseInsensitive(k, namaBaru)) {
                return "Nama komplek '" + namaBaru + "' sudah terdaftar. Gunakan nama yang berbeda.";
            }
        }
    }
    
    // 1. Perbarui nama komplek di KomplekList
    bool found = false;
    for (auto& k : KomplekList) {
        if (IsEqualsCaseInsensitive(k, namaLama)) {
            k = namaBaru;
            found = true;
            break;
        }
    }
    if (!found) return "Komplek '" + namaLama + "' tidak ditemukan.";
    
    // 2. Hapus data jarak lama yang melibatkan komplek ini di JarakKomplekList (Makro)
    vector<JarakData> tempJarak;
    for (const auto& jk : JarakKomplekList) {
        if (IsEqualsCaseInsensitive(jk.Dari, namaLama) || IsEqualsCaseInsensitive(jk.Ke, namaLama)) {
            continue;
        }
        tempJarak.push_back(jk);
    }
    JarakKomplekList = tempJarak;
    
    // 3. Masukkan data jarak baru yang dispesifikasikan oleh pengguna
    for (const auto& pair : jarakKeKomplekLain) {
        JarakData jd;
        jd.Dari = namaBaru;
        jd.Ke = pair.first;
        jd.Jarak = pair.second;
        JarakKomplekList.push_back(jd);
    }
    
    // 4. Perbarui data komplek pelanggan yang terkena dampak (Cascade Update)
    for (auto& p : PelangganList) {
        if (IsEqualsCaseInsensitive(p.Komplek, namaLama)) {
            p.Komplek = namaBaru;
        }
    }
    
    // 5. Perbarui nama komplek & nama node gerbang masuk pelanggan di JarakPelangganList (Cascade Update)
    for (auto& jp : JarakPelangganList) {
        if (IsEqualsCaseInsensitive(jp.Komplek, namaLama)) {
            jp.Komplek = namaBaru;
        }
        if (IsEqualsCaseInsensitive(jp.Dari, "Masuk " + namaLama)) {
            jp.Dari = "Masuk " + namaBaru;
        }
        if (IsEqualsCaseInsensitive(jp.Ke, "Masuk " + namaLama)) {
            jp.Ke = "Masuk " + namaBaru;
        }
    }
    
    SaveSemua(); // Simpan perubahan ke file CSV
    return ""; // Berhasil
}

// Fungsi untuk menghapus komplek beserta seluruh data jarak dan data pelanggan di dalamnya (Cascade Delete)
// Mengembalikan string kosong jika berhasil, atau pesan error spesifik jika gagal
string Database::HapusKomplek(const string& nama) {
    // Depot adalah titik awal wajib dan tidak boleh dihapus
    if (IsEqualsCaseInsensitive(nama, "Depot")) {
        return "Komplek 'Depot' adalah titik awal sistem dan tidak dapat dihapus.";
    }
    
    // Mencari dan menghapus dari KomplekList
    bool found = false;
    for (auto it = KomplekList.begin(); it != KomplekList.end(); ) {
        if (IsEqualsCaseInsensitive(*it, nama)) {
            it = KomplekList.erase(it);
            found = true;
        } else {
            ++it;
        }
    }
    if (!found) return "Komplek '" + nama + "' tidak ditemukan.";
    
    // Menghapus relasi jarak komplek tersebut dari JarakKomplekList (Makro)
    for (auto it = JarakKomplekList.begin(); it != JarakKomplekList.end(); ) {
        if (IsEqualsCaseInsensitive(it->Dari, nama) || IsEqualsCaseInsensitive(it->Ke, nama)) {
            it = JarakKomplekList.erase(it);
        } else {
            ++it;
        }
    }
    
    // Cascade Delete: Hapus semua pelanggan yang tinggal di komplek tersebut
    for (auto it = PelangganList.begin(); it != PelangganList.end(); ) {
        if (IsEqualsCaseInsensitive(it->Komplek, nama)) {
            it = PelangganList.erase(it);
        } else {
            ++it;
        }
    }
    
    // Hapus seluruh relasi jarak mikro pelanggan di komplek tersebut
    for (auto it = JarakPelangganList.begin(); it != JarakPelangganList.end(); ) {
        if (IsEqualsCaseInsensitive(it->Komplek, nama)) {
            it = JarakPelangganList.erase(it);
        } else {
            ++it;
        }
    }
    
    SaveSemua(); // Simpan perubahan ke file CSV
    return ""; // Berhasil
}
