#include "common.h"
#include "transform.h"

attribute vec3  Position;
attribute vec3  Normal;

#if defined( USE_MAP ) 
attribute vec2  Texcoord0;
varying vec4 vTexCoord0;
#endif

#if defined( SKINNED )
attribute vec4  BlendWeights;
attribute vec4  BlendIndices;
#endif

#if defined( USE_COLOR )
attribute vec4  Color;
varying   vec4	vColor;
#endif

#if defined( SHADOW_RECEIVER )
varying vec4 vLightSpacePos;
#endif

varying vec3 vNormal;
varying vec3 vViewDir;
varying vec3 vPosWorld;

void main()
{
	GET_SPACE_POS_NORMAL(objPos, objNormal);
	
	gl_Position = g_modelViewProjMatrix * objPos;

#if defined( USE_COLOR )
	vColor = Color;
#endif
	
#if defined( USE_MAP ) 
	vTexCoord0 = vec4(Texcoord0,0.0,0.0);
#endif

	// world space normal
    vNormal    = (g_modelMatrix * vec4(objNormal,0.0)).xyz;
	vPosWorld  = (g_modelMatrix * objPos).xyz;
	vViewDir   = vPosWorld - g_eyePosition;
    
#if defined( SHADOW_RECEIVER )
    vec4 worldPos = g_modelMatrix * objPos;
    vLightSpacePos = g_texViewProjMatrix * worldPos;
    //vLightSpacePos.z = (vLightSpacePos.z - g_depthRange.x) * g_depthRange.w;
#endif
}