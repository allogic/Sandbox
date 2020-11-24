#version 460 core

layout (binding = 0) uniform GlobalUniform
{
  float uTime;
  float uTimeDelta;
  vec2 uScreenSize;
};
layout (binding = 1) uniform ProjectionUniform
{
  mat4 uProjection;
  mat4 uView;
  mat4 uTransformCamera;
  mat4 uTransformModel;
};
layout (binding = 2) uniform CameraUniform
{
  vec3 uCameraPosition;
  vec3 uCameraRotation;
  vec3 uCameraLocalRight;
  vec3 uCameraLocalUp;
  vec3 uCameraLocalFront;
};

layout (binding = 0) uniform sampler2D uNoise;

layout (location = 0) in VertOut
{
  vec2 uv;
} fragIn;

layout (location = 0) out vec4 oColor;

/* Hi there!
 * Here is a demo presenting volumetric rendering single with shadowing.
 * Did it quickly so I hope I have not made any big mistakes :)
 *
 * I also added the improved scattering integration I propose in my SIGGRAPH'15 presentation
 * about Frostbite new volumetric system I have developed. See slide 28 at http://www.frostbite.com/2015/08/physically-based-unified-volumetric-rendering-in-frostbite/
 * Basically it improves the scattering integration for each step with respect to extinction
 * The difference is mainly visible for some participating media having a very strong scattering value. 
 * I have setup some pre-defined settings for you to checkout below (to present the case it improves):
 * - D_DEMO_SHOW_IMPROVEMENT_xxx: shows improvement (on the right side of the screen). You can still see aliasing due to volumetric shadow and the low amount of sample we take for it.
 * - D_DEMO_SHOW_IMPROVEMENT_xxx_NOVOLUMETRICSHADOW: same as above but without volumetric shadow
 *
 * To increase the volumetric rendering accuracy, I constrain the ray marching steps to a maximum distance.
 *
 * Volumetric shadows are evaluated by raymarching toward the light to evaluate transmittance for each view ray steps (ouch!)
 *
 * Do not hesitate to contact me to discuss about all that :) 
 * SebH
 */



/*
 * This are predefined settings you can quickly use
 *    - D_DEMO_FREE play with parameters as you would like
 *    - D_DEMO_SHOW_IMPROVEMENT_FLAT show improved integration on flat surface
 *    - D_DEMO_SHOW_IMPROVEMENT_NOISE show improved integration on noisy surface
 *    - the two previous without volumetric shadows
 */
#define D_DEMO_FREE
//#define D_DEMO_SHOW_IMPROVEMENT_FLAT
//#define D_DEMO_SHOW_IMPROVEMENT_NOISE
//#define D_DEMO_SHOW_IMPROVEMENT_FLAT_NOVOLUMETRICSHADOW
//#define D_DEMO_SHOW_IMPROVEMENT_NOISE_NOVOLUMETRICSHADOW





#ifdef D_DEMO_FREE
	// Apply noise on top of the height fog?
    #define D_FOG_NOISE 1.0

	// Height fog multiplier to show off improvement with new integration formula
    #define D_STRONG_FOG 0.0

    // Enable/disable volumetric shadow (single scattering shadow)
    #define D_VOLUME_SHADOW_ENABLE 1

	// Use imporved scattering?
	// In this mode it is full screen and can be toggle on/off.
	#define D_USE_IMPROVE_INTEGRATION 1

//
// Pre defined setup to show benefit of the new integration. Use D_DEMO_FREE to play with parameters
//
#elif defined(D_DEMO_SHOW_IMPROVEMENT_FLAT)
    #define D_STRONG_FOG 10.0
    #define D_FOG_NOISE 0.0
	#define D_VOLUME_SHADOW_ENABLE 1
#elif defined(D_DEMO_SHOW_IMPROVEMENT_NOISE)
    #define D_STRONG_FOG 5.0
    #define D_FOG_NOISE 1.0
	#define D_VOLUME_SHADOW_ENABLE 1
#elif defined(D_DEMO_SHOW_IMPROVEMENT_FLAT_NOVOLUMETRICSHADOW)
    #define D_STRONG_FOG 10.0
    #define D_FOG_NOISE 0.0
	#define D_VOLUME_SHADOW_ENABLE 0
#elif defined(D_DEMO_SHOW_IMPROVEMENT_NOISE_NOVOLUMETRICSHADOW)
    #define D_STRONG_FOG 3.0
    #define D_FOG_NOISE 1.0
	#define D_VOLUME_SHADOW_ENABLE 0
