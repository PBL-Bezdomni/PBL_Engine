#version 330 core

in vec2 TexCoord;
uniform sampler2D texture_diffuse1;

void main()
{             

    float alpha = texture(texture_diffuse1, TexCoord).a;
    
    if (alpha < 0.1)
    {
        discard;
    }

    // gl_FragDepth = gl_FragCoord.z;
}