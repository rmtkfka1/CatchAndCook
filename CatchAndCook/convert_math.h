#pragma once


#define D2R 0.017453292519943295f
#define R2D 57.29577951308232f

namespace std
{
    inline const float to_radian() { return D2R; }
    inline const float to_degree() { return R2D; }
    inline int sign(const int& x) { return (x > 0) - (x < 0); }
    inline double sign(const double& x) { return (x > 0) - (x < 0); }
    inline float sign(const float& x) { return (x > 0) - (x < 0); }
}
