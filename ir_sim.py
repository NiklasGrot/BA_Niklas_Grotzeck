import matplotlib.pyplot as plt
import numpy as np
import time

SAMPLERATE = 44100
SPEED_OF_SOUND = 343.6

freqs = [125,250,500,1000,2000,4000]

mat_abs = {
    "INIT": [0.9, 0.9, 0.9, 0.9, 0.9, 0.9],
    "floor: concrete": [0.01, 0.01, 0.015, 0.02, 0.02, 0.02],
    "floor: linoleum/vinyl tile on concrete": [0.02, 0.03, 0.03, 0.03, 0.03, 0.02],
    "floor: wood on joists": [0.15, 0.11, 0.10, 0.07, 0.06, 0.07],
    "floor: parquet on concrete": [0.04, 0.04, 0.07, 0.06, 0.06, 0.07],
    "floor: carpet on concrete": [0.02, 0.06, 0.14, 0.37, 0.60, 0.65],
    "floor: carpet on foam": [0.08, 0.24, 0.57, 0.69, 0.71, 0.73],
    "seats: fully occupied - fabric upholstered": [0.60, 0.74, 0.88, 0.96, 0.93, 0.85],
    "seats: occupied wooden pews": [0.57, 0.61, 0.75, 0.86, 0.91, 0.86],
    "seats: empty - fabric upholstered": [0.49, 0.66, 0.80, 0.88, 0.82, 0.70],
    "seats: empty metal/wood seats": [0.15, 0.19, 0.22, 0.39, 0.38, 0.30],
    "wall: Brick: unglazed": [0.03, 0.03, 0.03, 0.04, 0.05, 0.07],
    "wall: Brick: unglazed & painted": [0.01, 0.01, 0.02, 0.02, 0.02, 0.03],
    "wall: Concrete block - coarse": [0.36, 0.44, 0.31, 0.29, 0.39, 0.25],
    "wall: Concrete block - painted": [0.10, 0.05, 0.06, 0.07, 0.09, 0.08],
    "wall: Curtain: 10 oz/sq yd fabric molleton": [0.03, 0.04, 0.11, 0.17, 0.24, 0.35],
    "wall: Curtain: 14 oz/sq yd fabric molleton": [0.07, 0.31, 0.49, 0.75, 0.70, 0.60],
    "wall: Curtain: 18 oz/sq yd fabric molleton": [0.14, 0.35, 0.55, 0.72, 0.70, 0.65],
    "wall: Fiberglass: 2'' 703 no airspace": [0.22, 0.82, 0.99, 0.99, 0.99, 0.99],
    "wall: Fiberglass: spray 5''": [0.05, 0.15, 0.45, 0.70, 0.80, 0.80],
    "wall: Fiberglass: spray 1''": [0.16, 0.45, 0.70, 0.90, 0.90, 0.85],
    "wall: Fiberglass: 2'' rolls": [0.17, 0.55, 0.80, 0.90, 0.85, 0.80],
    "wall: Foam: Sonex 2''": [0.06, 0.25, 0.56, 0.81, 0.90, 0.91],
    "wall: Foam: SDG 3''": [0.24, 0.58, 0.67, 0.91, 0.96, 0.99],
    "wall: Foam: SDG 4''": [0.33, 0.90, 0.84, 0.99, 0.98, 0.99],
    "wall: Foam: polyur. 1''": [0.13, 0.22, 0.68, 1.00, 0.92, 0.97],
    "wall: Foam: polyur. 1/2''": [0.09, 0.11, 0.22, 0.60, 0.88, 0.94],
    "wall: Glass: 1/4'' plate large": [0.18, 0.06, 0.04, 0.03, 0.02, 0.02],
    "wall: Glass: window": [0.35, 0.25, 0.18, 0.12, 0.07, 0.04],
    "wall: Plaster: smooth on tile/brick": [0.013, 0.015, 0.02, 0.03, 0.04, 0.05],
    "wall: Plaster: rough on lath": [0.02, 0.03, 0.04, 0.05, 0.04, 0.03],
    "wall: Marble/Tile": [0.01, 0.01, 0.01, 0.01, 0.02, 0.02],
    "wall: Sheetrock 1/2\" 16\" on center": [0.29, 0.10, 0.05, 0.04, 0.07, 0.09],
    "wall: Wood: 3/8'' plywood panel": [0.28, 0.22, 0.17, 0.09, 0.10, 0.11]
    }

