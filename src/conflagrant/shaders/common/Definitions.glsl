////////////////////////
// common definitions //
////////////////////////

#define GAMMA 1
#define MAX_POINTLIGHTS 1
#define MAX_DIRECTIONALLIGHTS 1

///////////////////////////
// shadowmap definitions //
///////////////////////////

#define SINGLE_SAMPLE 0
#define PCF 1
    #define PCF_RADIUS 1
#define POISSON 2
#define STRATIFIED_POISSON 3
    #define POISSON_SAMPLES 16
    #define POISSON_VALUE 4000.0
    #define POISSON_CENTER_WEIGHT 0

#define SHADOWMAP_METHOD STRATIFIED_POISSON

#define SHADOWMAP_BIAS 0.006
#define USE_SAMPLER2DSHADOW 1

#if USE_SAMPLER2DSHADOW == 1
    #define SAMPLER2DSHADOW sampler2DShadow
    #define SAMPLE_SHADOWMAP(shadowMap, projCoords, bias) texture(shadowMap, projCoords, bias)
#else // USE_SAMPLER2DSHADOW != 1
    #define SAMPLER2DSHADOW sampler2D
    #define SAMPLE_SHADOWMAP(shadowMap, projCoords, bias) texture(l.shadowMap, projCoords.xy).r
#endif // USE_SAMPLER2DSHADOW

#define RAYMARCH_MAX_STEPS 1024

////////////////////////
// VOXEL CONE TRACING //
////////////////////////

#define VCT_MIPMAP_MAX 6.0

#define VCT_INDIRECT_STRENGTH 1.0
#define VCT_INDIRECT_ORTHOGONALITY 0.5
#define VCT_INDIRECT_OFFSET (-0.01)
#define VCT_INDIRECT_NORMAL_WEIGHT 1.0
#define VCT_INDIRECT_SIDE_WEIGHT 1.0
#define VCT_INDIRECT_START_BIAS 0.2
#define VCT_INDIRECT_SPREAD 0.4
