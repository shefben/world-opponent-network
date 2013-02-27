#include "VecMath.h"
#include "report.h"

float anglemod(float a) {
   a = (360.0 / 65536.0) * (((int)(a * (65536.0 / 360.0))) & 0xFFFF);
   return a;
}
HLDS_DLLEXPORT void AngleVectors(const vec3_t angles, vec3_t forward, vec3_t right, vec3_t up) {

   //Since the FPU always casts to doubles, we might as well
   //keep our temps as doubles.

   double angle; //a temp var
   double sr, sp, sy, cr, cp, cy;

   angle = angles[1] * (M_PI*2 / 360); //The compiler should convert this to 0.017453292519943295769236907684886
   sy = sin(angle); //Ugh, a sin in the code... Maybe these days it's a lookup table...
   cy = cos(angle);
   angle = angles[0] * (M_PI*2 / 360);
   sp = sin(angle);
   cp = cos(angle);
   angle = angles[2] * (M_PI*2 / 360);
   sr = sin(angle);
   cr = cos(angle);

   //I assume all of this is correct, but I admit I only gave it a cursory checking.

   if(forward != NULL) { //I don't think this ever happens...

      forward[0] = cp*cy;
      forward[1] = cp*sy;
      forward[2] = -sp;
   }
   if(right != NULL) {

      right[0] = (-1*sr*sp*cy+-1*cr*-sy);
      right[1] = (-1*sr*sp*sy+-1*cr*cy);
      right[2] = -1*sr*cp;
   }
   if(up != NULL) {

      up[0] = (cr*sp*cy+-sr*-sy);
      up[1] = (cr*sp*sy+-sr*cy);
      up[2] = cr*cp;
   }
}
void AngleVectorsTranspose(const vec3_t angles, vec3_t forward, vec3_t right, vec3_t up) {

   double angle;
   double sr, sp, sy, cr, cp, cy;

   angle = angles[1] * (M_PI*2 / 360);
   sy = sin(angle);
   cy = cos(angle);
   angle = angles[0] * (M_PI*2 / 360);
   sp = sin(angle);
   cp = cos(angle);
   angle = angles[2] * (M_PI*2 / 360);
   sr = sin(angle);
   cr = cos(angle);

   if(forward != NULL) {

      forward[0] = cp*cy;
      forward[1] = (sr*sp*cy+cr*-sy);
      forward[2] = (cr*sp*cy+-sr*-sy);
   }
   if(right != NULL) {

      right[0] = cp*sy;
      right[1] = (sr*sp*sy+cr*cy);
      right[2] = (cr*sp*sy+-sr*cy);
   }
   if(up != NULL) {

      up[0] = -sp;
      up[1] = sr*cp;
      up[2] = cr*cp;
   }
}
void AngleMatrix(const vec3_t angles, vec_t matrix[3][4]) {

   double angle;
   double sr, sp, sy, cr, cp, cy;

   angle = angles[1] * (M_PI*2 / 360);
   sy = sin(angle);
   cy = cos(angle);
   angle = angles[0] * (M_PI*2 / 360);
   sp = sin(angle);
   cp = cos(angle);
   angle = angles[2] * (M_PI*2 / 360);
   sr = sin(angle);
   cr = cos(angle);

   matrix[0][0] = cp*cy;
   matrix[1][0] = cp*sy;
   matrix[2][0] = -sp;
   matrix[0][1] = sr*sp*cy+cr*-sy;
   matrix[1][1] = sr*sp*sy+cr*cy;
   matrix[2][1] = sr*cp;
   matrix[0][2] = (cr*sp*cy+-sr*-sy);
   matrix[1][2] = (cr*sp*sy+-sr*cy);
   matrix[2][2] = cr*cp;
   matrix[0][3] = 0.0;
   matrix[1][3] = 0.0;
   matrix[2][3] = 0.0;
}
void AngleIMatrix(const vec3_t angles, vec_t matrix[3][4]) {

   double angle;
   double sr, sp, sy, cr, cp, cy;

   angle = angles[1] * (M_PI*2 / 360);
   sy = sin(angle);
   cy = cos(angle);
   angle = angles[0] * (M_PI*2 / 360);
   sp = sin(angle);
   cp = cos(angle);
   angle = angles[2] * (M_PI*2 / 360);
   sr = sin(angle);
   cr = cos(angle);

   matrix[0][0] = cp*cy;
   matrix[0][1] = cp*sy;
   matrix[0][2] = -sp;
   matrix[1][0] = sr*sp*cy+cr*-sy;
   matrix[1][1] = sr*sp*sy+cr*cy;
   matrix[1][2] = sr*cp;
   matrix[2][0] = (cr*sp*cy+-sr*-sy);
   matrix[2][1] = (cr*sp*sy+-sr*cy);
   matrix[2][2] = cr*cp;
   matrix[0][3] = 0.0;
   matrix[1][3] = 0.0;
   matrix[2][3] = 0.0;
}
void NormalizeAngles(vec3_t angles) {

   int i;
   for(i = 0; i < 3; i++) {

      if(angles[i] > 180.0) { angles[i] -= 360.0; }
      else if(angles[i] < -180.0) { angles[i] += 360.0; }
   }
}

