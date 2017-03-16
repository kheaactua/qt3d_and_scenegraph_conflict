#version 430

// Test stuff in the GLSL sandbox http://mrdoob.com/projects/glsl_sandbox

in VertexBlock
{
	vec3 pos;
} frag_in;

out vec4 colour;

// Colour representing the "bottom" or lower end of the spectrum
uniform vec4 col0;
uniform float zLow;

// Colour representing the "top" or lower end of the spectrum
uniform vec4 col1;
uniform float zHigh;

// zRangeI = 1/(zHigh - zLow), this is used to switch a division to a multiplication
uniform float zRangeI;

void main()
{
	vec4 white = vec4(1, 1, 1, 1);

	// Elevation is placed [zLow, zHigh], no interpolation.
	float elevation = max(min(frag_in.pos.z, zHigh), zLow);

	// Normalize the elevation
	elevation = (elevation - zLow) * zRangeI;


	colour = mix(mix(col0, white, elevation), col1, elevation);
}

// vim: ts=3 sw=3 noet ffs=unix :
