#include <fstream>

#include "cassette.hpp"

CHeader Cassette::Load(const std::string &path, Mem &prg, Mem &chr) {

    std::ifstream file(path, std::ios::binary);

    CHeader header;

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + path);
    }

    if (!file.read((char *)&header, sizeof(CHeader))) {
        throw std::runtime_error("Failed to read header");
    }

    // If a "trainer" exists we just need to read past it before we get to the
    // good stuff
    if (header.mapper1 & 0x04)
        file.seekg(512, std::ios_base::cur);

    // read prg rom
    prg.resize(header.n_chunk_prg * 16384); // 16kb chunks
    chr.resize(header.n_chunk_chr * 8192);  // 8kb chunks

    if (!file.read((char *)prg.data(), prg.size())) {
        throw std::runtime_error("Failed to read prg rom");
    }
    if (!file.read((char *)chr.data(), chr.size())) {
        throw std::runtime_error("Failed to read chr rom");
    }

    return header;
}
