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

vec3 computeLighting(vec3 objectColor)
{
    vec3 viewDirection = normalize(cameraPosition - vWorldPosition);
    vec3 normal = normalize(vNormal);

    vec3 result = vec3(0.0);

    for (int i = 0; i < MAX_LIGHTS; i++)
    {
        if (i >= lightCount)
            continue;

        vec3 lightColor = lightColors[i].rgb * lightColors[i].a;

        // Ambient
        float ambientStrength = 0.1;
        vec3 ambient = ambientStrength * lightColor;

        // Diffuse
        vec3 lightDirection = normalize(-lightDirections[i].xyz);
        float diffuseFactor = max(dot(normal, lightDirection), 0.0);
        vec3 diffuse = diffuseFactor * lightColor;

        // Specular (Phong)
        float specularStrength = 0.04;
        vec3 reflectDirection = reflect(-lightDirection, normal);
        float specularFactor = pow(max(dot(viewDirection, reflectDirection), 0.0), 12.0);
        vec3 specular = specularStrength * specularFactor * lightColor;

        result += (ambient + diffuse + specular) * objectColor;
    }

    return result;
}

void main()
{
    vec4 albedo = texture2D(albedoTexture, vUV);
    vec3 objectColor = albedo.rgb * materialColor.rgb;
    vec3 result = objectColor;

    if (materialUnlit == 0)
    {
        result = computeLighting(objectColor);
    }

    gl_FragColor = vec4(result, albedo.a * materialColor.a);
}
