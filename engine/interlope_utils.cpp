
#include <iostream>
#include <algorithm>

#ifdef WINDOWS
#include <unordered_map>
#include <sstream>
std::unordered_map<std::wstring, std::wstring> parse_headers(const wchar_t* headers) {
    std::unordered_map<std::wstring, std::wstring> headersMap;
    std::wistringstream iss(headers);
    std::wstring line;

    while (std::getline(iss, line)) {
        line.erase(std::remove(line.begin(), line.end(), L'\r'), line.end());

        if (line.find(L"HTTP/") == 0) {
            continue;
        }

        size_t colonPos = line.find(L": ");
        if (colonPos == std::wstring::npos) {
            continue;
        }

        std::wstring key = line.substr(0, colonPos);
        std::wstring value = line.substr(colonPos + 2);

        std::transform(key.begin(), key.end(), key.begin(), ::towlower);
        headersMap[key] = value;
    }

    return headersMap;
}
#endif
#ifdef LINUX
#include <sstream>
#include <vector>
#include "basetypes.h"

size_t header_callback(char* buffer, size_t size, size_t nitems, std::string* headers) {
    headers->append(buffer, size * nitems);
    return size * nitems;
}
size_t write_callback(char* ptr, size_t size, size_t nmemb, std::vector<BYTE>* buffer) {
    size_t realsize = size * nmemb;
    buffer->insert(buffer->end(), ptr, ptr + realsize);
    return realsize;
}

std::string extract_commit_header(const std::string& headers) {
    std::istringstream iss(headers);
    std::string line;
    
    while (std::getline(iss, line)) {
        line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
        
        size_t colon_pos = line.find(':');
        if (colon_pos == std::string::npos) continue;
        
        std::string key = line.substr(0, colon_pos);
        std::transform(key.begin(), key.end(), key.begin(), 
                      [](unsigned char c){ return std::tolower(c); });
        
        if (key == "commit") {
            std::string value = line.substr(colon_pos + 1);
            value.erase(value.begin(), 
                       std::find_if(value.begin(), value.end(), 
                       [](int ch) { return !std::isspace(ch); }));
            return value;
        }
    }
    
    return "";
}
#endif