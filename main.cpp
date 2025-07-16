#include "raylib.h"

#include "SimState.h"
#include "SimSource.h"

int main(void)
{
    // sim settings from FluidSim's match.json
    // https://github.com/HollowaySean/FluidSimulator/blob/2d71e00ca14076590034b5e943e9e67a5ae3d3d9/src/json/match.json
    int sim_resolution = 80;
    int sim_texWidth = sim_resolution + 2;
    SimParams sim_params;
    sim_params.lengthScale = 0.5f;
    sim_params.timeScale = 1.0f;
    sim_params.visc = 0.000018f;
    sim_params.diff = 0.000028f;
    sim_params.grav = -9.8f;
    sim_params.airDens = 1.29235f;
    sim_params.massRatio = 0.54f;
    sim_params.airTemp = 300.0f;
    sim_params.diffTemp = 0.0002338f;
    sim_params.densDecay = 0.0f;
    sim_params.tempFactor = 0.0f;
    sim_params.tempDecay = 0.0f;
    sim_params.closedBoundaries = false;
    sim_params.advancedCoefficients = true;
    sim_params.gravityOn = true;
    sim_params.temperatureOn = true;
    sim_params.solverSteps = 20;
    SimState sim_state(sim_resolution, sim_params);
    SimSource sim_sources(&sim_state);
    sim_sources.CreateGasSourceDynamic(SimSource::circle, 25.0f, 2500.0f, 0.0f, -0.5f, 0.05f, 1.0f, 100.0f);
    sim_sources.CreateWindBoundaryDynamic(0.0f, 0.01f);

    InitWindow(800, 450, "raylib + FluidSim");

    while (!WindowShouldClose())
    {
        sim_sources.UpdateSourcesDynamic();
        sim_state.SimulationStep(GetFrameTime());

        BeginDrawing();
            int const pix_size = 3;
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
            ClearBackground(RAYWHITE);
            DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
