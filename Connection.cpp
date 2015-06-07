#include "Connection.hpp"

double Connection::toVoltage(int value) {
    return (double) value / valueMax * voltageMax;
}
