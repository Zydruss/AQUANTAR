#define UNICODE
#define _UNICODE
#define _WIN32_WINNT 0x0600
#define _WIN32_IE 0x0600

#include <algorithm>
#include <commctrl.h>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <windows.h>

#include "Database.h"
#include "Dijkstra.h"

#pragma comment(                                                               \
    linker,                                                                    \
    "\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

using namespace std;

// Custom helpers for older MinGW compatibility
template <typename T> wstring to_wstring_custom(const T &val) {
  wstringstream wss;
  wss << val;
  return wss.str();
}

static int SafeStoi(const string &s, int defVal = 9999999) {
  try {
    if (s.empty())
      return defVal;
    return stoi(s);
  } catch (...) {
    return defVal;
  }
}

// IDs
#define ID_BTN_MENU_HOME 101
#define ID_BTN_MENU_KOMPLEK 102
#define ID_BTN_MENU_RUTE 103
#define ID_BTN_MENU_CRUD 104
#define ID_BTN_MENU_EXIT 105

#define IDC_SEARCH_KOMPLEK 201
#define IDC_LIST_KOMPLEK 202
#define IDC_LIST_KOMPLEK_PELANGGAN 203
#define ID_BTN_KOMPLEK_BACK 204

#define IDC_LIST_RUTE_CUSTOMERS 301
#define ID_BTN_RUTE_SELECT_ALL 302
#define ID_BTN_RUTE_RESET 303
#define ID_BTN_GENERATE 304
#define IDC_RESULT_OUTPUT 305
#define ID_BTN_RUTE_BACK 306

#define IDC_CRUD_ADD_NAMA 401
#define IDC_CRUD_ADD_KOMPLEK 402
#define IDC_CRUD_ADD_JARAK_MASUK 403
#define IDC_CRUD_ADD_JARAK_LAIN 404
#define ID_BTN_CRUD_ADD 405

#define IDC_CRUD_SELECT_PELANGGAN 406
#define IDC_CRUD_EDIT_NAMA 407
#define IDC_CRUD_EDIT_JARAK_MASUK 408
#define IDC_CRUD_EDIT_JARAK_LAIN 409
#define ID_BTN_CRUD_SAVE 410
#define ID_BTN_CRUD_DELETE 411
#define IDC_CRUD_EDIT_KOMPLEK 412
#define ID_BTN_CRUD_BACK 413

// Panels Enum
enum ActivePanel { PANEL_HOME, PANEL_KOMPLEK, PANEL_RUTE, PANEL_CRUD };

// Global Variables
Database db;
ActivePanel activePanel = PANEL_HOME;

// Modern slate dark theme palette
COLORREF colorBg = RGB(15, 23, 42);             // Slate 900
COLORREF colorSidebar = RGB(2, 6, 23);          // Slate 950
COLORREF colorCard = RGB(30, 41, 59);           // Slate 800
COLORREF colorText = RGB(248, 250, 252);        // Slate 50
COLORREF colorTextDim = RGB(148, 163, 184);     // Slate 400
COLORREF colorPrimary = RGB(37, 99, 235);       // Blue 600
COLORREF colorPrimaryHover = RGB(59, 130, 246); // Blue 500
COLORREF colorSuccess = RGB(22, 163, 74);       // Green 600
COLORREF colorDanger = RGB(220, 38, 38);        // Red 600

HBRUSH brushBg;
HBRUSH brushSidebar;
HBRUSH brushCard;
HBRUSH brushPrimary;
HBRUSH brushSuccess;
HBRUSH brushDanger;

HFONT hFontMain;
HFONT hFontBold;
HFONT hFontTitle;

// Window Handles
HWND hwndMain;
HWND hwndSidebarTitle, hwndSidebarSubtitle;
HWND hwndMenuHome, hwndMenuKomplek, hwndMenuRute, hwndMenuCrud, hwndMenuExit;

// Panel control vectors (for showing/hiding groups)
vector<HWND> hwndHomeCtrls;
vector<HWND> hwndKomplekCtrls;
vector<HWND> hwndRuteCtrls;
vector<HWND> hwndCrudCtrls;

// Home panel controls
HWND hwndHomeTitle, hwndHomeDesc;
HWND hwndHomeCard1, hwndHomeCard2;

// Komplek panel controls
HWND hwndKomplekTitle, hwndSearchLabel, hwndSearchEdit, hwndKomplekList,
    hwndCustListTitle, hwndCustDetailList, hwndBtnKomplekBack;

// Rute panel controls
HWND hwndRuteTitle, hwndSelectLabel, hwndCustChecklist, hwndBtnSelectAll,
    hwndBtnReset, hwndBtnGenerate, hwndResultOutput, hwndBtnRuteBack;

// CRUD panel controls
HWND hwndCrudTitle, hwndAddSectionTitle, hwndAddLabelNama, hwndAddEditNama,
    hwndAddLabelKomplek, hwndAddComboKomplek;
HWND hwndAddLabelJarakMasuk, hwndAddEditJarakMasuk, hwndAddLabelJarakLain,
    hwndAddEditJarakLain, hwndBtnAdd;
HWND hwndEditSectionTitle, hwndEditLabelKomplek, hwndEditComboKomplek,
    hwndEditLabelPelanggan, hwndEditListPelanggan, hwndEditLabelNama,
    hwndEditEditNama;
HWND hwndEditLabelJarakMasuk, hwndEditEditJarakMasuk, hwndEditLabelJarakLain,
    hwndEditEditJarakLain, hwndBtnSave, hwndBtnDelete, hwndBtnCrudBack;

// Helper prototypes
void SwitchPanel(ActivePanel panel);
void ReloadAllUIs();
void GenerateDeliveryRoute();
string AmbilJarakLainFormat(const string &nama, const string &komplek);
map<string, int> ParseJarakLain(const string &input);
int AmbilJarakKeMasuk(const string &nama, const string &komplek);

