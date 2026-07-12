attribute vec3 aPosition;
attribute vec4 aColor;

varying vec4 vColor;

uniform mat4 mvpMatrix;

void main()
{
    gl_Position = mvpMatrix * vec4(aPosition, 1.0);
    vColor = aColor;
}
