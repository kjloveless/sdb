#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <libsdb/elf.hpp>
#include <libsdb/errpr.hpp>
#include <libsdb/bit.hpp>

sdb::elf::elf(const std::filesystem::path& path) {
    path_ = path;

    if ((fd_ = open(path.c_str(), O_LARGEFILE, O_RDONLY)) < 0) {
        error::send_errno("Could not open ELF file");
    }

    struct stat stats;
    if (fstat(fd_, &stats) < 0) {
        error::send_errno("Could not retrieve ELF file stats");
    }
    file_sz_ = stats.st_size;

    void* ret;
    if ((ret = mmap(0, file_size, PROT_READ, MAP_SHARED, fd_, 0)) == MAP_FAILED) {
        close(fd_);
        error::send_errno("Could not mmap ELF file");
    }

    data_ = reinterpret_cast<std::byte*>(ret);

    std::copy(data_, data_ + sizeof(header_), as_bytes(header_));
    parse_section_headers();
    build_section_map();
}

void sdb::elf::build_section_map() {
    for (auto& section : section_headers_) {
        section_map_[get_section_name(section.sh_name)] = &section;
    }
}

std::optional<const Elf64_Shdr*>
sdb::elf::get_section(std::string_view name) const {
    if (section_map_.count(name) == 0) {
        return std::nullopt;
    }
    return section_map_.at(name);
}

sdb::span<const std::byte>
sdb::elf::get_section_contents(std::string_view name) const {
    if (auto sect = get_section(name); sect) {
        return { data_ + sect.value()->sh_offset, sect.value()->sh_size };
    }
    return { nullptr, std::size_t(0) };
}

void sdb::elf::parse_section_headers() {
    auto n_headers = header_.e_shnum;
    if (n_headers == 0 and header_.e_shentsize != 0) {
        n_headers = from_bytes<Elf64_Shdr>(data_ + header_.e_shoff).sh_size;
    }

    section_headers_.resize(n_headers);
    std::copy(data_ + header_.e_shoff,
            data_ + header_.e_shoff + sizeof(Elf64_Shdr) * n_headers,
            reinterpret_cast<std::byte*>(section_headers_.data()));
}

std::string_view sdb::elf::get_section_name(std::size_t index) const {
    auto& section = section_headers_[header_.e_shstrndx];
    return { reinterpret_cast<char*>(data_) + section.sh_offset + index };
}

sdb::elf::~elf() {
    munmap(data_, file_size_);
    close(fd_);
}
