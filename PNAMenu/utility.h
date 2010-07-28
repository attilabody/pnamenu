#include <string>
#include <vector>

bool getcmd( std::wstring &cmd, std::wstring &params, const std::wstring &cmdline );
void towide( std::wstring &dst, const std::string &src );
void tombcs( std::string &dst, const std::wstring &src );
void tombcs( std::string &dst, const wchar_t *src );
void rtrim( std::string &s );
void rtrim( std::wstring &s );
void ltrim( std::string &s );
void ltrim( std::wstring &s );
bool getparams( std::vector<std::wstring> &params, const std::wstring &cmdline );