def to_ref_coeff(abs):
    """
    konvertiert Absorptionskoeffizienten zu Reflexionskoeffizienten
    abs: Absorptionskoeffizient
    """
    return np.sqrt(1-abs) 

def calc_t60(room_dim,wall_coeff,freq): #REF-Coeff
    """
    Berechnung der Nachhallzeit (t60)
    :return: Nachhallzeit
    """
    Lx = room_dim[0]
    Ly = room_dim[1]
    Lz = room_dim[2]

    volume = Lx * Ly * Lz
    abs_mat = 0
    for i in range(3):
        surface = None
        if i ==  0: 
            surface = Ly * Lz 
        elif i == 1: 
            surface = Lx * Lz
        elif i == 2: 
            surface = Lx * Ly

        wall_1 = wall_coeff[2*i] * surface
        wall_2 = wall_coeff[2*i+1] * surface
        abs_mat += (wall_1+wall_2) 
        
    humidity = 8.7 #20°C 50% Luftfeuchte 
    abs_air = 5.5 * 0.0001 * (50/humidity) * np.power((freq/1000),1.7)
    t60 = 55.3 * volume / (SPEED_OF_SOUND * (abs_mat + 4 * abs_air * volume))
    return t60

def ir_no_LUT(room_dim,wall_coeff,sound_pos,receiver_pos,t60,remove_sweeping_echoes = False):
    """
    Berechnung der Impulsantwort nach dem Ansatz von Allen und Berkley
    room_dim: Raumdimensionen [Breite,Länge,Höhe]
    wall_coeff: Reflektionskoeffizienten der Wände [bx1,bx2,by1,by2,bz1,bz2]
    sound_pos: Position der Schallquelle [x,y,z]
    receiver_pos: Position des Empfängers [x,y,z]
    t60: Nachhallzeit in s
    remove_sweeping_echoes: true --> sweeping echoes werden durch Zufallsfaktor entfernt 
    """
    # room dimensions
    Lx = room_dim[0]
    Ly = room_dim[1]
    Lz = room_dim[2]

    s_pos = sound_pos
    r_pos = receiver_pos
    wall_coeff = wall_coeff

    def calc_N(l):
        return int((SPEED_OF_SOUND * t60) /(2 * l))

    Nx = calc_N(Lx)
    Ny = calc_N(Ly)
    Nz = calc_N(Lz)

    N_total = np.power(2,3) * (2*Nx + 1) * (2*Ny + 1) * (2*Nz +1)

    random_factor = 0.08 #m
    rir = np.zeros(int(t60 * SAMPLERATE)+1)  

    for mx in range(-Nx,Nx+1):
        for px in range(2):
            mpx = (r_pos[0]- s_pos[0] + 2 * px * s_pos[0]) + (2 * mx * Lx)
            for my in range(-Ny,Ny+1):
                for py in range(2):
                    mpy = (r_pos[1]- s_pos[1] + 2 * py * s_pos[1]) + (2 * my * Ly) 

                    for mz in range(-Nz,Nz+1):
                        for pz in range(2):
                            mpz = (r_pos[2] - s_pos[2] + 2 * pz * s_pos[2]) + (2 * mz * Lz) 
                            ref = np.power(wall_coeff[0],np.abs(mx+px)) * np.power(wall_coeff[1],np.abs(mx)) * np.power(wall_coeff[2],np.abs(my+py)) * np.power(wall_coeff[3],np.abs(my)) * np.power(wall_coeff[4],np.abs(mz + pz)) * np.power(wall_coeff[5],np.abs(mz))
                           
                            abs_vec = np.linalg.norm([mpx,mpy,mpz])  
                            alpha = ref / (4 * np.pi * abs_vec)
                            thau = abs_vec / SPEED_OF_SOUND
                            if remove_sweeping_echoes:
                                thau += np.random.uniform(-random_factor, random_factor) / SPEED_OF_SOUND
                            thau_samples =int(thau * SAMPLERATE)

                            if thau  < t60:
                                rir[thau_samples] += alpha
    return rir

