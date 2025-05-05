#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;

out vec2 TexCoords;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

void main()
{
    TexCoords = a_TexCoord;    
    gl_Position = u_Projection * u_View * u_Model * vec4(a_Position, 1.0);
}

#type fragment
#version 450 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D u_DiffuseTexture;

void main()
{
    vec4 texColor = texture(u_DiffuseTexture, TexCoords);
    if(texColor.rgb == vec3(0))
        texColor = vec4(1.0);
    FragColor = texColor;
}