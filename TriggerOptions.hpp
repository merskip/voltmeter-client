#pragma once

struct TriggerOptions {
    bool isActive;
    int channel;
    double voltage;

    enum EdgeType {
        AnyEdge,
        FallingEdge,
        RisingEdge
    } edge;
};

const TriggerOptions DEFAULT_TRIGGER_OPTIONS = {
        .isActive = true,
        .channel = 1,
        .voltage = 2.7,
        .edge = TriggerOptions::FallingEdge
};;