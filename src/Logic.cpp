#include <imgui.h>
#include <curl/curl.h>

#include <fstream>
#include <future>
#include <mutex>
#include <semaphore>

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

// bool Logic::downloadFile(const std::string &directory, const std::string &url) {
//     Xenia::logMessage("Downloading file: " + url);
//     Xenia::logMessage(" Downloading to: " + directory + stripFilename(url));
//     const std::string filepath = directory + stripFilename(url);
//     const char* fileOut = filepath.c_str();
//     CURL *curl_handle = curl_easy_init();
//     FILE *pagefile = fopen(fileOut, "wb");
//     if(!pagefile) { curl_easy_cleanup(curl_handle); return false;}
//
//     // at least its "easy"
//     curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
//     curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "Minecraft Offline Launcher");
//     curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);
//     curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, NULL);
//     curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, pagefile);
//
//     CURLcode res = curl_easy_perform(curl_handle);
//
//     fclose(pagefile);
//     curl_easy_cleanup(curl_handle);
//
//     if(res != CURLE_OK) {
//         remove(fileOut);
//         return false;
//     }
//
//     return true;
// }

bool Logic::downloadFile(const std::string &directory, const std::string &url) {
    Xenia::logMessage("Downloading file: " + url);
    const std::string filepath = directory + stripFilename(url);
    Xenia::logMessage(" Downloading to: " + filepath);

    CURL *curl_handle = curl_easy_init();
    if (!curl_handle) {
        Xenia::logMessage("Failed to initialize curl");
        return false;
    }

    FILE *pagefile = fopen(filepath.c_str(), "wb");
    if (!pagefile) {
        curl_easy_cleanup(curl_handle);
        return false;
    }

    curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "Minecraft Offline Launcher");
    curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L); // handle redirects
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, fwrite);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, pagefile);

    CURLcode res = curl_easy_perform(curl_handle);
    fclose(pagefile);
    curl_easy_cleanup(curl_handle);

    if (res != CURLE_OK) {
        Xenia::logMessage("Download failed: " + std::string(curl_easy_strerror(res)));
        std::filesystem::remove(filepath);
        return false;
    }
    return true;
}

// bool Logic::downloadMinecraft(const Xenia::version& v, const std::string& instanceName) {
//     Logic::downloadFile("cache/", v.url);
//
//     std::ifstream version;
//     nlohmann::json j;
//
//     version.open("cache/" + stripFilename(v.url));
//     version >> j;
//
//     std::filesystem::path targetDir = std::filesystem::path("Instances") / instanceName;
//     std::filesystem::create_directories(targetDir);
//     Logic::downloadFile("Instances/" + instanceName + "/", j["downloads"]["client"]["url"]);
//
//     return true;
// }

// Limit concurrent downloads
static constexpr int MAX_CONCURRENT_DOWNLOADS = 16;

