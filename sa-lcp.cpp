// Construcción del suffix array, LCP array y búsqueda de patrones
//
// Prerrequisitos: Tener la biblioteca SDSL instalada
//
// Compilación: g++ -O3 -o SA-LCP sa-lcp.cpp utils.cpp -lsdsl -ldivsufsort -ldivsufsort64

#include <sdsl/suffix_arrays.hpp>
#include <sdsl/lcp.hpp>
#include <string>
#include <iostream>
#include <algorithm>
#include "utils.hpp"

using namespace sdsl;
using namespace std;

// Algoritmo de Kasai para construir LCP en O(n)
// Referencia: Kasai et al. (2001) "Linear-Time Longest-Common-Prefix Computation
// in Suffix Arrays and Its Applications"
void construir_lcp_kasai(int_vector<>& lcp, const int_vector<>& sa, const int_vector<>& seq) {
    size_t n = sa.size();
    lcp.resize(n);
    lcp[0] = 0;

    // Construir el array inverso del SA (rank)
    int_vector<> rank(n);
    for (size_t i = 0; i < n; ++i) {
        rank[sa[i]] = i;
    }

    size_t h = 0;
    for (size_t i = 0; i < n; ++i) {
        if (rank[i] > 0) {
            size_t j = sa[rank[i] - 1];
            while (i + h < n && j + h < n && seq[i + h] == seq[j + h]) {
                h++;
            }
            lcp[rank[i]] = h;
            if (h > 0) h--;
        }
    }
}

// Búsqueda de patrón usando SA + LCP
// Usa búsqueda binaria mejorada con información del LCP
size_t buscar_patron_sa_lcp(const int_vector<>& sa, const int_vector<>& seq,
                            const int_vector<>& lcp, const string& patron,
                            vector<size_t>& posiciones) {
    posiciones.clear();

    size_t n = sa.size();
    size_t m = patron.size();

    if (m == 0) return 0;

    // Función auxiliar para comparar patrón con sufijo
    auto comparar = [&](size_t idx_sa) -> int {
        size_t pos = sa[idx_sa];
        for (size_t i = 0; i < m && pos + i < n; ++i) {
            if ((unsigned char)patron[i] < seq[pos + i]) return -1;
            if ((unsigned char)patron[i] > seq[pos + i]) return 1;
        }
        // Si llegamos aquí, el patrón coincide con los primeros m caracteres
        return 0;
    };

    // Búsqueda binaria para encontrar el primer sufijo que coincide
    size_t left = 0, right = n - 1;
    size_t first = n;  // Usar n como valor inválido en lugar de -1

    while (left <= right) {
        size_t mid = (left + right) / 2;
        int cmp = comparar(mid);

        if (cmp == 0) {
            first = mid;
            if (mid == 0) break;
            right = mid - 1; // Seguir buscando hacia la izquierda
        } else if (cmp < 0) {
            if (mid == 0) break;
            right = mid - 1;
        } else {
            left = mid + 1;
        }
    }

    if (first == n) {
        return 0; // No se encontró el patrón
    }

    // Encontrar el último sufijo que coincide
    left = first;
    right = n - 1;
    size_t last = first;

    while (left <= right) {
        size_t mid = (left + right) / 2;
        int cmp = comparar(mid);

        if (cmp == 0) {
            last = mid;
            left = mid + 1; // Seguir buscando hacia la derecha
        } else if (cmp < 0) {
            if (mid == 0) break;
            right = mid - 1;
        } else {
            left = mid + 1;
        }
    }

    // Recolectar todas las posiciones
    for (size_t i = first; i <= last; ++i) {
        posiciones.push_back(sa[i]);
    }

    sort(posiciones.begin(), posiciones.end());
    return posiciones.size();
}

