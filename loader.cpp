#include <windows>
#include <iostream>
#include <arpa/inet.h>
#include <fstream>
#include <vector>
#include <cstdint>
#include <cstddef>
#include <iomanip>

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

vector<char> read_private_data_block(string& woff2_file) {
  WOFF2_Header header;
  ifstream file(woff2_file, ios::binary | ios::ate);

  if (!file) {
    throw runtime_error("[!] Impossible to open the WOFF2 file");
  }

  streamsize size = file.tellg();

  file.seekg(0, ios::beg);

  if (!file.read(reinterpret_cast<char*>(&header), sizeof(WOFF2_Header))) {
    throw runtime_error("[!] Impossible to read the Header");
  }

  if (ntohl(header.signature) != 0x774f4632) {
    cerr << "[!] Bad magic\n";
  }

  file.seekg(ntohl(header.privOffset), ios::beg);
  if(!file) {
    throw runtime_error("[!] Invalid Offset");
  }

  vector<char> shellcode(ntohl(header.privLength));
  if(!file.read(shellcode.data(), (int)ntohl(header.privLength))) {
    throw runtime_error("[!] Impossible to read private data block");
  }

  file.close();

  return shellcode;
}

int main (int argc, char *argv[]) {
  
  string filename = "test.woff2";

  vector<char> shellcode = read_private_data_block(filename);

  for (unsigned char c : shellcode) {
    cout << "\\x"
         << hex << setw(2) << setfill('0') << (int)c;
  }

  cout << dec << endl;

  return 0;
}
