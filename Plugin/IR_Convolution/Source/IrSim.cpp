#include "IrSim.h"

// ROOM class implementation
void ROOM::set_dim(double x, double y, double z) {
    room_dim[0] = x;
    room_dim[1] = y;
    room_dim[2] = z;
}

void ROOM::set_sound(double x, double y, double z) {
    sound_pos[0] = x;
    sound_pos[1] = y;
    sound_pos[2] = z;
}

void ROOM::set_receiver(double x, double y, double z) {
    rec_pos[0] = x;
    rec_pos[1] = y;
    rec_pos[2] = z;
}

void ROOM::set_wall_mats(std::string x1, std::string x2, std::string y1, std::string y2, std::string z1, std::string z2) {
    wall_mats[0][0] = x1;
    wall_mats[0][1] = x2;
    wall_mats[1][0] = y1;
    wall_mats[1][1] = y2;
    wall_mats[2][0] = z1;
    wall_mats[2][1] = z2;
}

// MATERIALS class implementation
MATERIALS::MATERIALS() {
    // Quelle: https://sengpielaudio.com/calculator-RT60Coeff.htm
    abs = {
            {"_INIT_", {0.9, 0.9, 0.9, 0.9, 0.9, 0.9}},
            {"floor: concrete", {0.01, 0.01, 0.015, 0.02, 0.02, 0.02}},
            {"floor: linoleum/vinyl tile on concrete", {0.02, 0.03, 0.03, 0.03, 0.03, 0.02}},
            {"floor: wood on joists", {0.15, 0.11, 0.10, 0.07, 0.06, 0.07}},
            {"floor: parquet on concrete", {0.04, 0.04, 0.07, 0.06, 0.06, 0.07}},
            {"floor: carpet on concrete", {0.02, 0.06, 0.14, 0.37, 0.60, 0.65}},
            {"floor: carpet on foam", {0.08, 0.24, 0.57, 0.69, 0.71, 0.73}},
            {"seats: fully occupied - fabric upholstered", {0.60, 0.74, 0.88, 0.96, 0.93, 0.85}},
            {"seats: occupied wooden pews", {0.57, 0.61, 0.75, 0.86, 0.91, 0.86}},
            {"seats: empty - fabric upholstered", {0.49, 0.66, 0.80, 0.88, 0.82, 0.70}},
            {"seats: empty metal/wood seats", {0.15, 0.19, 0.22, 0.39, 0.38, 0.30}},
            {"wall: Brick: unglazed", {0.03, 0.03, 0.03, 0.04, 0.05, 0.07}},
            {"wall: Brick: unglazed & painted", {0.01, 0.01, 0.02, 0.02, 0.02, 0.03}},
            {"wall: Concrete block - coarse", {0.36, 0.44, 0.31, 0.29, 0.39, 0.25}},
            {"wall: Concrete block - painted", {0.10, 0.05, 0.06, 0.07, 0.09, 0.08}},
            {"wall: Curtain: 10 oz/sq yd fabric molleton", {0.03, 0.04, 0.11, 0.17, 0.24, 0.35}},
            {"wall: Curtain: 14 oz/sq yd fabric molleton", {0.07, 0.31, 0.49, 0.75, 0.70, 0.60}},
            {"wall: Curtain: 18 oz/sq yd fabric molleton", {0.14, 0.35, 0.55, 0.72, 0.70, 0.65}},
            {"wall: Fiberglass: 2'' 703 no airspace", {0.22, 0.82, 0.99, 0.99, 0.99, 0.99}},
            {"wall: Fiberglass: spray 5''", {0.05, 0.15, 0.45, 0.70, 0.80, 0.80}},
            {"wall: Fiberglass: spray 1''", {0.16, 0.45, 0.70, 0.90, 0.90, 0.85}},
            {"wall: Fiberglass: 2'' rolls", {0.17, 0.55, 0.80, 0.90, 0.85, 0.80}},
            {"wall: Foam: Sonex 2''", {0.06, 0.25, 0.56, 0.81, 0.90, 0.91}},
            {"wall: Foam: SDG 3''", {0.24, 0.58, 0.67, 0.91, 0.96, 0.99}},
            {"wall: Foam: SDG 4''", {0.33, 0.90, 0.84, 0.99, 0.98, 0.99}},
            {"wall: Foam: polyur. 1''", {0.13, 0.22, 0.68, 1.00, 0.92, 0.97}},
            {"wall: Foam: polyur. 1/2''", {0.09, 0.11, 0.22, 0.60, 0.88, 0.94}},
            {"wall: Glass: 1/4'' plate large", {0.18, 0.06, 0.04, 0.03, 0.02, 0.02}},
            {"wall: Glass: window", {0.35, 0.25, 0.18, 0.12, 0.07, 0.04}},
            {"wall: Plaster: smooth on tile/brick", {0.013, 0.015, 0.02, 0.03, 0.04, 0.05}},
            {"wall: Plaster: rough on lath", {0.02, 0.03, 0.04, 0.05, 0.04, 0.03}},
            {"wall: Marble/Tile", {0.01, 0.01, 0.01, 0.01, 0.02, 0.02}},
            {"wall: Sheetrock 1/2\" 16\" on center", {0.29, 0.10, 0.05, 0.04, 0.07, 0.09}},
            {"wall: Wood: 3/8'' plywood panel", {0.28, 0.22, 0.17, 0.09, 0.10, 0.11}}
        };
}