int main(int argc, char** argv) {
    if (argc < 2 || argc > 4) {
        cout << "Uso: " << argv[0] << " <archivo_entrada> [archivo_patrones] [repeticiones]" << endl;
        cout << "  archivo_entrada: archivo de texto para indexar" << endl;
        cout << "  archivo_patrones: (opcional) archivo con patrones para buscar" << endl;
        cout << "  repeticiones: (opcional) número de veces que se ejecuta cada búsqueda (default: 1)" << endl;
        cout << "                Si no se especifica archivo_patrones, se usa modo interactivo" << endl;
        return 1;
    }

    string archivo_entrada = argv[1];
    string archivo_patrones = (argc >= 3) ? argv[2] : "";
    int num_repeticiones = (argc == 4) ? atoi(argv[3]) : 1;
    bool modo_archivo = !archivo_patrones.empty();

    if (num_repeticiones < 1) {
        cerr << "Error: el número de repeticiones debe ser al menos 1" << endl;
        return 1;
    }

    // Leemos el archivo de entrada y guardamos el contenido en 'seq'
    int_vector<> seq;
    size_t n;
    {
        load_vector_from_file(seq, archivo_entrada, 1);
        n = seq.size();

        seq.resize(n+1);
        n = seq.size();
        seq[n-1] = 0; // Representa el final de texto. Suele representarse por el símbolo $
    }

    cout << "Construyendo el Suffix array ..." << endl;

    Temporizador timer;
    int_vector<> sa(1, 0, bits::hi(n)+1);
    sa.resize(n);
    algorithm::calculate_sa((const unsigned char*)seq.data(), n, sa);

    cout << "Construyendo el LCP array (algoritmo de Kasai O(n)) ..." << endl;
    int_vector<> lcp;
    construir_lcp_kasai(lcp, sa, seq);

    long t_construccion = timer.transcurrido_ms();

    double tamano_original_mb = obtener_tamano_archivo_mb(archivo_entrada);
    double tamano_sa_mb = size_in_mega_bytes(sa);
    double tamano_lcp_mb = size_in_mega_bytes(lcp);
    double tamano_total_mb = tamano_sa_mb + tamano_lcp_mb;

    cout << "Tamaño del archivo original: " << tamano_original_mb << " MB." << endl;
    cout << "Tamaño del SA: " << tamano_sa_mb << " MB." << endl;
    cout << "Tamaño del LCP: " << tamano_lcp_mb << " MB." << endl;
    cout << "Tamaño total (SA + LCP): " << tamano_total_mb << " MB." << endl;
    cout << "Tiempo empleado en la construcción: " << t_construccion << " ms" << endl;

    // Registrar construcción en CSV
    escribir_csv_construccion("exp-SA-LCP-construccion.csv",
                              archivo_entrada,
                              tamano_original_mb,
                              t_construccion,
                              tamano_total_mb);

    // =============================
    //  BÚSQUEDAS
    // =============================

    vector<string> patrones;

    if (modo_archivo) {
        // Modo archivo: leer patrones desde archivo
        cout << "Leyendo patrones desde: " << archivo_patrones << endl;
        patrones = leer_patrones_desde_archivo(archivo_patrones);
        cout << "Patrones leídos: " << patrones.size() << endl;
        cout << "Repeticiones por patrón: " << num_repeticiones << endl;
    } else {
        // Modo interactivo: leer patrones desde entrada estándar
        cout << "Modo interactivo: ingrese patrones (escriba 'exit' para terminar)" << endl;
        while (true) {
            string patron;
            cout << "Ingrese un patrón a buscar (o 'exit' para terminar): ";
            if (!getline(cin, patron)) {
                break;
            }
            if (patron == "exit") {
                cout << "Terminando las búsquedas." << endl;
                break;
            }
            patrones.push_back(patron);
        }
    }

    // Ejecutar búsquedas para todos los patrones
    for (size_t i = 0; i < patrones.size(); ++i) {
        const string& patron = patrones[i];

        // Repetir la búsqueda num_repeticiones veces
        for (int rep = 0; rep < num_repeticiones; ++rep) {
            // Búsqueda
            timer.reiniciar();
            vector<size_t> posiciones;
            size_t occs = buscar_patron_sa_lcp(sa, seq, lcp, patron, posiciones);
            long long t_busqueda = timer.transcurrido_ns();

            if (!modo_archivo) {
                cout << "Patrón #" << (i + 1) << " (repetición " << (rep + 1) << "): " << patron << endl;
                cout << "# de ocurrencias: " << occs << endl;
                cout << "Tiempo de búsqueda: " << t_busqueda << " ns" << endl;

                // Si hay ocurrencias, las mostramos (solo en la primera repetición)
                if (rep == 0 && occs > 0) {
                    cout << "Las ocurrencias comienzan en las siguientes posiciones: " << endl;
                    for (size_t j = 0; j < posiciones.size(); ++j) {
                        cout << posiciones[j];
                        if (j + 1 < posiciones.size()) cout << ",";
                    }
                    cout << "\n";
                }
            }

            // Registrar búsqueda en CSV
            escribir_csv_busqueda("exp-SA-LCP-busquedas.csv",
                                 archivo_entrada,
                                 patron.size(),
                                 t_busqueda,
                                 occs);
        }
    }

    if (modo_archivo) {
        cout << "Búsquedas completadas: " << (patrones.size() * num_repeticiones)
             << " búsquedas (" << patrones.size() << " patrones × "
             << num_repeticiones << " repeticiones)" << endl;
    }

    return 0;
}
