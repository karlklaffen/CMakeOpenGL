#version 460 core

out vec4 FragColor;

const uint MAX_NUM_POINT_LIGHTS = 10;

in vec3 WorldPos;
//in vec3 Normal;
in vec2 TexCoords;

//in mat3 TBN;

struct PointLight {
	vec3 Pos;
	vec3 Color;
//	vec3 Power;
};

float near = 0.1;

uniform bool Fog;
uniform vec3 FogColor;

uniform vec3 ViewPos;
uniform float Far;

uniform uint NumPointLights;
uniform PointLight[MAX_NUM_POINT_LIGHTS] PointLights;

//uniform float AmbientStrength = 0.2;

uniform sampler2D DiffuseTex;
//uniform sampler2D SpecularTex;
uniform sampler2D NormalTex;
//uniform float Shininess;

//struct Material {
//	sampler2D Diffuse;
//	sampler2D Specular;
//	float Shininess;
//};
//
//struct DirLight {
//	vec3 Dir;
//
//	vec3 Ambient;
//	vec3 Diffuse;
//	vec3 Specular;
//};
//
//struct PointLight {
//	vec3 Pos;
//
//	float Constant;
//	float Linear;
//	float Quadratic;
//
//	vec3 Ambient;
//	vec3 Diffuse;
//	vec3 Specular;
//};
//


//
//uniform Material SelfMaterial;
//
//uniform DirLight MainLight;

//
//vec3 GetColor(DirLight light, vec3 normal, vec3 viewDir, vec3 diffTex) {
//
//	// ambient
//	vec3 ambientColor = light.Ambient * diffTex;
//
//	vec3 lightDir = normalize(-light.Dir);
//
//	// diffuse
//	float diffuseStrength = max(dot(normal, lightDir), 0.0);
//	vec3 diffuseColor = light.Diffuse * diffuseStrength * diffTex;
//
//	// specular
//	vec3 reflectDir = reflect(-lightDir, normal);
//	float specularStrength = pow(max(dot(viewDir, reflectDir), 0.0), SelfMaterial.Shininess);
//	vec3 specularColor = light.Specular * specularStrength * texture(SelfMaterial.Specular, TexCoords).rgb;
//
//	return ambientColor + diffuseColor + specularColor;
//}
//
//vec3 GetColor(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffTex) {
//
//	// reused calculations
//	vec3 lightDir = normalize(light.Pos - fragPos);
//
//	// ambient
//	vec3 ambientColor = light.Ambient * diffTex;
//
//	// diffuse
//	float diffuseStrength = max(dot(normal, lightDir), 0.0);
//	vec3 diffuseColor = light.Diffuse * diffuseStrength * diffTex;
//
//	// specular
//	vec3 reflectDir = reflect(-lightDir, normal);
//	float specularStrength = pow(max(dot(viewDir, reflectDir), 0.0), SelfMaterial.Shininess);
//	vec3 specularColor = light.Specular * specularStrength * texture(SelfMaterial.Specular, TexCoords).rgb;
//
//	// distance attenuation
//	float dist = length(light.Pos - FragPos);
//	float atten = 1.0 / (light.Constant + light.Linear * dist + light.Quadratic * (dist * dist));
//
//	ambientColor *= atten;
//	diffuseColor *= atten;
//	specularColor *= atten;
//
//	return ambientColor + diffuseColor + specularColor;
//}

float LinearizeZ(float z) {
	return (2.0 * near * Far) / (Far + near - (z * 2.0 - 1.0) * (Far - near));
}

void main()
{
//	vec3 viewDir = normalize(ViewPos - FragPos);
//	vec3 diffTex = texture(SelfMaterial.Diffuse, TexCoords).rgb;


//	vec3 finalColor = GetColor(MainLight, Normal, viewDir, diffTex);

//	for (int i = 0; i < NumPointLights; i++) {
//		finalColor += GetColor(PointLights[i], Normal, FragPos, viewDir, diffTex);
//	}
//		addedLight += PointLights[i].Color * (1.0 / max(distance(PointLights[i].Pos, WorldPos), 1.0));

	vec3 diffuseColor = texture(DiffuseTex, TexCoords).rgb;
//	vec3 specularColor = texture(SpecularTex, TexCoords).rgb;
//
//	vec3 normalTan = texture(NormalTex, TexCoords).rgb * 2.0 - 1.0; // normal in tangent space
//	vec3 normal = normalize(TBN * normalTan);
//	vec3 viewDir = normalize(-WorldPos);
//
//	vec3 finalColor = vec3(0.0);
//
//	for (uint i = 0; i < NumPointLights; i++) {
//
//		vec3 lightDir = PointLights[i].Pos - WorldPos;
//		vec3 halfDir = normalize(lightDir + viewDir);
//
//		float diffuseStrength = max(dot(lightDir, normal), 0.0);
//
//		float specAngle = max(dot(halfDir, normal), 0.0);
//		float specularStrength = pow(specAngle, Shininess);
//
//		float dist = distance(WorldPos, PointLights[i].Pos);
//
//		vec3 ambientColor = AmbientStrength * diffuseColor;
//		vec3 diffuseColor = diffuseStrength * diffuseColor * PointLights[i].Color / dist;
//		vec3 specularColor = specularStrength * specularColor * PointLights[i].Color / dist;
//
//		finalColor += ambientColor + diffuseColor + specularColor;
//	}
//
//	// fog
//	if (Fog) {
//		float depth = LinearizeZ(gl_FragCoord.z) / Far;
//		float fogStrength = min(depth, 1.0);
//
//		finalColor = mix(finalColor, FogColor, fogStrength);
//	}

	vec3 finalColor = diffuseColor;
	FragColor = vec4(finalColor, 1.0);
	
}