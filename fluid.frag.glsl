#version 100

precision highp float;

// Input vertex attributes (from vertex shader)
varying vec2 fragTexCoord;
//varying vec4 fragColor;

uniform sampler2D tempTex;
uniform sampler2D densTex;

// Input parameters
uniform float bMod;
const float sbConstant = 0.000000000001;

void main()
{
    // Read texutres
    float temp = texture2D(tempTex, fragTexCoord).x;
    float dens = texture2D(densTex, fragTexCoord).x;

    // Blackbody intensity
    float intensity = sbConstant * temp * temp * temp * temp;

    // BLackbody color temperature
    float t = (temp - 1900.0) / (5400.0 - 1900.0);
    float red = 1.0;
    float green = (147.0 + (255.0 - 147.0) * t) / 255.0;
    float blue = (41.0 + (251.0 - 41.0) * t) / 255.0;

    // Final color as alpha mix of blackbody and density
    vec3 outColor = dens * bMod * intensity * vec3(red, green, blue);
    gl_FragColor = vec4(outColor, 1.0);
}
