#include <sdsl/suffix_arrays.hpp>
#include <string>
#include <iostream>
#include <algorithm>
#include <vector>
#include "utils.hpp"

using namespace sdsl;
using namespace std;

int main(int argc, char** argv) {
    string archivo_entrada = validar_archivo_entrada(argc, argv);

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
        size_t occs = sdsl::count(fm_index, patron.begin(), patron.end());
        long t_busqueda = timer.transcurrido_ms();

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

        // Registrar búsqueda en CSV
        escribir_csv_busqueda("exp-FM-busquedas.csv",
                             archivo_entrada,
                             patron,
                             patron.size(),
                             t_busqueda,
                             occs);
    }

    return 0;
}