// Subclass Proc for hover states
LRESULT CALLBACK ButtonSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                                    LPARAM lParam, UINT_PTR uIdSubclass,
                                    DWORD_PTR dwRefData) {
  switch (uMsg) {
  case WM_MOUSEMOVE: {
    TRACKMOUSEEVENT tme;
    tme.cbSize = sizeof(TRACKMOUSEEVENT);
    tme.dwFlags = TME_LEAVE;
    tme.hwndTrack = hWnd;
    TrackMouseEvent(&tme);

    if (!GetPropW(hWnd, L"HOVER")) {
      SetPropW(hWnd, L"HOVER", (HANDLE)1);
      InvalidateRect(hWnd, NULL, FALSE);
    }
    break;
  }
  case WM_MOUSELEAVE: {
    RemovePropW(hWnd, L"HOVER");
    InvalidateRect(hWnd, NULL, FALSE);
    break;
  }
  }
  return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// Window Procedure
LRESULT CALLBACK MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                             LPARAM lParam) {
  switch (uMsg) {
  case WM_CREATE: {
    // Set font helper for a window
    auto SetCtrlFont = [](HWND hwndCtrl, HFONT hFont) {
      SendMessage(hwndCtrl, WM_SETFONT, (WPARAM)hFont, TRUE);
    };

    // ---- SIDEBAR CONTROLS ----
    hwndSidebarTitle = CreateWindowExW(0, L"STATIC", L"AQUANTAR",
                                       WS_CHILD | WS_VISIBLE | SS_LEFT, 20, 25,
                                       180, 30, hWnd, NULL, NULL, NULL);
    SetCtrlFont(hwndSidebarTitle, hFontTitle);

    hwndSidebarSubtitle =
        CreateWindowExW(0, L"STATIC", L"Water Delivery Route Planner",
                        WS_CHILD | WS_VISIBLE | SS_LEFT, 20, 55, 180, 35, hWnd,
                        NULL, NULL, NULL);
    SetCtrlFont(hwndSidebarSubtitle, hFontMain);

    hwndMenuHome = CreateWindowExW(
        0, L"BUTTON", L"Menu Utama", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, 10,
        110, 200, 40, hWnd, (HMENU)ID_BTN_MENU_HOME, NULL, NULL);
    hwndMenuKomplek =
        CreateWindowExW(0, L"BUTTON", L"Informasi Komplek",
                        WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, 10, 160, 200, 40,
                        hWnd, (HMENU)ID_BTN_MENU_KOMPLEK, NULL, NULL);
    hwndMenuRute = CreateWindowExW(
        0, L"BUTTON", L"Generate Rute", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
        10, 210, 200, 40, hWnd, (HMENU)ID_BTN_MENU_RUTE, NULL, NULL);
    hwndMenuCrud = CreateWindowExW(
        0, L"BUTTON", L"Kelola Pelanggan", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
        10, 260, 200, 40, hWnd, (HMENU)ID_BTN_MENU_CRUD, NULL, NULL);
    hwndMenuExit = CreateWindowExW(
        0, L"BUTTON", L"Keluar", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, 10, 600,
        200, 40, hWnd, (HMENU)ID_BTN_MENU_EXIT, NULL, NULL);

    SetWindowSubclass(hwndMenuHome, ButtonSubclassProc, 0, 0);
    SetWindowSubclass(hwndMenuKomplek, ButtonSubclassProc, 0, 0);
    SetWindowSubclass(hwndMenuRute, ButtonSubclassProc, 0, 0);
    SetWindowSubclass(hwndMenuCrud, ButtonSubclassProc, 0, 0);
    SetWindowSubclass(hwndMenuExit, ButtonSubclassProc, 0, 0);

    // ---- PANEL 1: HOME ----
    hwndHomeTitle = CreateWindowExW(0, L"STATIC", L"Selamat Datang di Aquantar",
                                    WS_CHILD | WS_VISIBLE | SS_LEFT, 240, 25,
                                    720, 35, hWnd, NULL, NULL, NULL);
    SetCtrlFont(hwndHomeTitle, hFontTitle);
    hwndHomeCtrls.push_back(hwndHomeTitle);

    hwndHomeDesc = CreateWindowExW(
        0, L"STATIC",
        L"Aquantar membantu Anda memetakan, mengelola data pelanggan, serta "
        L"merencanakan rute distribusi air secara optimal. "
        L"Aplikasi ini menggunakan algoritma Dijkstra Makro untuk rute antar "
        L"komplek dan Dijkstra Mikro untuk rute antar pelanggan di dalam "
        L"komplek.\n\n"
        L"Gunakan menu di samping untuk melihat data komplek, merencanakan "
        L"rute pengiriman, atau mengelola data pelanggan.",
        WS_CHILD | WS_VISIBLE | SS_LEFT, 240, 75, 720, 100, hWnd, NULL, NULL,
        NULL);
    SetCtrlFont(hwndHomeDesc, hFontMain);
    hwndHomeCtrls.push_back(hwndHomeDesc);

    hwndHomeCard1 = CreateWindowExW(0, L"STATIC", L"Total Komplek\n0",
                                    WS_CHILD | WS_VISIBLE | SS_OWNERDRAW, 240,
                                    195, 230, 95, hWnd, NULL, NULL, NULL);
    hwndHomeCtrls.push_back(hwndHomeCard1);

    hwndHomeCard2 = CreateWindowExW(0, L"STATIC", L"Total Pelanggan\n0",
                                    WS_CHILD | WS_VISIBLE | SS_OWNERDRAW, 490,
                                    195, 230, 95, hWnd, NULL, NULL, NULL);
    hwndHomeCtrls.push_back(hwndHomeCard2);

    // ---- PANEL 2: KOMPLEK ----
    hwndKomplekTitle = CreateWindowExW(
        0, L"STATIC", L"Informasi Komplek & Pelanggan", WS_CHILD | SS_LEFT, 240,
        25, 720, 35, hWnd, NULL, NULL, NULL);
    SetCtrlFont(hwndKomplekTitle, hFontTitle);
    hwndKomplekCtrls.push_back(hwndKomplekTitle);

    hwndSearchLabel =
        CreateWindowExW(0, L"STATIC", L"Cari Komplek:", WS_CHILD | SS_LEFT, 240,
                        80, 110, 25, hWnd, NULL, NULL, NULL);
    SetCtrlFont(hwndSearchLabel, hFontBold);
    hwndKomplekCtrls.push_back(hwndSearchLabel);

    hwndSearchEdit = CreateWindowExW(
        0, L"EDIT", L"", WS_CHILD | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL, 360,
        78, 250, 25, hWnd, (HMENU)IDC_SEARCH_KOMPLEK, NULL, NULL);
    SetCtrlFont(hwndSearchEdit, hFontMain);
    hwndKomplekCtrls.push_back(hwndSearchEdit);

    hwndKomplekList = CreateWindowExW(
        0, L"LISTBOX", L"", WS_CHILD | WS_BORDER | WS_VSCROLL | LBS_NOTIFY, 240,
        120, 230, 470, hWnd, (HMENU)IDC_LIST_KOMPLEK, NULL, NULL);
    SetCtrlFont(hwndKomplekList, hFontMain);
    hwndKomplekCtrls.push_back(hwndKomplekList);

    hwndCustListTitle = CreateWindowExW(
        0, L"STATIC", L"Daftar Pelanggan & Jarak (Masuk)", WS_CHILD | SS_LEFT,
        490, 120, 470, 25, hWnd, NULL, NULL, NULL);
    SetCtrlFont(hwndCustListTitle, hFontBold);
    hwndKomplekCtrls.push_back(hwndCustListTitle);

    hwndCustDetailList = CreateWindowExW(
        0, L"LISTBOX", L"", WS_CHILD | WS_BORDER | WS_VSCROLL, 490, 150, 470,
        440, hWnd, (HMENU)IDC_LIST_KOMPLEK_PELANGGAN, NULL, NULL);
    SetCtrlFont(hwndCustDetailList, hFontMain);
    hwndKomplekCtrls.push_back(hwndCustDetailList);

    hwndBtnKomplekBack = CreateWindowExW(
        0, L"BUTTON", L"Kembali ke Menu Utama", WS_CHILD | BS_OWNERDRAW, 490,
        605, 470, 35, hWnd, (HMENU)ID_BTN_KOMPLEK_BACK, NULL, NULL);
    SetWindowSubclass(hwndBtnKomplekBack, ButtonSubclassProc, 0, 0);
    hwndKomplekCtrls.push_back(hwndBtnKomplekBack);

    // ---- PANEL 3: RUTE ----
    hwndRuteTitle = CreateWindowExW(
        0, L"STATIC", L"Perencanaan & Optimasi Rute", WS_CHILD | SS_LEFT, 240,
        25, 720, 35, hWnd, NULL, NULL, NULL);
    SetCtrlFont(hwndRuteTitle, hFontTitle);
    hwndRuteCtrls.push_back(hwndRuteTitle);

    hwndSelectLabel = CreateWindowExW(
        0, L"STATIC", L"Pilih Pelanggan Tujuan Pengiriman:", WS_CHILD | SS_LEFT,
        240, 80, 320, 25, hWnd, NULL, NULL, NULL);
    SetCtrlFont(hwndSelectLabel, hFontBold);
    hwndRuteCtrls.push_back(hwndSelectLabel);

    // Checklist ListView
    hwndCustChecklist = CreateWindowExW(
        0, WC_LISTVIEWW, L"",
        WS_CHILD | WS_BORDER | LVS_REPORT | LVS_NOCOLUMNHEADER | WS_VSCROLL,
        240, 110, 320, 450, hWnd, (HMENU)IDC_LIST_RUTE_CUSTOMERS, NULL, NULL);
    ListView_SetExtendedListViewStyle(hwndCustChecklist,
                                      LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT);
    SetCtrlFont(hwndCustChecklist, hFontMain);
    // Insert single wide column
    LVCOLUMNW col;
    col.mask = LVCF_WIDTH;
    col.cx = 295;
    ListView_InsertColumn(hwndCustChecklist, 0, &col);
    hwndRuteCtrls.push_back(hwndCustChecklist);

    hwndBtnSelectAll = CreateWindowExW(
        0, L"BUTTON", L"Pilih Semua", WS_CHILD | BS_OWNERDRAW, 240, 570, 150,
        32, hWnd, (HMENU)ID_BTN_RUTE_SELECT_ALL, NULL, NULL);
    SetWindowSubclass(hwndBtnSelectAll, ButtonSubclassProc, 0, 0);
    hwndRuteCtrls.push_back(hwndBtnSelectAll);

    hwndBtnReset = CreateWindowExW(0, L"BUTTON", L"Reset",
                                   WS_CHILD | BS_OWNERDRAW, 410, 570, 150, 32,
                                   hWnd, (HMENU)ID_BTN_RUTE_RESET, NULL, NULL);
    SetWindowSubclass(hwndBtnReset, ButtonSubclassProc, 0, 0);
    hwndRuteCtrls.push_back(hwndBtnReset);

    hwndBtnGenerate = CreateWindowExW(
        0, L"BUTTON", L"Generate Rute Terpendek", WS_CHILD | BS_OWNERDRAW, 240,
        612, 320, 38, hWnd, (HMENU)ID_BTN_GENERATE, NULL, NULL);
    SetWindowSubclass(hwndBtnGenerate, ButtonSubclassProc, 0, 0);
    hwndRuteCtrls.push_back(hwndBtnGenerate);

    hwndResultOutput = CreateWindowExW(
        0, L"EDIT",
        L"Pilih pelanggan di samping kemudian klik \"Generate Rute "
        L"Terpendek\"...",
        WS_CHILD | WS_BORDER | WS_VSCROLL | ES_MULTILINE | ES_READONLY |
            ES_AUTOVSCROLL,
        580, 110, 380, 495, hWnd, (HMENU)IDC_RESULT_OUTPUT, NULL, NULL);
    SetCtrlFont(hwndResultOutput, hFontMain);
    hwndRuteCtrls.push_back(hwndResultOutput);

    hwndBtnRuteBack = CreateWindowExW(
        0, L"BUTTON", L"Kembali ke Menu Utama", WS_CHILD | BS_OWNERDRAW, 580,
        615, 380, 35, hWnd, (HMENU)ID_BTN_RUTE_BACK, NULL, NULL);
    SetWindowSubclass(hwndBtnRuteBack, ButtonSubclassProc, 0, 0);
    hwndRuteCtrls.push_back(hwndBtnRuteBack);

    // ---- PANEL 4: CRUD ----
    hwndCrudTitle = CreateWindowExW(0, L"STATIC", L"Kelola Database Pelanggan",
                                    WS_CHILD | SS_LEFT, 240, 25, 720, 35, hWnd,
                                    NULL, NULL, NULL);
    SetCtrlFont(hwndCrudTitle, hFontTitle);
    hwndCrudCtrls.push_back(hwndCrudTitle);

    // LEFT SIDE: TAMBAH PELANGGAN
    hwndAddSectionTitle = CreateWindowExW(
        0, L"STATIC", L"Tambah Pelanggan Baru", WS_CHILD | SS_LEFT, 240, 80,
        300, 25, hWnd, NULL, NULL, NULL);
    SetCtrlFont(hwndAddSectionTitle, hFontBold);
    hwndCrudCtrls.push_back(hwndAddSectionTitle);

    hwndAddLabelNama =
        CreateWindowExW(0, L"STATIC", L"Nama Pelanggan:", WS_CHILD | SS_LEFT,
                        240, 115, 300, 20, hWnd, NULL, NULL, NULL);
    SetCtrlFont(hwndAddLabelNama, hFontMain);
    hwndCrudCtrls.push_back(hwndAddLabelNama);

    hwndAddEditNama = CreateWindowExW(
        0, L"EDIT", L"", WS_CHILD | WS_BORDER | ES_LEFT, 240, 135, 300, 25,
        hWnd, (HMENU)IDC_CRUD_ADD_NAMA, NULL, NULL);
    SetCtrlFont(hwndAddEditNama, hFontMain);
    hwndCrudCtrls.push_back(hwndAddEditNama);

    hwndAddLabelKomplek =
        CreateWindowExW(0, L"STATIC", L"Komplek / Blok:", WS_CHILD | SS_LEFT,
                        240, 175, 300, 20, hWnd, NULL, NULL, NULL);
    SetCtrlFont(hwndAddLabelKomplek, hFontMain);
    hwndCrudCtrls.push_back(hwndAddLabelKomplek);

    hwndAddComboKomplek = CreateWindowExW(
        0, L"COMBOBOX", L"", WS_CHILD | CBS_DROPDOWNLIST | WS_VSCROLL, 240, 195,
        300, 200, hWnd, (HMENU)IDC_CRUD_ADD_KOMPLEK, NULL, NULL);
    SetCtrlFont(hwndAddComboKomplek, hFontMain);
    hwndCrudCtrls.push_back(hwndAddComboKomplek);

    hwndAddLabelJarakMasuk = CreateWindowExW(
        0, L"STATIC", L"Jarak ke Pintu Masuk Komplek (meter):",
        WS_CHILD | SS_LEFT, 240, 235, 300, 20, hWnd, NULL, NULL, NULL);
    SetCtrlFont(hwndAddLabelJarakMasuk, hFontMain);
    hwndCrudCtrls.push_back(hwndAddLabelJarakMasuk);

    hwndAddEditJarakMasuk = CreateWindowExW(
        0, L"EDIT", L"", WS_CHILD | WS_BORDER | ES_NUMBER, 240, 255, 300, 25,
        hWnd, (HMENU)IDC_CRUD_ADD_JARAK_MASUK, NULL, NULL);
    SetCtrlFont(hwndAddEditJarakMasuk, hFontMain);
    hwndCrudCtrls.push_back(hwndAddEditJarakMasuk);

    hwndAddLabelJarakLain = CreateWindowExW(
        0, L"STATIC", L"Jarak ke Pelanggan Lain (Nama:Jarak, ...):",
        WS_CHILD | SS_LEFT, 240, 295, 300, 20, hWnd, NULL, NULL, NULL);
    SetCtrlFont(hwndAddLabelJarakLain, hFontMain);
    hwndCrudCtrls.push_back(hwndAddLabelJarakLain);

    hwndAddEditJarakLain = CreateWindowExW(
        0, L"EDIT", L"", WS_CHILD | WS_BORDER | ES_LEFT, 240, 315, 300, 25,
        hWnd, (HMENU)IDC_CRUD_ADD_JARAK_LAIN, NULL, NULL);
    SetCtrlFont(hwndAddEditJarakLain, hFontMain);
    hwndCrudCtrls.push_back(hwndAddEditJarakLain);

    hwndBtnAdd = CreateWindowExW(0, L"BUTTON", L"Tambah Pelanggan",
                                 WS_CHILD | BS_OWNERDRAW, 240, 365, 300, 38,
                                 hWnd, (HMENU)ID_BTN_CRUD_ADD, NULL, NULL);
    SetWindowSubclass(hwndBtnAdd, ButtonSubclassProc, 0, 0);
    hwndCrudCtrls.push_back(hwndBtnAdd);

    // RIGHT SIDE: EDIT & HAPUS PELANGGAN
    hwndEditSectionTitle = CreateWindowExW(
        0, L"STATIC", L"Edit / Hapus Pelanggan", WS_CHILD | SS_LEFT, 580, 80,
        380, 25, hWnd, NULL, NULL, NULL);
    SetCtrlFont(hwndEditSectionTitle, hFontBold);
    hwndCrudCtrls.push_back(hwndEditSectionTitle);

    hwndEditLabelKomplek =
        CreateWindowExW(0, L"STATIC", L"Pilih Komplek:", WS_CHILD | SS_LEFT,
                        580, 110, 380, 20, hWnd, NULL, NULL, NULL);
    SetCtrlFont(hwndEditLabelKomplek, hFontMain);
    hwndCrudCtrls.push_back(hwndEditLabelKomplek);

    hwndEditComboKomplek = CreateWindowExW(
        0, L"COMBOBOX", L"", WS_CHILD | CBS_DROPDOWNLIST | WS_VSCROLL, 580, 130,
        380, 200, hWnd, (HMENU)IDC_CRUD_EDIT_KOMPLEK, NULL, NULL);
    SetCtrlFont(hwndEditComboKomplek, hFontMain);
    hwndCrudCtrls.push_back(hwndEditComboKomplek);

    hwndEditLabelPelanggan =
        CreateWindowExW(0, L"STATIC", L"Pilih Pelanggan:", WS_CHILD | SS_LEFT,
                        580, 165, 380, 20, hWnd, NULL, NULL, NULL);
    SetCtrlFont(hwndEditLabelPelanggan, hFontMain);
    hwndCrudCtrls.push_back(hwndEditLabelPelanggan);

    hwndEditListPelanggan = CreateWindowExW(
        0, L"LISTBOX", L"", WS_CHILD | WS_BORDER | WS_VSCROLL | LBS_NOTIFY, 580,
        185, 380, 100, hWnd, (HMENU)IDC_CRUD_SELECT_PELANGGAN, NULL, NULL);
    SetCtrlFont(hwndEditListPelanggan, hFontMain);
    hwndCrudCtrls.push_back(hwndEditListPelanggan);

    hwndEditLabelNama = CreateWindowExW(0, L"STATIC", L"Nama Pelanggan (Baru):",
                                        WS_CHILD | SS_LEFT, 580, 295, 380, 20,
                                        hWnd, NULL, NULL, NULL);
    SetCtrlFont(hwndEditLabelNama, hFontMain);
    hwndCrudCtrls.push_back(hwndEditLabelNama);

    hwndEditEditNama = CreateWindowExW(
        0, L"EDIT", L"", WS_CHILD | WS_BORDER | ES_LEFT, 580, 315, 380, 25,
        hWnd, (HMENU)IDC_CRUD_EDIT_NAMA, NULL, NULL);
    SetCtrlFont(hwndEditEditNama, hFontMain);
    hwndCrudCtrls.push_back(hwndEditEditNama);

    hwndEditLabelJarakMasuk = CreateWindowExW(
        0, L"STATIC", L"Jarak ke Pintu Masuk Komplek (meter):",
        WS_CHILD | SS_LEFT, 580, 350, 380, 20, hWnd, NULL, NULL, NULL);
    SetCtrlFont(hwndEditLabelJarakMasuk, hFontMain);
    hwndCrudCtrls.push_back(hwndEditLabelJarakMasuk);

    hwndEditEditJarakMasuk = CreateWindowExW(
        0, L"EDIT", L"", WS_CHILD | WS_BORDER | ES_NUMBER, 580, 370, 380, 25,
        hWnd, (HMENU)IDC_CRUD_EDIT_JARAK_MASUK, NULL, NULL);
    SetCtrlFont(hwndEditEditJarakMasuk, hFontMain);
    hwndCrudCtrls.push_back(hwndEditEditJarakMasuk);

    hwndEditLabelJarakLain = CreateWindowExW(
        0, L"STATIC", L"Jarak ke Pelanggan Lain (Nama:Jarak, ...):",
        WS_CHILD | SS_LEFT, 580, 405, 380, 20, hWnd, NULL, NULL, NULL);
    SetCtrlFont(hwndEditLabelJarakLain, hFontMain);
    hwndCrudCtrls.push_back(hwndEditLabelJarakLain);

    hwndEditEditJarakLain = CreateWindowExW(
        0, L"EDIT", L"", WS_CHILD | WS_BORDER | ES_LEFT, 580, 425, 380, 25,
        hWnd, (HMENU)IDC_CRUD_EDIT_JARAK_LAIN, NULL, NULL);
    SetCtrlFont(hwndEditEditJarakLain, hFontMain);
    hwndCrudCtrls.push_back(hwndEditEditJarakLain);

    hwndBtnSave = CreateWindowExW(0, L"BUTTON", L"Simpan Perubahan",
                                  WS_CHILD | BS_OWNERDRAW, 580, 465, 180, 38,
                                  hWnd, (HMENU)ID_BTN_CRUD_SAVE, NULL, NULL);
    SetWindowSubclass(hwndBtnSave, ButtonSubclassProc, 0, 0);
    hwndCrudCtrls.push_back(hwndBtnSave);

    hwndBtnDelete = CreateWindowExW(
        0, L"BUTTON", L"Hapus Pelanggan", WS_CHILD | BS_OWNERDRAW, 780, 465,
        180, 38, hWnd, (HMENU)ID_BTN_CRUD_DELETE, NULL, NULL);
    SetWindowSubclass(hwndBtnDelete, ButtonSubclassProc, 0, 0);
    hwndCrudCtrls.push_back(hwndBtnDelete);

    hwndBtnCrudBack = CreateWindowExW(
        0, L"BUTTON", L"Kembali ke Menu Utama", WS_CHILD | BS_OWNERDRAW, 780,
        605, 180, 35, hWnd, (HMENU)ID_BTN_CRUD_BACK, NULL, NULL);
    SetWindowSubclass(hwndBtnCrudBack, ButtonSubclassProc, 0, 0);
    hwndCrudCtrls.push_back(hwndBtnCrudBack);

    // Set background colors for specific views using ListView macros
    ListView_SetBkColor(hwndCustChecklist, colorCard);
    ListView_SetTextBkColor(hwndCustChecklist, colorCard);
    ListView_SetTextColor(hwndCustChecklist, colorText);

    // Populate all data lists on load
    ReloadAllUIs();
    SwitchPanel(PANEL_HOME);
  } break;

  case WM_PAINT: {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);

    RECT rectClient;
    GetClientRect(hWnd, &rectClient);
    FillRect(hdc, &rectClient, brushBg);

    RECT rectSidebar = rectClient;
    rectSidebar.right = 220;
    FillRect(hdc, &rectSidebar, brushSidebar);

    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(30, 41, 59)); // Slate 800
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    MoveToEx(hdc, 220, 0, NULL);
    LineTo(hdc, 220, rectClient.bottom);
    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);

    EndPaint(hWnd, &ps);
  } break;

  case WM_CTLCOLORSTATIC: {
    HDC hdc = (HDC)wParam;
    HWND hwndCtrl = (HWND)lParam;

    SetTextColor(hdc, colorText);
    SetBkMode(hdc, TRANSPARENT);

    if (hwndCtrl == hwndSidebarTitle) {
      SetTextColor(hdc, RGB(255, 255, 255));
      return (INT_PTR)brushSidebar;
    }
    if (hwndCtrl == hwndSidebarSubtitle) {
      SetTextColor(hdc, colorTextDim);
      return (INT_PTR)brushSidebar;
    }

    // Labels inside panels
    if (hwndCtrl == hwndAddSectionTitle || hwndCtrl == hwndEditSectionTitle ||
        hwndCtrl == hwndCustListTitle || hwndCtrl == hwndSelectLabel ||
        hwndCtrl == hwndSearchLabel) {
      SetTextColor(hdc, colorText);
      return (INT_PTR)brushBg;
    }

    SetTextColor(hdc, colorTextDim);
    return (INT_PTR)brushBg;
  } break;

  case WM_CTLCOLOREDIT:
  case WM_CTLCOLORLISTBOX: {
    HDC hdc = (HDC)wParam;
    SetTextColor(hdc, colorText);
    SetBkColor(hdc, colorCard);
    return (INT_PTR)brushCard;
  } break;

  case WM_DRAWITEM: {
    LPDRAWITEMSTRUCT lpDrawItem = (LPDRAWITEMSTRUCT)lParam;
    if (lpDrawItem->CtlType == ODT_BUTTON) {
      HDC hdc = lpDrawItem->hDC;
      RECT rect = lpDrawItem->rcItem;
      UINT state = lpDrawItem->itemState;

      HBRUSH hBtnBrush = brushCard;
      COLORREF txtColor = colorText;
      bool needDeleteBrush = false; // Track if we created a temporary brush

      bool isActive = false;
      if (lpDrawItem->CtlID == ID_BTN_MENU_HOME && activePanel == PANEL_HOME)
        isActive = true;
      else if (lpDrawItem->CtlID == ID_BTN_MENU_KOMPLEK &&
               activePanel == PANEL_KOMPLEK)
        isActive = true;
      else if (lpDrawItem->CtlID == ID_BTN_MENU_RUTE &&
               activePanel == PANEL_RUTE)
        isActive = true;
      else if (lpDrawItem->CtlID == ID_BTN_MENU_CRUD &&
               activePanel == PANEL_CRUD)
        isActive = true;

      bool isHovered = GetPropW(lpDrawItem->hwndItem, L"HOVER") != NULL;

      if (isActive) {
        hBtnBrush = brushPrimary;
        txtColor = RGB(255, 255, 255);
      } else if (state & ODS_SELECTED) {
        hBtnBrush = brushPrimary;
        txtColor = RGB(255, 255, 255);
      } else if (isHovered) {
        hBtnBrush = CreateSolidBrush(RGB(51, 65, 85)); // Slate 700
        needDeleteBrush = true;
      }

      // Special actions overrides
      if (lpDrawItem->CtlID == ID_BTN_GENERATE) {
        if (needDeleteBrush) {
          DeleteObject(hBtnBrush);
          needDeleteBrush = false;
        }
        if (state & ODS_SELECTED) {
          hBtnBrush = CreateSolidBrush(RGB(15, 118, 110));
          needDeleteBrush = true;
        } else if (isHovered) {
          hBtnBrush = CreateSolidBrush(RGB(21, 128, 61));
          needDeleteBrush = true;
        } else {
          hBtnBrush = brushSuccess;
        }
        txtColor = RGB(255, 255, 255);
      } else if (lpDrawItem->CtlID == ID_BTN_CRUD_DELETE) {
        if (needDeleteBrush) {
          DeleteObject(hBtnBrush);
          needDeleteBrush = false;
        }
        if (state & ODS_SELECTED) {
          hBtnBrush = CreateSolidBrush(RGB(185, 28, 28));
          needDeleteBrush = true;
        } else if (isHovered) {
          hBtnBrush = CreateSolidBrush(RGB(153, 27, 27));
          needDeleteBrush = true;
        } else {
          hBtnBrush = brushDanger;
        }
        txtColor = RGB(255, 255, 255);
      } else if (lpDrawItem->CtlID == ID_BTN_CRUD_ADD ||
                 lpDrawItem->CtlID == ID_BTN_CRUD_SAVE) {
        if (needDeleteBrush) {
          DeleteObject(hBtnBrush);
          needDeleteBrush = false;
        }
        if (state & ODS_SELECTED) {
          hBtnBrush = CreateSolidBrush(RGB(29, 78, 216));
          needDeleteBrush = true;
        } else if (isHovered) {
          hBtnBrush = CreateSolidBrush(RGB(30, 64, 175));
          needDeleteBrush = true;
        } else {
          hBtnBrush = brushPrimary;
        }
        txtColor = RGB(255, 255, 255);
      }

      FillRect(hdc, &rect, hBtnBrush);

      SetTextColor(hdc, txtColor);
      SetBkMode(hdc, TRANSPARENT);

      wchar_t text[128];
      GetWindowTextW(lpDrawItem->hwndItem, text, 128);

      HFONT hOldFont = (HFONT)SelectObject(hdc, hFontBold);
      DrawTextW(hdc, text, -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
      SelectObject(hdc, hOldFont);

      // Single cleanup point — only delete if we created a temporary brush
      if (needDeleteBrush) {
        DeleteObject(hBtnBrush);
      }

      return TRUE;
    } else if (lpDrawItem->CtlType == ODT_STATIC) {
      HDC hdc = lpDrawItem->hDC;
      RECT rect = lpDrawItem->rcItem;

      // Draw custom stats card background
      FillRect(hdc, &rect, brushCard);

      HPEN hPen = CreatePen(PS_SOLID, 1, RGB(71, 85, 105)); // Slate 600
      HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
      MoveToEx(hdc, rect.left, rect.top, NULL);
      LineTo(hdc, rect.right - 1, rect.top);
      LineTo(hdc, rect.right - 1, rect.bottom - 1);
      LineTo(hdc, rect.left, rect.bottom - 1);
      LineTo(hdc, rect.left, rect.top);
      SelectObject(hdc, hOldPen);
      DeleteObject(hPen);

      SetBkMode(hdc, TRANSPARENT);

      wchar_t text[256];
      GetWindowTextW(lpDrawItem->hwndItem, text, 256);

      wstring wstr(text);
      size_t newline = wstr.find(L'\n');
      if (newline != wstring::npos) {
        wstring title = wstr.substr(0, newline);
        wstring value = wstr.substr(newline + 1);

        SetTextColor(hdc, colorTextDim);
        RECT rectTitle = rect;
        rectTitle.top += 15;
        rectTitle.bottom = rectTitle.top + 25;
        SelectObject(hdc, hFontMain);
        DrawTextW(hdc, title.c_str(), -1, &rectTitle,
                  DT_CENTER | DT_SINGLELINE);

        SetTextColor(hdc, colorPrimaryHover);
        RECT rectVal = rect;
        rectVal.top += 40;
        SelectObject(hdc, hFontTitle);
        DrawTextW(hdc, value.c_str(), -1, &rectVal, DT_CENTER | DT_SINGLELINE);
      }
      return TRUE;
    }
  } break;

  case WM_COMMAND: {
    int wmId = LOWORD(wParam);
    int wmEvent = HIWORD(wParam);

    switch (wmId) {
    // Sidebar panel switches
    case ID_BTN_MENU_HOME:
      SwitchPanel(PANEL_HOME);
      break;
    case ID_BTN_MENU_KOMPLEK:
      SwitchPanel(PANEL_KOMPLEK);
      break;
    case ID_BTN_MENU_RUTE:
      SwitchPanel(PANEL_RUTE);
      break;
    case ID_BTN_MENU_CRUD:
      SwitchPanel(PANEL_CRUD);
      break;
    case ID_BTN_MENU_EXIT: {
      int msgBoxID =
          MessageBoxW(hWnd, L"Apakah Anda yakin ingin keluar dari program?",
                      L"Konfirmasi Keluar", MB_ICONQUESTION | MB_YESNO);
      if (msgBoxID == IDYES) {
        DestroyWindow(hWnd);
      }
    } break;

    // Complex Search Filter change
    case IDC_SEARCH_KOMPLEK: {
      if (wmEvent == EN_CHANGE) {
        wchar_t searchStr[128];
        GetWindowTextW(hwndSearchEdit, searchStr, 128);
        wstring ws(searchStr);
        string query(ws.begin(), ws.end());
        transform(query.begin(), query.end(), query.begin(), ::tolower);

        SendMessage(hwndKomplekList, LB_RESETCONTENT, 0, 0);
        for (const auto &comp : db.KomplekList) {
          string compLower = comp;
          transform(compLower.begin(), compLower.end(), compLower.begin(),
                    ::tolower);
          if (query.empty() || compLower.find(query) != string::npos) {
            wstring compW(comp.begin(), comp.end());
            SendMessageW(hwndKomplekList, LB_ADDSTRING, 0,
                         (LPARAM)compW.c_str());
          }
        }
      }
    } break;

    // Select Complex listbox
    case IDC_LIST_KOMPLEK: {
      if (wmEvent == LBN_SELCHANGE) {
        int selIndex = SendMessage(hwndKomplekList, LB_GETCURSEL, 0, 0);
        if (selIndex != LB_ERR) {
          wchar_t compW[128];
          SendMessageW(hwndKomplekList, LB_GETTEXT, selIndex, (LPARAM)compW);
          wstring wstr(compW);
          string compName(wstr.begin(), wstr.end());

          SendMessage(hwndCustDetailList, LB_RESETCONTENT, 0, 0);

          // Check if Depot
          if (compName == "Depot") {
            SendMessageW(
                hwndCustDetailList, LB_ADDSTRING, 0,
                (LPARAM)L"Depot merupakan titik awal / pangkalan rute.");
            return TRUE;
          }

          // Load micro graph to show customers & their distance relationships
          vector<string> namaNodes;
          auto grafMikro = db.AmbilGrafMikro(compName, namaNodes);

          // Print list of customers
          bool hasCustomers = false;
          for (size_t i = 1; i < namaNodes.size(); i++) {
            hasCustomers = true;
            string custName = namaNodes[i];
            int distToMasuk = grafMikro[0][i];

            wstringstream wss;
            wss << L"Pelanggan: " << wstring(custName.begin(), custName.end());
            if (distToMasuk == INF) {
              wss << L" | Jarak ke Masuk: INF";
            } else {
              wss << L" | Jarak ke Masuk: " << distToMasuk << L" meter";
            }

            // Relasi jarak ke pelanggan lain di komplek ini
            wstring relasi = L" (Hubungan: ";
            bool hasRelasi = false;
            for (size_t j = 1; j < namaNodes.size(); j++) {
              if (i != j && grafMikro[i][j] != INF) {
                if (hasRelasi)
                  relasi += L", ";
                relasi += wstring(namaNodes[j].begin(), namaNodes[j].end()) +
                          L":" + to_wstring_custom(grafMikro[i][j]) + L"m";
                hasRelasi = true;
              }
            }
            if (hasRelasi) {
              relasi += L")";
              wss << relasi;
            }

            SendMessageW(hwndCustDetailList, LB_ADDSTRING, 0,
                         (LPARAM)wss.str().c_str());
          }

          if (!hasCustomers) {
            SendMessageW(hwndCustDetailList, LB_ADDSTRING, 0,
                         (LPARAM)L"Belum ada data pelanggan di komplek ini.");
          }
        }
      }
    } break;

    // Select Complex in CRUD Edit ComboBox
    case IDC_CRUD_EDIT_KOMPLEK: {
      if (wmEvent == CBN_SELCHANGE) {
        int selCompIdx = SendMessage(hwndEditComboKomplek, CB_GETCURSEL, 0, 0);
        if (selCompIdx != CB_ERR) {
          wchar_t compW[128];
          SendMessageW(hwndEditComboKomplek, CB_GETLBTEXT, selCompIdx,
                       (LPARAM)compW);
          wstring wstr(compW);
          string compName(wstr.begin(), wstr.end());

          SendMessage(hwndEditListPelanggan, LB_RESETCONTENT, 0, 0);
          for (const auto &p : db.PelangganList) {
            if (p.Komplek == compName) {
              wstring custW(p.Nama.begin(), p.Nama.end());
              SendMessageW(hwndEditListPelanggan, LB_ADDSTRING, 0,
                           (LPARAM)custW.c_str());
            }
          }

          // Clear fields
          SetWindowTextW(hwndEditEditNama, L"");
          SetWindowTextW(hwndEditEditJarakMasuk, L"");
          SetWindowTextW(hwndEditEditJarakLain, L"");
        }
      }
    } break;

    // Select Customer in CRUD Edit ListBox
    case IDC_CRUD_SELECT_PELANGGAN: {
      if (wmEvent == LBN_SELCHANGE) {
        try {
          int selIdx = SendMessage(hwndEditListPelanggan, LB_GETCURSEL, 0, 0);
          if (selIdx != LB_ERR) {
            wchar_t custW[256];
            SendMessageW(hwndEditListPelanggan, LB_GETTEXT, selIdx,
                         (LPARAM)custW);
            wstring wsCust(custW);
            string custName(wsCust.begin(), wsCust.end());

            int selCompIdx =
                SendMessage(hwndEditComboKomplek, CB_GETCURSEL, 0, 0);
            if (selCompIdx != CB_ERR) {
              wchar_t compW[128];
              SendMessageW(hwndEditComboKomplek, CB_GETLBTEXT, selCompIdx,
                           (LPARAM)compW);
              wstring wsComp(compW);
              string compName(wsComp.begin(), wsComp.end());

              int distToMasuk = AmbilJarakKeMasuk(custName, compName);
              string distLainStr = AmbilJarakLainFormat(custName, compName);

              wstring wCustName(custName.begin(), custName.end());
              wstring wDistMasuk =
                  to_wstring_custom(distToMasuk == INF ? 9999999 : distToMasuk);
              wstring wDistLain(distLainStr.begin(), distLainStr.end());

              SetWindowTextW(hwndEditEditNama, wCustName.c_str());
              SetWindowTextW(hwndEditEditJarakMasuk, wDistMasuk.c_str());
              SetWindowTextW(hwndEditEditJarakLain, wDistLain.c_str());
            }
          }
        } catch (...) {
          MessageBoxW(hWnd, L"Gagal memuat data pelanggan.", L"Error",
                      MB_ICONERROR);
        }
      }
    } break;

    // Back buttons
    case ID_BTN_KOMPLEK_BACK:
    case ID_BTN_RUTE_BACK:
    case ID_BTN_CRUD_BACK:
      SwitchPanel(PANEL_HOME);
      break;

    // Rute Checklist Helper buttons
    case ID_BTN_RUTE_SELECT_ALL: {
      int count = ListView_GetItemCount(hwndCustChecklist);
      for (int i = 0; i < count; i++) {
        ListView_SetCheckState(hwndCustChecklist, i, TRUE);
      }
    } break;

    case ID_BTN_RUTE_RESET: {
      int count = ListView_GetItemCount(hwndCustChecklist);
      for (int i = 0; i < count; i++) {
        ListView_SetCheckState(hwndCustChecklist, i, FALSE);
      }
      SetWindowTextW(hwndResultOutput, L"Pilih pelanggan di samping kemudian "
                                       L"klik \"Generate Rute Terpendek\"...");
    } break;

    // Generate Route Clicked
    case ID_BTN_GENERATE:
      GenerateDeliveryRoute();
      break;

    // CRUD: Tambah Pelanggan
    case ID_BTN_CRUD_ADD: {
      try {
        wchar_t namaW[128], jarakMasukW[128], jarakLainW[512];
        GetWindowTextW(hwndAddEditNama, namaW, 128);
        GetWindowTextW(hwndAddEditJarakMasuk, jarakMasukW, 128);
        GetWindowTextW(hwndAddEditJarakLain, jarakLainW, 512);

        int selCompIdx = SendMessage(hwndAddComboKomplek, CB_GETCURSEL, 0, 0);
        if (selCompIdx == CB_ERR) {
          MessageBoxW(hWnd, L"Silakan pilih komplek/blok terlebih dahulu!",
                      L"Input Invalid", MB_ICONWARNING);
          break;
        }

        wchar_t compW[128];
        SendMessageW(hwndAddComboKomplek, CB_GETLBTEXT, selCompIdx,
                     (LPARAM)compW);

        wstring wsNama(namaW);
        wstring wsComp(compW);
        wstring wsJarakMasuk(jarakMasukW);
        wstring wsJarakLain(jarakLainW);

        string name(wsNama.begin(), wsNama.end());
        string compName(wsComp.begin(), wsComp.end());
        int distMasuk =
            SafeStoi(string(wsJarakMasuk.begin(), wsJarakMasuk.end()));
        string otherDists(wsJarakLain.begin(), wsJarakLain.end());

        if (name.empty()) {
          MessageBoxW(hWnd, L"Nama pelanggan tidak boleh kosong!",
                      L"Input Invalid", MB_ICONWARNING);
          break;
        }

        map<string, int> otherDistMap = ParseJarakLain(otherDists);

        if (db.TambahPelanggan(name, compName, distMasuk, otherDistMap)) {
          MessageBoxW(hWnd, L"Pelanggan berhasil ditambahkan!", L"Sukses",
                      MB_ICONINFORMATION);
          SetWindowTextW(hwndAddEditNama, L"");
          SetWindowTextW(hwndAddEditJarakMasuk, L"");
          SetWindowTextW(hwndAddEditJarakLain, L"");
          ReloadAllUIs();
        } else {
          MessageBoxW(
              hWnd,
              L"Gagal menambahkan pelanggan. Nama mungkin duplikat atau "
              L"tidak valid.",
              L"Error", MB_ICONERROR);
        }
      } catch (...) {
        MessageBoxW(hWnd,
                    L"Terjadi kesalahan internal saat menambah pelanggan.",
                    L"Error", MB_ICONERROR);
      }
    } break;

    // CRUD: Simpan Perubahan Edit
    case ID_BTN_CRUD_SAVE: {
      try {
        int selIdx = SendMessage(hwndEditListPelanggan, LB_GETCURSEL, 0, 0);
        if (selIdx == LB_ERR) {
          MessageBoxW(
              hWnd,
              L"Silakan pilih pelanggan yang ingin diedit di daftar atas!",
              L"Input Invalid", MB_ICONWARNING);
          break;
        }

        wchar_t originalW[256];
        SendMessageW(hwndEditListPelanggan, LB_GETTEXT, selIdx,
                     (LPARAM)originalW);
        wstring wsOldName(originalW);
        string oldName(wsOldName.begin(), wsOldName.end());

        int selCompIdx = SendMessage(hwndEditComboKomplek, CB_GETCURSEL, 0, 0);
        if (selCompIdx == CB_ERR) {
          MessageBoxW(hWnd, L"Silakan pilih komplek terlebih dahulu!",
                      L"Input Invalid", MB_ICONWARNING);
          break;
        }
        wchar_t compW[128];
        SendMessageW(hwndEditComboKomplek, CB_GETLBTEXT, selCompIdx,
                     (LPARAM)compW);
        wstring wsComp(compW);
        string compName(wsComp.begin(), wsComp.end());

        wchar_t newNamaW[128], jarakMasukW[128], jarakLainW[512];
        GetWindowTextW(hwndEditEditNama, newNamaW, 128);
        GetWindowTextW(hwndEditEditJarakMasuk, jarakMasukW, 128);
        GetWindowTextW(hwndEditEditJarakLain, jarakLainW, 512);

        wstring wsNewName(newNamaW);
        wstring wsJarakMasuk(jarakMasukW);
        wstring wsJarakLain(jarakLainW);

        string newName(wsNewName.begin(), wsNewName.end());
        int distMasuk =
            SafeStoi(string(wsJarakMasuk.begin(), wsJarakMasuk.end()));
        string otherDists(wsJarakLain.begin(), wsJarakLain.end());

        if (newName.empty()) {
          MessageBoxW(hWnd, L"Nama pelanggan baru tidak boleh kosong!",
                      L"Input Invalid", MB_ICONWARNING);
          break;
        }

        map<string, int> otherDistMap = ParseJarakLain(otherDists);

        if (db.EditPelanggan(oldName, newName, compName, distMasuk,
                             otherDistMap)) {
          MessageBoxW(hWnd, L"Informasi pelanggan berhasil diubah!", L"Sukses",
                      MB_ICONINFORMATION);
          SetWindowTextW(hwndEditEditNama, L"");
          SetWindowTextW(hwndEditEditJarakMasuk, L"");
          SetWindowTextW(hwndEditEditJarakLain, L"");
          ReloadAllUIs();
        } else {
          MessageBoxW(hWnd,
                      L"Gagal menyimpan perubahan. Nama baru mungkin duplikat "
                      L"atau tidak valid.",
                      L"Error", MB_ICONERROR);
        }
      } catch (...) {
        MessageBoxW(hWnd,
                    L"Terjadi kesalahan internal saat menyimpan perubahan.",
                    L"Error", MB_ICONERROR);
      }
    } break;

    // CRUD: Hapus Pelanggan
    case ID_BTN_CRUD_DELETE: {
      try {
        int selIdx = SendMessage(hwndEditListPelanggan, LB_GETCURSEL, 0, 0);
        if (selIdx == LB_ERR) {
          MessageBoxW(
              hWnd,
              L"Silakan pilih pelanggan yang ingin dihapus di daftar atas!",
              L"Input Invalid", MB_ICONWARNING);
          break;
        }

        wchar_t originalW[256];
        SendMessageW(hwndEditListPelanggan, LB_GETTEXT, selIdx,
                     (LPARAM)originalW);
        wstring wsName(originalW);
        string name(wsName.begin(), wsName.end());

        int selCompIdx = SendMessage(hwndEditComboKomplek, CB_GETCURSEL, 0, 0);
        if (selCompIdx == CB_ERR) {
          MessageBoxW(hWnd, L"Silakan pilih komplek terlebih dahulu!",
                      L"Input Invalid", MB_ICONWARNING);
          break;
        }
        wchar_t compW[128];
        SendMessageW(hwndEditComboKomplek, CB_GETLBTEXT, selCompIdx,
                     (LPARAM)compW);
        wstring wsComp(compW);
        string compName(wsComp.begin(), wsComp.end());

        wstring confirmMsg = L"Apakah Anda yakin ingin menghapus pelanggan \"" +
                             wstring(name.begin(), name.end()) + L"\" di " +
                             wstring(compName.begin(), compName.end()) + L"?";
        int confirm = MessageBoxW(hWnd, confirmMsg.c_str(), L"Konfirmasi Hapus",
                                  MB_YESNO | MB_ICONWARNING);
        if (confirm == IDYES) {
          if (db.HapusPelanggan(name, compName)) {
            MessageBoxW(hWnd, L"Pelanggan berhasil dihapus!", L"Sukses",
                        MB_ICONINFORMATION);
            SetWindowTextW(hwndEditEditNama, L"");
            SetWindowTextW(hwndEditEditJarakMasuk, L"");
            SetWindowTextW(hwndEditEditJarakLain, L"");
            ReloadAllUIs();
          } else {
            MessageBoxW(hWnd, L"Gagal menghapus pelanggan.", L"Error",
                        MB_ICONERROR);
          }
        }
      } catch (...) {
        MessageBoxW(hWnd,
                    L"Terjadi kesalahan internal saat menghapus pelanggan.",
                    L"Error", MB_ICONERROR);
      }
    } break;
    }
  } break;

  case WM_DESTROY:
    // Remove subclasses
    RemoveWindowSubclass(hwndMenuHome, ButtonSubclassProc, 0);
    RemoveWindowSubclass(hwndMenuKomplek, ButtonSubclassProc, 0);
    RemoveWindowSubclass(hwndMenuRute, ButtonSubclassProc, 0);
    RemoveWindowSubclass(hwndMenuCrud, ButtonSubclassProc, 0);
    RemoveWindowSubclass(hwndMenuExit, ButtonSubclassProc, 0);
    RemoveWindowSubclass(hwndBtnSelectAll, ButtonSubclassProc, 0);
    RemoveWindowSubclass(hwndBtnReset, ButtonSubclassProc, 0);
    RemoveWindowSubclass(hwndBtnGenerate, ButtonSubclassProc, 0);
    RemoveWindowSubclass(hwndBtnAdd, ButtonSubclassProc, 0);
    RemoveWindowSubclass(hwndBtnSave, ButtonSubclassProc, 0);
    RemoveWindowSubclass(hwndBtnDelete, ButtonSubclassProc, 0);
    RemoveWindowSubclass(hwndBtnKomplekBack, ButtonSubclassProc, 0);
    RemoveWindowSubclass(hwndBtnRuteBack, ButtonSubclassProc, 0);
    RemoveWindowSubclass(hwndBtnCrudBack, ButtonSubclassProc, 0);

    PostQuitMessage(0);
    break;

  default:
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
  }
  return 0;
}