double Length(vec3_t v) {

   int i;
   double length;

   length = 0;
   for(i = 0; i < 3; i++) { length += v[i]*v[i]; }
   length = sqrt(length);

   return(length);
}
int VectorCompare(vec3_t v1, vec3_t v2) {

   int i;

   for(i = 0; i < 3; i++) {
//      if(fabs(v1[i]-v2[i]) > EQUAL_EPSILON) { return(0); } //This is not how the asm looks...
      if(v1[i] != v2[i]) { return(0); }
   }

   return(1);
}

void VectorMA(vec3_t va, float scale, vec3_t vb, vec3_t vc) {

   vc[0] = va[0] + scale*vb[0];
   vc[1] = va[1] + scale*vb[1];
   vc[2] = va[2] + scale*vb[2];
}
void CrossProduct(vec3_t v1, vec3_t v2, vec3_t cross) {

   cross[0] = v1[1]*v2[2] - v1[2]*v2[1];
   cross[1] = v1[2]*v2[0] - v1[0]*v2[2];
   cross[2] = v1[0]*v2[1] - v1[1]*v2[0];
}
void VectorScale(vec3_t v, vec_t scale, vec3_t out) {

   out[0] = v[0] * scale;
   out[1] = v[1] * scale;
   out[2] = v[2] * scale;
}
vec_t VectorNormalize(vec3_t v) { //Much different in the SDK

   double length, ilength;

   length = v[0]*v[0] + v[1]*v[1] + v[2]*v[2];
   length = sqrt(length);

   if(length != 0) {

      ilength = 1/length;
      v[0] *= ilength;
      v[1] *= ilength;
      v[2] *= ilength;
   }

   return(length);
}

void VectorInverse(vec3_t v) {

   v[0] = -v[0];
   v[1] = -v[1];
   v[2] = -v[2];
}
void VectorTransform(const vec3_t in1, vec_t in2[3][4], vec3_t out) {

   out[0] = DotProduct(in1, in2[0]) + in2[0][3];
   out[1] = DotProduct(in1, in2[1]) + in2[1][3];
   out[2] = DotProduct(in1, in2[2]) + in2[2][3];
}
//SDK
void VectorAngles(const vec3_t forward, vec3_t angles) {

   float   tmp, yaw, pitch;

   if(forward[1] == 0 && forward[0] == 0) {

      yaw = 0;
      if(forward[2] > 0) { pitch = 90; }
      else { pitch = 270; }
   }
   else {

      yaw = (atan2(forward[1], forward[0]) * 180 / M_PI);
      if(yaw < 0) { yaw += 360; }

      tmp = sqrt(forward[0]*forward[0] + forward[1]*forward[1]);
      pitch = (atan2(forward[2], tmp) * 180 / M_PI);
      if(pitch < 0) { pitch += 360; }
   }
   angles[0] = pitch;
   angles[1] = yaw;
   angles[2] = 0;
}
void VectorMatrix(vec3_t forward, vec3_t right, vec3_t up) {

   vec3_t tmp;

   if(forward[0] == 0 && forward[1] == 0) {

      right[0] = 1;
      right[1] = 0;
      right[2] = 0;
      up[0] = -forward[2];
      up[1] = 0;
      up[2] = 0;
      return;
   }

   tmp[0] = 0; tmp[1] = 0; tmp[2] = 1.0;
   CrossProduct(forward, tmp, right);
   VectorNormalize(right);
   CrossProduct(right, forward, up);
   VectorNormalize(up);
}

//These look to be all inline (or defines) in the SW, but for completeness' sake...
vec_t _DotProduct(vec3_t v1, vec3_t v2) {

   return(v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2]);
}
void _VectorSubtract(vec3_t va, vec3_t vb, vec3_t out) {

   out[0] = va[0]-vb[0];
   out[1] = va[1]-vb[1];
   out[2] = va[2]-vb[2];
}
void _VectorAdd(vec3_t va, vec3_t vb, vec3_t out) {

   out[0] = va[0]+vb[0];
   out[1] = va[1]+vb[1];
   out[2] = va[2]+vb[2];
}
void _VectorCopy(vec3_t in, vec3_t out) {
   out[0] = in[0];
   out[1] = in[1];
   out[2] = in[2];
}