double MATERIALS::get_ref_coeff(std::string material, int position) {
    // Umrechnung in Reflexionskoeffizient
    double abs_coeff = abs[material][position];
    return  sqrt(1 - abs_coeff);
}
double MATERIALS::get_abs_coeff(std::string material, int position) {
    
    return abs[material][position];
}

// RIRGenerator class implementation
const double RIRGenerator::C = 343.6;

double RIRGenerator::calc_t60(ROOM room, int freq, int freq_idx) {
    double Lx = room.room_dim[0];
    double Ly = room.room_dim[1];
    double Lz = room.room_dim[2];

    double V = Lx * Ly * Lz;
    
    double abs_mat = 0;
    for(int i = 0; i < 3; i++) {
        double surface = 0;
        if (i == 0) {
            surface = Ly * Lz;
        } else if(i == 1) {
            surface = Lx * Lz;
        } else if(i == 2) {
            surface = Lx * Ly;
        }
        
        double coeff1 = MATERIALS().get_abs_coeff(room.wall_mats[i][0], freq_idx);
        double coeff2 = MATERIALS().get_abs_coeff(room.wall_mats[i][1], freq_idx);
        double w1 = coeff1 * surface;
        double w2 = coeff2 * surface;
        
        abs_mat += w1 + w2;
    }

    double humidity = 8.7; // 20°C 50%
    double abs_air = 5.5 * 0.0001 * (50 / humidity) * std::pow((freq / 1000), 1.7);
    return 55.3 * V / (C * (abs_mat + 4 * abs_air * V));
}

void RIRGenerator::fill_lookup_tables(ROOM room, double* dist_lookup, double* coeff_lookup, int n, int freq_idx, int axis) {
    // Hier werden die Lookup-Tabellen befüllt. Quelle: Stephen McGovern, Fast image method for impulse response calculations of box-shaped rooms
    coeff_lookup[0] = 1.0;
    double dir_sound = room.sound_pos[axis] - room.rec_pos[axis];
    dist_lookup[0] = dir_sound * dir_sound;
    
    double l_dist = dir_sound;
    double r_dist = dir_sound;
    double l_coeff = 1;
    double r_coeff = 1;
    std::string mat1 = room.wall_mats[axis][0];
    std::string mat2 = room.wall_mats[axis][1];

    double wall_coeff1 = MATERIALS().get_ref_coeff(mat1, freq_idx);
    double wall_coeff2 = MATERIALS().get_ref_coeff(mat2, freq_idx);
    
    for (int i = 1; i <= n; i++) {
        if(i % 2 != 0) {
            l_dist -= 2 * (room.room_dim[axis] + room.sound_pos[axis]);
            r_dist += 2 * (room.room_dim[axis] - room.sound_pos[axis]);
            l_coeff *= wall_coeff1;
            r_coeff *= wall_coeff2;
        } else {
            l_dist -= 2 * (room.room_dim[axis] - room.sound_pos[axis]);
            r_dist += 2 * (room.room_dim[axis] + room.sound_pos[axis]);
            l_coeff *= wall_coeff2;
            r_coeff *= wall_coeff1;
        }

        if ((i % 2 != 0 && (room.rec_pos[axis] + room.sound_pos[axis]) < 0) || (i % 2 == 0 && (room.rec_pos[axis] - room.sound_pos[axis]) < 0)) {
            dist_lookup[2 * i - 1] = l_dist * l_dist;
            dist_lookup[2 * i] = r_dist * r_dist;
            coeff_lookup[2 * i - 1] = l_coeff;
            coeff_lookup[2 * i] = r_coeff;
        } else {
            dist_lookup[2 * i - 1] = r_dist * r_dist;
            dist_lookup[2 * i] = l_dist * l_dist;
            coeff_lookup[2 * i - 1] = r_coeff;
            coeff_lookup[2 * i] = l_coeff;
        }
    }
}

double RIRGenerator::rndm() {
    // Quelle: https://thompsonsed.co.uk/random-number-generators-for-c-performance-tested
    static uint64_t MAX_VALUE = (18446744073709551615 >> 32);
    static uint64_t x = 10; /* The state can be seeded with any value. */
    uint64_t z = (x += UINT64_C(0x9E3779B97F4A7C15));
    z = (z ^ (z >> 30)) * UINT64_C(0xBF58476D1CE4E5B9);
    z = (z ^ (z >> 27)) * UINT64_C(0x94D049BB133111EB);
    double scaledValue = (static_cast<double>(z >> 32) / MAX_VALUE * 2.0) - 1 ; //from -1 to 1
    return scaledValue;
}

