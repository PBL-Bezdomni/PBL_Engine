#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform ivec4 u_Needs; 
uniform int u_NumNeeds; 

void main()
{
    vec2 centerUV = TexCoords - vec2(0.5);
    float radius = length(centerUV);
    
    if (radius > 0.5) discard;

    float angle = atan(centerUV.y, centerUV.x); 
    float normalizedAngle = (angle + 3.14159265) / (2.0 * 3.14159265);

    int currentSlice = int(floor(normalizedAngle * float(u_NumNeeds)));
    currentSlice = clamp(currentSlice, 0, u_NumNeeds - 1);

    int currentNeed = -1;

    if (currentSlice == 0) currentNeed = u_Needs.x;
    else if (currentSlice == 1) currentNeed = u_Needs.y;
    else if (currentSlice == 2) currentNeed = u_Needs.z;
    else if (currentSlice == 3) currentNeed = u_Needs.w;

    vec3 sliceColor = vec3(0.5); 
    
    if (currentNeed == 0)      sliceColor = vec3(0.0, 0.4, 1.0);
    else if (currentNeed == 1) sliceColor = vec3(1.0, 0.2, 0.2);
    else if (currentNeed == 2) sliceColor = vec3(1.0, 0.75, 0.2);
    else if (currentNeed == 3) sliceColor = vec3(0.5, 0.5, 0.5);

    FragColor = vec4(sliceColor, 0.45); 
}