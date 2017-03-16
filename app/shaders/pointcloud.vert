#version 430

// The variable names set in the AttributeSetup in the geometry. (so don't rename Color to Colour)
layout(location = 0) in vec4 vertexPosition;

out VertexBlock
{
	vec3 pos;
} v_out;


// Projection matrix applies the aspect ratio, near/far plane, etc
uniform mat4 projectionMatrix;

uniform mat4 worldMatrix;
uniform mat4 viewMatrix;

// http://www.thecodecrate.com/opengl-es/opengl-viewport-matrix/
uniform mat4 viewportMatrix;

uniform float pointSize;
uniform float maxDistance;

void main()
{
	v_out.pos    = vec3(worldMatrix * vec4(vertexPosition.xyz, 1.0));


	// TODO send in this matrix pre-multiplicated (mvp I bet) to save operations
	gl_Position = projectionMatrix * viewMatrix * worldMatrix * vertexPosition;

	gl_PointSize = min(viewportMatrix[1][1] * projectionMatrix[1][1] * pointSize, 10);
}

// vim: ts=3 sw=3 noet ffs=unix :
