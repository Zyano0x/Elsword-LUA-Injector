#include "pch.h"

std::uint8_t* FindSignature(LPCSTR module_name, const std::string& byte_array)
{
	HMODULE module = GetModuleHandleA(module_name);

	if (!module)
		return nullptr;

	static const auto pattern_to_byte = [](const char* pattern)
		{
			auto bytes = std::vector<int>{};
			const auto start = const_cast<char*>(pattern);
			const auto end = const_cast<char*>(pattern) + std::strlen(pattern);

			for (auto current = start; current < end; ++current)
			{
				if (*current == '?')
				{
					++current;

					if (*current == '?')
						++current;

					bytes.push_back(-1);
				}
				else
				{
					bytes.push_back(std::strtoul(current, &current, 16));
				}
			}
			return bytes;
		};

	const auto dos_header = reinterpret_cast<PIMAGE_DOS_HEADER>(module);
	const auto nt_headers =
		reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<std::uint8_t*>(module) + dos_header->e_lfanew);

	const auto size_of_image = nt_headers->OptionalHeader.SizeOfImage;
	const auto pattern_bytes = pattern_to_byte(byte_array.c_str());
	const auto scan_bytes = reinterpret_cast<std::uint8_t*>(module);

	const auto pattern_size = pattern_bytes.size();
	const auto pattern_data = pattern_bytes.data();

	for (auto i = 0ul; i < size_of_image - pattern_size; ++i)
	{
		auto found = true;

		for (auto j = 0ul; j < pattern_size; ++j)
		{
			if (scan_bytes[i + j] == pattern_data[j] || pattern_data[j] == -1)
				continue;
			found = false;
			break;
		}
		if (!found)
			continue;
		return &scan_bytes[i];
	}

	return nullptr;
}

Signature::Signature(const std::string& sig)
{
	this->Imported = false;
	this->Sig = sig;
}

Signature Signature::Import(const std::string& module_name)
{
	this->Imported = true;
	this->Module = module_name;
	this->Pointer = (uint64_t)FindSignature(this->Module.data(), this->Sig);
	return *this;
}

Signature Signature::Add(uint32_t value)
{
	if (!this->Imported)
		*this = this->Import();

	this->Pointer += value;
	return *this;
}

Signature Signature::Sub(uint32_t value)
{
	if (!this->Imported)
		*this = this->Import();

	this->Pointer -= value;
	return *this;
}

Signature Signature::Dump(const std::string& func_name)
{
	if (!this->Imported)
		*this = this->Import();

	return *this;
}

Signature Signature::Instruction(uint32_t offset)
{
	if (!this->Imported)
		*this = this->Import();

	this->Pointer = *(int*)(this->Pointer + offset) + this->Pointer;
	return *this;
}

#ifdef _WIN64
uint64_t Signature::GetPointer()
{
	if (!this->Imported)
		*this = this->Import();
	return this->Pointer;
}
#else
uint32_t Signature::GetPointer()
{
	if (!this->imported)
		*this = this->import();
	return this->pointer;
}
#endif