#ifndef _TEST_UPDATE_BRAKE_SIGNAL_H_
#define _TEST_UPDATE_BRAKE_SIGNAL_H_


#include "test_main.h"
#include "etc_controller.h"
#include "mbed.h"
#include "unity.h"
#include <iostream>


void test_brake_pin_full_voltage_range() {
    DigitalOut brake_signal_pin(PC_14);  // The brake signal pin, so we can monitor while testing

    for (float brake_voltage = 0.0; brake_voltage < etcController->MAX_V; brake_voltage += 0.05) {
        ETCState new_state = {.brakes_read = brake_voltage};
        etcController->updateStateFromCAN(new_state);

        ThisThread::sleep_for(200ms);  // Wait for ticker

        TEST_ASSERT_EQUAL(brake_signal_pin.read(), brake_voltage > etcController->BRAKE_TOL);
    }
}

void test_brake_range_boundary() {
    DigitalOut brake_signal_pin(PC_14);  // The brake signal pin, so we can monitor while testing
    ETCState etc_state;

    etc_state.brakes_read = etcController->BRAKE_TOL - 0.01;
    etcController->updateStateFromCAN(etc_state);
    ThisThread::sleep_for(200ms);  // Wait for ticker
    TEST_ASSERT_EQUAL(brake_signal_pin.read(), false);

    etc_state.brakes_read = etcController->BRAKE_TOL;
    etcController->updateStateFromCAN(etc_state);
    ThisThread::sleep_for(200ms);  // Wait for ticker
    TEST_ASSERT_EQUAL(brake_signal_pin.read(), false);

    etc_state.brakes_read = etcController->BRAKE_TOL + 0.01;
    etcController->updateStateFromCAN(etc_state);
    ThisThread::sleep_for(200ms);  // Wait for ticker
    TEST_ASSERT_EQUAL(brake_signal_pin.read(), true);
}


#endif  // _TEST_UPDATE_BRAKE_SIGNAL_H_
