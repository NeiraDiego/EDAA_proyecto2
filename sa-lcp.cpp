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

// Función para calcular el LCP entre dos sufijos
int calcular_lcp(const int_vector<>& seq, int pos1, int pos2, int n) {
    int lcp = 0;
    while (pos1 + lcp < n && pos2 + lcp < n && seq[pos1 + lcp] == seq[pos2 + lcp]) {
        lcp++;
    }
    return lcp;
}

// Construir el array LCP manualmente
void construir_lcp_array(const int_vector<>& sa, const int_vector<>& seq,
                         int_vector<>& lcp) {
    int n = sa.size();
    lcp.resize(n);
    lcp[0] = 0;

    for (int i = 1; i < n; ++i) {
        lcp[i] = calcular_lcp(seq, sa[i-1], sa[i], n);
    }
}

// Búsqueda de patrón usando SA + LCP
// Usa búsqueda binaria mejorada con información del LCP
size_t buscar_patron_sa_lcp(const int_vector<>& sa, const int_vector<>& seq,
                            const int_vector<>& lcp, const string& patron,
                            vector<size_t>& posiciones) {
    posiciones.clear();

    int n = sa.size();
    int m = patron.size();

    if (m == 0) return 0;

    // Función auxiliar para comparar patrón con sufijo
    auto comparar = [&](int idx_sa) -> int {
        int pos = sa[idx_sa];
        for (int i = 0; i < m && pos + i < n; ++i) {
            if ((unsigned char)patron[i] < seq[pos + i]) return -1;
            if ((unsigned char)patron[i] > seq[pos + i]) return 1;
        }
        // Si llegamos aquí, el patrón coincide con los primeros m caracteres
        return 0;
    };

    // Búsqueda binaria para encontrar el primer sufijo que coincide
    int left = 0, right = n - 1;
    int first = -1;

    while (left <= right) {
        int mid = (left + right) / 2;
        int cmp = comparar(mid);

        if (cmp == 0) {
            first = mid;
            right = mid - 1; // Seguir buscando hacia la izquierda
        } else if (cmp < 0) {
            right = mid - 1;
        } else {
            left = mid + 1;
        }
    }

    if (first == -1) {
        return 0; // No se encontró el patrón
    }

    // Encontrar el último sufijo que coincide
    left = first;
    right = n - 1;
    int last = first;

    while (left <= right) {
        int mid = (left + right) / 2;
        int cmp = comparar(mid);

        if (cmp == 0) {
            last = mid;
            left = mid + 1; // Seguir buscando hacia la derecha
        } else if (cmp < 0) {
            right = mid - 1;
        } else {
            left = mid + 1;
        }
    }

    // Recolectar todas las posiciones
    for (int i = first; i <= last; ++i) {
        posiciones.push_back(sa[i]);
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

    cout << "Construyendo el Suffix array ..." << endl;

    Temporizador timer;
    int_vector<> sa(1, 0, bits::hi(n)+1);
    sa.resize(n);
    algorithm::calculate_sa((const unsigned char*)seq.data(), n, sa);

    cout << "Construyendo el LCP array ..." << endl;
    int_vector<> lcp;
    construir_lcp_array(sa, seq, lcp);

    cout << "Construyendo la BWT ..." << endl;
    int_vector<> bwt(1, 0, 8);
    bwt.resize(n);

    int32_t to_add[2] = {(int32_t)-1, n-1};
    for (int32_t i=0; i < n; ++i)
        bwt[i] = seq[sa[i] + to_add[sa[i]==0]];

    long t_construccion = timer.transcurrido_ms();

    double tamano_original_mb = obtener_tamano_archivo_mb(archivo_entrada);
    double tamano_sa_mb = size_in_mega_bytes(sa);
    double tamano_lcp_mb = size_in_mega_bytes(lcp);
    double tamano_bwt_mb = size_in_mega_bytes(bwt);
    double tamano_total_mb = tamano_sa_mb + tamano_lcp_mb + tamano_bwt_mb;

    cout << "Tamaño del archivo original: " << tamano_original_mb << " MB." << endl;
    cout << "Tamaño del SA: " << tamano_sa_mb << " MB." << endl;
    cout << "Tamaño del LCP: " << tamano_lcp_mb << " MB." << endl;
    cout << "Tamaño de la BWT: " << tamano_bwt_mb << " MB." << endl;
    cout << "Tamaño total (SA + LCP + BWT): " << tamano_total_mb << " MB." << endl;
    cout << "Tiempo empleado en la construcción: " << t_construccion << " ms" << endl;

    // Registrar construcción en CSV
    escribir_csv_construccion("exp-SA-LCP-construccion.csv",
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
        size_t occs = buscar_patron_sa_lcp(sa, seq, lcp, patron, posiciones);
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
        escribir_csv_busqueda("exp-SA-LCP-busquedas.csv",
                             archivo_entrada,
                             patron,
                             patron.size(),
                             t_busqueda,
                             occs);
    }

    return 0;
}
