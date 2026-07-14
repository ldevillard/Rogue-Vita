attribute vec3 aPosition;
attribute vec3 aNormal;
attribute vec4 aColor;

varying vec4 vColor;
varying vec3 vNormal;
varying vec3 vWorldPosition;

uniform mat4 viewProjectionMatrix;
uniform mat4 modelMatrix;

void main()
{
    vec4 worldPosition = modelMatrix * vec4(aPosition, 1.0);

    vec3 modelColumn0 = modelMatrix[0].xyz;
    vec3 modelColumn1 = modelMatrix[1].xyz;
    vec3 modelColumn2 = modelMatrix[2].xyz;

    vec3 normalColumn0 = cross(modelColumn1, modelColumn2);
    vec3 normalColumn1 = cross(modelColumn2, modelColumn0);
    vec3 normalColumn2 = cross(modelColumn0, modelColumn1);

    mat3 normalMatrix = mat3(normalColumn0, normalColumn1, normalColumn2);
    float determinant = dot(modelColumn0, normalColumn0);
    float orientation = determinant < 0.0 ? -1.0 : 1.0;

    gl_Position = viewProjectionMatrix * worldPosition;

    vColor = aColor;
    vNormal = normalize(normalMatrix * aNormal * orientation);
    vWorldPosition = worldPosition.xyz;
}
