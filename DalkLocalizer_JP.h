#ifndef _DALK_LOCALIZER_JP_
#define _DALK_LOCALIZER_JP_

namespace DalkLocalizer {

inline std::wstring SJISToUTF16(std::string const& src)
{
	auto const dest_size = ::MultiByteToWideChar(CP_ACP, 0U, src.data(), -1, nullptr, 0U);

	std::vector<wchar_t> dest(dest_size, L'\0');

	if (::MultiByteToWideChar(CP_ACP, 0U, src.data(), -1, dest.data(), dest.size()) == 0)
	{
		throw std::system_error{ static_cast<int>(::GetLastError()), std::system_category() };
	}

	dest.resize(std::char_traits<wchar_t>::length(dest.data()));
	dest.shrink_to_fit();

	return std::wstring(dest.begin(), dest.end());
}

inline std::string UTF16ToSJIS(std::wstring const& src)
{
	auto const dest_size = ::WideCharToMultiByte(CP_ACP, 0U, src.data(), -1, nullptr, 0, nullptr, nullptr);

	std::vector<char> dest(dest_size, '\0');

	if (::WideCharToMultiByte(CP_ACP, 0U, src.data(), -1, dest.data(), dest.size(), nullptr, nullptr) == 0)
	{
		throw std::system_error{ static_cast<int>(::GetLastError()), std::system_category() };
	}

	dest.resize(std::char_traits<char>::length(dest.data()));
	dest.shrink_to_fit();

	return std::string(dest.begin(), dest.end());
}

inline std::wstring UTF8ToUTF16(std::string const& src)
{
	auto const dest_size = ::MultiByteToWideChar(CP_UTF8, 0U, src.data(), -1, nullptr, 0U);

	std::vector<wchar_t> dest(dest_size, L'\0');

	if (::MultiByteToWideChar(CP_UTF8, 0U, src.data(), -1, dest.data(), dest.size()) == 0)
	{
		throw std::system_error{ static_cast<int>(::GetLastError()), std::system_category() };
	}

	dest.resize(std::char_traits<wchar_t>::length(dest.data()));
	dest.shrink_to_fit();

	return std::wstring(dest.begin(), dest.end());
}

std::string UTF16ToUTF8(std::wstring const& src)
{
	auto const dest_size = ::WideCharToMultiByte(CP_UTF8, 0U, src.data(), -1, nullptr, 0, nullptr, nullptr);

	std::vector<char> dest(dest_size, '\0');

	if (::WideCharToMultiByte(CP_UTF8, 0U, src.data(), -1, dest.data(), dest.size(), nullptr, nullptr) == 0)
	{
		throw std::system_error{ static_cast<int>(::GetLastError()), std::system_category() };
	}

	dest.resize(std::char_traits<char>::length(dest.data()));
	dest.shrink_to_fit();

	return std::string(dest.begin(), dest.end());

#if 0
	icu::UnicodeString src(value.c_str(), "utf8");
    int length = src.extract(0, src.length(), NULL, "shift_jis");

    std::vector<char> result(length + 1);
    src.extract(0, src.length(), &result[0], "shift_jis");

    return std::string(result.begin(), result.end() - 1);
#endif
}

inline std::string SJIStoUTF8(const std::string& value) {
	wstring const wide = SJISToUTF16(value);
	return UTF16ToUTF8(wide);
#if 0
	icu::UnicodeString src(value.c_str(), "ibm-943_P15A-2003"/*"shift_jis"*/);
    int length = src.extract(0, src.length(), NULL, "utf8");

    std::vector<char> result(length + 1);
    src.extract(0, src.length(), &result[0], "utf8");

    return std::string(result.begin(), result.end() - 1);
#endif
}

inline std::string UTF8toSJIS(const std::string& value) {
	wstring const wide = UTF8ToUTF16(value);
	return UTF16ToSJIS(wide);
}

#if 0
inline string_t stringTostring_t(const std::string& value) {
#ifdef _DALK_USE_UNICODE_
    return babel::manual_translate<string, string_t>(value, babel::base_encoding::sjis, babel::base_encoding::utf16le);
#else
	return value;
#endif

#if 0
	icu::UnicodeString src(value.c_str(), "ibm-943_P15A-2003"/*"shift_jis"*/);
    int length = src.extract(0, src.length(), NULL, "utf16");

    std::vector<wchar_t> result(length + 1);
    src.extract(0, src.length(), &result[0], 0);

    return std::wstring(result.begin(), result.end() - 1);
#endif
}
inline string string_tTostring(const string_t& value) {
#ifdef _DALK_USE_UNICODE_
    return babel::manual_translate<string_t, string>(value, babel::base_encoding::utf16le, babel::base_encoding::sjis);
    #else
	return value;
#endif
}
#endif

//----------------------------------------------------------------------------
// 文字処理(日本語対応)
//----------------------------------------------------------------------------