//I'm not gonna verify this unless it doesn't work...
void R_ConcatRotations(vec_t in1[3][3], vec_t in2[3][3], vec_t out[3][3]) {

   out[0][0] = in1[0][0] * in2[0][0] + in1[0][1] * in2[1][0] + in1[0][2] * in2[2][0];
   out[0][1] = in1[0][0] * in2[0][1] + in1[0][1] * in2[1][1] + in1[0][2] * in2[2][1];
   out[0][2] = in1[0][0] * in2[0][2] + in1[0][1] * in2[1][2] + in1[0][2] * in2[2][2];
   out[1][0] = in1[1][0] * in2[0][0] + in1[1][1] * in2[1][0] + in1[1][2] * in2[2][0];
   out[1][1] = in1[1][0] * in2[0][1] + in1[1][1] * in2[1][1] + in1[1][2] * in2[2][1];
   out[1][2] = in1[1][0] * in2[0][2] + in1[1][1] * in2[1][2] + in1[1][2] * in2[2][2];
   out[2][0] = in1[2][0] * in2[0][0] + in1[2][1] * in2[1][0] + in1[2][2] * in2[2][0];
   out[2][1] = in1[2][0] * in2[0][1] + in1[2][1] * in2[1][1] + in1[2][2] * in2[2][1];
   out[2][2] = in1[2][0] * in2[0][2] + in1[2][1] * in2[1][2] + in1[2][2] * in2[2][2];
}
void R_ConcatTransforms(vec_t in1[3][4],vec_t in2[3][4], vec_t out[3][4]) {

   out[0][0] = in1[0][0] * in2[0][0] + in1[0][1] * in2[1][0] + in1[0][2] * in2[2][0];
   out[0][1] = in1[0][0] * in2[0][1] + in1[0][1] * in2[1][1] + in1[0][2] * in2[2][1];
   out[0][2] = in1[0][0] * in2[0][2] + in1[0][1] * in2[1][2] + in1[0][2] * in2[2][2];
   out[0][3] = in1[0][0] * in2[0][3] + in1[0][1] * in2[1][3] + in1[0][2] * in2[2][3] + in1[0][3];
   out[1][0] = in1[1][0] * in2[0][0] + in1[1][1] * in2[1][0] + in1[1][2] * in2[2][0];
   out[1][1] = in1[1][0] * in2[0][1] + in1[1][1] * in2[1][1] + in1[1][2] * in2[2][1];
   out[1][2] = in1[1][0] * in2[0][2] + in1[1][1] * in2[1][2] + in1[1][2] * in2[2][2];
   out[1][3] = in1[1][0] * in2[0][3] + in1[1][1] * in2[1][3] + in1[1][2] * in2[2][3] + in1[1][3];
   out[2][0] = in1[2][0] * in2[0][0] + in1[2][1] * in2[1][0] + in1[2][2] * in2[2][0];
   out[2][1] = in1[2][0] * in2[0][1] + in1[2][1] * in2[1][1] + in1[2][2] * in2[2][1];
   out[2][2] = in1[2][0] * in2[0][2] + in1[2][1] * in2[1][2] + in1[2][2] * in2[2][2];
   out[2][3] = in1[2][0] * in2[0][3] + in1[2][1] * in2[1][3] + in1[2][2] * in2[2][3] + in1[2][3];
}

