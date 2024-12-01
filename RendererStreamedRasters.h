/*
    by gon_iss (c) 2024
*/

/*
    A class that creates defined quantity of rasters, to update them with unlimited amount of data. The latest updated raster will be rendered
*/

#pragma once
#include "Renderer.h"

static std::function<void(const unsigned char*, size_t)> storedCallback;

class RendererStreamedRasters : public Renderer
{
    int callback_num = 1;

    typedef void(__stdcall* CallbackFunc)(const unsigned char* pngData, size_t dataLength);

    static void __stdcall CallbackWrapper(const unsigned char* pngData, size_t dataLength) {
        if (storedCallback) {
            storedCallback(pngData, dataLength);
        }
    }

    typedef void(__stdcall* RegisterCallbackFunc)(int index, CallbackFunc func);
    typedef void(__stdcall* RunCallbackFunc)();

    template<int RasterNum, RwRaster*& Raster>
    CallbackFunc GenerateCallback() {
        CallbackFunc callbackPtr = [](const unsigned char* pngData, size_t dataLength) {

            Utils::rasterFromMemory(pngData, dataLength, Raster);
            latest_raster_num = RasterNum;
        };

        return callbackPtr;
    }

    void RegisterCallback(int index, CallbackFunc callback) {
        if (index >= 0 && index < rasters.size()) {
            g_Callbacks[index] = callback;
        }
    }

    void RunCallback(int index, const unsigned char* pngData, const size_t dataLength) {
        if (index >= 0 && index < rasters.size() && g_Callbacks[index]) {
            g_Callbacks[index](pngData, dataLength);
        }
    }

    static inline std::vector<CallbackFunc> g_Callbacks = std::vector<CallbackFunc>(rasters.size(), nullptr);

    public:

    /*
        streamedRastersQuantity is the quantity of raster updaters that will be created to avoid simultaneous data changing conflict 
    */
    RendererStreamedRasters(int streamedRastersQuantity) : Renderer() {
        rasters.resize(streamedRastersQuantity, nullptr);
        g_Callbacks.resize(streamedRastersQuantity, nullptr);
    }

    ~RendererStreamedRasters() override = default;

    void SpecificInitialize() override {

        auto& rasters = getRasters();

        Utils::rasterFromPngFile("dummy.png", rasters[0]);
        raster_index++;

        for (size_t i = 0; i < rasters.size(); ++i) {
            if (rasters[i] == nullptr) {

                storedCallback = [this, i, &rasters](const unsigned char* pngData, size_t dataLength) {
                    Utils::rasterFromMemory(pngData, dataLength, rasters[i]);
                    latest_raster_num = i + 1;
                };

                RegisterCallback(i, CallbackWrapper);
            }
        }
    }

    void SetFrame(const unsigned char* pngData, const size_t dataLength) override {
        callback_num++;

        if (callback_num > rasters.size()) {
            callback_num = 1;
        }

        int callback_index = callback_num - 1;

        if (g_Callbacks[callback_index]) {

            RunCallback(callback_index, pngData, dataLength);
        }
    }
};

