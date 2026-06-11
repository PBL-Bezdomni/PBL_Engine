#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D sceneColor;
uniform sampler2D sceneNormal;
uniform sampler2D sceneDepth;

uniform float near;
uniform float far;

float LinearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0; 
    return (2.0 * near * far) / (far + near - z * (far - near));    
}

void main() {
    float rawDepth = texture(sceneDepth, TexCoords).r;
    if (rawDepth >= 1.0) {
        FragColor = texture(sceneColor, TexCoords);
        return;
    }
    vec3 color = texture(sceneColor, TexCoords).rgb;
    vec2 texelSize = 1.0 / textureSize(sceneColor, 0);

    vec2 offsets[9] = vec2[](
        vec2(-texelSize.x,  texelSize.y), vec2(0.0,  texelSize.y), vec2(texelSize.x,  texelSize.y),
        vec2(-texelSize.x,         0.0), vec2(0.0,         0.0), vec2(texelSize.x,         0.0),
        vec2(-texelSize.x, -texelSize.y), vec2(0.0, -texelSize.y), vec2(texelSize.x, -texelSize.y)
    );

    float kernel[9] = float[](
        -1, -1, -1,
        -1,  8, -1,
        -1, -1, -1
    );


    float depthLaplacian = 0.0;
    vec3 normalLaplacian = vec3(0.0);

    for(int i = 0; i < 9; i++) {

        float sampleDepth = LinearizeDepth(texture(sceneDepth, TexCoords + offsets[i]).r);
        depthLaplacian += sampleDepth * kernel[i];


        vec3 sampleNormal = texture(sceneNormal, TexCoords + offsets[i]).rgb;
        normalLaplacian += sampleNormal * kernel[i];
    }


    float depthEdge = abs(depthLaplacian);

    float depthThreshold = 0.4; 
    depthEdge = (depthEdge > depthThreshold) ? 1.0 : 0.0;


    float normalEdge = length(normalLaplacian);

    float normalThreshold = 0.5; 
    normalEdge = (normalEdge > normalThreshold) ? 1.0 : 0.0;


    float outline = max(depthEdge, normalEdge);


    vec3 finalColor = mix(color, vec3(0.0, 0.0, 0.0), outline);

    FragColor = vec4(finalColor, 1.0);
}