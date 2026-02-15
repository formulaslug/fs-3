/**
 * NOTE: All profiles should only accept inputs in range [0.0, 1.0] and return
 *       outputs from [-1.0, 1.0]
 */

#ifndef REGEN_PROFILES_H_
#define REGEN_PROFILES_H_

/**
 * Variable profile that can be adjusted to be more or less linear based on
 * `linearity` parameter, and more throttle or brake heavy based on ratio parameter.
 *
 * NOTE: technically does work with any value for linearity, recommended below 2.3
 *
 * @param accel_input       normalized throttle travel ([0.0, 1.0])
 * @param brake_input       normalized brake travel ([0.0, 1.0])
 * @param linearity         lower val -> less linear (in range [0.0, 1.0])
 * @param speed             car speed in kph
 *
 * @return scalar (in range [0.0, 1.0] for torque demand
 */
float variable_profile(float accel_input, float brake_input, float linearity, float speed);

/**
 * Template profile.
 * Takes in the driver input and aligns it to a curve.
 * 
 * @param accel_input       normalized throttle travel ([0.0, 1.0])
 * @param brake_input       normalized brake travel ([0.0, 1.0])
 * @param speed             car speed in kph
 *
 * @return scalar (in range [0.0, 1.0] for torque demand
 */
float template_profile(float accel_input, float brake_input, float speed);

#endif //   REGEN_PROFILES_H_