// Switch between content panels
void SwitchPanel(ActivePanel panel) {
  activePanel = panel;

  // Hide all first
  for (HWND hwnd : hwndHomeCtrls)
    ShowWindow(hwnd, SW_HIDE);
  for (HWND hwnd : hwndKomplekCtrls)
    ShowWindow(hwnd, SW_HIDE);
  for (HWND hwnd : hwndRuteCtrls)
    ShowWindow(hwnd, SW_HIDE);
  for (HWND hwnd : hwndCrudCtrls)
    ShowWindow(hwnd, SW_HIDE);

  // Show active
  switch (panel) {
  case PANEL_HOME:
    for (HWND hwnd : hwndHomeCtrls)
      ShowWindow(hwnd, SW_SHOW);
    break;
  case PANEL_KOMPLEK:
    for (HWND hwnd : hwndKomplekCtrls)
      ShowWindow(hwnd, SW_SHOW);
    break;
  case PANEL_RUTE:
    for (HWND hwnd : hwndRuteCtrls)
      ShowWindow(hwnd, SW_SHOW);
    break;
  case PANEL_CRUD:
    for (HWND hwnd : hwndCrudCtrls)
      ShowWindow(hwnd, SW_SHOW);
    break;
  }

  // Repaint window
  InvalidateRect(hwndMain, NULL, TRUE);
}