#endif



/*
 * Other options you can tweak
 */

// Used to control wether transmittance is updated before or after scattering (when not using improved integration)
// If 0 strongly scattering participating media will not be energy conservative
// If 1 participating media will look too dark especially for strong extinction (as compared to what it should be)
// Toggle only visible zhen not using the improved scattering integration.
#define D_UPDATE_TRANS_FIRST 0

// Apply bump mapping on walls
#define D_DETAILED_WALLS 0

// Use to restrict ray marching length. Needed for volumetric evaluation.
#define D_MAX_STEP_LENGTH_ENABLE 1

// Light position and color
#define LPOS vec3(20.0 * cos(3.14/4.), 25.0, 0.0)
#define LCOL (300.0*vec3( 1.0, 0.9, 0.5))


float displacementSimple( vec2 p )
{
    float f;
    f  = 0.5000* textureLod( uNoise, p, 0.0 ).x; p = p*2.0;
    f += 0.2500* textureLod( uNoise, p, 0.0 ).x; p = p*2.0;
    f += 0.1250* textureLod( uNoise, p, 0.0 ).x; p = p*2.0;
    f += 0.0625* textureLod( uNoise, p, 0.0 ).x; p = p*2.0;
    
    return f;
}

vec3 project(in vec3 p)
{
  return vec4(inverse(uView) * vec4(p - uCameraPosition, 1.f)).xyz;
}

vec3 getSceneColor(vec3 p, float material)
{	
	return vec3(0.0, 0.0, 0.0);
}


float getClosestDistance(vec3 p, out float material)
{
	float d = 0.0;
#if D_MAX_STEP_LENGTH_ENABLE
    float minD = 1.0; // restrict max step for better scattering evaluation
#else
	float minD = 100.0;
#endif
	material = 0.0;

	return minD;
}


vec3 calcNormal( in vec3 pos)
{
    float material = 0.0;
    vec3 eps = vec3(0.3,0.0,0.0);
	return normalize( vec3(
           getClosestDistance(pos+eps.xyy, material) - getClosestDistance(pos-eps.xyy, material),
           getClosestDistance(pos+eps.yxy, material) - getClosestDistance(pos-eps.yxy, material),
           getClosestDistance(pos+eps.yyx, material) - getClosestDistance(pos-eps.yyx, material) ) );

}

vec3 evaluateLight(in vec3 pos)
{
    vec3 lightPos = LPOS;
    vec3 lightCol = LCOL;
    vec3 L = lightPos-pos;
    return lightCol * 1.0/dot(L,L);
}

vec3 evaluateLight(in vec3 pos, in vec3 normal)
{
    vec3 lightPos = LPOS;
    vec3 L = lightPos-pos;
    float distanceToL = length(L);
    vec3 Lnorm = L/distanceToL;
    return max(0.0,dot(normal,Lnorm)) * evaluateLight(pos);
}

const float nudge = 0.739513;	// size of perpendicular vector
float normalizer = 1.0 / sqrt(1.0 + nudge*nudge);	// pythagorean theorem on that perpendicular to maintain scale
float SpiralNoiseC(vec3 p)
{
    float n = 0.0;	// noise amount
    float iter = 1.0;
    for (int i = 0; i < 8; i++)
    {
        // add sin and cos scaled inverse with the frequency
        n += -abs(sin(p.y*iter) + cos(p.x*iter)) / iter;	// abs for a ridged look
        // rotate by adding perpendicular and scaling down
        p.xy += vec2(p.y, -p.x) * nudge;
        p.xy *= normalizer;
        // rotate on other axis
        p.xz += vec2(p.z, -p.x) * nudge;
        p.xz *= normalizer;
        // increase the frequency
        iter *= 1.733733;
    }
    return n;
}
float SpiralNoise3D(vec3 p)
{
    float n = 0.0;
    float iter = 1.0;
    for (int i = 0; i < 5; i++)
    {
        n += (sin(p.y*iter) + cos(p.x*iter)) / iter;
        //p.xy += vec2(p.y, -p.x) * nudge;
        //p.xy *= normalizer;
        p.xz += vec2(p.z, -p.x) * nudge;
        p.xz *= normalizer;
        iter *= 1.33733;
    }
    return n;
}