def create_LUTs(room_dim,wall_coeff,sound_pos,receiver_pos,N):
    """
    Erhält die Dimensionen / Werte einer Koordinatenachse und erstellt zwei Lookup Table. 
    Der erste enthält die quadrierten Distanzen der Spiegelschallquellen zum Empfänger.
    Der zweite enthält die resultierendern Reflexionskoeffizienten.
    Berechnung der Impulsantwort nach dem Ansatz von Allen und Berkley
    room_dim: Raumdimension einer Achse
    wall_coeff: Reflektionskoeffizienten der Wände einer Achse 
    sound_pos: Position der Schallquelle 
    receiver_pos: Position des Empfängers 
    N: Anzahl der Spiegelschallquellen der Achse
    """
    dist_lookup = np.zeros(N*2+1) 
    coeff_lookup = np.zeros(N*2+1)
    
    coeff_lookup[0] = 1
    direct_sound = sound_pos - receiver_pos
    dist_lookup[0] = direct_sound * direct_sound

    l_dist = direct_sound
    r_dist = direct_sound
    l_coeff = 1
    r_coeff = 1
    
    for i in range(1,N+1):
        if i%2 != 0:
            l_dist -= 2*(room_dim+sound_pos) 
            r_dist += 2*(room_dim-sound_pos)
            l_coeff *= wall_coeff[0]
            r_coeff *= wall_coeff[1]    
        else:
            l_dist -= 2*(room_dim-sound_pos)
            r_dist += 2*(room_dim+sound_pos)
            l_coeff *= wall_coeff[1]
            r_coeff *= wall_coeff[0] 

        if (i%2 != 0 and (receiver_pos + sound_pos)<0) or (i%2 == 0 and (receiver_pos - sound_pos)<0):
            dist_lookup[2*i-1] = l_dist * l_dist
            dist_lookup[2*i] = r_dist * r_dist
            coeff_lookup[2*i-1] = l_coeff
            coeff_lookup[2*i] = r_coeff
        else: 
            dist_lookup[2*i-1] = r_dist * r_dist
            dist_lookup[2*i] =  l_dist * l_dist
            coeff_lookup[2*i-1] = r_coeff
            coeff_lookup[2*i] = l_coeff
    return dist_lookup, coeff_lookup

