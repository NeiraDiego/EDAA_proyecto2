// Construcción del suffix array de un texto con búsqueda binaria
//
// Prerrequisitos: Tener la biblioteca SDSL instalada
//
// Compilación: g++ -O3 -o SA sa.cpp utils.cpp -lsdsl -ldivsufsort -ldivsufsort64

#include <sdsl/suffix_arrays.hpp>
#include <string>
#include <iostream>
#include <algorithm>
#include "utils.hpp"

using namespace sdsl;
using namespace std;

// Función auxiliar para buscar un patrón en el suffix array
// Retorna el número de ocurrencias y llena el vector de posiciones
size_t buscar_patron_sa(const int_vector<>& sa, const int_vector<>& seq,
                        const string& patron, vector<size_t>& posiciones) {
    posiciones.clear();

    int n = sa.size();
    int m = patron.size();

    // Búsqueda binaria para encontrar el rango en el SA
    int left = 0, right = n;

    // Encontrar el primer sufijo que comienza con el patrón
    int first = n;
    while (left < right) {
        int mid = (left + right) / 2;
        int pos = sa[mid];

        // Comparar el patrón con el sufijo en la posición mid
        bool menor = false;
        for (int i = 0; i < m && pos + i < n; ++i) {
            if (patron[i] < seq[pos + i]) {
                menor = true;
                break;
            } else if (patron[i] > seq[pos + i]) {
                break;
            }
        }

        if (menor) {
            right = mid;
        } else {
            left = mid + 1;
            first = mid;
        }
    }

    // Buscar todas las ocurrencias desde first hacia atrás
    for (int i = first; i >= 0; --i) {
        int pos = sa[i];
        bool coincide = true;

        for (int j = 0; j < m && pos + j < n; ++j) {
            if (patron[j] != seq[pos + j]) {
                coincide = false;
                break;
            }
        }

        if (coincide) {
            posiciones.push_back(pos);
        } else {
            break;
        }
    }

    // Buscar todas las ocurrencias desde first+1 hacia adelante
    for (int i = first + 1; i < n; ++i) {
        int pos = sa[i];
        bool coincide = true;

        for (int j = 0; j < m && pos + j < n; ++j) {
            if (patron[j] != seq[pos + j]) {
                coincide = false;
                break;
            }
        }

        if (coincide) {
            posiciones.push_back(pos);
        } else {
            break;
        }
    }

    sort(posiciones.begin(), posiciones.end());
    return posiciones.size();
}

int main(int argc, char** argv) {
    string archivo_entrada = validar_archivo_entrada(argc, argv);

    // Leemos el archivo de entrada y guardamos el contenido en 'seq'
    int_vector<> seq;
    int32_t n;
    {
        load_vector_from_file(seq, archivo_entrada, 1);
        n = seq.size();

        seq.resize(n+1);
        n = seq.size();
        seq[n-1] = 0; // Representa el final de texto. Suele representarse por el símbolo $
    }

    cout << "Construyendo el Suffix Array ..." << endl;

    Temporizador timer;
    int_vector<> sa(1, 0, bits::hi(n)+1);
    sa.resize(n);
    algorithm::calculate_sa((const unsigned char*)seq.data(), n, sa);

    long t_construccion = timer.transcurrido_ms();

    double tamano_original_mb = obtener_tamano_archivo_mb(archivo_entrada);
    double tamano_sa_mb = size_in_mega_bytes(sa);
    double tamano_total_mb = tamano_sa_mb;

    cout << "Tamaño del archivo original: " << tamano_original_mb << " MB." << endl;
    cout << "Tamaño del SA: " << tamano_sa_mb << " MB." << endl;
    cout << "Tiempo empleado en la construcción: " << t_construccion << " ms" << endl;

    // Registrar construcción en CSV
    escribir_csv_construccion("exp-SA-construccion.csv",
                              archivo_entrada,
                              tamano_original_mb,
                              t_construccion,
                              tamano_total_mb);

    // =============================
    //  BÚSQUEDAS INTERACTIVAS
    //  se repiten hasta que el patrón sea "exit"
    // =============================

    while (true) {
        string patron;
        cout << "Ingrese un patrón a buscar (o 'exit' para terminar): ";
        if (!(cin >> patron)) {
            // EOF o error en la entrada
            break;
        }
        if (patron == "exit") {
            cout << "Terminando las búsquedas." << endl;
            break;
        }

        // Búsqueda
        timer.reiniciar();
        vector<size_t> posiciones;
        size_t occs = buscar_patron_sa(sa, seq, patron, posiciones);
        long t_busqueda = timer.transcurrido_ms();

        cout << "# de ocurrencias: " << occs << endl;
        cout << "Tiempo de búsqueda: " << t_busqueda << " ms" << endl;

        // Si hay ocurrencias, las mostramos
        if (occs > 0) {
            cout << "Las ocurrencias comienzan en las siguientes posiciones: " << endl;
            for (size_t i = 0; i < posiciones.size(); ++i) {
                cout << posiciones[i];
                if (i + 1 < posiciones.size()) cout << ",";
            }
            cout << "\n";
        }

        // Registrar búsqueda en CSV
        escribir_csv_busqueda("exp-SA-busquedas.csv",
                             archivo_entrada,
                             patron,
                             patron.size(),
                             t_busqueda,
                             occs);
    }

    return 0;
}
