#version 460 core

const float PI = 3.14159265359;
const float EPS = 0.001;
const uint MAX_NUM_POINT_LIGHTS = 10;
const float near = 0.1;

//struct DirLight {
//	vec3 Dir;
//
//	vec3 Ambient;
//	vec3 Diffuse;
//	vec3 Specular;
//};

struct PointLight {
	vec3 Pos;
	vec3 Color;
};

out vec4 FragColor;

uniform sampler2D NormalTex;
uniform sampler2D AlbedoTex;
uniform sampler2D ARMTex;

uniform vec3 ViewPos;
uniform float Far;
uniform bool Fog;
uniform vec3 FogColor;

uniform uint NumPointLights;
uniform PointLight[MAX_NUM_POINT_LIGHTS] PointLights;
//uniform DirLight MainLight;

in vec3 WorldPos;
in vec2 TexCoords;
in mat3 TBN;

float DistributionGGX(vec3 N, vec3 H, float roughness) { // normal, halfway, roughness
	float a2 = pow(pow(roughness, 2), 2);
	float num = a2;
	float NdotH = max(dot(N, H), 0.0);
	float denom = PI * pow(pow(NdotH, 2) * (a2 - 1.0) + 1.0, 2);
	
	return num / denom;
}

float GeometrySchlickGGX(vec3 N, vec3 V, float roughness) {
	float NdotV = max(dot(N, V), 0.0);
	float k = pow(roughness + 1.0, 2) / 8.0; // direct lighting (not IBL)

	float num = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
	return GeometrySchlickGGX(N, V, roughness)
	     * GeometrySchlickGGX(N, L, roughness);
}

vec3 FresnelSchlick(float cosTheta, vec3 F0) {
	return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float LinearizeZ(float z) {
	return (2.0 * near * Far) / (Far + near - (z * 2.0 - 1.0) * (Far - near));
}

void main()
{
	vec3 albedo = pow(texture(AlbedoTex, TexCoords).rgb, vec3(2.2));
	float ao = texture(ARMTex, TexCoords).r;
	float roughness = texture(ARMTex, TexCoords).g;
	float metallic = texture(ARMTex, TexCoords).b;
	
	vec3 normalTan = texture(NormalTex, TexCoords).rgb * 2.0 - 1.0; // normal in tangent space
	vec3 normal = normalize(TBN * normalTan);

	vec3 viewDir = normalize(ViewPos - WorldPos);

	vec3 F0 = mix(vec3(0.04), albedo, metallic); // 0.04 = non-metal, albedo = metal (interpolate)

	vec3 L0 = vec3(0.0); // irradiance = total radiance

	for (int i = 0; i < NumPointLights; i++) {
		vec3 L = normalize(PointLights[i].Pos - WorldPos);
		vec3 halfwayVec = normalize(viewDir + L);
		float dist = length(PointLights[i].Pos - WorldPos);
		float atten = 1.0 / pow(dist, 2);
		vec3 radiance = PointLights[i].Color * atten;

		// Cook-Torrance BRDF
		float NDF = DistributionGGX(normal, halfwayVec, roughness);
		float G = GeometrySmith(normal, viewDir, L, roughness);
		vec3 F = FresnelSchlick(max(dot(halfwayVec, viewDir), 0.0), F0);

		vec3 num = NDF * G * F;
		float denom = 4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, L), 0.0) + EPS; // prevent divide by 0
		vec3 specular = num / denom;

		vec3 kD = vec3(1.0) - F; // kS = F = reflection, kD = refraction (diffuse)
		kD *= 1.0 - metallic; // metallic doesn't refract, therefore more metallic = less kD

		L0 += (kD * albedo / PI + specular) * radiance * max(dot(normal, L), 0.0);
	}

	vec3 ambient = vec3(0.03) * albedo * ao;

	vec3 color = ambient + L0;
	color /= color + vec3(1.0); // HDR tonemapping
	vec3 finalColor = pow(color, vec3(1.0/2.2)); // Gamma correction

	// fog
//	if (Fog) {
//		float depth = LinearizeZ(gl_FragCoord.z) / Far;
//		float fogStrength = min(depth, 1.0);
//
//		finalColor = mix(finalColor, FogColor, fogStrength);
//	}

	FragColor = vec4(finalColor, 1.0);
	
}