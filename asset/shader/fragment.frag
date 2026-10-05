varying vec2 vUV;
varying vec3 vNormal;
varying vec3 vWorldPosition;

uniform sampler2D albedoTexture;
uniform vec4 materialColor;
uniform int materialUnlit;

const int MAX_LIGHTS = 4;

uniform int lightCount;

// xyz = normalized direction or point position, w = 0 directional, 1 point
uniform vec4 lightVectors[MAX_LIGHTS];

// rgb = color * intensity, precomputed on CPU
uniform vec4 lightColors[MAX_LIGHTS];

// CPU precomputed attenuation: clamp(y - distanceSquared * x, 0, 1)
uniform vec2 lightParams[MAX_LIGHTS];

const vec3 SHADOW_TINT = vec3(0.52, 0.58, 0.72);
const float LIGHT_THRESHOLD = 0.70;

const vec3 AMBIENT_COLOR = vec3(0.019, 0.0215, 0.027);

const float SECONDARY_LIGHT_STRENGTH = 0.18;

vec3 computeToonTint(float ndl)
{
    return mix(SHADOW_TINT, vec3(1.0), step(LIGHT_THRESHOLD, ndl));
}

vec3 computePointLight(vec3 position, vec3 color, vec2 params)
{
    vec3 toLight = position - vWorldPosition;
    float distanceSquared = dot(toLight, toLight);

    // Short edge fade with CPU precomputed coefficients, no bands or smoothstep
    float attenuation = clamp(params.y - distanceSquared * params.x, 0.0, 1.0);
    
    return color * attenuation;
}

vec3 computeDirectionalLight(vec3 normal, vec3 direction, vec3 color, float primary)
{
    // Direction is normalized on CPU, no distance or attenuation calculation
    float ndl = max(dot(normal, -direction), 0.0);
    
    vec3 diffuse = mix(vec3(ndl * SECONDARY_LIGHT_STRENGTH), computeToonTint(ndl), primary);
    
    return diffuse * color;
}

vec3 computeLight(vec3 normal, vec4 lightData, vec3 color, vec2 params, float primary)
{
    if (lightData.w == 1.0)
    {
        return computePointLight(lightData.xyz, color, params);
    }
    
    return computeDirectionalLight(normal, lightData.xyz, color, primary);
}

vec3 computeLighting(vec3 objectColor)
{
    vec3 result = AMBIENT_COLOR;

    // Unrolled loop for shader performances
    if (lightCount > 0)
    {
        result += computeLight(vNormal, lightVectors[0], lightColors[0].rgb, lightParams[0], 1.0);
        
        if (lightCount > 1)
        {
            result += computeLight(vNormal, lightVectors[1], lightColors[1].rgb, lightParams[1], 0.0);
            
            if (lightCount > 2)
            {
                result += computeLight(vNormal, lightVectors[2], lightColors[2].rgb, lightParams[2], 0.0);
                
                if (lightCount > 3)
                {
                    result += computeLight(vNormal, lightVectors[3], lightColors[3].rgb, lightParams[3], 0.0);
                }
            }
        }
    }

    return result * objectColor;
}

void main()
{
    vec4 albedo = texture2D(albedoTexture, vUV);
    vec3 objectColor = albedo.rgb * materialColor.rgb;
    float alpha = albedo.a * materialColor.a;

    vec3 result = objectColor;

    if (materialUnlit == 0)
    {
        result = computeLighting(objectColor);
    }

    gl_FragColor = vec4(result, alpha);
}
