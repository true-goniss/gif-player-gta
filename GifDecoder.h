#pragma once
#include "FreeImage.h"
#include <vector>
#include <mutex>
#include <map>

static class GifDecoder
{
    static inline std::mutex mtx;

    static inline bool gifProcessed = false;

    static inline int allFramesQuan = 0;
    static inline std::map<const char*, int> gifsAllFramesCount;
    static inline std::map<const char*, int> gifsLoadedFramesCount;

    static std::vector<unsigned char> convertFIBitmapToBinary(FIBITMAP* bitmap) {
        if (!bitmap) {
            return {};
        }

        FIMEMORY* memStream = FreeImage_OpenMemory(nullptr, 0);
        if (!memStream) {
            return {};
        }

        if (!FreeImage_SaveToMemory(FIF_PNG, bitmap, memStream)) {
            FreeImage_CloseMemory(memStream);
            return {};
        }

        BYTE* memoryBuffer = nullptr;
        DWORD size = 0;
        if (!FreeImage_AcquireMemory(memStream, &memoryBuffer, &size)) {
            FreeImage_CloseMemory(memStream);
            return {};
        }

        std::vector<unsigned char> binaryData(memoryBuffer, memoryBuffer + size);

        FreeImage_CloseMemory(memStream);

        return binaryData;
    }

    public:

    static std::vector<std::vector<unsigned char>> processGif(const char* gifFilePath) {
        gifProcessed = false;

        std::vector<std::vector<unsigned char>> pngDataList;

        gifsLoadedFramesCount[gifFilePath] = 0;

        FreeImage_Initialise();

        FIMULTIBITMAP* gif = FreeImage_OpenMultiBitmap(FIF_GIF, gifFilePath, FALSE, TRUE);
        if (!gif) {
            FreeImage_DeInitialise();
            return pngDataList;
        }

        int frameCount = FreeImage_GetPageCount(gif);

        {
            std::lock_guard<std::mutex> lock(mtx);
            gifsAllFramesCount[gifFilePath] = frameCount;
        }

        for (int i = 0; i < frameCount; ++i) {
            FIBITMAP* frame = FreeImage_LockPage(gif, i);
            if (!frame) {
                FreeImage_CloseMultiBitmap(gif);
                FreeImage_DeInitialise();
                return pngDataList;
            }

            FIBITMAP* rgbFrame = FreeImage_ConvertTo32Bits(frame);

            {
                std::lock_guard<std::mutex> lock(mtx);
                pngDataList.push_back(convertFIBitmapToBinary(rgbFrame));
                gifsLoadedFramesCount[gifFilePath]++;
            }

            FreeImage_UnlockPage(gif, frame, FALSE);
            FreeImage_Unload(rgbFrame);
        }

        FreeImage_CloseMultiBitmap(gif);
        FreeImage_DeInitialise();

        gifProcessed = true;

        return pngDataList;
    }

    static bool checkIfGifWasLoaded(const char* path) {

        if (!gifProcessed) return false;

        std::lock_guard<std::mutex> lock(mtx);

        if (GifDecoder::gifsAllFramesCount.find(path) != GifDecoder::gifsAllFramesCount.end()) {
            allFramesQuan = GifDecoder::gifsAllFramesCount[path];

            if (gifsLoadedFramesCount[path] == allFramesQuan && allFramesQuan > 0) {
                return true;
            }
        }

        return false;
    }
};

