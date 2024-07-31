#version 460 core
out vec4 colourFragment;

in vec2 textureCoordinates;
in vec3 transposedNormals;
in vec3 fragmentPosition;
in vec3 lightColour;

uniform vec3 uLightPosition; 
uniform vec3 uViewPosition; 
uniform vec3 uObjectColour;
uniform sampler2D uTexture;

void main()
{
    // ambient light component
    float ambientCoefficient = 0.4f;
    vec3 ambient = ambientCoefficient * lightColour;
  	
    // diffuse light component
	float diffuseCoefficient = 0.8f;	
    vec3 normalizedNormal = normalize(transposedNormals);
    vec3 lightDirection = normalize(uLightPosition - fragmentPosition);
    float diffuseMax = max(dot(normalizedNormal, lightDirection), 0.0);
    vec3 diffuse = diffuseCoefficient * diffuseMax * lightColour;

	// resultant output fragment colour
	vec3 ambientDiffuseSpecular = (ambient + diffuse) * uObjectColour;
    colourFragment =  vec4(ambientDiffuseSpecular, 1.0) * texture(uTexture, textureCoordinates);
} 