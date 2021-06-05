#version 330

layout (location = 0) in vec4 position;
layout (location = 1) in vec2 texCoord;

uniform mat4 mvpMatrix;
out vec4 gl_Position;

out vec2 Texcoord;

in float u;
in float v;

void main()
{   
    
   gl_Position = position;
   Texcoord =  texCoord;
}
