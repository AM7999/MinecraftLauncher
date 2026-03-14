#ifndef STRUCTS_H
#define STRUCTS_H

#include <iostream>
#include <string>
#include <nlohmann/json.hpp>

namespace Xenia {
    enum ModLoader {
        NONE,
        FORGE,
        FABRIC,
        QUILT,
        NEOFORGE
    };

    inline Xenia::ModLoader intToModloader(const int ml) {
        switch(ml) {
            case Xenia::ModLoader::NONE:
                return Xenia::ModLoader::NONE;
                break;
            case Xenia::ModLoader::FORGE:
                return Xenia::ModLoader::FORGE;
                break;
            case Xenia::ModLoader::FABRIC:
                return Xenia::ModLoader::FABRIC;
                break;
            case Xenia::ModLoader::QUILT:
                return Xenia::ModLoader::QUILT;
                break;
            case Xenia::ModLoader::NEOFORGE:
                return Xenia::ModLoader::NEOFORGE;
                break;
        }
        return Xenia::ModLoader::NONE;
    }

    inline std::string ModLoaderToString(const ModLoader& ml) {
        switch (ml) {
            case Xenia::ModLoader::NONE:
                return "none";
                break;
            case Xenia::ModLoader::FORGE:
                return "forge";
                break;
            case Xenia::ModLoader::FABRIC:
                return "fabric";
                break;
            case Xenia::ModLoader::QUILT:
                return "quilt";
                break;
            case Xenia::ModLoader::NEOFORGE:
                return "neoforge";
                break;
        }

        return "";
    }

    struct clientSettings {
        std::string username;
        bool online;
        int memory;
    };

    struct JDK {
        std::string vendor;
        int javaVersion;
        std::string path;
    };

    struct Instance {
        std::string instanceName;
        std::string minecraftVersion;
        int javaVersion;
        bool isModded;
        ModLoader ml;
        std::string pathToInstance;
    };
}

// Serialization/Deserialization for Xenia::Instance and Xenia::JDK
namespace nlohmann {
    template<>
    struct adl_serializer<Xenia::clientSettings> {
        static void to_json(json &j, const Xenia::clientSettings &i) {
            j = json{
                {"username", i.username},
                {"maxMemoryAlloc", i.memory},
                {"online", i.online}
            };
        }
        static void from_json(const json &j, Xenia::clientSettings &i) {
            try {
                i.username = j.at("username").get<std::string>();
                i.memory = j.at("maxMemoryAlloc").get<int>();
                i.online = j.at("online").get<bool>();
            }
            catch (std::exception &e) {
                std::cerr << e.what();
            }
        }
    };
    template <>
    struct adl_serializer<Xenia::JDK> {
        static void to_json(json &j, const Xenia::JDK &jdk) {
            j = json{
                {"name", jdk.vendor},
                {"jdkVersion", jdk.javaVersion},
                {"pathToExec", jdk.path},
            };
        }
        static void from_json(const json &j, Xenia::JDK &jdk) {
            try {
                jdk.vendor = j.at("name").get<std::string>();
                jdk.javaVersion = j.at("jdkVersion").get<int>();
                jdk.path = j.at("pathToExec").get<std::string>();
            }
            catch(std::exception &e) {
                std::cout << e.what();
            }
        }
    };

    template <>
    struct adl_serializer<Xenia::Instance> {
        static void to_json(json &j, const Xenia::Instance &i) {
            std::string modloaderString;
            switch (i.ml) {
                case Xenia::NONE: modloaderString = "none"; break;
                case Xenia::FORGE: modloaderString = "forge"; break;
                case Xenia::FABRIC: modloaderString = "fabric"; break;
                case Xenia::QUILT: modloaderString = "quilt"; break;
                case Xenia::NEOFORGE: modloaderString = "neoforge"; break;
                default: modloaderString = nullptr; break;
            }
            j = json{
                {"name", i.instanceName},
                {"version", i.minecraftVersion},
                {"javaVersion", i.javaVersion},
                {"modded", {
                    {"isModded", i.isModded},
                    {"modLoader", modloaderString}
                }},
                {"pathToInstance", i.pathToInstance},
            };
        }
        static void from_json(const json &j, Xenia::Instance &i) {
            try {
                i.instanceName = j.at("name").get<std::string>();
                i.minecraftVersion = j.at("version").get<std::string>();
                i.javaVersion = j.at("javaVersion").get<int>();
                i.pathToInstance = j.at("pathToInstance").get<std::string>();
                auto modded = j.at("modded");
                i.isModded = modded.at("isModded").get<bool>();

                if (modded.at("modLoader").is_null()) {
                    i.ml = Xenia::ModLoader::NONE;
                } else {
                    std::string loader = modded.at("modLoader").get<std::string>();
                    if (loader == "forge") i.ml = Xenia::ModLoader::FORGE;
                    if (loader == "fabric") i.ml = Xenia::ModLoader::FABRIC;
                    if (loader == "quilt") i.ml = Xenia::ModLoader::QUILT;
                    if (loader == "neoforge") i.ml = Xenia::ModLoader::NEOFORGE;
                    else i.ml = Xenia::ModLoader::NONE;
                }
            }
            catch (const std::exception &e) {
                std::cerr << e.what() << '\n';
            }
        }
    };
}


#endif