// Reload lists and labels after data modification
void ReloadAllUIs() {
  // 1. Dashboard counts
  wstring compText =
      L"Total Komplek\n" + to_wstring_custom(db.KomplekList.size());
  SetWindowTextW(hwndHomeCard1, compText.c_str());

  wstring custText =
      L"Total Pelanggan\n" + to_wstring_custom(db.PelangganList.size());
  SetWindowTextW(hwndHomeCard2, custText.c_str());

  // 2. Complex List (Informasi Komplek)
  SendMessage(hwndKomplekList, LB_RESETCONTENT, 0, 0);
  for (const auto &comp : db.KomplekList) {
    wstring compW(comp.begin(), comp.end());
    SendMessageW(hwndKomplekList, LB_ADDSTRING, 0, (LPARAM)compW.c_str());
  }
  SendMessage(hwndCustDetailList, LB_RESETCONTENT, 0, 0);
  SetWindowTextW(hwndSearchEdit, L"");

  // 3. Checklist ListView (Route Planner)
  ListView_DeleteAllItems(hwndCustChecklist);
  int lvIdx = 0;
  for (const auto &p : db.PelangganList) {
    string displayStr = p.Nama + " [" + p.Komplek + "]";
    wstring displayW(displayStr.begin(), displayStr.end());

    LVITEMW item = {0};
    item.mask = LVIF_TEXT;
    item.iItem = lvIdx;
    item.iSubItem = 0;
    item.pszText = (LPWSTR)displayW.c_str();

    ListView_InsertItem(hwndCustChecklist, &item);
    ListView_SetCheckState(hwndCustChecklist, lvIdx, FALSE);
    lvIdx++;
  }

  // 4. ComboBox Complexes (CRUD Add)
  SendMessage(hwndAddComboKomplek, CB_RESETCONTENT, 0, 0);
  for (const auto &comp : db.KomplekList) {
    if (comp == "Depot")
      continue; // Tidak bisa tambah pelanggan ke depot langsung
    wstring compW(comp.begin(), comp.end());
    SendMessageW(hwndAddComboKomplek, CB_ADDSTRING, 0, (LPARAM)compW.c_str());
  }

  // 5. ComboBox Complexes (CRUD Edit)
  SendMessage(hwndEditComboKomplek, CB_RESETCONTENT, 0, 0);
  for (const auto &comp : db.KomplekList) {
    if (comp == "Depot")
      continue;
    wstring compW(comp.begin(), comp.end());
    SendMessageW(hwndEditComboKomplek, CB_ADDSTRING, 0, (LPARAM)compW.c_str());
  }

  // 6. Reset ListBox Customers (CRUD Edit/Delete)
  SendMessage(hwndEditListPelanggan, LB_RESETCONTENT, 0, 0);
}

