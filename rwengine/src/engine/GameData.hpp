#ifndef _RWENGINE_GAMEDATA_HPP_
#define _RWENGINE_GAMEDATA_HPP_

#include <array>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <platform/FileIndex.hpp>
#include <rw/debug.hpp>
#include <rw/forward.hpp>

#include <data/AnimGroup.hpp>
#include <data/ModelData.hpp>
#include <data/PedData.hpp>
#include <data/WeaponData.hpp>
#include <data/Weather.hpp>
#include <data/ZoneData.hpp>
#include <fonts/GameTexts.hpp>
#include <loaders/LoaderDFF.hpp>
#include <loaders/LoaderIMG.hpp>
#include <loaders/LoaderTXD.hpp>
#include <objects/VehicleInfo.hpp>

class Logger;
struct WeaponData;
class GameWorld;
class TextureAtlas;
class SCMFile;

/**
 * @brief Loads and stores all "static" data such as loaded models, handling
 * information, weather, object definitions etc.
 *
 * @todo Move parsing of one-off data files from this class.
 * @todo Improve how Loaders and written and used
 * @todo Considering implementation of streaming data and object handles.
 */
class GameData {
private:
    std::filesystem::path datpath;
    std::string splash;
    std::string currenttextureslot;

    Logger* logger;
    LoaderDFF dffLoader;

public:
    /**
     * ctor
     * @param path Path to the root of the game data.
     */
    GameData(Logger* log, const std::filesystem::path& path);
    ~GameData() = default;

    GameWorld* engine = nullptr;

    /**
     * Returns the current platform
     */
    std::string getPlatformString() {
        return "PC";
    }

    /**
     * Returns the game data path
     */
    const std::filesystem::path& getDataPath() const {
        return datpath;
    }

    /**
     * Loads items defined in the given IDE
     */
    void loadIDE(const std::string& path);

    /**
     * Handles the parsing of a COL file.
     */
    void loadCOL(const size_t zone, const std::string& name);

    /**
     * Handles the loading of an IMG's data
     */
    void loadIMG(const std::string& name);

    void loadIPL(const std::string& path);

    /**
     * Loads the Zones from a zon/IPL file
     */
    bool loadZone(const std::string& path);

    void loadCarcols(const std::string& path);

    void loadWeather(const std::string& path);

    void loadHandling(const std::string& path);

    SCMFile loadSCM(const std::string& path);

    void loadGXT(const std::string& name);

    /**
     * Loads water level data
     */
    void loadWaterpro(const std::string& path);
    void loadWater(const std::string& path);

    bool load();

    /**
     * Loads model, placement, models and textures from a level file
     */
    void loadLevelFile(const std::string& path);

    /**
     * Loads the txt slot if it is not already loaded and sets
     * the current TXD slot
     */
    void loadTXD(const std::string& name);

    /**
     * Loads a named texture archive from the game data
     */
    TextureArchive loadTextureArchive(const std::string& name);

    /**
     * Loads to named a texture archive from the game data
     */
    void loadToTextureArchive(const std::string& name, TextureArchive& archive);

    /**
     * Converts combined {name}_l{LOD} into name and lod.
     */
    static void getNameAndLod(std::string& name, int& lod);

    /**
     * Loads an archived model and returns it directly
     */
    ClumpPtr loadClump(const std::string& name);

    /**
     * Loads an archived model and returns it directly
     */
    ClumpPtr loadClump(const std::string& name, const std::string& textureSlot);

    /**
     * Loads a DFF and associates its atomics with models.
     */
    void loadModelFile(const std::string& name);

    /**
     * Loads and associates a model's data
     */
    bool loadModel(ModelID model);

    /**
     * Loads an IFP file containing animations
     */
    void loadIFP(const std::string& name, bool cutsceneAnimation = false);

    /**
     * Loads data from an object definition dat.
     */
    void loadDynamicObjects(const std::string& name);

    /**
     * Loads weapon.dat
     */
    void loadWeaponDAT(const std::string& path);

    /**
     * Loads pedestrian stats from e.g. pedstats.dat
     */
    void loadPedStats(const std::string& path);

