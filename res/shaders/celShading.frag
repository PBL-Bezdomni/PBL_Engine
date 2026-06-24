#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D sceneColor;
uniform sampler2D sceneNormal;
uniform sampler2D sceneDepth;
uniform sampler2D sceneGlow;

uniform float near;
uniform float far;
uniform bool isCellShaded;

float LinearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0; 
    return (2.0 * near * far) / (far + near - z * (far - near));    
}

void main() {
    if(isCellShaded){
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

        float depthThreshold = 0.1; 
        depthEdge = (depthEdge > depthThreshold) ? 1.0 : 0.0;


        float normalEdge = length(normalLaplacian);

        float normalThreshold = 0.9; 
        normalEdge = (normalEdge > normalThreshold) ? 1.0 : 0.0;


        float outline = max(depthEdge, normalEdge);

        vec3 glowColor = vec3(0.0);
        vec2 glowTexel = 1.0 / textureSize(sceneGlow, 0);

        for(int x = -2; x <= 2; ++x) {
            for(int y = -2; y <= 2; ++y) {
                vec2 offset = vec2(float(x), float(y)) * glowTexel;
                glowColor += texture(sceneGlow, TexCoords + offset).rgb;
            }
        }

        glowColor /= 25.0;

        vec3 finalColor = mix(color, vec3(0.0, 0.0, 0.0), outline) + glowColor;

        FragColor = vec4(finalColor, 1.0);
    }
    else{
        FragColor = texture(sceneColor, TexCoords);
    }
}