// iq's noise
float pn( in vec3 x )
{
    vec3 p = floor(x);
    vec3 f = fract(x);
	f = f*f*(3.0-2.0*f);
	vec2 uv = (p.xy+vec2(37.0,17.0)*p.z) + f.xy;
	vec2 rg = textureLod( uNoise, (uv+ 0.5)/256.0, 0.0 ).yx;
	return -1.0+2.4*mix( rg.x, rg.y, f.z );
}

float fpn(vec3 p) 
{
   return pn(p*.06125)*.5 + pn(p*.125)*.25 + pn(p*.25)*.125;
}

// To simplify: wavelength independent scattering and extinction
void getParticipatingMediaOld(out float sigmaS, out float sigmaE, in vec3 pos)
{    
    //const float noiseFactor = 0.07;
    //float noiseFog = -pos.y;
    //noiseFog += SpiralNoiseC(pos.zxy*0.123+100.0)*10.0;	// large scale terrain features
    //noiseFog -= SpiralNoise3D(vec3(pos.x, 0, pos.z) * 0.2 + vec3(0, pos.y, 0) * 0.005) * 8.;	// more large scale features, but 3d, so not just a height map.
    
    const float fogFactor = 0.3;
    float heightFog = -pos.y;
    heightFog += 1.0 + D_FOG_NOISE*3.0*clamp(displacementSimple(pos.xz*0.005 + uTime*0.01),0.0,1.0);
    heightFog = clamp(heightFog * fogFactor, 0.0, 1.0);
    
    const float sphereRadius = 5.0;
    float sphereFog = clamp((sphereRadius-length(pos-vec3(20.0,19.0,-17.0)))/sphereRadius, 0.0,1.0);
    
    const float constantFog = 0.02;

    sigmaS = constantFog + heightFog + sphereFog;
   
    const float sigmaA = 0.0;
    sigmaE = max(0.000000001, sigmaA + sigmaS); // to avoid division by zero extinction
}
void getParticipatingMedia(out float sigmaS, out float sigmaE, in vec3 pos)
{
    const float constantFog = 0.;
    
    const float fogFactor = 0.3;
    float heightFog = -pos.y;
    heightFog += 10.0*clamp(displacementSimple(pos.xz*0.005 + uTime*0.01),0.0,1.0);
    heightFog = clamp(heightFog * fogFactor, 0.0, 5.0);
    
    const float noiseFactor = 0.07;
    float noiseFog = -pos.y;
    noiseFog += SpiralNoiseC(pos.zxy*0.123+100.0)*10.0;
    noiseFog -= SpiralNoise3D(vec3(pos.x, 0, pos.z) * 0.2 + vec3(0, pos.y, 0) * 0.005) * 8.;

    const float sphereRadius = 5.0;
    float sphereFog = clamp((sphereRadius-length(pos-vec3(0.0,20.0,0.0)))/sphereRadius, 0.0,1.0);

    sigmaS = constantFog + heightFog + sphereFog + noiseFog;
   
    const float sigmaA = 0.0;
    sigmaE = max(0.000000001, sigmaA + sigmaS); // to avoid division by zero extinction
}

float phaseFunction()
{
    return 1.0/(4.0*3.14);
}

float volumetricShadow(in vec3 from, in vec3 to)
{
#if D_VOLUME_SHADOW_ENABLE
    const float numStep = 4.0; // quality control. Bump to avoid shadow alisaing
    float shadow = 1.0;
    float sigmaS = 0.0;
    float sigmaE = 0.0;
    float dd = length(to-from) / numStep;
    for(float s=0.5; s<(numStep-0.1); s+=1.0)// start at 0.5 to sample at center of integral part
    {
        vec3 pos = from + (to-from)*(s/(numStep));
        getParticipatingMedia(sigmaS, sigmaE, pos);
        shadow *= exp(-sigmaE * dd);
    }
    return shadow;
#else
    return 1.0;
#endif
}

float rand(vec2 co)
{// implementation found at: lumina.sourceforge.net/Tutorials/Noise.html
	return fract(sin(dot(co*0.123,vec2(12.9898,78.233))) * 43758.5453);
}

