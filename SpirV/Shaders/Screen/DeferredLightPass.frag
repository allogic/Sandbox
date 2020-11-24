#version 460 core

struct LightPoint
{
  vec3 position;
  float radius;
  vec4 color;
  uint enabled;
  float attenuationLinear;
  float attenuationQuadratic;
};

layout (binding = 0) uniform sampler2D uPosition;
layout (binding = 1) uniform sampler2D uAlbedo;
layout (binding = 2) uniform sampler2D uNormal;
layout (binding = 3) uniform sampler2D uUv;
layout (binding = 4) uniform sampler2D uNoise;

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
  vec3 uCameraRight;
  vec3 uCameraUp;
  vec3 uCameraFront;
  vec3 uCameraLocalRight;
  vec3 uCameraLocalUp;
  vec3 uCameraLocalFront;
};
layout (binding = 3) uniform PointLightsUniform
{
  LightPoint uPointLights[1024];
};

layout (location = 0) in VertOut
{
  vec2 uv;
} fragIn;

layout (location = 0) out vec4 oColor;

#define IQCOLOUR
#define IQLIGHT
#define DITHERING

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

float rand(vec2 co)
{// implementation found at: lumina.sourceforge.net/Tutorials/Noise.html
	return fract(sin(dot(co*0.123,vec2(12.9898,78.233))) * 43758.5453);
}

// otaviogood's noise from https://www.shadertoy.com/view/ld2SzK
//--------------------------------------------------------------
// This spiral noise works by successively adding and rotating sin waves while increasing frequency.
// It should work the same on all computers since it's not based on a hash function like some other noises.
// It can be much faster than other noise functions if you're ok with some repetition.
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
float SpiralNoiseCC(vec3 p)
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
        iter *= 1.333733;
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

float Clouds(vec3 p)
{
	float final = p.y;
    
  float d0 = SpiralNoiseC(p.zyx*0.2 - 714.);
  float d1 = SpiralNoiseC(vec3(p.z, 0, p.x) * 0.04 + vec3(0, p.y, 0) * 0.0002);
  float d2 = SpiralNoiseC(vec3(p.x, 0, p.z) * 0.078 + vec3(0, p.y, 0) * 0.0002 + 323.);
  float d3 = SpiralNoiseC(vec3(p.x, 0, p.z) * 0.028 + vec3(0, p.y, 0) * 0.0002 - 788.);
    
  final -= d0 * 8.f;
  final += d1 * 6.f;
  //final += d2 * 40.0;
  
  //final -= mix(d3, d1, SpiralNoise3D(p * 0.2)) * 10.0;
  //final -= SpiralNoise3D(p.yzx * 0.02) * 30.;
    
  return final;
}
mat4 Rotate3D(in vec3 axis, in float angle)
{
  axis = normalize(axis);
  float s = sin(angle);
  float c = cos(angle);
  float oc = 1.0f - c;

  return mat4(
    oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.f,
    oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.f,
    oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.f,
    0.f,                                0.f,                                0.f,                                1.f
  );
}
float map(vec3 p) 
{
   //R(p.yz, -25.53);//-0.4+iMouse.y*0.003); //change this string to be able to find "cloudy caves" :)
   //R(p.xz, iMouse.x*0.008*pi+iTime*0.1);

   //mat4 rot = mat4(1.f);
   //rot = Rotate3D(uCameraLocalRight, uCameraRotation.x);
   //rot = Rotate3D(uCameraLocalUp, uCameraRotation.y);
   //rot = Rotate3D(uCameraLocalFront, uCameraRotation.z);

   //p = vec4(uTransformCamera * vec4(p, 1.f)).xyz;
   //p = vec4(uProjection * vec4(p, 1.f)).xyz;

   //R(p.xz, uCameraRotation.x);
   //p -= uCameraPosition;

   //p.xy /= p.z;

   vec3 samplePoint = vec4(vec4(p, 1.f)).xyz;

   return Clouds(samplePoint) + fpn(samplePoint*50.+uTime*5.);
}