bool Logic::downloadMinecraft(const Xenia::version& v, const std::string& instanceName, const Xenia::clientSettings& settings) {
    const std::string instancePath = "Instances/" + instanceName + "/";
    std::filesystem::path targetDir = std::filesystem::path("Instances") / instanceName;
    std::filesystem::create_directories(targetDir);

    if (!Logic::downloadFile("cache/", v.url)) {
        Xenia::logMessage("Failed to download version manifest");
        return false;
    }

    const std::string manifestPath = "cache/" + stripFilename(v.url);
    nlohmann::json j;
    {
        std::ifstream version(manifestPath);
        if (!version.is_open()) {
            Xenia::logMessage("Failed to open version manifest: " + manifestPath);
            return false;
        }
        try {
            version >> j;
        } catch (const nlohmann::json::parse_error& e) {
            Xenia::logMessage("Failed to parse version manifest: " + std::string(e.what()));
            return false;
        }
    }

    if (!j.contains("downloads") || !j["downloads"].contains("client")) {
        Xenia::logMessage("Version manifest missing client download entry");
        return false;
    }
    if (!Logic::downloadFile(instancePath, j["downloads"]["client"]["url"])) {
        Xenia::logMessage("Failed to download client jar");
        return false;
    }

    static constexpr int MAX_CONCURRENT_DOWNLOADS = 16;
    std::counting_semaphore<MAX_CONCURRENT_DOWNLOADS> sem(MAX_CONCURRENT_DOWNLOADS);
    std::mutex logMutex;

    auto asyncDownload = [&](const std::string& dir, const std::string& url) -> std::future<bool> {
        return std::async(std::launch::async, [&sem, &logMutex, dir, url]() {
            sem.acquire();
            bool result = Logic::downloadFile(dir, url);
            if (!result) {
                std::lock_guard<std::mutex> lock(logMutex);
                Xenia::logMessage("Failed to download: " + url);
            }
            sem.release();
            return result;
        });
    };

    if (j.contains("libraries")) {
        std::vector<std::future<bool>> futures;

        for (const auto& lib : j["libraries"]) {
            if (!lib.contains("downloads") || !lib["downloads"].contains("artifact"))
                continue;
            const auto& artifact = lib["downloads"]["artifact"];
            if (!artifact.contains("url") || !artifact.contains("path"))
                continue;

            std::string libRelPath = artifact["path"].get<std::string>();
            std::filesystem::path libDir = targetDir / "libraries" / libRelPath;
            std::filesystem::create_directories(libDir.parent_path());

            futures.push_back(asyncDownload(libDir.parent_path().string() + "/", artifact["url"]));
        }

        for (auto& f : futures) {
            if (!f.get()) {
                Xenia::logMessage("A library download failed");
                return false;
            }
        }
    }

    Xenia::logMessage("Libraries downloaded");

    if (!j.contains("assetIndex")) {
        Xenia::logMessage("Version manifest missing assetIndex");
        return false;
    }

    const std::string assetIndexUrl = j["assetIndex"]["url"];
    std::filesystem::path indexesDir = targetDir / "assets" / "indexes";
    std::filesystem::path objectsDir = targetDir / "assets" / "objects";
    std::filesystem::create_directories(indexesDir);
    std::filesystem::create_directories(objectsDir);

    if (!Logic::downloadFile(indexesDir.string() + "/", assetIndexUrl)) {
        Xenia::logMessage("Failed to download asset index");
        return false;
    }

    nlohmann::json assets;
    {
        std::string assetIndexPath = indexesDir.string() + "/" + stripFilename(assetIndexUrl);
        std::ifstream assetFile(assetIndexPath);
        if (!assetFile.is_open()) {
            Xenia::logMessage("Failed to open asset index");
            return false;
        }
        try {
            assetFile >> assets;
        } catch (const nlohmann::json::parse_error& e) {
            Xenia::logMessage("Failed to parse asset index: " + std::string(e.what()));
            return false;
        }
    }

    if (!assets.contains("objects")) {
        Xenia::logMessage("Asset index missing objects");
        return false;
    }

    {
        std::vector<std::future<bool>> futures;
        std::atomic<int> assetCount = 0;
        int assetTotal = assets["objects"].size();

        for (const auto& [name, obj] : assets["objects"].items()) {
            const std::string hash = obj["hash"];
            const std::string prefix = hash.substr(0, 2);
            const std::string url = "https://resources.download.minecraft.net/" + prefix + "/" + hash;

            std::filesystem::path objDir = objectsDir / prefix;
            std::filesystem::create_directories(objDir);

            if (std::filesystem::exists(objDir / hash)) {
                assetCount++;
                continue;
            }

            futures.push_back(std::async(std::launch::async, [&sem, &logMutex, &assetCount, assetTotal, objDir, url]() {
                sem.acquire();
                bool result = Logic::downloadFile(objDir.string() + "/", url);
                sem.release();
                int count = ++assetCount;
                if (count % 100 == 0) {
                    std::lock_guard<std::mutex> lock(logMutex);
                    Xenia::logMessage("Assets: " + std::to_string(count) + "/" + std::to_string(assetTotal));
                }
                return result;
            }));
        }

        for (auto& f : futures) {
            if (!f.get()) {
                Xenia::logMessage("An asset download failed");
                return false;
            }
        }
    }

    Xenia::logMessage("Download complete for " + instanceName);
    return true;
}

