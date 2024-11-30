#pragma once
#include "CSprite2d.h"
#include <mutex>

using namespace plugin;

static class Renderer
{
    static inline bool initialized = false;

    static inline CSprite2d* dummySprite = nullptr;

    static inline RwRaster* raster1 = nullptr;
    static inline RwRaster* raster2 = nullptr;
    static inline RwRaster* raster3 = nullptr;
    static inline RwRaster* raster4 = nullptr;


    static inline RwRaster* raster5 = nullptr;
    static inline RwRaster* raster6 = nullptr;
    static inline RwRaster* raster7 = nullptr;
    static inline RwRaster* raster8 = nullptr;


    static inline RwRaster* raster9 = nullptr;
    static inline RwRaster* raster10 = nullptr;
    static inline RwRaster* raster11 = nullptr;
    static inline RwRaster* raster12 = nullptr;
    static inline RwRaster* raster13 = nullptr;
    static inline RwRaster* raster14 = nullptr;
    static inline RwRaster* raster15 = nullptr;
    static inline RwRaster* raster16 = nullptr;


    static inline RwRaster* raster17 = nullptr;
    static inline RwRaster* raster18 = nullptr;
    static inline RwRaster* raster19 = nullptr;
    static inline RwRaster* raster20 = nullptr;
    static inline RwRaster* raster21 = nullptr;
    static inline RwRaster* raster22 = nullptr;
    static inline RwRaster* raster23 = nullptr;

    static inline int latest_raster_num = 1;
    static inline int raster_index = 0;

    static inline int callback_num = 1;

    static inline std::mutex mtx;

    static RwRaster* getRasterToRender() {

        switch (latest_raster_num)
        {
        case 1:
            return raster1;
            break;
        case 2:
            return raster2;
            break;
        case 3:
            return raster3;
            break;
        case 4:
            return raster4;
            break;
        case 5:
            return raster5;
        case 6:
            return raster6;
            break;
        case 7:
            return raster7;
            break;
        case 8:
            return raster8;
            break;
        case 9:
            return raster9;
            break;
        case 10:
            return raster10;
            break;
        case 11:
            return raster11;
            break;
        case 12:
            return raster12;
            break;
        case 13:
            return raster13;
            break;
        case 14:
            return raster14;
            break;
        case 15:
            return raster15;
            break;
        case 16:
            return raster16;
            break;
        case 17:
            return raster17;
            break;
        case 18:
            return raster18;
            break;
        case 19:
            return raster19;
            break;
        case 20:
            return raster20;
            break;
        case 21:
            return raster21;
            break;
        case 22:
            return raster22;
            break;
        case 23:
            return raster23;
            break;
        default:
            return nullptr;
            break;
        }
    }

    static void rasterFromMemory(const unsigned char* data, const size_t size, RwRaster*& raster) {

        int w, h, d, f;

        {
            std::lock_guard<std::mutex> lock(mtx);

            patch::SetChar(0x7CF9CA, rwSTREAMMEMORY);
            RwMemory memoryImage;
            RwInt32 width, height, depth, flags;
            memoryImage.start = (RwUInt8*)data;
            memoryImage.length = size;
            RwImage* image = RtPNGImageRead((char*)&memoryImage);

            if (image) {
                RwImageFindRasterFormat(image, rwRASTERTYPETEXTURE, &w, &h, &d, &f);

                if (!raster) {
                    if (raster = RwRasterCreate(w, h, d, f)) {
                        RwRasterSetFromImage(raster, image);
                    }
                }
                else {
                    RwRasterSetFromImage(raster, image);
                }

                RwImageDestroy(image);
            }

            patch::SetChar(0x7CF9CA, rwSTREAMFILENAME);
        }
    }

    static void rasterFromPngFile(const char* file, RwRaster*& raster) {
    int w, h, d, f;

    /*raster = nullptr;*/

    if (RwImage* img = RtPNGImageRead(file)) {
        RwImageFindRasterFormat(img, rwRASTERTYPETEXTURE, &w, &h, &d, &f);

        if (!raster) {
            if (raster = RwRasterCreate(w, h, d, f)) {
                RwRasterSetFromImage(raster, img);
            }
        }
        else {
            RwRasterSetFromImage(raster, img);
        }

        RwImageDestroy(img);
    }
}

    static RwTexture* textureFromPngFile(const char* file) {

    int w, h, d, f;
    RwTexture* texture = NULL;


    if (RwImage* img = RtPNGImageRead(file)) {
        RwImageFindRasterFormat(img, rwRASTERTYPETEXTURE, &w, &h, &d, &f);

        if (RwRaster* raster = RwRasterCreate(w, h, d, f)) {
            RwRasterSetFromImage(raster, img);

            if (texture = RwTextureCreate(raster)) {
                if ((texture->raster->cFormat & 0x80) == 0)
                    RwTextureSetFilterMode(texture, rwFILTERLINEAR);
                else
                    RwTextureSetFilterMode(texture, rwFILTERLINEARMIPLINEAR);

                RwTextureSetAddressing(texture, rwTEXTUREADDRESSWRAP);
            }
        }

        RwImageDestroy(img);
    }


    return texture;
}

