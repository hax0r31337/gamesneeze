#pragma once
#include "classes/vector.h"
#include "sdk.hpp"
#include <math.h>

inline QAngle originalAngle;
inline float originalForwardMove, originalSideMove;

inline void startMovementFix(CUserCmd* cmd) {
    originalAngle = cmd->viewangles;
    originalForwardMove = cmd->forwardmove;
    originalSideMove = cmd->sidemove;
}

inline void endMovementFix(CUserCmd* cmd) {
    // this was just taken from designer bc im lazy
    // https://github.com/designer1337/csgo-cheat-base/blob/09fa2ba8de52eef482bbc82f682976e369191077/dependencies/math/math.cpp#L4

  //   float deltaViewAngles;
	// float f1;
	// float f2;

	// if (originalAngle.y < 0.f)
	// 	f1 = 360.0f + originalAngle.y;
	// else
	// 	f1 = originalAngle.y;

	// if (cmd->viewangles.y < 0.0f)
	// 	f2 = 360.0f + cmd->viewangles.y;
	// else
	// 	f2 = cmd->viewangles.y;

	// if (f2 < f1)
	// 	deltaViewAngles = abs(f2 - f1);
	// else
	// 	deltaViewAngles = 360.0f - abs(f1 - f2);

	// deltaViewAngles = 360.0f - deltaViewAngles;

	// cmd->forwardmove = cos(DEG2RAD(deltaViewAngles)) * originalForwardMove + cos(DEG2RAD(deltaViewAngles + 90.f)) * originalSideMove;
	// cmd->sidemove = sin(DEG2RAD(deltaViewAngles)) * originalForwardMove + sin(DEG2RAD(deltaViewAngles + 90.f)) * originalSideMove;

  // from osiris
  float oldYaw = originalAngle.y + (originalAngle.y < 0.0f ? 360.0f : 0.0f);
  float newYaw =
      cmd->viewangles.y + (cmd->viewangles.y < 0.0f ? 360.0f : 0.0f);
  float yawDelta = newYaw < oldYaw ? fabsf(newYaw - oldYaw)
                                   : 360.0f - fabsf(newYaw - oldYaw);
  yawDelta = 360.0f - yawDelta;
  
  const float forwardmove = cmd->forwardmove;
  const float sidemove = cmd->sidemove;
  cmd->forwardmove = std::cos(DEG2RAD(yawDelta)) * forwardmove +
                     std::cos(DEG2RAD(yawDelta + 90.0f)) * sidemove;
  cmd->sidemove = std::sin(DEG2RAD(yawDelta)) * forwardmove +
                  std::sin(DEG2RAD(yawDelta + 90.0f)) * sidemove;
}

inline void normalizeAngles(QAngle& angle) {
	while (angle.x > 89.0f)
		angle.x -= 180.f;

	while (angle.x < -89.0f)
		angle.x += 180.f;

	while (angle.y > 180.f)
		angle.y -= 360.f;

	while (angle.y < -180.f)
		angle.y += 360.f;
}

inline QAngle calcAngle(const Vector& src, const Vector& dst) {
	QAngle vAngle;
	Vector delta((src.x - dst.x), (src.y - dst.y), (src.z - dst.z));
	double hyp = sqrt(delta.x*delta.x + delta.y*delta.y);

	vAngle.x = float(atanf(float(delta.z / hyp)) * 57.295779513082f);
	vAngle.y = float(atanf(float(delta.y / delta.x)) * 57.295779513082f);
	vAngle.z = 0.0f;

	if (delta.x >= 0.0)
		vAngle.y += 180.0f;

	return vAngle;
}

inline void vectorAngles(const Vector &forward, const Vector &up, QAngle &angles) {
  Vector left = up * forward;
  left.NormalizeInPlace();

  float forwardDist = forward.Length2D();

  if (forwardDist > 0.001f) {
    angles.x = atan2f(-forward.z, forwardDist) * 180 / M_PI_F;
    angles.y = atan2f(forward.y, forward.x) * 180 / M_PI_F;

    float upZ = (left.y * forward.x) - (left.x * forward.y);
    angles.z = atan2f(left.z, upZ) * 180 / M_PI_F;
  } else {
    angles.x = atan2f(-forward.z, forwardDist) * 180 / M_PI_F;
    angles.y = atan2f(-left.x, left.y) * 180 / M_PI_F;
    angles.z = 0;
  }
}

inline void vectorAngles(const Vector &forward, QAngle &angles) {
  if (forward.y == 0.0f && forward.x == 0.0f) {
    angles.x = (forward.z > 0.0f) ? 270.0f : 90.0f; // Pitch (up/down)
    angles.y = 0.0f;                                 // yaw left/right
  } else {
    angles.x = atan2(-forward[2], forward.Length2D()) * -180 / M_PI;
    angles.y = atan2(forward[1], forward[0]) * 180 / M_PI;

    if (angles.y > 90)
      angles.y -= 180;
    else if (angles.y < 90)
      angles.y += 180;
    else if (angles.y == 90)
      angles.y = 0;
  }

  angles.z = 0.0f;
}