bool Logic::generateLaunchScript(const Xenia::Instance& instance, const Xenia::JDK& jdk, const Xenia::clientSettings& settings) {
    std::filesystem::path instanceDir = instance.pathToInstance;
    std::filesystem::path scriptPath = instanceDir / "launch.sh";
    std::filesystem::path assetsDir = instanceDir / "assets";

    // Get asset index name
    std::string assetIndex;
    for (const auto& entry : std::filesystem::directory_iterator(assetsDir / "indexes")) {
        assetIndex = entry.path().stem().string();
        break;
    }
    if (assetIndex.empty()) {
        Xenia::logMessage("Failed to find asset index for launch script");
        return false;
    }

    std::ofstream script(scriptPath);
    if (!script.is_open()) {
        Xenia::logMessage("Failed to create launch script");
        return false;
    }

    script << "#!/bin/bash\n\n";
    script << "INSTANCE_DIR=\"$(dirname \"$(realpath \"$0\")\")\"\n";
    script << "CLIENT_JAR=\"$INSTANCE_DIR/client.jar\"\n";
    script << "LIBRARIES_DIR=\"$INSTANCE_DIR/libraries\"\n";
    script << "ASSETS_DIR=\"$INSTANCE_DIR/assets\"\n";
    script << "JAVA=\"" << jdk.path << "\"\n\n";

    script << "# Build classpath from all library jars\n";
    script << "CLASSPATH=\"\"\n";
    script << "while IFS= read -r -d '' jar; do\n";
    script << "    CLASSPATH=\"$CLASSPATH:$jar\"\n";
    script << "done < <(find \"$LIBRARIES_DIR\" -name \"*.jar\" -print0)\n";
    script << "CLASSPATH=\"$CLIENT_JAR$CLASSPATH\"\n\n";

    if (settings.online) {
        script << "# Online mode - replace with real auth token\n";
        script << "ACCESS_TOKEN=\"\"\n";
        script << "UUID=\"\"\n";
    } else {
        script << "# Offline mode\n";
        script << "ACCESS_TOKEN=\"0\"\n";
        script << "UUID=\"00000000-0000-0000-0000-000000000000\"\n";
    }
    script << "USERNAME=\"" << settings.username << "\"\n\n";

    script << "$JAVA \\\n";
    script << "    -Xmx" << settings.memory << "M \\\n";
    script << "    -Xms512M \\\n";
    script << "    -Djava.library.path=\"$INSTANCE_DIR/natives\" \\\n";
    script << "    -cp \"$CLASSPATH\" \\\n";

    // if (instance.isModded) {
    //     script << "    " << Logic::getModdedMainClass(instance.ml) << " \\\n";
    // } else {
    //     script << "    net.minecraft.client.main.Main \\\n";
    // }

    script << "    --username \"$USERNAME\" \\\n";
    script << "    --version \"" << instance.minecraftVersion << "\" \\\n";
    script << "    --gameDir \"$INSTANCE_DIR\" \\\n";
    script << "    --assetsDir \"$ASSETS_DIR\" \\\n";
    script << "    --assetIndex \"" << assetIndex << "\" \\\n";
    script << "    --uuid \"$UUID\" \\\n";
    script << "    --accessToken \"$ACCESS_TOKEN\" \\\n";
    script << "    --userType " << (settings.online ? "mojang" : "legacy") << " \\\n";
    script << "    --versionType release\n";

    script.close();

    std::filesystem::permissions(scriptPath,
        std::filesystem::perms::owner_exec |
        std::filesystem::perms::group_exec |
        std::filesystem::perms::others_exec,
        std::filesystem::perm_options::add);

    Xenia::logMessage("Launch script created: " + scriptPath.string());
    return true;
}