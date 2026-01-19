#type vertex
#version 450 core

layout(location = 0) in vec3 a_WorldPosition;
layout(location = 1) in vec2 a_LocalPosition;
layout(location = 2) in vec4 a_Color;
layout(location = 3) in float a_Thickness;
layout(location = 4) in float a_Fade;
layout(location = 5) in int a_EntityID;

layout(std140, binding = 0) uniform Camera {
	mat4 u_ViewProjection;
};

struct VertexOutput {
	vec4 Color;
	vec2 LocalPosition;
	float Thickness;
	float Fade;
};

layout(location = 0) out VertexOutput Output;
layout(location = 4) out flat int v_EntityID;

void main() {
	Output.Color = a_Color;
	Output.LocalPosition = a_LocalPosition;
	Output.Thickness = a_Thickness;
	Output.Fade = a_Fade;
	v_EntityID = a_EntityID;

	gl_Position = u_ViewProjection * vec4(a_WorldPosition, 1.0);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 color;
layout(location = 1) out int entityID;

struct VertexOutput {
	vec4 Color;
	vec2 LocalPosition;
	float Thickness;
	float Fade;
};

layout(location = 0) in VertexOutput Input;
layout(location = 3) in flat int v_EntityID;

void main() {
	vec4 texColor = Input.Color;

	float dist = length(Input.LocalPosition);
	float delta = fwidth(dist);
	float alpha = smoothstep(Input.Thickness / 2.0 + delta, Input.Thickness / 2.0 - delta, dist);
	texColor.a *= alpha;

	color = texColor;
	if(color.a < 0.01)
		discard;

	entityID = v_EntityID;
}