void traceScene(bool improvedScattering, vec3 rO, vec3 rD, inout vec3 finalPos, inout vec3 normal, inout vec3 albedo, inout vec4 scatTrans)
{
	const int numIter = 64;
	
    float sigmaS = 0.0;
    float sigmaE = 0.0;
    
    vec3 lightPos = LPOS;
    
    // Initialise volumetric scattering integration (to view)
    float transmittance = 1.0;
    vec3 scatteredLight = vec3(0.0, 0.0, 0.0);
    
	float d = 1.0; // hack: always have a first step of 1 unit to go further
	float material = 0.0;
	vec3 p = vec3(0.0, 0.0, 0.0);
  float dd = 0.0;

  vec2 pos1 = ( gl_FragCoord.xy / uScreenSize.xy );
  vec2 seed = pos1 + fract(uTime);
  d=(1.+0.2*rand(seed*vec2(1)));

	for(int i=0; i<numIter;++i)
	{
		vec3 p = rO + d*rD;
        
    p = project(p);

    	getParticipatingMedia(sigmaS, sigmaE, p);
        
#ifdef D_DEMO_FREE
        if(D_USE_IMPROVE_INTEGRATION>0) // freedom/tweakable version
#else
        if(improvedScattering)
#endif
        {
            // See slide 28 at http://www.frostbite.com/2015/08/physically-based-unified-volumetric-rendering-in-frostbite/
            vec3 S = evaluateLight(p) * sigmaS * phaseFunction()* volumetricShadow(p,lightPos);// incoming light
            vec3 Sint = (S - S * exp(-sigmaE * dd)) / sigmaE; // integrate along the current step segment
            scatteredLight += transmittance * Sint; // accumulate and also take into account the transmittance from previous steps

            // Evaluate transmittance to view independentely
            transmittance *= exp(-sigmaE * dd);
        }
		else
        {
            // Basic scatering/transmittance integration
        #if D_UPDATE_TRANS_FIRST
            transmittance *= exp(-sigmaE * dd);
        #endif
            scatteredLight += sigmaS * evaluateLight(p) * phaseFunction() * volumetricShadow(p,lightPos) * transmittance * dd;
        #if !D_UPDATE_TRANS_FIRST
            transmittance *= exp(-sigmaE * dd);
        #endif
        }
        
		
        dd = getClosestDistance(p, material);
        if(dd<0.2)
            break; // give back a lot of performance without too much visual loss

    dd=abs(dd)*(1.+0.28*rand(seed*vec2(i)));

		d += dd;
	}
	
	albedo = getSceneColor(p, material);
	
    finalPos = rO + d*rD;
    finalPos = project(finalPos);

    normal = calcNormal(finalPos);
    
    scatTrans = vec4(scatteredLight, transmittance);
}


void main()
{
    //iTime
    //iMouse
    //iResolution

	//vec2 uv = fragCoord.xy / iResolution.xy;
    
  //float hfactor = float(iResolution.y) / float(iResolution.x); // make it screen ratio independent
	//vec2 uv2 = vec2(2.0, 2.0*hfactor) * fragCoord.xy / iResolution.xy - vec2(1.0, hfactor);
	
	//vec3 camPos = vec3( 20.0, 18.0,-50.0);
  //   if(iMouse.x+iMouse.y > 0.0) // to handle first loading and see somthing on screen
  //      camPos += vec3(0.05,0.12,0.0)*(vec3(iMouse.x, iMouse.y, 0.0)-vec3(iResolution.xy*0.5, 0.0));
	
	vec3 rO = uCameraPosition;
	vec3 rD = normalize(vec3((gl_FragCoord.xy-0.5*uScreenSize)/uScreenSize.y, -1.f));
	vec3 finalPos = rO;
	vec3 albedo = vec3( 0.0, 0.0, 0.0 );
	vec3 normal = vec3( 0.0, 0.0, 0.0 );
  vec4 scatTrans = vec4( 0.0, 0.0, 0.0, 0.0 );

  traceScene( gl_FragCoord.x>(uScreenSize.x/2.0), rO, rD, finalPos, normal, albedo, scatTrans);
	    
    //lighting
    vec3 color = (albedo/3.14) * evaluateLight(finalPos, normal) * volumetricShadow(finalPos, LPOS);
    // Apply scattering/transmittance
    color = color * scatTrans.w + scatTrans.xyz;
    
    // Gamma correction
	color = pow(color, vec3(1.0/2.2)); // simple linear to gamma, exposure of 1.0
   
#ifndef D_DEMO_FREE
    // Separation line
    if(abs(gl_FragCoord.x-(uScreenSize.x*0.5))<0.6)
        color.r = 0.5;
#endif
    
	oColor = vec4(color ,1.0);
}