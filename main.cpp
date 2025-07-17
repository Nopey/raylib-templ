#include "raylib.h"

#include "SimSource.h"
#include "SimState.h"

#include "embed.cmake.h"

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
                                           .grav = 9.8f,
                                           .airDens = 1.29235f,
                                           .massRatio = 0.54f,
                                           .airTemp = 300.0f,
                                           .diffTemp = 0.0002338f,
                                           .densDecay = 0.0f,
                                           .tempFactor = 0.0f,
                                           .tempDecay = 0.0f,
                                       });
    SimSource sim_sources(&sim_state);
    sim_sources.CreateGasSourceDynamic(SimSource::circle, 25.0f, 2500.0f, 0.0f, 0.5f, 0.05f, 1.0f, 100.0f);
    sim_sources.CreateWindBoundaryDynamic(0.0f, 0.01f);

    InitWindow(800, 450, "raylib + FluidSim");

    Image temp_img{
        .data = sim_state.fields.temp,
        .width = sim_texWidth,
        .height = sim_texWidth,
        .mipmaps = 1,
        .format = PIXELFORMAT_UNCOMPRESSED_R32,
    };
    Texture2D temp_texture = LoadTextureFromImage(temp_img);
    SetTextureFilter(temp_texture, TEXTURE_FILTER_BILINEAR);
    Image dens_img{
        .data = sim_state.fields.dens,
        .width = sim_texWidth,
        .height = sim_texWidth,
        .mipmaps = 1,
        .format = PIXELFORMAT_UNCOMPRESSED_R32,
    };
    Texture2D dens_texture = LoadTextureFromImage(dens_img);
    SetTextureFilter(dens_texture, TEXTURE_FILTER_BILINEAR);


    Shader shader = LoadShaderFromMemory(nullptr, (char const *)fluid_frag_glsl);

    // Get variable (uniform) location on the shader to connect with the program
    // NOTE: If uniform variable could not be found in the shader, function returns -1
    int temp_location = GetShaderLocation(shader, "tempTex");
    int dens_location = GetShaderLocation(shader, "densTex");
    int brightness_location = GetShaderLocation(shader, "bMod");

    while (!WindowShouldClose())
    {
        sim_sources.UpdateSourcesDynamic();
        sim_state.SimulationStep(GetFrameTime());

        UpdateTexture(temp_texture, sim_state.fields.temp);
        UpdateTexture(dens_texture, sim_state.fields.dens);

        BeginDrawing();
        {
            ClearBackground(DARKPURPLE);

            BeginShaderMode(shader);
            {
                SetShaderValueTexture(shader, temp_location, temp_texture);
                SetShaderValueTexture(shader, dens_location, dens_texture);
                float brightness = 50.0f;
                SetShaderValueV(shader, brightness_location, &brightness, SHADER_UNIFORM_FLOAT, 1);
                DrawTexture(temp_texture, 0, 0, WHITE);
                Rectangle source = { 0.0f, 0.0f, (float)temp_texture.width, (float)temp_texture.height };
                Rectangle dest = { 0.0f, 0.0f, (float)GetRenderWidth(), (float)GetRenderHeight() };
                DrawTexturePro(temp_texture, source, dest, Vector2(0.0f, 0.0f), 0.0f, WHITE);
            }
            EndShaderMode();

            DrawText("look, a match!", 190, 200, 20, LIGHTGRAY);
            DrawFPS(700, 0);
        }
        EndDrawing();
    }

    UnloadShader(shader); // Unload shader
    UnloadTexture(temp_texture);
    UnloadTexture(dens_texture);

    CloseWindow();

    return 0;
}
