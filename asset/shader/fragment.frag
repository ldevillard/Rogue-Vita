varying vec4 vColor;
varying vec3 vNormal;
varying vec3 vWorldPosition;

const int MAX_LIGHTS = 4;

uniform int lightCount;
uniform vec4 lightDirections[4];
uniform vec4 lightColors[4];
uniform vec3 cameraPosition;

void main()
{
    vec3 normal = normalize(vNormal);
    vec3 viewDirection = normalize(cameraPosition - vWorldPosition);
    vec3 lighting = vec3(0.1);

    const float specularStrength = 0.5;
    const float shininess = 32.0;

    for (int i = 0; i < MAX_LIGHTS; ++i)
    {
        if (i < lightCount)
        {
            vec3 lightDirection = normalize(-lightDirections[i].xyz);
            vec3 halfDirection = normalize(lightDirection + viewDirection);
            vec3 radiance = lightColors[i].rgb * lightColors[i].a;

            float diffuse = max(dot(normal, lightDirection), 0.0);
            float specular = 0.0;

            if (diffuse > 0.0)
            {
                specular = pow(max(dot(normal, halfDirection), 0.0), shininess);
            }

            lighting += radiance * (diffuse + specularStrength * specular);
        }
    }

    gl_FragColor = vec4(vColor.rgb * lighting, vColor.a);
}
