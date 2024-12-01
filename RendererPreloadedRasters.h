/*
	by gon_iss (c) 2024
*/

/*
	A class that creates defined quantity of rasters, with defined amount of data. The latest updated raster will be rendered
*/

#pragma once
#include "Renderer.h"

class RendererPreloadedRasters : public Renderer
{
	std::vector<std::vector<unsigned char>> frames;

public:

	RendererPreloadedRasters(std::vector<std::vector<unsigned char>> frames) {
		this->frames = frames;
		rasters.resize(frames.size(), nullptr);
	}

	void SpecificInitialize() override {
		auto& rasters = getRasters();

		if (rasters.size() > 0) {
			Utils::rasterFromPngFile("dummy.png", rasters[0]);
			raster_index++;
		}

		for (size_t i = 0; i < frames.size(); ++i) {
			std::vector<unsigned char> frame = frames[i];
			Utils::rasterFromMemory(frame.data(), frame.size(), rasters[i]);
		}
	}

	void NextFrame() override {
		if (latest_raster_num > rasters.size() - 1) latest_raster_num = 1;
		latest_raster_num++;
	}

	~RendererPreloadedRasters() override = default;

};

