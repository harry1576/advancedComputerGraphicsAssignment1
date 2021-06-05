#version 400
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;


uniform mat4 mvMatrixl;
uniform mat4 mvpMatrix;
uniform mat4 norMatrix;
uniform vec4 LightPos;
uniform vec4 norVector;

vec4 u;
vec4 v;
vec3 n;

vec3 lightPos;
vec3 lightColor;
vec3 ambientColor;
vec3 teaPotColor;
vec3 fragCentre;

vec3 viewPosition;
vec3 viewDirection;
vec3 reflectDir;
vec3 specular;

out vec4 oColour;

void main()
{             
    
    teaPotColor =  vec3(1, 0.1, 1); // Set Base Teapot Colour

    float ambientStrength = 0.3;    // Set Ambient Strength
    ambientColor = vec3(1.0, 1.0, 1.0); // Set Ambient Colour to White
    vec3 ambient = ambientStrength * ambientColor; // Calculate Ambient Vector

    u = gl_in[1].gl_Position - gl_in[0].gl_Position; // Work out two triangle vectors for the normal
    v = gl_in[2].gl_Position - gl_in[0].gl_Position;

    n.x = (u.y * v.z) - (u.z * v.y); // Calculate surface normal
    n.y = (u.z * v.x) - (u.x * v.z);
    n.z = (u.x * v.y) - (u.y * v.x);
    
    n = normalize(n);

    // add condition to only render fragments in view
    
    fragCentre.x = (gl_in[0].gl_Position.x + gl_in[1].gl_Position.x + gl_in[2].gl_Position.x)/3; // Find centre of triangle fragment
    fragCentre.y = (gl_in[0].gl_Position.y + gl_in[1].gl_Position.y + gl_in[2].gl_Position.y)/3; // Find centre of triangle fragment
    fragCentre.z = (gl_in[0].gl_Position.z + gl_in[1].gl_Position.z + gl_in[2].gl_Position.z)/3; // Find centre of triangle fragment

    lightPos =   vec3(1.0, 3.0, -10.0); // Put a light directly above teapot
    lightColor = vec3(1.0, 1.0, 1.0); // Set Light Colour to white.
    
    
    float diffCoff = 0.8;

    vec3 lightDir = normalize(lightPos - fragCentre);  // Calculate light direction and normalize
    float diff = max(dot(n,lightDir), 0.0); // Calculate diffuse coeffecient
    vec3 diffuse = diffCoff * diff * lightColor;    // Calculate diffuse vector
    

    float specularStrength = 8;

    viewPosition.x = mvMatrixl[3][0];
    viewPosition.y = mvMatrixl[3][1];
    viewPosition.z = mvMatrixl[3][2];

    //viewPosition = vec3(0.0, 3.0, -40.0);
    

    viewDirection = normalize(viewPosition - fragCentre);
    vec3 reflectDir = reflect(-lightDir, n);  

    float spec = pow(max(dot(viewDirection, reflectDir), 0.0), 5);
    if(dot(lightDir, n) > 0){
    specular = specularStrength * spec * lightColor; 
    }else{ specular = vec3(0,0,0);}

    

    int i;

    for(i=0; i < gl_in.length(); i++)
    {

        gl_Position = mvpMatrix * gl_in[i].gl_Position;
        oColour = vec4(((ambient + diffuse + specular) * teaPotColor),1.0);    // Update Triangle Colour    

        EmitVertex();
    }

    EndPrimitive();
}
