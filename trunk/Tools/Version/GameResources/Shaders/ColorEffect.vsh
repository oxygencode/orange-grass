attribute vec4 inVertex;
attribute vec2 inTexCoord;
attribute vec4 inColor;

uniform mat4 MVPMatrix;
uniform mat4 MVMatrix;
uniform /*highp*/ float  FogEnd;
uniform /*highp*/ float  FogRcpEndStartDiff;

varying vec4 VertexColor;
varying vec2 TexCoord;
varying /*lowp*/ vec3 FogIntensity;

void main()
{
	// transform position to view space as we need the distance to the eye for fog
	/*highp*/ vec3 viewPos = vec3(MVMatrix * inVertex);
	/*highp*/ float eyeDist = length(viewPos);

	gl_Position = MVPMatrix * inVertex;
	VertexColor = inColor;
	TexCoord = inTexCoord;
	/*highp*/ float fogIntensity = (FogEnd - eyeDist) * FogRcpEndStartDiff;

	// clamp the intensity within a valid range
	FogIntensity = vec3(clamp(fogIntensity, 0.0, 1.0));
}
