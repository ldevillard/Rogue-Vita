attribute vec3 aPosition;
attribute vec3 aNormal;
attribute vec2 aUV;

attribute vec4 aBoneIndices;
attribute vec4 aBoneWeights;

const int MAX_BONES = 32;
const int MAX_BONE_INFLUENCES = 4;

varying vec2 vUV;
varying vec3 vNormal;
varying vec3 vWorldPosition;

uniform vec4 skinningMatrices[MAX_BONES * 3];
uniform mat4 viewProjectionMatrix;
uniform mat4 modelMatrix;

void skinVertex(out vec3 skinnedPosition, out vec3 skinnedNormal)
{
    skinnedPosition = vec3(0.0);
    skinnedNormal = vec3(0.0);

    vec4 localPosition = vec4(aPosition, 1.0);
    vec4 localNormal = vec4(aNormal, 0.0);

    for (int i = 0; i < MAX_BONE_INFLUENCES; i++)
    {
        float boneWeight = aBoneWeights[i];

        if (boneWeight <= 0.0)
            continue;

        int matrixIndex = int(aBoneIndices[i]) * 3;

        // Rebuild mat4 * vec4 from the three packed matrix rows
        vec3 bonePosition = vec3(
            dot(skinningMatrices[matrixIndex], localPosition),
            dot(skinningMatrices[matrixIndex + 1], localPosition),
            dot(skinningMatrices[matrixIndex + 2], localPosition));

        vec3 boneNormal = vec3(
            dot(skinningMatrices[matrixIndex], localNormal),
            dot(skinningMatrices[matrixIndex + 1], localNormal),
            dot(skinningMatrices[matrixIndex + 2], localNormal));

        skinnedPosition += bonePosition * boneWeight;
        skinnedNormal += boneNormal * boneWeight;
    }

    skinnedNormal = normalize(skinnedNormal);
}

void main()
{
    vec3 skinnedPosition;
    vec3 skinnedNormal;
    skinVertex(skinnedPosition, skinnedNormal);

    vec4 worldPosition = modelMatrix * vec4(skinnedPosition, 1.0);

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

    vUV = aUV;
    vNormal = normalize(normalMatrix * skinnedNormal * orientation);
    vWorldPosition = worldPosition.xyz;
}