vec4 RayMarchCloud(vec3 fragPosition, vec3 fragAlbedo)
{  
 // ro: ray origin
 // rd: direction of the ray
 //vec3 rd = vec3((fragIn.uv - 0.5f) * 2.f, -1.f);
 vec3 rd = normalize(vec3((gl_FragCoord.xy-0.5*uScreenSize)/uScreenSize.y, 1.f));

 mat4 rotX = Rotate3D(uCameraRight, radians(uCameraRotation.x));
 mat4 rotY = Rotate3D(uCameraUp, radians(uCameraRotation.y));
 mat4 rotZ = Rotate3D(uCameraFront, radians(180.f));

 rd = normalize(vec4(rotZ * vec4(rd, 1.f)).xyz);

 vec3 ro = uCameraPosition;
 //vec3 rd = vec4(uProjection * vec4(fragIn.uv, 1.f, 1.f)).xyz;

 //rd = vec4(uView * vec4(rd, 1.f)).xyz;

 // ld, td: local, total density 
 // w: weighting factor
 float ld=0., td=0., w;

 // t: length of the ray
 // d: distance function
 float d=1., t=0.;
 
 // Distance threshold.
 const float h = .1;
  
 vec3 sundir = normalize( vec3(-1.0,0.75,1.0) );
 // background sky     
	float sun = clamp( dot(sundir,rd), 0.0, 1.0 );
	vec3 col = vec3(0.6,0.71,0.75) - rd.y*0.2*vec3(1.0,0.5,1.0) + 0.15*0.5;
	col += 0.2*vec3(1.0,.6,0.1)*pow( sun, 8.0 );
    // clouds  
    vec3 bgcol = col;
	vec4 sum = vec4(0.0);
   
   #ifdef DITHERING
   vec2 pos1 = ( gl_FragCoord.xy / uScreenSize );
   vec2 seed = pos1 + fract(uTime);
   t=(1.+0.2*rand(seed*vec2(1)));
   #endif 
   
   // rm loop
   for (int i=0; i<64; i++) {
	 
      vec3 pos = ro + t*rd;

      // Loop break conditions.
      if(td>(1.-1./80.) || d<0.0006*t || t>120. || sum.a > 0.99) break;
       
      // evaluate distance function
      d = map(pos) * 0.07;
      
      // fix holes deep inside clouds
      d=max(d,-.4);
       
      // check whether we are close enough
      if (d<0.4) 
      {
        // compute local density and weighting factor 
      	ld = 0.1 - d;
        
        #ifdef IQLIGHT
      	ld *= clamp((ld - map(pos+0.3*sundir))/0.6, 0.0, 1.0 );
        const float kmaxdist = 0.6;
      	#else
        ld *= 0.15;
        const float kmaxdist = 0.6;
        #endif
          
        w = (1. - td) * ld;   
     
      	// accumulate density
      	td += w;// + 1./90.;
              
      	vec3 lin = vec3(0.65,0.68,0.7)*1.3 + 0.5*vec3(0.7, 0.5, 0.3)*ld;
      
      	#ifdef IQCOLOUR
      	vec4 col = vec4( mix( 1.25*vec3(1.0,0.95,0.9), vec3(0.765), d ), max(kmaxdist,d) );
      	#else
      	vec4 col = vec4(vec3(1./exp( d * 0.2 ) * 1.05), max(kmaxdist,d));
	  	#endif
      
      	col.xyz *= lin;
        col.xyz = mix( col.xyz, bgcol, 1.0-exp(-0.0004*t*t) );
      	// front to back blending    
      	col.a *= 0.4;
      	col.rgb *= col.a;
      	sum = sum + col*(1.0-sum.a);
        
      }
      
      td += 1./70.;
       
      // enforce minimum stepsize
      d = max(d, 0.04); 
      
	  #ifdef DITHERING
      // add in noise to reduce banding and create fuzz
      d=abs(d)*(1.+0.28*rand(seed*vec2(i)));
      #endif 
	  
      // step forward
      t += d*.5;
      
   }
 
   	sum = clamp( sum, 0.0, 1.0 );
    col = vec3(fragAlbedo) - rd.y*0.2*vec3(1.0,0.5,1.0) + 0.15*0.5;
    col = col*(1.0-sum.w) + sum.xyz;
    
    // sun glare    
	col += 0.1*vec3(1.0,0.4,0.2)*pow( sun, 3.0 );
   
  return vec4(col, 1.0);
}

void main()
{
  vec3 fragPosition = texture(uPosition, fragIn.uv).rgb;
  vec3 fragAlbedo = texture(uAlbedo, fragIn.uv).rgb;
  vec3 fragNormal = texture(uNormal, fragIn.uv).rgb;
  float fragSpecular = texture(uAlbedo, fragIn.uv).a;

  vec3 lighting = fragAlbedo * 0.1f;

  vec3 viewDir = normalize(uCameraPosition - fragPosition);

  for (uint i = 0; i < 1024; i++)
  {
    if (uPointLights[i].enabled == 0) continue;

    vec3 lightPosDelta = uPointLights[i].position - fragPosition;
    float lightDist = length(lightPosDelta);

    if (lightDist < uPointLights[i].radius)
    {
      // diffuse
      vec3 lightDir = normalize(lightPosDelta);
      vec3 albedo = max(dot(fragNormal, lightDir), 0.f) * fragAlbedo * uPointLights[i].color.rgb;

      // specular
      vec3 halfWayDir = normalize(lightDir + viewDir);
      float specularAmount = pow(max(dot(fragNormal, halfWayDir), 0.f), 16.f);
      vec3 specular = uPointLights[i].color.rgb * specularAmount * fragSpecular;

      // attenuation
      float attenuation = 1.f / (1.f + uPointLights[i].attenuationLinear * lightDist + uPointLights[i].attenuationQuadratic * lightDist * lightDist);
      
      albedo *= attenuation;
      specular *= attenuation;

      lighting += albedo + specular;
    }
  }

  //oColor = vec4(lighting, 1.f);
  oColor = RayMarchCloud(fragPosition, fragAlbedo);
}