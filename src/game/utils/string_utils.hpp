#pragma once

#include <algorithm>
#include <sstream>
#include <string>

namespace utils::str {
	template <typename ... Args>
	static std::string string_format(const std::string& format, Args ... args) {
		const size_t size = snprintf(nullptr, 0, format.c_str(), args ...) + 1;
		if (size <= 0) { throw std::runtime_error("Error during formatting."); }
		const std::unique_ptr<char[]> buf(new char[size]);
		snprintf(buf.get(), size, format.c_str(), args ...);
		return std::string(buf.get(), buf.get() + size - 1);
	}


	static std::string replace(std::string str, const std::string from, const std::string to) {
		while (true) {
			const size_t found = str.find(from);
			if (found == std::string::npos) break;
			str = str.replace(found, from.size(), to);
		}
		return str;
	}

	static std::string uppercase(std::string str) {
		std::ranges::transform(str, str.begin(), toupper);
		return str;
	}

	static std::string lowercase(std::string str) {
		std::ranges::transform(str, str.begin(), tolower);
		return str;
	}

	static std::string capitalize(std::string str) {
		return string_format("%c%s", toupper(str[0]), str.substr(1).c_str());
	}
}