// Run routing logic and output results
void GenerateDeliveryRoute() {
  int itemCount = ListView_GetItemCount(hwndCustChecklist);
  vector<string> checkedCustomers;

  for (int i = 0; i < itemCount; i++) {
    if (ListView_GetCheckState(hwndCustChecklist, i)) {
      wchar_t text[256];
      ListView_GetItemText(hwndCustChecklist, i, 0, text, 256);
      wstring wstr(text);
      string itemStr(wstr.begin(), wstr.end());

      size_t bracket = itemStr.find(" [");
      if (bracket != string::npos) {
        string name = itemStr.substr(0, bracket);
        checkedCustomers.push_back(name);
      }
    }
  }

  if (checkedCustomers.empty()) {
    SetWindowTextW(hwndResultOutput,
                   L"Error:\r\nPilih minimal satu pelanggan di daftar sebelah "
                   L"kiri untuk membuat rute!");
    return;
  }

  // Find parent complexes
  vector<string> targetComplexes;
  for (const auto &name : checkedCustomers) {
    for (const auto &p : db.PelangganList) {
      if (p.Nama == name) {
        if (find(targetComplexes.begin(), targetComplexes.end(), p.Komplek) ==
            targetComplexes.end()) {
          targetComplexes.push_back(p.Komplek);
        }
        break;
      }
    }
  }

  auto grafMakro = db.AmbilGrafMakro();
  vector<int> blokTujuanIndices;
  for (const auto &compName : targetComplexes) {
    auto it = find(db.KomplekList.begin(), db.KomplekList.end(), compName);
    if (it != db.KomplekList.end()) {
      blokTujuanIndices.push_back(distance(db.KomplekList.begin(), it));
    }
  }

  // Run Macro Router
  RuteHasil hasilMakro = HitungRuteMakro(grafMakro, 0, blokTujuanIndices);

  wstringstream custSummary;
  bool firstCust = true;
  int totalJarakMikro = 0;
  wstringstream microReport;

  for (int nodeIdx : hasilMakro.RuteNode) {
    if (nodeIdx == 0)
      continue; // Skip Depot

    string compName = db.KomplekList[nodeIdx];

    vector<string> customersInComplex;
    for (const auto &name : checkedCustomers) {
      for (const auto &p : db.PelangganList) {
        if (p.Nama == name && p.Komplek == compName) {
          customersInComplex.push_back(name);
          break;
        }
      }
    }

    if (customersInComplex.empty())
      continue;

    vector<string> namaNodes;
    auto grafMikro = db.AmbilGrafMikro(compName, namaNodes);

    vector<int> targetCustIndices;
    for (const auto &name : customersInComplex) {
      auto it = find(namaNodes.begin(), namaNodes.end(), name);
      if (it != namaNodes.end()) {
        targetCustIndices.push_back(distance(namaNodes.begin(), it));
      }
    }

    // Run Micro Router
    RuteHasil hasilMikro = HitungRuteMikro(grafMikro, 0, targetCustIndices);
    totalJarakMikro += hasilMikro.TotalJarak;

    // Catat urutan nama pelanggan saja
    for (size_t i = 0; i < hasilMikro.RuteNode.size(); i++) {
      int microNode = hasilMikro.RuteNode[i];
      if (microNode != 0) { // Bukan pintu masuk
        string custName = namaNodes[microNode];
        if (!firstCust) {
          custSummary << L" -> ";
        }
        custSummary << wstring(custName.begin(), custName.end());
        firstCust = false;
      }
    }

    microReport << L"[" << wstring(compName.begin(), compName.end())
                << L"]:\r\n";
    microReport << L"  Jalur: ";
    for (size_t i = 0; i < hasilMikro.RuteNode.size(); i++) {
      string nodeName = namaNodes[hasilMikro.RuteNode[i]];
      microReport << wstring(nodeName.begin(), nodeName.end());
      if (i < hasilMikro.RuteNode.size() - 1) {
        microReport << L" -> ";
      }
    }
    microReport << L"\r\n  Jarak Komplek: " << hasilMikro.TotalJarak
                << L" meter\r\n\r\n";
  }

  wstringstream report;
  report << L"=======================================\r\n";
  report << L"          RUTE PENGIRIMAN OPTIMAL\r\n";
  report << L"=======================================\r\n\r\n";

  report << L"URUTAN KUNJUNGAN PELANGGAN:\r\n";
  if (firstCust) {
    report << L"(Tidak ada pelanggan dikunjungi)\r\n\r\n";
  } else {
    report << custSummary.str() << L"\r\n\r\n";
  }
  report << L"---------------------------------------\r\n\r\n";

  report << L"RUTE MAKRO (Antar Komplek):\r\n";
  for (size_t i = 0; i < hasilMakro.RuteNode.size(); i++) {
    string blockName = db.KomplekList[hasilMakro.RuteNode[i]];
    report << wstring(blockName.begin(), blockName.end());
    if (i < hasilMakro.RuteNode.size() - 1) {
      report << L" -> ";
    }
  }
  report << L"\r\nJarak Makro: " << hasilMakro.TotalJarak << L" meter\r\n\r\n";

  report << L"RUTE MIKRO (Detail Dalam Komplek):\r\n";
  report << L"---------------------------------------\r\n";
  report << microReport.str();

  report << L"=======================================\r\n";
  report << L"TOTAL DISTRIBUSI: " << (hasilMakro.TotalJarak + totalJarakMikro)
         << L" meter\r\n";
  report << L"=======================================\r\n";

  SetWindowTextW(hwndResultOutput, report.str().c_str());
}

