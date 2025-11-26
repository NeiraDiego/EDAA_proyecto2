#include <sdsl/suffix_arrays.hpp>
#include <string>
#include <iostream>
#include <algorithm>
#include <vector>
#include "utils.hpp"

using namespace sdsl;
using namespace std;

int main(int argc, char** argv) {
    if (argc < 2 || argc > 3) {
        cout << "Uso: " << argv[0] << " <archivo_entrada> [archivo_patrones]" << endl;
        cout << "  archivo_entrada: archivo de texto para indexar" << endl;
        cout << "  archivo_patrones: (opcional) archivo con patrones para buscar" << endl;
        cout << "                    Si no se especifica, se usa modo interactivo" << endl;
        return 1;
    }

    string archivo_entrada = argv[1];
    string archivo_patrones = (argc == 3) ? argv[2] : "";
    bool modo_archivo = !archivo_patrones.empty();

    cout << "Construyendo el FM-index ..." << endl;
    csa_wt<wt_int<>> fm_index;

    Temporizador timer;
    construct(fm_index, archivo_entrada, 1);
    long t_construccion = timer.transcurrido_ms();

    double tamano_original_mb = obtener_tamano_archivo_mb(archivo_entrada);
    double tamano_estructura_mb = size_in_mega_bytes(fm_index);

    cout << "Tamaño del archivo original: " << tamano_original_mb << " MB." << endl;
    cout << "Tamaño del FM-index: " << tamano_estructura_mb << " MB." << endl;
    cout << "Tiempo empleado en la construcción: " << t_construccion << " ms" << endl;

    // Registrar construcción en CSV
    escribir_csv_construccion("exp-FM-construccion.csv",
                              archivo_entrada,
                              tamano_original_mb,
                              t_construccion,
                              tamano_estructura_mb);

    // =============================
    //  BÚSQUEDAS
    // =============================

    vector<string> patrones;

    if (modo_archivo) {
        // Modo archivo: leer patrones desde archivo
        cout << "Leyendo patrones desde: " << archivo_patrones << endl;
        patrones = leer_patrones_desde_archivo(archivo_patrones);
        cout << "Patrones leídos: " << patrones.size() << endl;
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

        // Búsqueda
        timer.reiniciar();
        size_t occs = sdsl::count(fm_index, patron.begin(), patron.end());
        long long t_busqueda = timer.transcurrido_ns();

        if (!modo_archivo) {
            cout << "Patrón #" << (i + 1) << ": " << patron << endl;
            cout << "# de ocurrencias: " << occs << endl;
            cout << "Tiempo de búsqueda: " << t_busqueda << " ns" << endl;

            // Si hay ocurrencias, las mostramos
            if (occs > 0) {
                cout << "Las ocurrencias comienzan en las siguientes posiciones: " << endl;
                auto posiciones = sdsl::locate(fm_index, patron.begin(), patron.end());
                sort(posiciones.begin(), posiciones.end());

                for (size_t j = 0; j < posiciones.size(); ++j) {
                    cout << posiciones[j];
                    if (j + 1 < posiciones.size()) cout << ",";
                }
                cout << "\n";
            }
        }

        // Registrar búsqueda en CSV
        escribir_csv_busqueda("exp-FM-busquedas.csv",
                             archivo_entrada,
                             patron,
                             patron.size(),
                             t_busqueda,
                             occs);
    }

    if (modo_archivo) {
        cout << "Búsquedas completadas: " << patrones.size() << " patrones procesados" << endl;
    }

    return 0;
}