void RIRGenerator::create_single_rir(RIR_DATA rir, ROOM room, double t60, double* h, int* n) {
    // Erzeugung einer einzelnen Impulsantwort mit Auslöschung der Sweeping Echoes
    int length = (int)((double)sample_rate * t60);
    double dist, t;
    int samp, i, j, k;
    double random_factor = 0.08;
    for(i = 0; i < 2 * n[0] + 1; i++) {
        for(j = 0; j < 2 * n[1] + 1; j++) {
            for(k = 0; k < 2 * n[2] + 1; k++) {
                dist = sqrt(rir.x_dist_lookup[i] + rir.y_dist_lookup[j] + rir.z_dist_lookup[k]);
                t = dist / C;
                t += rndm() * random_factor / C;
                samp = (int)(t * (double)(sample_rate));
                if(samp >= length)
                    break;
                h[samp] += rir.x_coeff_lookup[i] * rir.y_coeff_lookup[j] * rir.z_coeff_lookup[k] / dist;
            }
            if (k == 0) {
                break;
            }
        }
        if (j == 0) {
            break;
        }
    }
}


juce::AudioBuffer<float>RIRGenerator::create_full_rir(RIR_DATA rir, ROOM room) {
    // Erzeugung einer Gesamtimpulsantwort über 6 Frequenzen
    int freqs[] = {125, 250, 500, 1000, 2000, 4000};
    int freq_len = sizeof(freqs) / sizeof(freqs[0]);
    
    // Berechnung der 6 Nachhallzeiten
    double t60s[freq_len];
    for(int i = 0; i < freq_len; i++) {
        t60s[i] = calc_t60(room, freqs[i], i);
        
    }

    double t60_max = *std::max_element(t60s, t60s + freq_len);
    int array_length = (int)((double)sample_rate * t60_max);
    
    // Speicherzuweisung des Impulsantwort-Arrays
    double* h =  (double*)calloc(array_length, 8);
    float* h_float = (float*)calloc(array_length,sizeof(float));
    
    for(int i = 0; i < freq_len; i++) {
        int n[3];
        n[0] = C * t60s[i] / (2 * room.room_dim[0]);
        n[1] = C * t60s[i] / (2 * room.room_dim[1]);
        n[2] = C * t60s[i] / (2 * room.room_dim[2]);
        // Speicherzuweisung
        rir.x_coeff_lookup = (double*)malloc((n[0] * 2 + 1) * sizeof(double));
        rir.y_coeff_lookup = (double*)malloc((n[1] * 2 + 1) * sizeof(double));
        rir.z_coeff_lookup = (double*)malloc((n[2] * 2 + 1) * sizeof(double));

        rir.x_dist_lookup  = (double*)malloc((n[0] * 2 + 1) * sizeof(double));
        rir.y_dist_lookup  = (double*)malloc((n[1] * 2 + 1) * sizeof(double));
        rir.z_dist_lookup  = (double*)malloc((n[2] * 2 + 1) * sizeof(double));

        double *dist_tables[3] = {rir.x_dist_lookup, rir.y_dist_lookup, rir.z_dist_lookup};
        double *coeff_tables[3] = {rir.x_coeff_lookup, rir.y_coeff_lookup, rir.z_coeff_lookup};
        for (int axis = 0; axis < 3; axis++) {
            fill_lookup_tables(room, dist_tables[axis], coeff_tables[axis], n[axis], i, axis);
        }

        create_single_rir(rir, room, t60s[i], h, n);
            
        free(rir.x_coeff_lookup);
        free(rir.y_coeff_lookup);
        free(rir.z_coeff_lookup);
        free(rir.x_dist_lookup);
        free(rir.y_dist_lookup);
        free(rir.z_dist_lookup);
    }
    
    // Normalisierung
    double max_h = *std::max_element(h, h + array_length);
    for(int i = 0; i < array_length; i++) {
        h_float[i] = static_cast<float>(h[i]);///max_h
    }
    
    float* ir_data[1] = {h_float};
    juce::AudioBuffer<float> IR_buffer = juce::AudioBuffer<float>(ir_data, 1, array_length);
    
    free(h);
    return IR_buffer;
}

juce::AudioBuffer<float> RIRGenerator::generate(double roomW,double roomL, double roomH,double soundX, double soundY,double soundZ,double receiverX,double receiverY,double receiverZ,std::vector<std::string> materials, int sample_rate)
{
    ROOM room;
    RIR_DATA rir;
    sample_rate = sample_rate;
    room.set_dim(roomW, roomL, roomH);
    room.set_sound(soundX,soundY, soundZ);
    room.set_receiver(receiverX, receiverY,receiverZ);
    room.set_wall_mats(materials.at(0), materials.at(1), materials.at(2), materials.at(3), materials.at(4), materials.at(5));
    auto ir = RIRGenerator::create_full_rir(rir, room);
    return ir;
}
