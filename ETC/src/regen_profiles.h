#ifndef REGEN_PROFILES_H_
#define REGEN_PROFILES_H_

#include <cstdint>

/**
 * Template profile
 * Takes in the driver input and aligns it to a curve 
 * 
 * @param driver_input      considers both braking and throttle inputs
 *
 * @return something like the strength of regen braking
 */
int16_t template_profile(int16_t driver_input);

#endif //   REGEN_PROFILES_H_