def ir_with_LUT(room_dim,wall_coeff,sound_pos,receiver_pos,t60,remove_sweeping_echoes=False):
    """
    Berechnung der Impulsantwort mit einem optimierten Ansatz durch Verwendung von Lookup Tablen und Sortierung 
    room_dim: Raumdimensionen [Breite,Länge,Höhe]
    wall_coeff: Reflektionskoeffizienten der Wände [bx1,bx2,by1,by2,bz1,bz2]
    sound_pos: Position der Schallquelle [x,y,z]
    receiver_pos: Position des Empfängers [x,y,z]
    t60: Nachhallzeit in s
    sweeping_echoes_off: true --> sweeping echoes werden durch Zufallsfaktor entfernt
    """

    # Verschiebung des Raums von 0 -> Raumende zu zentriert um Ursprung
    room_dim = np.array(room_dim) / 2
    sound_pos = np.array(sound_pos) - room_dim
    receiver_pos = np.array(receiver_pos) - room_dim
    
    Nx = int(SPEED_OF_SOUND*t60/(2*room_dim[0]))
    Ny = int(SPEED_OF_SOUND*t60/(2*room_dim[1]))
    Nz = int(SPEED_OF_SOUND*t60/(2*room_dim[2]))

    x_dist, x_coeff = create_LUTs(room_dim[0],wall_coeff[0:2],sound_pos[0],receiver_pos[0],Nx)
    y_dist, y_coeff = create_LUTs(room_dim[1],wall_coeff[2:4],sound_pos[1],receiver_pos[1],Ny)
    z_dist, z_coeff = create_LUTs(room_dim[2],wall_coeff[4:6],sound_pos[2],receiver_pos[2],Nz)
    
    rir_len = int(SAMPLERATE*t60)
    rir = np.zeros(rir_len) 

    random_factor = 0.08
    counter = 0
    for i in range(len(x_dist)):
        for j in range(len(y_dist)):
            for k in range(len(z_dist)):

                dist = np.sqrt(x_dist[i] + y_dist[j] + z_dist[k])
                t = dist / SPEED_OF_SOUND
                if remove_sweeping_echoes:
                    t += np.random.uniform(-random_factor, random_factor) / SPEED_OF_SOUND
                sample = int(t * SAMPLERATE)
                if sample >= rir_len:
                    break
                rir[sample] += x_coeff[i]*y_coeff[j] * z_coeff[k] / dist
                counter += 1
            if k == 0:
                break
        if j == 0:
            break
    return rir

def full_ir(room_dim,wall_mat,sound_pos,receiver_pos,method="lut",remove_sweeping_echoes=False):
    """
    Erzeugt eine vollständige Impulsantwort über die gegebenen Frequenzen [125,250,500,1000,2000,4000]
    Zuerst wird die Nachhallzeit t60 für die jewilige Frequenz berechnet. 
    room_dim: Raumdimensionen [Breite,Länge,Höhe]
    wall_mat: Wandmaterialien 
    sound_pos: Position der Schallquelle [x,y,z]
    receiver_pos: Position des Empfängers [x,y,z]
    method: Verwendete Methode (mit oder ohne Look Up Table)
    """
    start_time = time.time()
    rirs = []
    t60s =[]
    for i in range(len(freqs)):
        f = freqs[i]
        wall_coeffs = [to_ref_coeff(mat_abs[wall][i]) for wall in wall_mat]
        wall_coeffs_abs = [mat_abs[wall][i] for wall in wall_mat]
        t60 = calc_t60(room_dim,wall_coeffs_abs, f)
        if method == "lut":
            rir = ir_with_LUT(room_dim,wall_coeffs,sound_pos,receiver_pos,t60,remove_sweeping_echoes=remove_sweeping_echoes)
        elif method == "no_lut":
            rir = ir_no_LUT(room_dim,wall_coeffs,sound_pos,receiver_pos,t60,remove_sweeping_echoes=remove_sweeping_echoes)
        rirs.append(rir)
        t60s.append(t60)

    rirs = zero_pad(rirs)
    rir = np.sum(rirs, axis=0)
    rir = rir/np.max(rir)
    t60 = np.mean(t60s)
    end_time = time.time()
    calc_time = end_time-start_time
    return rir,t60,calc_time

def zero_pad(arrays):
    lengths = [len(arr) for arr in arrays]
    max_len = max(lengths)
    new_list = []
    for arr in arrays:
        padded_array = np.pad(arr, (0, max_len-len(arr)),mode ='constant')
        new_list.append(padded_array)
    return new_list

