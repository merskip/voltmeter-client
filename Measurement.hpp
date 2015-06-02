#pragma once

struct Measurement {
    double time;
    struct {
        int value;
        double voltage;
    } channel[5];
};