#include <fstream>
#include <iostream>
#include <string>
#include <vector>

char HammingEncode(std::vector<bool>& side);

void MakeNewSymForEncode(const char* sym, char* buff_writer);

void CreateArch(const std::string& arch_name, const std::string& p);

void AddFile(const std::string& file_name, const std::string& arch_name, const std::string& p);

std::string AddInfo(const std::string& file_name, const long long& file_size);

long long CountSizeFile(const std::string& file_name, const std::string& p);

std::string ListFileName(const std::string& arch_name, const std::string& p);

std::pair<long long, long long> SearchSidesFile(const std::string& arch_name, const std::string& file_name, const std::string& p);

void CheckHamming(std::vector<bool>& binary_form);

char HammingDecode(const char* glued_letters);

char MakeNewSymDecode(std::vector<bool>& count_sym_bool);

void Extract(const std::string& arch_name, const std::string& file_name,  const std::string& p_arch, const std::string& p_file);

