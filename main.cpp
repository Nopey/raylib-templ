#include "raylib.h"

#include "SimState.h"
#include "SimSource.h"

#if defined(PLATFORM_DESKTOP)
    #define GLSL_VERSION            330
#else   // PLATFORM_ANDROID, PLATFORM_WEB
    #define GLSL_VERSION            100
#endif

// TODO: break this off into its own file
char const *fragmentShader =
"#version 100\n"
"\n"
"precision highp float;\n"
"\n"
"// Input vertex attributes (from vertex shader)\n"
"varying vec2 fragTexCoord;\n"
//"varying vec4 fragColor;\n"
"\n"
"uniform sampler2D tempTex;\n"
"uniform sampler2D densTex;\n"
"\n"
"// Input parameters\n"
"uniform float bMod;\n"
"const float sbConstant = 0.000000000001;\n"
"\n"
"void main()\n"
"{\n"
"    // Read texutres\n"
"    float temp = texture2D(tempTex, fragTexCoord).x;\n"
"    float dens = texture2D(densTex, fragTexCoord).x;\n"
"\n"
"    // Blackbody intensity\n"
"    float intensity = sbConstant * temp * temp * temp * temp;\n"
"\n"
"    // BLackbody color temperature\n"
"    float t = (temp - 1900.0) / (5400.0 - 1900.0);\n"
"    float red = 1.0;\n"
"    float green = (147.0 + (255.0 - 147.0) * t) / 255.0;\n"
"    float blue = (41.0 + (251.0 - 41.0) * t) / 255.0;\n"
"\n"
"    // Final color as alpha mix of blackbody and density\n"
"    vec3 outColor = dens * bMod * intensity * vec3(red, green, blue);\n"
"    gl_FragColor = vec4(outColor, 1.0);\n"
"}\n"
;

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

    Image temp_img{
        .data = sim_state.fields.temp,
        .width = sim_texWidth,
        .height = sim_texWidth,
        .mipmaps = 1,
        .format = PIXELFORMAT_UNCOMPRESSED_R32,
    };
    Texture2D temp_texture = LoadTextureFromImage(temp_img);
    Image dens_img{
        .data = sim_state.fields.dens,
        .width = sim_texWidth,
        .height = sim_texWidth,
        .mipmaps = 1,
        .format = PIXELFORMAT_UNCOMPRESSED_R32,
    };
    Texture2D dens_texture = LoadTextureFromImage(dens_img);

    Shader shader = LoadShaderFromMemory(nullptr, fragmentShader);

    // Get variable (uniform) location on the shader to connect with the program
    // NOTE: If uniform variable could not be found in the shader, function returns -1
    int temp_location = GetShaderLocation(shader, "tempTex");
    int dens_location = GetShaderLocation(shader, "densTex");
    int brightness_location = GetShaderLocation(shader, "bMod");

    while (!WindowShouldClose())
    {
        sim_sources.UpdateSourcesDynamic();
        sim_state.SimulationStep(GetFrameTime());

        BeginDrawing();
            ClearBackground(RAYWHITE);
            int const pix_size = 4;

            // TODO: move texture updates out of BeginDrawing() block
#ifdef PLATFORM_WEB
            // HACK: raylib UpdateTexture doesn't work on web, so recreate the texture every frame!
            UnloadTexture(temp_texture);
            temp_texture = LoadTextureFromImage(temp_img);
            UnloadTexture(dens_texture);
            dens_texture = LoadTextureFromImage(dens_img);
#else
            UpdateTexture(temp_texture, sim_state.fields.temp);
            UpdateTexture(dens_texture, sim_state.fields.dens);
#endif

            BeginShaderMode(shader);
                SetShaderValueTexture(shader, temp_location, temp_texture);
                SetShaderValueTexture(shader, dens_location, dens_texture);
                float brightness = 50.0f;
                SetShaderValueV(shader, brightness_location, &brightness, SHADER_UNIFORM_FLOAT, 1);
                DrawTexture(temp_texture, 0, 0, WHITE);
            EndShaderMode();

            DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);
        EndDrawing();
    }

    UnloadShader(shader);       // Unload shader
    UnloadTexture(temp_texture);
    UnloadTexture(dens_texture);

    CloseWindow();

    return 0;
}
