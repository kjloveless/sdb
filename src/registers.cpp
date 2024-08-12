#include <iostream>
#include <libsdb/process.hpp>
#include <libsdb/registers.hpp>
#include <libsdb/bit.hpp>

sdb::registers::value sdb::registers::read(const register_info& info) const {
    auto bytes = as_bytes(data_);

    if (info.format == register_format::uint) {
        switch (info.size) {
            case 1: return from_bytes<std::uint8_t>(bytes + info.offset);
            case 2: return from_bytes<std::uint16_t>(bytes + info.offset);
            case 4: return from_bytes<std::uint32_t>(bytes + info.offset);
            case 8: return from_bytes<std::uint64_t>(bytes + info.offset);
        }
    } 
    else if (info.format == register_format::double_float) {
        return from_bytes<double>(bytes + info.offset);
    } 
    else if (info.format == register_format::long_double) {
        return from_bytes<long double>(bytes + info.offset);
    } 
    else if (info.format == register_format::vector and info.size == 8) {
        return from_bytes<byte64>(bytes + info.offset);
    } 
    else {
        return from_bytes<byte128>(bytes + info.offset);
    }
}

void sdb::registers::write(const register_info& info, value val) {
    auto bytes = as_bytes(data_);
    std::visit([&info](auto& v) {
        if (sizeof(v) == info.size) {
            auto val_bytes = as_bytes(v);
            std::copy(val_bytes, val_bytes + sizeof(v), bytes + info.offset);
        }
        else {
            std::cerr << "sdb::register::write called with "
                "mismatched register and value sizes";
            std::terminate();
        }
    }, val);
}
