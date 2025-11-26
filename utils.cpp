#include "utils.hpp"
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <vector>

using namespace std;

// ============================================================================
// Implementación de la clase Temporizador
// ============================================================================

Temporizador::Temporizador()
    : inicio(chrono::high_resolution_clock::now()) {
}

long Temporizador::transcurrido_ms() const {
    auto fin = chrono::high_resolution_clock::now();
    return chrono::duration_cast<chrono::milliseconds>(fin - inicio).count();
}

long long Temporizador::transcurrido_ns() const {
    auto fin = chrono::high_resolution_clock::now();
    return chrono::duration_cast<chrono::nanoseconds>(fin - inicio).count();
}

void Temporizador::reiniciar() {
    inicio = chrono::high_resolution_clock::now();
}

// ============================================================================
// Funciones de utilidad
// ============================================================================

string validar_archivo_entrada(int argc, char** argv) {
    if (argc != 2) {
        cout << "Uso: " << argv[0] << " <archivo_entrada>" << endl;
        exit(1);
    }
    return string(argv[1]);
}

double obtener_tamano_archivo_mb(const string& nombre_archivo) {
    struct stat stat_buf;
    int rc = stat(nombre_archivo.c_str(), &stat_buf);
    if (rc != 0) {
        cerr << "Advertencia: no se pudo obtener el tamaño de " << nombre_archivo << endl;
        return 0.0;
    }
    return stat_buf.st_size / (1024.0 * 1024.0);
}

void escribir_csv_construccion(const string& nombre_csv,
                                const string& archivo_entrada,
                                double tamano_original_mb,
                                long tiempo_construccion_ms,
                                double tamano_estructura_mb) {
    ofstream out(nombre_csv, ios::app);
    if (!out) {
        cerr << "Advertencia: no se pudo abrir " << nombre_csv << " para escribir." << endl;
        return;
    }

    // Escribir encabezado si el archivo está vacío
    if (out.tellp() == 0) {
        out << "archivo_original,tamano_original_mb,tiempo_construccion_ms,tamano_estructura_mb\n";
    }

    out << archivo_entrada << ","
        << tamano_original_mb << ","
        << tiempo_construccion_ms << ","
        << tamano_estructura_mb << "\n";

    out.close();
}

void escribir_csv_busqueda(const string& nombre_csv,
                           const string& archivo_entrada,
                           const string& patron,
                           size_t tamano_patron,
                           long long tiempo_busqueda_ns,
                           size_t ocurrencias) {
    ofstream out(nombre_csv, ios::app);
    if (!out) {
        cerr << "Advertencia: no se pudo abrir " << nombre_csv << " para escribir." << endl;
        return;
    }

    // Escribir encabezado si el archivo está vacío
    if (out.tellp() == 0) {
        out << "archivo_original,patron,tamano_patron,tiempo_busqueda_ns,ocurrencias\n";
    }

    out << archivo_entrada << ","
        << patron << ","
        << tamano_patron << ","
        << tiempo_busqueda_ns << ","
        << ocurrencias << "\n";

    out.close();
}

vector<string> leer_patrones_desde_archivo(const string& nombre_archivo) {
    vector<string> patrones;

    // Abrir archivo en modo binario para lectura exacta de bytes
    ifstream archivo(nombre_archivo, ios::binary);

    if (!archivo.is_open()) {
        cerr << "Advertencia: no se pudo abrir " << nombre_archivo << " para leer patrones." << endl;
        return patrones;
    }

    size_t largo_bytes;

    // Leer patrones mientras haya datos
    while (archivo >> largo_bytes) {
        // Consumir el salto de línea después del número
        archivo.ignore();

        // Leer exactamente largo_bytes bytes
        vector<char> buffer(largo_bytes);
        if (archivo.read(buffer.data(), largo_bytes)) {
            // Convertir a string y agregar al vector
            string patron(buffer.begin(), buffer.end());
            patrones.push_back(patron);
        } else {
            cerr << "Advertencia: se esperaban " << largo_bytes << " bytes pero el archivo terminó antes." << endl;
            break;
        }
    }

    archivo.close();
    return patrones;
}