// Helpers for data format
string AmbilJarakLainFormat(const string &nama, const string &komplek) {
  string result = "";
  for (const auto &jp : db.JarakPelangganList) {
    if (jp.Komplek == komplek) {
      if (jp.Dari == nama && jp.Ke != "Masuk " + komplek) {
        if (!result.empty())
          result += ", ";
        result += jp.Ke + ":" + to_string(jp.Jarak);
      } else if (jp.Ke == nama && jp.Dari != "Masuk " + komplek) {
        if (!result.empty())
          result += ", ";
        result += jp.Dari + ":" + to_string(jp.Jarak);
      }
    }
  }
  return result;
}

map<string, int> ParseJarakLain(const string &input) {
  map<string, int> result;
  stringstream ss(input);
  string token;
  while (getline(ss, token, ',')) {
    while (!token.empty() && token.front() == ' ')
      token.erase(token.begin());
    while (!token.empty() && token.back() == ' ')
      token.pop_back();
    if (token.empty())
      continue;

    size_t colon = token.find(':');
    if (colon != string::npos) {
      string nama = token.substr(0, colon);
      string jarakStr = token.substr(colon + 1);
      while (!nama.empty() && nama.back() == ' ')
        nama.pop_back();
      while (!jarakStr.empty() && jarakStr.front() == ' ')
        jarakStr.erase(jarakStr.begin());

      int jarak = SafeStoi(jarakStr, 9999999);
      result[nama] = jarak;
    }
  }
  return result;
}

