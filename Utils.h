#pragma once
#include "CSprite2d.h"
#include <mutex>

using namespace plugin;

std::mutex mtx;

static class Utils
{

public:

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
};