def plot_schroeder(ax,signal):

    def energie(data):
        """
        berechnet die aufsummierte Energie zu jedem Zeitpunkt
        :return: Array mit allen Energiewerten
        """
        energieWerte = np.array([],dtype = np.float64) #erstellt ein neues Array zum speichern der Energiewerte
        for i in range(len(data)-1):
            e = np.sum(np.square(data[i:-1])) #berechnet die Energie summiert vom Wert an der Stelle i bis zum letzten Wert des Datensatzes
            energieWerte = np.append(energieWerte,e)
        return energieWerte

    def gesamtEnergie(data):
        """
        berechnet die Gesamtenergie
        :return: Wert der Gesamtenergie
        """
        return np.sum(np.square(data))

    energy = energie(signal)
    en_db = 10 * np.log10(energy / gesamtEnergie(signal))

    ax.plot(np.arange(len(en_db))/SAMPLERATE, en_db,color = 'red')
    ax.grid()
    ax.set_xlabel('Zeit [s]')
    ax.set_ylabel('[dB]')
    ax.set_title('Schröder-Plot')
    ax.set_ylim(-75, 2)

def plot_spectrogram(ax,signal, window_size=256, overlap=128):

    ax.specgram(signal, Fs=SAMPLERATE, NFFT=window_size, noverlap=overlap)
    ax.set_title("Spektrogram")
    ax.set_xlabel('Zeit [s]')
    ax.set_ylabel('Frequenz [Hz]')

############### SET ROOM DATA ###############
room_dim = [5,7,3]
sound_pos = [4,3.5,2]
receiver_pos = [1,3.5,1.8]
wall_mat =["wall: Wood: 3/8'' plywood panel","wall: Wood: 3/8'' plywood panel","wall: Wood: 3/8'' plywood panel","wall: Wood: 3/8'' plywood panel","floor: concrete","floor: carpet on concrete"]

##############################################

# Check if Dimensions are legal
assert sound_pos[0] < room_dim[0] and sound_pos[1] < room_dim[1] and sound_pos[2] < room_dim[2], "sound out of bounds"
assert receiver_pos[0] < room_dim[0] and receiver_pos[1] < room_dim[1] and receiver_pos[2] < room_dim[2], "receiver out of bounds"

rir_no_LUT,t60,calc_time_no_LUT = full_ir(room_dim,wall_mat,sound_pos,receiver_pos,method="no_lut")
rir_LUT,t60,calc_time_LUT = full_ir(room_dim,wall_mat,sound_pos,receiver_pos,method="lut")


# Plotting 
len_rir = (int(t60 * SAMPLERATE))

fig, axs = plt.subplots(3, 2,figsize=(10, 8))
fig.suptitle('Raumimpulsantwort', fontsize=16)

text = ("Nachhallzeit: {:.3f}s \n Berechnungszeit:           {:.4f}s                                             {:.4f}s".format(t60,calc_time_no_LUT,calc_time_LUT))
plt.figtext(0.5, 0.89, text, ha='center', fontsize=10)

axs[0,0].stem(rir_no_LUT,markerfmt='')
axs[0,0].set_xlim(0,len_rir + 100)
axs[0,0].axvline(x=len_rir, color='red', linestyle='--')
axs[0,0].set_title("Methode ohne LUTs")
axs[0,0].set_ylabel("Amplitude")
axs[0,0].set_xlabel("Samples")

axs[0,1].stem(rir_LUT,markerfmt='')
axs[0,1].set_xlim(0,len_rir + 100)
axs[0,1].axvline(x=len_rir, color='red', linestyle='--')
axs[0,1].set_title("Methode mit LUTs")
axs[0,1].set_ylabel("Amplitude")
axs[0,1].set_xlabel("Samples")

plot_spectrogram(axs[1,0],rir_no_LUT)
plot_spectrogram(axs[1,1],rir_LUT)

plot_schroeder(axs[2,0],rir_no_LUT)
plot_schroeder(axs[2,1],rir_LUT)

plt.tight_layout(rect=[0, 0, 1, 0.93])
plt.show()