int AmbilJarakKeMasuk(const string &nama, const string &komplek) {
  string masuk = "Masuk " + komplek;
  for (const auto &jp : db.JarakPelangganList) {
    if (jp.Komplek == komplek) {
      if ((jp.Dari == masuk && jp.Ke == nama) ||
          (jp.Dari == nama && jp.Ke == masuk)) {
        return jp.Jarak;
      }
    }
  }
  return 9999999;
}

// Entry Point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
  // Enable visual styles
  INITCOMMONCONTROLSEX icex;
  icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
  icex.dwICC = ICC_WIN95_CLASSES | ICC_STANDARD_CLASSES;
  InitCommonControlsEx(&icex);

  // Create GDI Brushes
  brushBg = CreateSolidBrush(colorBg);
  brushSidebar = CreateSolidBrush(colorSidebar);
  brushCard = CreateSolidBrush(colorCard);
  brushPrimary = CreateSolidBrush(colorPrimary);
  brushSuccess = CreateSolidBrush(colorSuccess);
  brushDanger = CreateSolidBrush(colorDanger);

  // Create UI Fonts
  hFontMain =
      CreateFontW(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                  OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                  DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
  hFontBold =
      CreateFontW(16, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                  OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                  DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
  hFontTitle =
      CreateFontW(24, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
                  DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                  DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");

  // Register Class
  WNDCLASSEXW wcex = {0};
  wcex.cbSize = sizeof(WNDCLASSEXW);
  wcex.style = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc = MainWndProc;
  wcex.hInstance = hInstance;
  wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
  wcex.hbrBackground = brushBg;
  wcex.lpszClassName = L"AquantarMainWindow";

  if (!RegisterClassExW(&wcex)) {
    MessageBoxW(NULL, L"Gagal mendaftarkan kelas Window!", L"Fatal Error",
                MB_ICONERROR);
    return 1;
  }

  // Create Window
  // Centered window on screen
  int screenWidth = GetSystemMetrics(SM_CXSCREEN);
  int screenHeight = GetSystemMetrics(SM_CYSCREEN);
  int winWidth = 1000;
  int winHeight = 700;
  int posX = (screenWidth - winWidth) / 2;
  int posY = (screenHeight - winHeight) / 2;

  hwndMain = CreateWindowExW(
      0, L"AquantarMainWindow",
      L"Aquantar - Sistem Manajemen & Optimasi Rute Pengiriman Air",
      WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, posX, posY,
      winWidth, winHeight, NULL, NULL, hInstance, NULL);

  if (!hwndMain) {
    MessageBoxW(NULL, L"Gagal membuat Window!", L"Fatal Error", MB_ICONERROR);
    return 1;
  }

  ShowWindow(hwndMain, nCmdShow);
  UpdateWindow(hwndMain);

  // Message Loop
  MSG msg;
  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  // Cleanup GDI
  DeleteObject(brushBg);
  DeleteObject(brushSidebar);
  DeleteObject(brushCard);
  DeleteObject(brushPrimary);
  DeleteObject(brushSuccess);
  DeleteObject(brushDanger);
  DeleteObject(hFontMain);
  DeleteObject(hFontBold);
  DeleteObject(hFontTitle);

  return (int)msg.wParam;
}