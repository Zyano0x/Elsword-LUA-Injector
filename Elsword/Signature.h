#pragma once

std::uint8_t* FindSignature(LPCSTR module_name, const std::string& byte_array);

class Signature
{
public:

	Signature(const std::string& sig);

	Signature Import(const std::string& module_name = ("x2.exe"));

	Signature Add(uint32_t offset);
	Signature Sub(uint32_t offset);
	Signature Instruction(uint32_t offset);

	Signature Dump(const std::string& func_name);

#ifdef _WIN64
	uint64_t GetPointer();
#else
	uint32_t GetPointer();
#endif

private:
#ifdef _WIN64
	uint64_t Pointer;
#else
	uint32_t Pointer;
#endif

	bool Imported;
	std::string Sig;
	std::string Module;
};