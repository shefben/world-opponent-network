/* Straight out of QW and the SDK.
// Good thing about math is it's tried and true for several thousand years.
*/

#include "endianin.h"
#include "world.h"
#include "ModCalls.h"

#define DotProduct(x,y) (x[0]*y[0]+x[1]*y[1]+x[2]*y[2])
#define VectorSubtract(a,b,c) {(c)[0]=(a)[0]-(b)[0];(c)[1]=(a)[1]-(b)[1];(c)[2]=(a)[2]-(b)[2];}
#define VectorAdd(a,b,c)  {(c)[0]=(a)[0]+(b)[0];(c)[1]=(a)[1]+(b)[1];(c)[2]=(a)[2]+(b)[2];}
#define VectorCopy(a,b)   {(b)[0]=(a)[0];(b)[1]=(a)[1];(b)[2]=(a)[2];}
#define VectorClear(a)    ((a)[0]=(a)[1]=(a)[2]=0)
#define VectorNegate(a,b) ((b)[0]=-(a)[0],(b)[1]=-(a)[1],(b)[2]=-(a)[2])
#define VectorLength(a)   Length(a)

#define EQUAL_EPSILON 0.001 //Note to self--look into this fudge factor.

#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846 //I so want to play with this...
#endif

//For some annoying reason, using 'const' on vec_t arrays makes incompatible pointer errors.  Bug in the compiler?  Weird thing I missed?  It's not really a big deal, just...
float anglemod(float a);
HLDS_DLLEXPORT void AngleVectors(const vec3_t angles, vec3_t forward, vec3_t right, vec3_t up);
void AngleVectorsTranspose(const vec3_t angles, vec3_t forward, vec3_t right, vec3_t up);
void AngleMatrix(const vec3_t angles, vec_t matrix[3][4]);
void AngleIMatrix(const vec3_t angles, vec_t matrix[3][4]);
void NormalizeAngles(vec3_t angles);

double Length(vec3_t v);
int VectorCompare(vec3_t v1, vec3_t v2);
void VectorMA(vec3_t va, float scale, vec3_t vb, vec3_t vc);
void CrossProduct(vec3_t v1, vec3_t v2, vec3_t cross);
void VectorScale(vec3_t v, vec_t scale, vec3_t out);
vec_t VectorNormalize(vec3_t v);
void VectorInverse(vec3_t v);
void VectorTransform(const vec3_t in1, vec_t in2[3][4], vec3_t out);

void VectorMatrix(vec3_t forward, vec3_t right, vec3_t up);
void VectorAngles(const vec3_t forward, vec3_t angles);


vec_t _DotProduct(vec3_t v1, vec3_t v2);
void _VectorSubtract(vec3_t va, vec3_t vb, vec3_t out);
void _VectorAdd(vec3_t va, vec3_t vb, vec3_t out);
void _VectorCopy(vec3_t in, vec3_t out);

void R_ConcatRotations(vec_t in1[3][3], vec_t in2[3][3], vec_t out[3][3]);
void R_ConcatTransforms(vec_t in1[3][4], vec_t in2[3][4], vec_t out[3][4]);

void AngleQuaternion(const vec3_t angles, vec4_t quaternion);
void QuaternionMatrix(const vec4_t quaternion, vec_t matrix[3][4]);
void QuaternionSlerp(const vec4_t p, vec4_t q, vec_t t, vec4_t qt);

// If you can read this, you're a better man than I.
// Orm more patient.  I went the easy route and stole from QW.

int BoxOnPlaneSide(vec3_t emins, vec3_t emaxs, mplane_t *p);

#define BOX_ON_PLANE_SIDE(emins, emaxs, p)   \
   (((p)->type < 3)?                  \
   (                              \
      ((p)->dist <= (emins)[(p)->type])?   \
         1                        \
      :                           \
      (                           \
         ((p)->dist >= (emaxs)[(p)->type])?\
            2                     \
         :                        \
            3                     \
      )                           \
   )                              \
   :                              \
      BoxOnPlaneSide( (emins), (emaxs), (p)))
