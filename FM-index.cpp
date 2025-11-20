#include <sdsl/suffix_arrays.hpp>
#include <string>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <fstream>   // <-- para los CSV
#include <vector>

using namespace sdsl;
using namespace std;
using namespace std::chrono;

int main(int argc, char** argv) {
    if (argc != 2) {
        cout << "Uso: " << argv[0] << " <archivo entrada>" << endl;
        return 1;
    }

    string archivo_entrada = argv[1];

    cout << "Construyendo el FM-index ..." << endl;
    csa_wt<wt_int<>> fm_index;

    auto inicio = high_resolution_clock::now();
    construct(fm_index, archivo_entrada, 1);
    auto fin = high_resolution_clock::now();

    auto t_construccion = duration_cast<milliseconds>(fin - inicio).count();
    double tamano_mb = size_in_mega_bytes(fm_index);

    cout << "Tamaño del FM-index: " << tamano_mb << " MB." << endl;
    cout << "Tiempo empleado en la construcción: " << t_construccion << " ms" << endl;

    // =============================
    //  LOG: exp-FM-creacion.csv
    //  columnas: archivo,tiempo_ms,tamano_mb
    // =============================
    {
        ofstream out("exp-FM-creacion.csv", ios::app);
        if (!out) {
            cerr << "Advertencia: no se pudo abrir exp-FM-creacion.csv para escribir." << endl;
        } else {
            // Si el archivo está vacío, escribimos cabecera
            if (out.tellp() == 0) {
                out << "archivo,tiempo_ms,tamano_mb\n";
            }
            out << archivo_entrada << ","
                << t_construccion << ","
                << tamano_mb << "\n";
        }
    }

    // =============================
    //  BÚSQUEDAS INTERACTIVAS
    //  se repiten hasta que el patrón sea "exit"
    // =============================

    // Abrimos (una vez) el archivo de log de búsquedas
    // columnas: archivo,patron,tiempo_ms,ocurrencias
    ofstream out_busq("exp-FM-busquedas.csv", ios::app);
    if (!out_busq) {
        cerr << "Advertencia: no se pudo abrir exp-FM-busquedas.csv para escribir." << endl;
    } else {
        if (out_busq.tellp() == 0) {
            out_busq << "archivo,patron,tiempo_ms,ocurrencias\n";
        }
    }

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
        inicio = high_resolution_clock::now();
        size_t occs = sdsl::count(fm_index, patron.begin(), patron.end());
        fin = high_resolution_clock::now();
        auto t_busqueda = duration_cast<milliseconds>(fin - inicio).count();

        cout << "# de ocurrencias: " << occs << endl;
        cout << "Tiempo de búsqueda: " << t_busqueda << " ms" << endl;

        // Si hay ocurrencias, las mostramos
        if (occs > 0) {
            cout << "Las ocurrencias comienzan en las siguientes posiciones: " << endl;
            auto posiciones = sdsl::locate(fm_index, patron.begin(), patron.end());
            sort(posiciones.begin(), posiciones.end());

            for (size_t i = 0; i < posiciones.size(); ++i) {
                cout << posiciones[i];
                if (i + 1 < posiciones.size()) cout << ",";
            }
            cout << "\n";
        }

        // Escribimos al CSV de búsquedas
        if (out_busq) {
            out_busq << archivo_entrada << ","
                     << patron << ","
                     << t_busqueda << ","
                     << occs << "\n";
        }
    }

    return 0;
}
