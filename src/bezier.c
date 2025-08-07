/* 
 * @authors Roth
 * @date 18.07.2025
 */

#include "../inc/common.h"

double reduce_angle(double angle) {
    if (M_PI < angle) {
        return angle - 2.0 * M_PI;
    }
    if (-M_PI > angle) {
        return angle + 2.0 * M_PI;
    }
    return angle;
}
