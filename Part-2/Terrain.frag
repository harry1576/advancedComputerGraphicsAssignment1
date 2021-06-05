#version 400



uniform sampler2D grass;
uniform sampler2D water;
uniform sampler2D rock;
uniform sampler2D snow;

in vec4 oColour;

in vec2 Texcoord;
out vec4 outColor;
in vec4 texColor1;
in float texVector;

in float u;
in float v; 
in vec2 textcoord;

in float waterWeight;
in float grassWeight;
in float rockWeight;
in float snowWeight;



void main() 
{    
     outColor = (texture2D(water,textcoord) * waterWeight) + (texture2D(grass,textcoord) * grassWeight)  
    + (texture2D(rock,textcoord) * rockWeight) + (texture2D(snow,textcoord) * snowWeight)+(oColour * 0.35) ;
}
