#ifndef REGEN_PROFILES_HPP_
#define REGEN_PROFILES_HPP_

/**
 * Template profile
 * Takes in the driver input and aligns it to a curve 
 * 
 * @param driver_input      considers both braking and throttle inputs
 *
 * @return something like the strength of regen braking
 */
float template_profile(float driver_input);

#endif //   REGEN_PROFILES_HPP_
