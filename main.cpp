#include "raylib.h"

#include "SimState.h"
#include "SimSource.h"
#include <cstring>

int main(void)
{
    // sim settings from FluidSim's match.json
    // https://github.com/HollowaySean/FluidSimulator/blob/2d71e00ca14076590034b5e943e9e67a5ae3d3d9/src/json/match.json
    int sim_resolution = 80;
    int sim_texWidth = sim_resolution + 2;
    SimState sim_state(sim_resolution, SimParams{
        .closedBoundaries = false,
        .advancedCoefficients = true,
        .gravityOn = true,
        .temperatureOn = true,
        .solverSteps = 20,

        .lengthScale = 0.5f,
        .timeScale = 1.0f,
        .visc = 0.000018f,
        .diff = 0.000028f,
        .grav = -9.8f,
        .airDens = 1.29235f,
        .massRatio = 0.54f,
        .airTemp = 300.0f,
        .diffTemp = 0.0002338f,
        .densDecay = 0.0f,
        .tempFactor = 0.0f,
        .tempDecay = 0.0f,
    });
    SimSource sim_sources(&sim_state);
    sim_sources.CreateGasSourceDynamic(SimSource::circle, 25.0f, 2500.0f, 0.0f, -0.5f, 0.05f, 1.0f, 100.0f);
    sim_sources.CreateWindBoundaryDynamic(0.0f, 0.01f);

    InitWindow(800, 450, "raylib + FluidSim");

    size_t imageDataSize = sim_texWidth * sim_texWidth * 3;
    char *imageData = new char[imageDataSize];
    std::memset(imageData, 0, imageDataSize);
    Image img{
        .data = imageData,
        .width = sim_texWidth,
        .height = sim_texWidth,
        .mipmaps = 1,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8,
    };
    Texture2D texture = LoadTextureFromImage(img);

    while (!WindowShouldClose())
    {
        sim_sources.UpdateSourcesDynamic();
        sim_state.SimulationStep(GetFrameTime());

        BeginDrawing();
            ClearBackground(RAYWHITE);
            int const pix_size = 4;
#if 0
            for(int x = 0; x < sim_texWidth; x++){
                for(int y = 0; y < sim_texWidth; y++){
                    Color color;
                    color.r = sim_state.fields.temp[x + y * sim_texWidth] / 5.0f;
                    color.g = sim_state.fields.dens[x + y * sim_texWidth] * 1000.0f;
                    color.b = 0;
                    color.a = 255;
                    DrawRectangle(pix_size * x, pix_size * (sim_texWidth - y), pix_size, pix_size, color);
                }
            }
#else
            for(int x = 0; x < sim_texWidth; x++){
                for(int y = 0; y < sim_texWidth; y++){
                    int i = x + y * sim_texWidth;
                    int i3 = (x + (sim_texWidth - y - 1) * sim_texWidth) * 3;
                    imageData[i3] = sim_state.fields.temp[i] / 5.0f;
                    imageData[i3 + 1] = sim_state.fields.dens[i] / 5.0f;
                    // [i3 + 2] = 0;
                }
            }
            // HACK: raylib UpdateTexture doesn't work on web, so recreate the texture every frame!
            UnloadTexture(texture);
            texture = LoadTextureFromImage(img);
            DrawTextureEx(texture, Vector2{0, 0}, 0.0f, pix_size, WHITE);
#endif
            DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);
        EndDrawing();
    }

    UnloadTexture(texture);

    CloseWindow();

    return 0;
}
