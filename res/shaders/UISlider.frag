#version 330 core

out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D texture_diffuse1;

uniform float slider_load;

vec3 red_color = vec3(1, 0, 0);
vec3 green_color = vec3(0, 1, 0);
vec3 yellow_color = vec3(1, 1, 0);

void main()
{
    vec4 texColor = texture(texture_diffuse1, TexCoord);

    if (TexCoord.x < slider_load)
    {
        if (slider_load < 0.3)
        { 
            texColor = vec4(red_color, 1);
        }
        else if(slider_load < 0.7)
        {
            texColor = vec4(yellow_color, 1);
        }
        else
        {
            texColor = vec4(green_color, 1);
        }
    }
    
    FragColor = texColor;
}