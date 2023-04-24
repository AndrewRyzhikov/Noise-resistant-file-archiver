#include "archiver.h"

void MakeNewSymForEncode(const char* sym, char* buff_writer) {
    std::vector<bool> bin_arr_left(4);
    std::vector<bool> bin_arr_right(4);

    for (int i = 0; i <= 3; ++i) {
        bin_arr_right[3 - i] = (((1 << i) & (sym[0])) >> i);
    }

    for (int i = 4; i < 8; ++i) {
        bin_arr_left[7 - i] = (((1 << i) & (sym[0])) >> i);
    }

    buff_writer[0] = HammingEncode(bin_arr_left);
    buff_writer[1] = HammingEncode(bin_arr_right);
}

std::string ListFileName(const std::string& arch_name, const std::string& p) {
    std::ifstream arch;
    arch.open(p + arch_name);

    std::string list;
    long long position = 0;
    while (position < CountSizeFile(arch_name, p)) {
        char buff[1];
        arch.read(buff, 1);
        while (buff[0] != ' ') {
            list += buff[0];
            arch.read(buff, 1);
            position++;
        }
        list += " ";

        std::string count;
        arch.read(buff, 1);
        while (buff[0] != '\n') {
            count += buff[0];
            position++;
            arch.read(buff, 1);
        }

        long long remove = stoi(count);
        position += (remove + 3);
        arch.seekg(position);
    }
    if (list.empty()) {
        return "Archive is empty";
    }
    return list;
}

long long CountSizeFile(const std::string& file_name, const std::string& p) {
    std::ifstream file;
    file.open(p + file_name);
    file.seekg(0, std::ios::end);
    long long length = file.tellg();
    return length;
}

std::string AddInfo(const std::string& file_name, const long long& file_size) {
    return file_name + " " + std::to_string(file_size * 2);
}

void CreateArch(const std::string& arch_name, const std::string& p) {
    std::ofstream arch;
    std::string path = p + arch_name;
    arch.open(path);
}

char HammingEncode(std::vector<bool>& side) {
    std::vector<bool> result(8);
    result[3] = side[0];
    result[5] = side[1];
    result[6] = side[2];
    result[7] = side[3];
    result[1] = result[3] ^ result[5] ^ result[7];
    result[2] = result[3] ^ result[6] ^ result[7];
    result[4] = result[5] ^ result[6] ^ result[7];
    result[0] = result[1] ^ result[2] ^ result[3] ^ result[4] ^ result[5] ^ result[6] ^ result[7];

    int number_sym = 0;
    int two_in_degree = 1;
    for (int i = 0; i < 8; i++) {
        number_sym += two_in_degree * result[7 - i];
        two_in_degree *= 2;
    }
    return char(number_sym);
}

void AddFile(const std::string& file_name, const std::string& arch_name, const std::string& p) {
    std::ifstream file;
    file.open(p + file_name, std::ios::in | std::ios::binary);
    long long sizeFile = CountSizeFile(file_name, p);

    std::ofstream arch;
    arch.open(p + arch_name, std::ios::app | std::ios::binary);

    if (CountSizeFile(arch_name, p) != 0) {
        arch << '\n';
    }
    arch << AddInfo(file_name, sizeFile) << '\n';
    char* buff = new char[1];
    char* buff_write = new char[2];
    for (long long i = 0; i < sizeFile; ++i) {
        file.read(buff, 1);
        MakeNewSymForEncode(buff, buff_write);
        arch.write(buff_write, 2);
    }
}

std::pair<long long, long long> SearchSidesFile(const std::string& arch_name, const std::string& file_name, const std::string& p) {
    std::ifstream arch;
    arch.open(p + arch_name);

    std::pair<long long, long long> sidesFile;
    bool left_side_exist = false;
    long long position = 0;
    while (position < CountSizeFile(arch_name, p)) {
        char buff[1];
        std::string current_file_name;
        arch.read(buff, 1);
        while (buff[0] != ' ') {
            current_file_name += buff[0];
            arch.read(buff, 1);
            position++;
        }

        if (current_file_name == file_name) {
            sidesFile.first = position - file_name.size();
            left_side_exist = true;
        }

        std::string count;
        arch.read(buff, 1);
        while (buff[0] != '\n') {
            count += buff[0];
            position++;
            arch.read(buff, 1);
        }

        long long remove = stoi(count);
        position += (remove + 3);

        if (left_side_exist) {
            sidesFile.second = position;
            return sidesFile;
        }

        arch.seekg(position);
    }
}

void CheckHamming(const std::vector<bool>& binary_form) {
    int number_fail = 0;
    int count_fail = 0;
    std::vector<bool> parity_bits(8);
    parity_bits[1] = binary_form[3] ^ binary_form[5] ^ binary_form[7];
    if (parity_bits[1] != binary_form[1]) {
        number_fail += 1;
        count_fail++;
    }
    parity_bits[2] = binary_form[3] ^ binary_form[6] ^ binary_form[7];
    if (parity_bits[2] != binary_form[2]) {
        number_fail += 2;
        count_fail++;
    }
    parity_bits[4] = binary_form[5] ^ binary_form[6] ^ binary_form[7];
    if (parity_bits[4] != binary_form[4]) {
        number_fail += 4;
        count_fail++;
    }
    if (count_fail > 0) {
        binary_form[number_fail] = not(binary_form[number_fail]);
    }
}

char HammingDecode(const char* glued_letters) {
    std::vector<bool> count_sym;
    for (int i = 0; i < 2; i++) {
        char now = glued_letters[i];
        std::vector<bool> binary_form(8);
        for (int z = 0; z < 8; ++z) {
            binary_form[7 - z] = (((1 << z) & (now)) >> z);
        }
        CheckHamming(binary_form);
        count_sym.push_back(binary_form[3]);
        count_sym.push_back(binary_form[5]);
        count_sym.push_back(binary_form[6]);
        count_sym.push_back(binary_form[7]);
    }
    return MakeNewSymDecode(count_sym);
}

char MakeNewSymDecode(const std::vector<bool>& count_sym_bool) {
    int count_sym = 0;
    int two_in_degree = 1;
    for (int i = 0; i < 8; i++) {
        count_sym += two_in_degree * count_sym_bool[7 - i];
        two_in_degree *= 2;
    }
    return char(count_sym);
}

void Extract(const std::string& arch_name, const std::string& file_name, const std::string& p_arch, const std::string& p_file) {
    std::ifstream arch;
    arch.open(p_arch + arch_name, std::ios::binary | std::ios::in);

    std::ofstream file;
    file.open(p_file + file_name, std::ios::binary | std::ios::app);

    std::pair<long long, long long> side_file;
    side_file = SearchSidesFile(arch_name, file_name, p_arch);
    arch.seekg(side_file.first + file_name.size() + 1);

    std::string size_file;
    char buff[1];
    arch.read(buff, 1);
    std::cout << buff;

    while (buff[0] != '\n') {
        size_file += buff[0];
        arch.read(buff, 1);
    }

    long long side = std::stoi(size_file);
    side = side / 2;

    char* buff_read = new char[2];
    char buff_write[1];
    for (long long i = 0; i < side; i++) {
        arch.read(buff_read, 2);
        buff_write[0] = HammingDecode(buff_read);
        file.write(buff_write, 1);
    }
}