void AngleQuaternion(const vec3_t angles, vec4_t quaternion) {

   double angle;
   double sr, sp, sy, cr, cp, cy;

   angle = angles[2] * 0.5;
   sy = sin(angle);
   cy = cos(angle);
   angle = angles[1] * 0.5;
   sp = sin(angle);
   cp = cos(angle);
   angle = angles[0] * 0.5;
   sr = sin(angle);
   cr = cos(angle);

   quaternion[0] = sr*cp*cy-cr*sp*sy; // X
   quaternion[1] = cr*sp*cy+sr*cp*sy; // Y
   quaternion[2] = cr*cp*sy-sr*sp*cy; // Z
   quaternion[3] = cr*cp*cy+sr*sp*sy; // W
}
//Not the same function.  Looks like HL optimizes--probably the same results tho.
void QuaternionMatrix(const vec4_t quaternion, vec_t matrix[3][4]) {

   matrix[0][0] = 1.0 - 2.0 * quaternion[1] * quaternion[1] - 2.0 * quaternion[2] * quaternion[2];
   matrix[1][0] = 2.0 * quaternion[0] * quaternion[1] + 2.0 * quaternion[3] * quaternion[2];
   matrix[2][0] = 2.0 * quaternion[0] * quaternion[2] - 2.0 * quaternion[3] * quaternion[1];

   matrix[0][1] = 2.0 * quaternion[0] * quaternion[1] - 2.0 * quaternion[3] * quaternion[2];
   matrix[1][1] = 1.0 - 2.0 * quaternion[0] * quaternion[0] - 2.0 * quaternion[2] * quaternion[2];
   matrix[2][1] = 2.0 * quaternion[1] * quaternion[2] + 2.0 * quaternion[3] * quaternion[0];

   matrix[0][2] = 2.0 * quaternion[0] * quaternion[2] + 2.0 * quaternion[3] * quaternion[1];
   matrix[1][2] = 2.0 * quaternion[1] * quaternion[2] - 2.0 * quaternion[3] * quaternion[0];
   matrix[2][2] = 1.0 - 2.0 * quaternion[0] * quaternion[0] - 2.0 * quaternion[1] * quaternion[1];
}
//Another function I'm just gonna assume is correct...
void QuaternionSlerp(const vec4_t p, vec4_t q, vec_t t, vec4_t qt) {

   int i;
   double omega, cosom, sinom, sclp, sclq;

   // decide if one of the quaternions is backwards
   double a = 0;
   double b = 0;
   for (i = 0; i < 4; i++) {
      a += (p[i]-q[i])*(p[i]-q[i]);
      b += (p[i]+q[i])*(p[i]+q[i]);
   }
   if (a > b) {
      for (i = 0; i < 4; i++) {
         q[i] = -q[i];
      }
   }

   cosom = p[0]*q[0] + p[1]*q[1] + p[2]*q[2] + p[3]*q[3];

   if ((1.0 + cosom) > 0.00000001) {
      if ((1.0 - cosom) > 0.00000001) {
         omega = acos( cosom );
         sinom = sin( omega );
         sclp = sin( (1.0 - t)*omega) / sinom;
         sclq = sin( t*omega ) / sinom;
      }
      else {
         sclp = 1.0 - t;
         sclq = t;
      }
      for (i = 0; i < 4; i++) {
         qt[i] = sclp * p[i] + sclq * q[i];
      }
   }
   else {
      qt[0] = -p[1];
      qt[1] = p[0];
      qt[2] = -p[3];
      qt[3] = p[2];
      sclp = sin( (1.0 - t) * 0.5 * M_PI);
      sclq = sin( t * 0.5 * M_PI);
      for (i = 0; i < 3; i++) {
         qt[i] = sclp * p[i] + sclq * qt[i];
      }
   }
}


int BoxOnPlaneSide(vec3_t emins, vec3_t emaxs, mplane_t *p) {

   double dist1, dist2;
   int sides;


   switch (p->signbits) {

   case 0:
      dist1 = p->normal[0]*emaxs[0] + p->normal[1]*emaxs[1] + p->normal[2]*emaxs[2];
      dist2 = p->normal[0]*emins[0] + p->normal[1]*emins[1] + p->normal[2]*emins[2];
      break;
   case 1:
      dist1 = p->normal[0]*emins[0] + p->normal[1]*emaxs[1] + p->normal[2]*emaxs[2];
      dist2 = p->normal[0]*emaxs[0] + p->normal[1]*emins[1] + p->normal[2]*emins[2];
      break;
   case 2:
      dist1 = p->normal[0]*emaxs[0] + p->normal[1]*emins[1] + p->normal[2]*emaxs[2];
      dist2 = p->normal[0]*emins[0] + p->normal[1]*emaxs[1] + p->normal[2]*emins[2];
      break;
   case 3:
      dist1 = p->normal[0]*emins[0] + p->normal[1]*emins[1] + p->normal[2]*emaxs[2];
      dist2 = p->normal[0]*emaxs[0] + p->normal[1]*emaxs[1] + p->normal[2]*emins[2];
      break;
   case 4:
      dist1 = p->normal[0]*emaxs[0] + p->normal[1]*emaxs[1] + p->normal[2]*emins[2];
      dist2 = p->normal[0]*emins[0] + p->normal[1]*emins[1] + p->normal[2]*emaxs[2];
      break;
   case 5:
      dist1 = p->normal[0]*emins[0] + p->normal[1]*emaxs[1] + p->normal[2]*emins[2];
      dist2 = p->normal[0]*emaxs[0] + p->normal[1]*emins[1] + p->normal[2]*emaxs[2];
      break;
   case 6:
      dist1 = p->normal[0]*emaxs[0] + p->normal[1]*emins[1] + p->normal[2]*emins[2];
      dist2 = p->normal[0]*emins[0] + p->normal[1]*emaxs[1] + p->normal[2]*emaxs[2];
      break;
   case 7:
      dist1 = p->normal[0]*emins[0] + p->normal[1]*emins[1] + p->normal[2]*emins[2];
      dist2 = p->normal[0]*emaxs[0] + p->normal[1]*emaxs[1] + p->normal[2]*emaxs[2];
      break;
   default:
      Sys_Error("%s: Bad sign bits.", __FUNCTION__);
      break;
   }

   sides = 0;
   if(dist1 >= p->dist) { sides = 1; }
   if(dist2 < p->dist) { sides |= 2; }

   return(sides);
}
