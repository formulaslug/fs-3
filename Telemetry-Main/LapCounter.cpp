#include <cmath>

#include "LapCounter.hpp"

#define EARTH_RADIUS 6378137    // earths radius in meters, used for conversion to local coordinate plane
#define X_TOLERANCE 8.0f        // check lap completion if x is within +- X_TOLERANCE meters of home x coord
#define Y_TOLERANCE 8.0f        // check lap completion if y is within +- Y_TOLERANCE meters of home y coord
#define HEADING_TOLERANCE 30.0f // check lap completion if heading is within +- HEADING_TOLERANCE degrees of home
#define MIN_LAP_TIME 10         // minimum lap time in seconds. used to ensure same lap isn't counted twice

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
    //  Weighted avg for prev to average out noise 
    data.lap_prev_latitude_f = 0.25f * data.lap_prev_latitude_f + 0.75f * data.lap_curr_latitude_f;
    data.lap_prev_longitude_f = 0.25f * data.lap_prev_longitude_f + 0.75f * data.lap_curr_longitude_f;
    data.lap_prev_x_f = 0.25f * data.lap_prev_x_f + 0.75f * data.lap_curr_x_f;
    data.lap_prev_y_f = 0.25f * data.lap_prev_y_f + 0.75f * data.lap_curr_y_f;
    data.lap_prev_heading_f = 0.25f * data.lap_prev_heading_f + 0.75f * data.lap_curr_heading_f;

    //  Get curr data, transform to local coordinate plane
    data.lap_curr_latitude_f = state.vdmGpsLatLong.LATITUDE;
    data.lap_curr_longitude_f = state.vdmGpsLatLong.LONGITUDE;
    data.lap_curr_x_f = EARTH_RADIUS * (data.lap_curr_longitude_f -
            data.lap_home_longitude_f) * cos(M_PI * data.lap_home_latitude_f /
                180);
    data.lap_curr_y_f = EARTH_RADIUS * (data.lap_curr_latitude_f -
            data.lap_home_latitude_f);
    //  TRANSFORM: Apply rotation matrix, home heading is x axis.
    double home_heading_rad_f = M_PI * data.lap_home_heading_f / 180;           // convert to rad for trig funcs
    double temp_x_f = data.lap_curr_x_f * cos(home_heading_rad_f) + data.lap_curr_y_f * sin(home_heading_rad_f);
    double temp_y_f = data.lap_curr_x_f * -sin(home_heading_rad_f) + data.lap_curr_y_f * cos(home_heading_rad_f);
    data.lap_curr_x_f = temp_x_f;
    data.lap_curr_y_f = temp_y_f;

    data.lap_curr_heading_f = (float) (state.vdmGpsData.TRUE_COURSE % 36000) * 0.01f; //scale to 0-359.99

    //  If nowhere near home coords/heading, return immediately
    if (abs(data.lap_curr_x_f - data.lap_home_x_f) > X_TOLERANCE) return;
    if (abs(data.lap_curr_y_f - data.lap_home_y_f) > Y_TOLERANCE) return;
    if (abs(data.lap_curr_heading_f - data.lap_home_heading_f) > HEADING_TOLERANCE) return;
    if (timer.elapsed_time().count() < MIN_LAP_TIME) return;

    //  If close enough and crossed home line, increment counter
    if (data.lap_curr_x_f > data.lap_home_x_f && data.lap_prev_x_f < data.lap_home_x_f) {
        data.lap_counter++;
        data.lap_time = timer.elapsed_time();

        timer.reset();
        timer.start();
        return;
    }
}

std::chrono::microseconds LapCounter::getTime() {
    return timer.elapsed_time();
}
