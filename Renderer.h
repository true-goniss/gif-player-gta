/*
    by gon_iss (c) 2024
*/

#pragma once
#include "CSprite2d.h"
#include "Utils.h"

using namespace plugin;

class Renderer
{

public:

	static inline bool initialized = false;
	static inline int frameDelayMs = 16; // delay between the frames (ms)
	static inline CSprite2d* dummySprite = nullptr;

    static inline std::vector<RwRaster*> rasters = { nullptr };

    int latest_raster_num = 1;
    int raster_index = 0;

	virtual ~Renderer() {}

	virtual void Initialize() { 
		if (initialized) return;

		initialized = true;

        if (!dummySprite) {
            dummySprite = new CSprite2d();
            dummySprite->m_pTexture = Utils::textureFromPngFile("dummy.png");
        }

		SpecificInitialize();

        Events::shutdownRwEvent += [this] {
            dummySprite->Delete();
        };


        Events::d3dLostEvent += [this] {
            dummySprite->Delete();
        };

        Events::d3dResetEvent += [this] {
            dummySprite->Delete();
        };
	}

    virtual void Render(CRect rect, CRGBA color) {
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

    std::vector<RwRaster*>& getRasters() {
        return rasters;
    }

    RwRaster* getRasterToRender() {
        return rasters[latest_raster_num - 1];
    }

    virtual void SetFrame(const unsigned char* pngData, const size_t dataLength) { } // for streamed raster renderer
    virtual void NextFrame() { };                                                   // for preloaded raster renderer

protected:

	virtual void SpecificInitialize() = 0;
};

