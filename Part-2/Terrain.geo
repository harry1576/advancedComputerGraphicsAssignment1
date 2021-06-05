#version 400

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

uniform mat4 mvpMatrix;
uniform sampler2D grass;
uniform float waterLevel;
uniform float snowLevel;


vec4 u;
vec4 v;
vec3 n;

vec3 lightPos;
vec3 lightColor;
vec3 fragCentre;

vec3 terrainColour;

vec3 ambientColor;

out vec4 oColour;

out vec4 texColor1;

out vec2 textcoord;


out float waterWeight;
out float grassWeight;
out float rockWeight;
out float snowWeight;

void main()
{
   
    terrainColour =  vec3(0, 0, 1); // Set Base Teapot Colour

    float ambientStrength = 0.15;    // Set Ambient Strength
    ambientColor = vec3(1.0, 1.0, 1.0); // Set Ambient Colour to White
    vec3 ambient = ambientStrength * ambientColor; // Calculate Ambient Vector
    
    u = gl_in[1].gl_Position - gl_in[0].gl_Position; // Work out two triangle vectors for the normal
    v = gl_in[2].gl_Position - gl_in[0].gl_Position;
    
    n.x = (u.y * v.z) - (u.z * v.y); // Calculate surface normal
    n.y = (u.z * v.x) - (u.x * v.z);
    n.z = (u.x * v.y) - (u.y * v.x);
    n = normalize(n);
    
    fragCentre.x = (gl_in[0].gl_Position.x + gl_in[1].gl_Position.x + gl_in[2].gl_Position.x)/3; // Find centre of triangle fragment
    fragCentre.y = (gl_in[0].gl_Position.y + gl_in[1].gl_Position.y + gl_in[2].gl_Position.y)/3; // Find centre of triangle fragment
    fragCentre.z = (gl_in[0].gl_Position.z + gl_in[1].gl_Position.z + gl_in[2].gl_Position.z)/3; // Find centre of triangle fragment


    lightPos =   vec3(0.0, 20.0, -80.0); // Put a light directly above teapot
    lightColor = vec3(1.0, 1.0, 1.0); // Set Light Colour to white.

    
    vec3 lightDir = normalize(lightPos - fragCentre);  // Calculate light direction and normalize  
    float diff = max(dot(n,lightDir), 0.0); // Calculate diffuse coeffecient
    vec3 diffuse =  diff * lightColor;    // Calculate diffuse vector

    int i;

    for(i=0; i < 3; i++)
    {   

        waterWeight=0;
        grassWeight=0;
        rockWeight=0;
        snowWeight=0;
       
         if (fragCentre.y <= waterLevel + 0.01)
        {
            waterWeight = 1;

        }


        if (fragCentre.y <= 5 && fragCentre.y > waterLevel + 0.01)
        {
            grassWeight = 1;
        }
        if (fragCentre.y <= 5.5 && fragCentre.y > 4)
        {
            grassWeight = 1 - (fragCentre.y-4)/(5.5-4);
            rockWeight = 1 - (1 - (fragCentre.y-4)/(5.5-4));
        }

        if (fragCentre.y <= snowLevel - 3.5 && fragCentre.y > 5.5)
        {
            rockWeight = 1;
        }


        if (fragCentre.y <= snowLevel && fragCentre.y > snowLevel - 3.5)
        {
            rockWeight = 1 - (fragCentre.y-(snowLevel - 3.5))/(snowLevel-(snowLevel - 3.5));
            snowWeight = 1 - (1 - (fragCentre.y-(snowLevel - 3.5))/(snowLevel-(snowLevel - 3.5)));
        }
        if (fragCentre.y > snowLevel)
        {
            snowWeight = 1;
        }


        if(i == 0)
        {
             textcoord = vec2(0.1,0);
        }
        else if(i == 2)
        {
             textcoord = vec2(0.9,0.5);
        }
        else if(i == 1)
        {
            textcoord = vec2(.9,0.00);
        }


        oColour = vec4(diffuse + ambient,1.0);
        gl_Position = mvpMatrix * gl_in[i].gl_Position;

        EmitVertex();
    }





    EndPrimitive();
}