	//SJISか？
inline bool IsSJIS(BYTE c) {
	return ( (unsigned char)((int)((unsigned char)(c) ^ 0x20) - 0x0A1) < 0x3C );
}

// マルチバイト文字の1文字目か？
inline bool IsMbHead(BYTE c) {
//	return (c>=0x80 && c<=0xa0) || (c>=0xe0 && c<=0xff);
	return (0x81<=c && c<=0x9F) || (0xE0<=c && c<=0xFC);
}

// マルチバイト文字の2文字目か？
inline bool IsMbSub(BYTE c) {
	return (0x40<=c && c<=0x7E) || (0x80<=c && c<=0xFC);
}

// 漢字の1文字目か？
inline bool IsMbKanji(BYTE c) {
	return (0x88<=c && c<=0x9F) || (0xE0<=c && c<=0xFC);
}

//半角を全角に変換
inline const char* ToSJIS(char code){
	static std::string table[256];
	table['0'] = "０";
	table['1'] = "１";
	table['2'] = "２";
	table['3'] = "３";
	table['4'] = "４";
	table['5'] = "５";
	table['6'] = "６";
	table['7'] = "７";
	table['8'] = "８";
	table['9'] = "９";
	table['A'] = "Ａ";
	table['B'] = "Ｂ";
	table['C'] = "Ｃ";
	table['D'] = "Ｄ";
	table['E'] = "Ｅ";
	table['F'] = "Ｆ";
	table['G'] = "Ｇ";
	table['H'] = "Ｈ";
	table['I'] = "Ｉ";
	table['J'] = "Ｊ";
	table['K'] = "Ｋ";
	table['L'] = "Ｌ";
	table['M'] = "Ｍ";
	table['N'] = "Ｎ";
	table['O'] = "Ｏ";
	table['P'] = "Ｐ";
	table['Q'] = "Ｑ";
	table['R'] = "Ｒ";
	table['S'] = "Ｓ";
	table['T'] = "Ｔ";
	table['U'] = "Ｕ";
	table['V'] = "Ｖ";
	table['W'] = "Ｗ";
	table['X'] = "Ｘ";
	table['Y'] = "Ｙ";
	table['Z'] = "Ｚ";
	table['a'] = "ａ";
	table['b'] = "ｂ";
	table['c'] = "ｃ";
	table['d'] = "ｄ";
	table['e'] = "ｅ";
	table['f'] = "ｆ";
	table['g'] = "ｇ";
	table['h'] = "ｈ";
	table['i'] = "ｉ";
	table['j'] = "ｊ";
	table['k'] = "ｋ";
	table['l'] = "ｌ";
	table['m'] = "ｍ";
	table['n'] = "ｎ";
	table['o'] = "ｏ";
	table['p'] = "ｐ";
	table['q'] = "ｑ";
	table['r'] = "ｒ";
	table['s'] = "ｓ";
	table['t'] = "ｔ";
	table['u'] = "ｕ";
	table['v'] = "ｖ";
	table['w'] = "ｗ";
	table['x'] = "ｘ";
	table['y'] = "ｙ";
	table['z'] = "ｚ";
	table['_'] = "＿";
	table['#'] = "＿";
	table['$'] = "＿";
	table['%'] = "＿";
	table['&'] = "＿";
	table['\''] = "＿";
	table['('] = "＿";
	table[')'] = "＿";
	table['='] = "＿";
	table['~'] = "＿";
	table['|'] = "＿";
	table['`'] = "＿";
	table['{'] = "＿";
	table['+'] = "＿";
	table['*'] = "＿";
	table['}'] = "＿";
	table['<'] = "＿";
	table['>'] = "＿";
	table['?'] = "＿";
	table['_'] = "＿";
	table['-'] = "＿";
	table['^'] = "＿";
	table['\\'] = "＿";
	table['@'] = "＿";
	table['['] = "＿";
	table[';'] = "＿";
	table[':'] = "＿";
	table[']'] = "＿";
	table[','] = "＿";
	table['.'] = "＿";
	table['/'] = "＿";
	return table[code].data();
}


}; //namespace DalkLocalizer

#endif