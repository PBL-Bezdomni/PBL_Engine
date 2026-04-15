#version 330 core

out vec4 FragColor;
in vec3 TexCoord;
uniform samplerCube skybox;
uniform float time;

void main()
{             
    //FragColor = texture(skybox, TexCoords);
	//to nizej jest do niestalego tla
	float speed = time * 0.1;
	float s = sin(speed);
	float c = cos(speed);

	mat3 rotationY = mat3(
		c,   0.0, s,
		0.0, 1.0, 0.0,
		-s,  0.0, c
	);

	vec3 rotatedTexCoord = rotationY * TexCoord;

	FragColor = texture(skybox, rotatedTexCoord);
}