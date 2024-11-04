#ifndef SDB_ELF_HPP
#define SDB_ELF_HPP

#include <filesystem>
#include <elf.h>
#include <vector>
#include <string_view>
#include <unordered_map>
#include <optional>
#include <libsdb/types.hpp>

namespace sdb {
    class elf {
    public:
        elf(const std::filesystem::path& path);
        ~elf();

        elf(const &elf) = delete;
        elf& operator=(const elf&) = delete;

        std::string_view get_section_name(std::size_t index) const;

        std::filesystem::path path() const { return path_; }
        const Elf64_Ehdr& get_header() const { return header_; }

        std::optional<const Elf64_Shdr*> 
        get_section(std::string_view name) const;
        span<const std::byte> get_section_contents(std::string_view name) const;

    private:
        void parse_section_headers();
        void build_section_map();

        int fd_;
        std::filesystem::path path_;
        std::size_t file_size_;
        std::byte* data_;
        Elf64_Ehdr header_;
        std::vector<Elf64_Shdr> section_headers_;
        std::unordered_map<std::string_view, Elf64_Shdr*> section_map_;
    };
}

#endif
