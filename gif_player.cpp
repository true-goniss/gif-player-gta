#include "plugin.h"

#include <stdexcept>
#include <iostream>

#include "RendererStreamedRasters.h"
#include "RendererPreloadedRasters.h"
#include "GifDecoder.h"

using namespace plugin;
using namespace std;

class gif_player {

    static inline const char* gifPath = "example.gif";

    std::vector<std::vector<unsigned char>> frames_gif = GifDecoder::processGif(gifPath);

    Renderer* renderer = new RendererPreloadedRasters(frames_gif);

    inline void DisplayFrames(const char* gifPath, std::vector<std::vector<unsigned char>> frames_gif) {

        std::this_thread::sleep_for(std::chrono::milliseconds(6000));

        while (!GifDecoder::checkIfGifWasLoaded(gifPath)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }

        if (typeid(*renderer) == typeid(RendererStreamedRasters)) {

            while (true) {
                for (const auto& frame : frames_gif) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(renderer->frameDelayMs));
                    renderer->SetFrame(frame.data(), frame.size());
                }
            }
        }
        else if (typeid(*renderer) == typeid(RendererPreloadedRasters)) {

            while (true) {

                std::this_thread::sleep_for(std::chrono::milliseconds(renderer->frameDelayMs));
                renderer->NextFrame();
            }
        }
    }

public:

    gif_player() {

        Events::drawHudEvent += [this] {
            renderer->Initialize();
        };

        //float mult1 = 2.7f;
        float mult2 = 0.55f;

        int screenShortSide = 227;
        int screenWideSide = 378;

        int gifWidth = screenShortSide;
        int gifHeight = screenWideSide; // (screenWideSide / 7) * 3;

        Events::drawingEvent += [this, screenShortSide, screenWideSide, gifHeight] {

            if (GifDecoder::checkIfGifWasLoaded(gifPath)){

                try {
                    int screenWidth = RsGlobal.maximumWidth;
                    int screenHeight = RsGlobal.maximumHeight;

                    int x1 = (screenWidth / 2) + 522;
                    int y1 = (screenHeight / 2) + 52; // + ((screenWideSide / 7) * 2);
                    int x2 = x1 + screenShortSide;
                    int y2 = y1 + gifHeight;

                    CRect rect(SCREEN_COORD(x1), SCREEN_COORD(y1),
                        SCREEN_COORD(x2), SCREEN_COORD(y2));

                    CRGBA color = CRGBA(255, 255, 255, 255);

                    renderer->Render(rect, color);
                }
                catch (std::exception ee) {}
            }
        };

        std::thread threadObj(&gif_player::DisplayFrames, this, gifPath, frames_gif);
        threadObj.detach();
    }
} _gif_player_SA;
