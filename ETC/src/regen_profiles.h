/**
 * NOTE: All profiles should only accept inputs in range [0.0, 1.0] and return
 *       outputs from [-1.0, 1.0]
 */

#ifndef REGEN_PROFILES_H_
#define REGEN_PROFILES_H_

/**
 * Generic profile
 *
 * @param driver_input      normalized throttle travel
 * @param linearity         lower val -> less linear (in range [0.0, 1.0])
 * @param speed             car speed in kph
 */
float generic_profile(float driver_input, float linearity, float speed);

/**
 * Template profile
 * Takes in the driver input and aligns it to a curve 
 * 
 * @param driver_input      considers both braking and throttle inputs
 * @param speed             car speed in kph
 *
 * @return something like the strength of regen braking
 */
float template_profile(float driver_input, float speed);

#endif //   REGEN_PROFILES_H_
