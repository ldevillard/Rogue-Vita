varying vec2 vUV;
varying vec3 vNormal;
varying vec3 vWorldPosition;

uniform sampler2D albedoTexture;
uniform vec4 materialColor;
uniform int materialUnlit;

const int MAX_LIGHTS = 4;

uniform int lightCount;
uniform vec4 lightDirections[MAX_LIGHTS];
uniform vec4 lightColors[MAX_LIGHTS];

uniform vec3 cameraPosition;

const vec3 SHADOW_TINT = vec3(0.52, 0.58, 0.72);
const vec3 MID_TINT = vec3(0.76, 0.79, 0.84);
const float SHADOW_THRESHOLD = 0.35;
const float LIGHT_THRESHOLD = 0.70;
const float BAND_SMOOTHNESS = 0.025;

const vec3 RIM_COLOR = vec3(0.35, 0.55, 0.85);
const float RIM_STRENGTH = 0.4;

const vec3 SKY_AMBIENT = vec3(0.12, 0.15, 0.20);
const vec3 GROUND_AMBIENT = vec3(0.07, 0.065, 0.07);
const float AMBIENT_STRENGTH = 0.20;

const float SECONDARY_LIGHT_STRENGTH = 0.18;

vec3 computeAmbient(vec3 normal)
{
    float up = normal.y * 0.5 + 0.5;
    vec3 ambientColor = mix(GROUND_AMBIENT, SKY_AMBIENT, up);

    return ambientColor * AMBIENT_STRENGTH;
}

vec3 computeToonColor(vec3 objectColor, float ndl)
{
    float shadowToMid = smoothstep(
        SHADOW_THRESHOLD - BAND_SMOOTHNESS,
        SHADOW_THRESHOLD + BAND_SMOOTHNESS,
        ndl
    );

    float midToLight = smoothstep(
        LIGHT_THRESHOLD - BAND_SMOOTHNESS,
        LIGHT_THRESHOLD + BAND_SMOOTHNESS,
        ndl
    );

    vec3 shadowColor = objectColor * SHADOW_TINT;
    vec3 midColor = objectColor * MID_TINT;
    vec3 lightColor = objectColor;

    vec3 color = mix(shadowColor, midColor, shadowToMid);
    color = mix(color, lightColor, midToLight);

    return color;
}

vec3 computeLighting(vec3 objectColor)
{
    vec3 normal = normalize(vNormal);
    vec3 viewDirection = normalize(cameraPosition - vWorldPosition);

    vec3 result = objectColor * computeAmbient(normal);

    for (int i = 0; i < MAX_LIGHTS; i++)
    {
        if (i >= lightCount)
            continue;

        vec3 lightDirection = normalize(-lightDirections[i].xyz);
        vec3 lightColor = lightColors[i].rgb * lightColors[i].a;

        float ndl = max(dot(normal, lightDirection), 0.0);

        if (i == 0)
        {
            result += computeToonColor(objectColor, ndl) * lightColor;
        }
        else
        {
            result += objectColor * lightColor * ndl * SECONDARY_LIGHT_STRENGTH;
        }
    }

    float rim = 1.0 - max(dot(normal, viewDirection), 0.0);
    rim = smoothstep(0.74, 0.94, rim);
    result += RIM_COLOR * rim * RIM_STRENGTH;

    return result;
}

void main()
{
    vec4 albedo = texture2D(albedoTexture, vUV);

    vec3 objectColor = albedo.rgb * materialColor.rgb;
    float alpha = albedo.a * materialColor.a;

    vec3 result;

    if (materialUnlit != 0)
    {
        result = objectColor;
    }
    else
    {
        result = computeLighting(objectColor);
    }

    gl_FragColor = vec4(result, alpha);
}
