
#include "common.h"

uniform sampler2D image;

uniform vec3 vLinearBright;

void main()
{
	gl_FragColor = texture2D(image, gl_TexCoord[0].xy);
	
	float fBrightness = max(gl_FragColor.r, max(gl_FragColor.g, gl_FragColor.b)) + dot(gl_FragColor.rgb, vec3(1.0, 1.0, 1.0));
	float fScale = clamp(fBrightness * vLinearBright.x + vLinearBright.y, 0.0, 1.0) * vLinearBright.z;

	gl_FragColor.rgb *= fScale;
}

