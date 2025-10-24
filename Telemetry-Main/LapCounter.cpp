#include <cmath>

#include "LapCounter.hpp"

#define DEG_TO_RAD ((float) M_PI / 180.0f)

#define EARTH_RADIUS 6378137    // earths radius in meters, used for conversion to local coordinate plane
#define X_TOLERANCE 8.0f        // check lap completion if x is within +- X_TOLERANCE meters of home x coord
#define Y_TOLERANCE 8.0f        // check lap completion if y is within +- Y_TOLERANCE meters of home y coord
#define HEADING_TOLERANCE 80.0f // check lap completion if heading is within +- HEADING_TOLERANCE degrees of home
#define MIN_LAP_TIME 10         // minimum lap time in seconds. used to ensure same lap isn't counted twice

using namespace std;

LapCounter::LapCounter(VehicleState state) {
    resetLapCounter(state);
}

void LapCounter::resetLapCounter(VehicleState state) {
    data.lap_home_latitude_f = state.vdmGpsLatLong.LATITUDE;
    data.lap_home_longitude_f = state.vdmGpsLatLong.LONGITUDE;
    data.lap_home_x_f = 0.0f;
    data.lap_home_y_f = 0.0f;
    data.lap_home_heading_f = (float) (state.vdmGpsData.TRUE_COURSE % 36000) * 0.01f; //scale to 0-359.99

    data.lap_prev_latitude_f = 0.0f;
    data.lap_prev_longitude_f = 0.0f;
    data.lap_prev_x_f = 0.0f;
    data.lap_prev_y_f = 0.0f;
    data.lap_prev_heading_f = 0.0f;

    data.lap_curr_latitude_f = data.lap_home_latitude_f;
    data.lap_curr_longitude_f = data.lap_home_longitude_f;
    data.lap_curr_x_f = data.lap_home_x_f;
    data.lap_curr_y_f = data.lap_home_y_f;
    data.lap_curr_heading_f = data.lap_home_heading_f;

    data.lap_counter = 0;

    timer.reset();
    timer.start();
}

void LapCounter::updateLapCounter(VehicleState state) {
    /**
     * UPDATE POSITION
     */
    //  Get prev data, weighted avg to average out noise 
    data.lap_prev_latitude_f = 0.1f * data.lap_prev_latitude_f + 0.9f * data.lap_curr_latitude_f;
    data.lap_prev_longitude_f = 0.1f * data.lap_prev_longitude_f + 0.9f * data.lap_curr_longitude_f;
    data.lap_prev_x_f = 0.1f * data.lap_prev_x_f + 0.9f * data.lap_curr_x_f;
    data.lap_prev_y_f = 0.1f * data.lap_prev_y_f + 0.9f * data.lap_curr_y_f;
    data.lap_prev_heading_f = 0.1f * data.lap_prev_heading_f + 0.9f * data.lap_curr_heading_f;

    //  Get curr data, transform to local coordinate plane
    data.lap_curr_latitude_f = state.vdmGpsLatLong.LATITUDE;
    data.lap_curr_longitude_f = state.vdmGpsLatLong.LONGITUDE;
    data.lap_curr_x_f = EARTH_RADIUS * DEG_TO_RAD * (data.lap_curr_longitude_f - data.lap_home_longitude_f) * cosf(DEG_TO_RAD * data.lap_home_latitude_f);  // lat, long is degrees -> translate to rad
    data.lap_curr_y_f = EARTH_RADIUS * DEG_TO_RAD * (data.lap_curr_latitude_f - data.lap_home_latitude_f);
    data.lap_curr_heading_f = (float) (state.vdmGpsData.TRUE_COURSE % 36000) * 0.01f; //scale to 0-359.99

    //  TRANSFORM: Apply rotation matrix, home heading is x axis.
    float home_heading_rad_f = DEG_TO_RAD * data.lap_home_heading_f;           // convert to rad for trig funcs
    float temp_x_f = data.lap_curr_x_f * cosf(home_heading_rad_f) + data.lap_curr_y_f * sinf(home_heading_rad_f);
    float temp_y_f = data.lap_curr_x_f * -sinf(home_heading_rad_f) + data.lap_curr_y_f * cosf(home_heading_rad_f);
    data.lap_curr_x_f = temp_x_f;
    data.lap_curr_y_f = temp_y_f;

    /**
     * CHECK IF LAP COMPLETED
     */
    //  If nowhere near home coords/heading, return immediately
    if (abs(data.lap_curr_x_f - data.lap_home_x_f) > X_TOLERANCE) return;
    if (abs(data.lap_curr_y_f - data.lap_home_y_f) > Y_TOLERANCE) return;
    if (abs(data.lap_curr_heading_f - data.lap_home_heading_f) > HEADING_TOLERANCE) return;
    if (timer.elapsed_time().count() < MIN_LAP_TIME) {
        timer.reset();
        timer.start();

        return;
    }

    //  Lap has been completed, all conditions pass
    data.lap_counter++;
    data.lap_time = std::chrono::duration<float>{timer.elapsed_time()}.count();

    timer.reset();
    timer.start();
    return;
}

float LapCounter::getTime() {
    return std::chrono::duration<float>{timer.elapsed_time()}.count();
}