    typedef void(__stdcall* CallbackFunc)(const unsigned char* pngData, size_t dataLength);
    typedef void(__stdcall* RegisterCallbackFunc)(int index, CallbackFunc func);
    typedef void(__stdcall* RunCallbackFunc)();

    static const int MaxCallbacks = 23;
    static inline CallbackFunc g_Callbacks[MaxCallbacks] = { nullptr };

    template<int RasterNum, RwRaster*& Raster>
    static CallbackFunc GenerateCallback() {
        CallbackFunc callbackPtr = [](const unsigned char* pngData, size_t dataLength) {

            rasterFromMemory(pngData, dataLength, Raster);

            latest_raster_num = RasterNum;

        };

        return callbackPtr;
    }

    static void RegisterCallback(int index, CallbackFunc callback) {
        if (index >= 0 && index < MaxCallbacks) {
            g_Callbacks[index] = callback;
        }
    }

    static void RunCallback(int index, const unsigned char* pngData, const size_t dataLength) {
        if (index >= 0 && index < MaxCallbacks && g_Callbacks[index]) {
            g_Callbacks[index](pngData, dataLength);
        }
    }

    public:

    static void Initialize() {
        if (initialized) return;
    
        initialized = true;

        CallbackFunc callback1 = GenerateCallback<1, raster1>();
        CallbackFunc callback2 = GenerateCallback<2, raster2>();
        CallbackFunc callback3 = GenerateCallback<3, raster3>();
        CallbackFunc callback4 = GenerateCallback<4, raster4>();
        CallbackFunc callback5 = GenerateCallback<5, raster5>();
        CallbackFunc callback6 = GenerateCallback<6, raster6>();
        CallbackFunc callback7 = GenerateCallback<7, raster7>();
        CallbackFunc callback8 = GenerateCallback<8, raster8>();
        CallbackFunc callback9 = GenerateCallback<9, raster9>();
        CallbackFunc callback10 = GenerateCallback<10, raster10>();
        CallbackFunc callback11 = GenerateCallback<11, raster11>();
        CallbackFunc callback12 = GenerateCallback<12, raster12>();
        CallbackFunc callback13 = GenerateCallback<13, raster13>();
        CallbackFunc callback14 = GenerateCallback<14, raster14>();
        CallbackFunc callback15 = GenerateCallback<15, raster15>();
        CallbackFunc callback16 = GenerateCallback<16, raster16>();

        CallbackFunc callback17 = GenerateCallback<17, raster17>();
        CallbackFunc callback18 = GenerateCallback<18, raster18>();
        CallbackFunc callback19 = GenerateCallback<19, raster19>();
        CallbackFunc callback20 = GenerateCallback<20, raster20>();
        CallbackFunc callback21 = GenerateCallback<21, raster21>();
        CallbackFunc callback22 = GenerateCallback<22, raster22>();
        CallbackFunc callback23 = GenerateCallback<23, raster23>();

        RegisterCallback(0, callback1);
        RegisterCallback(1, callback2);
        RegisterCallback(2, callback3);
        RegisterCallback(3, callback4);
        RegisterCallback(4, callback5);
        RegisterCallback(5, callback6);
        RegisterCallback(6, callback7);
        RegisterCallback(7, callback8);
        RegisterCallback(8, callback9);
        RegisterCallback(9, callback10);
        RegisterCallback(10, callback11);
        RegisterCallback(11, callback12);
        RegisterCallback(12, callback13);
        RegisterCallback(13, callback14);
        RegisterCallback(14, callback15);
        RegisterCallback(15, callback16);

        RegisterCallback(16, callback17);
        RegisterCallback(17, callback18);
        RegisterCallback(18, callback19);
        RegisterCallback(19, callback20);
        RegisterCallback(20, callback21);
        RegisterCallback(21, callback22);
        RegisterCallback(22, callback23);

        if (!dummySprite) {

            dummySprite = new CSprite2d();
            dummySprite->m_pTexture = textureFromPngFile("dummy.png");
            rasterFromPngFile("dummy.png", raster1);

            raster_index++;
        }

        Events::shutdownRwEvent += [] {
            dummySprite->Delete();
        };


        Events::d3dLostEvent += [] {
            dummySprite->Delete();
        };

        Events::d3dResetEvent += [] {
            dummySprite->Delete();
        };
    }

    static void Render(CRect rect, CRGBA color) {
        float u1 = 0.0f;
        float v1 = 0.0f;
        float u2 = 1.0f;
        float v2 = 0.0f;
        float u3 = 0.0f;
        float v3 = 1.0f;
        float u4 = 1.0f;
        float v4 = 1.0f;

        CSprite2d::SetVertices(rect, color, color, color, color, u1, v1, u2, v2, u3, v3, u4, v4);


        RwRenderStateSet(rwRENDERSTATETEXTURERASTER, getRasterToRender());

        RwIm2DRenderPrimitive(rwPRIMTYPETRIFAN, dummySprite->maVertices, 4);
        RwRenderStateSet(rwRENDERSTATETEXTURERASTER, 0);
    }

    static void SetFrame(const unsigned char* pngData, const size_t dataLength) {
            callback_num++;

            if (callback_num > MaxCallbacks) {
                callback_num = 1;
            }

            int callback_index = callback_num - 1;

            if (g_Callbacks[callback_index]) {

                RunCallback(callback_index, pngData, dataLength);
            }
    }
};

