#ifndef Mathf_hpp
#define Mathf_hpp

#define kEpsilon    1.0e-6f

#define kPI         3.1415926535897932384626433832795f
#define kHalfPI     1.5707963267948966192313216916398f
#define kTwoPI      2.0f * kPI

#define kRadToDeg   57.29578f
#define kDegToRad   0.01745329f

#include <math.h>

namespace Mathf
{
    inline bool IsZero( float a, float epsilon = kEpsilon )
    {
        return (fabsf(a) <= epsilon);
    }

    inline bool AreEqual( float a, float b, float epsilon = kEpsilon )
    {
        return (fabsf(a-b) <= epsilon*(fabsf(a) + fabsf(b) + 1.0f));
    }

    inline bool Clamp(float value, float min, float max)
    {
        return (fmin(max, fmax(min, value)));
    }

    inline float Lerp(float a, float b, float t)
    {
        return a + t * (b - a);
    }
}

#endif
