//
// Created by Kyle on 2/16/2026.
//

#ifndef OPENGL_RENDERER_ELECTRONS_H
#define OPENGL_RENDERER_ELECTRONS_H


#define E_CUOLOMBS 1.6021766343e-19
#define CUOLOMBS_CONSTANT_NC2_PER_M2 8.987551785972e9
#define ELECTRON_MASS_KG 9.1093837139e-31
#define PROTON_MASS_KG 1.67262192595e-27
#define ELECTRON_RADIUS 1.0e-22 // this is not a true radius, but a functional size indication
#define PROTON_RADIUS_mm 0.84075e-15 * 1.0e3
#define HYDROGEN_ORBIT_RADIUS_mm 5.29e-11 * 1.0e3

// acceleration per distance squared between the two particles.
// these are all surprisingly tame values between 0.01 and 300.0
constexpr float accel_on_e_from_p_per_m2 = CUOLOMBS_CONSTANT_NC2_PER_M2 * -E_CUOLOMBS * E_CUOLOMBS / (ELECTRON_MASS_KG * 1.0e3);
constexpr float accel_on_p_from_e_per_m2 = CUOLOMBS_CONSTANT_NC2_PER_M2 * -E_CUOLOMBS * E_CUOLOMBS / (PROTON_MASS_KG * 1.0e3);
constexpr float accel_on_e_from_e_per_m2 = CUOLOMBS_CONSTANT_NC2_PER_M2 * E_CUOLOMBS * E_CUOLOMBS / (ELECTRON_MASS_KG * 1.0e3);
constexpr float accel_on_p_from_p_per_m2 = CUOLOMBS_CONSTANT_NC2_PER_M2 * E_CUOLOMBS * E_CUOLOMBS / (PROTON_MASS_KG * 1.0e3);


#endif //OPENGL_RENDERER_ELECTRONS_H