////////////////////////
// common definitions //
////////////////////////

#define MAX_POINTLIGHTS 16
#define MAX_DIRECTIONALLIGHTS 16

///////////////////////////
// shadowmap definitions //
///////////////////////////

#define SINGLE_SAMPLE 0
#define PCF 1
    #define PCF_RADIUS 1
#define POISSON 2
#define STRATIFIED_POISSON 3
    #define POISSON_SAMPLES 16
    #define POISSON_VALUE 700.0
    #define POISSON_CENTER_WEIGHT 0

#define SHADOWMAP_METHOD SINGLE_SAMPLE

#define SHADOWMAP_BIAS 0.005
#define USE_SAMPLER2DSHADOW 1

#if USE_SAMPLER2DSHADOW == 1
    #define SAMPLER2DSHADOW sampler2DShadow
    #define SAMPLE_SHADOWMAP(shadowMap, projCoords, bias) texture(shadowMap, projCoords, bias)
#else // USE_SAMPLER2DSHADOW != 1
    #define SAMPLER2DSHADOW sampler2D
    #define SAMPLE_SHADOWMAP(shadowMap, projCoords, bias) texture(l.shadowMap, projCoords.xy).r
#endif // USE_SAMPLER2DSHADOW