    /**
     * Loads pedestrian relations e.g. peds.dat
     */
    void loadPedRelations(const std::string& path);

    /**
     * Loads pedestrian groups e.g. pedgrp.dat for zone info
     */
    void loadPedGroups(const std::string& path);

    bool loadAudioStream(const std::string& name);
    bool loadAudioClip(const std::string& name, const std::string& fileName);

    void loadSplash(const std::string& name);

    TextureData* findSlotTexture(const std::string& slot,
                                        const std::string& texture) const;

    FileIndex index;

    /**
     * Files that have been loaded previously
     */
    std::map<std::string, bool> loadedFiles;

    /**
     * IPL file locations
     */
    std::map<std::string, std::string> iplLocations;

    /**
     * Map of loaded archives
     */
    std::map<std::string, LoaderIMG> archives;

    ZoneDataList gamezones;

    ZoneDataList mapzones;

    ZoneData* findZone(const std::string& name);

    ZoneData* findZoneAt(const glm::vec3& pos);

    std::unordered_map<ModelID, std::unique_ptr<BaseModelInfo>> modelinfo;

    uint16_t findModelObject(const std::string model);

    template <class T>
    T* findModelInfo(ModelID id) {
        auto f = modelinfo.find(id);
        if (f != modelinfo.end() && f->second->type() == T::kType) {
            return static_cast<T*>(f->second.get());
        }
        return nullptr;
    }

    /**
     * The vehicle colour palettes
     */
    std::vector<glm::u8vec3> vehicleColours;

    /**
     * The vehicle colours for each vehicle type
     */
    std::map<std::string, std::vector<std::pair<size_t, size_t>>>
        vehiclePalettes;

    /**
     * Vehicle information
     */
    std::unordered_map<std::string, VehicleInfo> vehicleInfos;

    /**
     * Texture Loader
     */
    TextureLoader textureLoader;

    /**
     * Weather Data
     */
    Weather weather;

    /**
     * Texture slots, containing loaded textures.
     */
    std::unordered_map<std::string, TextureArchive> textureSlots;

    /**
     * Texture atlases.
     */
    std::vector<TextureAtlas*> atlases;

    /**
     * Loaded Animations (doesn't contain animations of cutscene)
     */
    AnimationSet animations;

    /**
     * Loaded Animations (only) for cutscene
     */
    AnimationSet animationsCutscene;

    /**
     * Pedestrian Animation Groups
     */
    AnimGroups pedAnimGroups;

    /**
     * Finds or loads the named animation group
     *
     * if the group can't be found, a default is returned.
     */
    AnimGroup* getAnimGroup(const std::string& group);

    /**
     * DynamicObjectData
     */
    std::unordered_map<std::string, DynamicObjectData> dynamicObjectData;

    std::vector<WeaponData> weaponData;

    /**
     * Pedstrian type stats
     */
    std::vector<PedStats> pedstats;

    /**
     * Pedestrian relationships
     */
    std::array<PedRelationship, PedModelInfo::_NUM_PEDTYPE> pedrels;

    /**
     * Pedestrian groups
     */
    PedGroupList pedgroups;

    /**
     * @struct WaterArea
     *  Stores Water Rectangle Information
     */
    struct WaterArea {
        float height;
        float xLeft, yBottom;
        float xRight, yTop;

        WaterArea(float _height, float _xLeft, float _yBottom, float _xRight, float _yTop)
            : height(_height)
            , xLeft(_xLeft)
            , yBottom(_yBottom)
            , xRight(_xRight)
            , yTop(_yTop) {
        }
    };

    /**
     * Water Areas
     */
    std::vector<WaterArea> waterBlocks;

    /**
     * Water heights
     */
    float waterHeights[48];

    /**
     * Visible water heights
     */
    uint8_t visibleWater[64 * 64];

    /**
     * The "real" water heights
     */
    uint8_t realWater[128 * 128];

    int getWaterIndexAt(const glm::vec3& ws) const;
    float getWaveHeightAt(const glm::vec3& ws) const;

    GameTexts texts;

private:
    /**
     * Determines whether the given path is a valid game directory.
     */
    bool isValidGameDirectory() const;
};

#endif
