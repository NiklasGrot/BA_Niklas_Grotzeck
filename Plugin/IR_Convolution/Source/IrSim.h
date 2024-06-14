#ifndef ROOM_IMPULSE_RESPONSE_GENERATOR_H
#define ROOM_IMPULSE_RESPONSE_GENERATOR_H

#include <JuceHeader.h>
#include <iostream>
#include <fstream> // For file handling
#include <chrono>
#include <map>
#include <array>


struct RIR_DATA{
    double* x_dist_lookup;
    double* y_dist_lookup;
    double* z_dist_lookup;

    double* x_coeff_lookup;
    double* y_coeff_lookup;
    double* z_coeff_lookup;
};

class ROOM {
public:
    double room_dim[3];
    double sound_pos[3];
    double rec_pos[3];
    std::string wall_mats[3][2];
        
    void set_dim(double x,double y,double z);
    void set_sound(double x,double y,double z);
    void set_receiver(double x,double y,double z);
    void set_wall_mats(std::string x1, std::string x2, std::string y1, std::string y2, std::string z1, std::string z2);
};

class MATERIALS{
public:
    using coeffs = std::array<double,6>;
    std::map<std::string, coeffs> abs;

    MATERIALS();
    double get_ref_coeff(std::string material,int position);
    double get_abs_coeff(std::string material,int position);
};

class RIRGenerator {
public:
    static const int sample_rate = 44100; //initiale Abtastrate wird beim ersten aufrufen überschrieben
    static const double C;

    static const int DIST_ARR = 0;
    static const int COEFF_ARR = 1;

    static double calc_t60(ROOM room, int freq, int freq_idx);

    static void fill_lookup_tables(ROOM room, double* dist_lookup, double* coeff_lookup, int n, int freq_idx, int axis);

    static double rndm();

    static void create_single_rir(RIR_DATA rir, ROOM room, double t60, double* h, int* n);
    static double mean(const double array[], int size);
    static juce::AudioBuffer<float>create_full_rir(RIR_DATA rir, ROOM room);
    static juce::AudioBuffer<float>generate(double roomW,double roomL, double roomH,double soundX, double soundY,double soundZ,double receiverX,double receiverY,double receiverZ,std::vector<std::string> materials,int sample_rate);
};

#endif // ROOM_IMPULSE_RESPONSE_GENERATOR_H