inline void SinCos(float radians, float *sine, float *cosine) {
  double __cosr, __sinr;
  __asm("fsincos" : "=t"(__cosr), "=u"(__sinr) : "0"(radians));

  *sine = __sinr;
  *cosine = __cosr;
}

inline void angleVectors(const QAngle &angles, Vector *forward, Vector *right,
                         Vector *up) {

  float sr, sp, sy, cr, cp, cy;

  SinCos(DEG2RAD(angles[1]), &sy, &cy);
  SinCos(DEG2RAD(angles[0]), &sp, &cp);
  SinCos(DEG2RAD(angles[2]), &sr, &cr);

  if (forward) {
    forward->x = cp * cy;
    forward->y = cp * sy;
    forward->z = -sp;
  }

  if (right) {
    right->x = (-1 * sr * sp * cy + -1 * cr * -sy);
    right->y = (-1 * sr * sp * sy + -1 * cr * cy);
    right->z = -1 * sr * cp;
  }

  if (up) {
    up->x = (cr * sp * cy + -sr * -sy);
    up->y = (cr * sp * sy + -sr * cy);
    up->z = cr * cp;
  }
}

inline void angleVectors(const QAngle &angles, Vector& forward) {
	forward.x = cos(DEG2RAD(angles.x)) * cos(DEG2RAD(angles.y));
	forward.y = cos(DEG2RAD(angles.x)) * sin(DEG2RAD(angles.y));
	forward.z = -sin(DEG2RAD(angles.x));
}

inline float dotProduct(const Vector &v1, const float *v2) {
  return v1.x * v2[0] + v1.y * v2[1] + v1.z * v2[2];
}

inline void vectorTransform(const Vector &in1, const matrix3x4_t &in2,
                            Vector &out) {
  out.x = dotProduct(in1, in2[0]) + in2[0][3];
  out.y = dotProduct(in1, in2[1]) + in2[1][3];
  out.z = dotProduct(in1, in2[2]) + in2[2][3];
}

inline float normalizePitch(float pitch) {
  while (pitch > 89.0f)
    pitch -= 180.0f;

  while (pitch < -89.0f)
    pitch += 180.0f;

  return pitch;
}

inline float normalizeYaw(float yaw) {
  while (yaw < -180.f)
    yaw += 360.f;
  while (yaw > 180.f)
    yaw -= 360.f;
  return yaw;
}

inline float getDistance(Vector pos1, Vector pos2) {
    // Do 3d pythag
    float a = abs(pos1.x-pos2.x);
    float b = abs(pos1.y-pos2.y);
    float c = abs(pos1.z-pos2.z);
    return sqrt(pow(a, 2.f) + pow(b, 2.f) + pow(c, 2.f));
}

inline float getDistanceNoSqrt(Vector pos1, Vector pos2) {
    // When you dont need an exact distance and just want to see if 
	// something is x further than something else for example theres no need to sqrt it
    float a = abs(pos1.x-pos2.x);
    float b = abs(pos1.y-pos2.y);
    float c = abs(pos1.z-pos2.z);
    return pow(a, 2.f) + pow(b, 2.f) + pow(c, 2.f);
}

inline float magnitude(Vector a) { return sqrt((a.x * a.x) + (a.y * a.y)); }

inline Vector normalizeMagnitude(Vector value) {
  float num = magnitude(value);
  if (num != 0.f)
    return value / num;
  return Vector(0.f, 0.f, 0.f);
}

inline Vector clampMagnitude(Vector vector, float maxLength) {
  if (magnitude(vector) > maxLength)
    return Vector(normalizeMagnitude(vector).x * maxLength,
                  normalizeMagnitude(vector).y * maxLength, 0);
  return vector;
}

inline void angleMatrix(const Vector angles, matrix3x4_t &matrix) {
  float sr, sp, sy, cr, cp, cy;

  sy = sin(DEG2RAD(angles[1]));
  cy = cos(DEG2RAD(angles[1]));

  sp = sin(DEG2RAD(angles[0]));
  cp = cos(DEG2RAD(angles[0]));

  sr = sin(DEG2RAD(angles[2]));
  cr = cos(DEG2RAD(angles[2]));

  // matrix = (YAW * PITCH) * ROLL
  matrix[0][0] = cp * cy;
  matrix[1][0] = cp * sy;
  matrix[2][0] = -sp;

  float crcy = cr * cy;
  float crsy = cr * sy;
  float srcy = sr * cy;
  float srsy = sr * sy;

  matrix[0][1] = sp * srcy - crsy;
  matrix[1][1] = sp * srsy + crcy;
  matrix[2][1] = sr * cp;

  matrix[0][2] = (sp * crcy + srsy);
  matrix[1][2] = (sp * crsy - srcy);
  matrix[2][2] = cr * cp;

  matrix[0][3] = 0.0f;
  matrix[1][3] = 0.0f;
  matrix[2][3] = 0.0f;
}

bool worldToScreen(const Vector& origin, Vector& screen);