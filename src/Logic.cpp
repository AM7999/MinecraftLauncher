#include <imgui.h>
#include <curl/curl.h>

#include <fstream>

#include <cstdlib>
#include <future>

#include "Logic.hpp"
#include "Logger.hpp"

std::string stripFilename(const std::string &url) {
    std::string filename = url.substr(url.find_last_of("/") + 1);
    if (filename.empty())
        return "";

    size_t queryPos = filename.find("?");
    if (queryPos != std::string::npos)
        filename = filename.substr(0, queryPos);

    return filename;
}

bool Logic::downloadFile(const std::string &directory, const std::string &url) {
    Xenia::logMessage("Downloading file: " + url);
    Xenia::logMessage(" Downloading to: " + directory + stripFilename(url));
    const std::string filepath = directory + stripFilename(url);
    const char* fileOut = filepath.c_str();
    CURL *curl_handle = curl_easy_init();
    FILE *pagefile = fopen(fileOut, "wb");
    if(!pagefile) { curl_easy_cleanup(curl_handle); return false; }

    // at least its "easy"
    curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "Minecraft Offline Launcher");
    curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, NULL);
    curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, 30);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, pagefile);

    CURLcode res = curl_easy_perform(curl_handle);

    fclose(pagefile);
    curl_easy_cleanup(curl_handle);

    if(res != CURLE_OK) {
        remove(fileOut);
        return false;
    }

    return true;
}

bool Logic::downloadMinecraft(const Xenia::version& v, const std::string& instanceName) {
    // download version json
    Logic::downloadFile("cache/", v.url);
    std::ifstream version;
    nlohmann::json j;
    const std::string ASSET_URL = "https://resources.download.minecraft.net/";
    version.open("cache/" + stripFilename(v.url));
    version >> j;
    version.close();

    std::filesystem::path targetDir = std::filesystem::path("Instances") / instanceName;
    std::filesystem::create_directories(targetDir);

    return true;
}

std::string Logic::getOperatingSystem() {
    #ifdef _WIN32
        return "win32";
    #elif _WIN64
        return "win64";
    #elif __APPLE_ || __MACH__
        return "macOS"
    #elif __linux__
        return "linux";
    #elif __FreeBSD__
        return "freebsd";
    #elif __unix || __unix__
        rturn "unix";
    #else
        return "other"
    #endif
}
