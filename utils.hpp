#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <vector>
#include <chrono>

// Clase para medición de tiempo de ejecución
class Temporizador {
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> inicio;

public:
    // Constructor: inicia el temporizador automáticamente
    Temporizador();

    // Retorna el tiempo transcurrido en milisegundos
    long transcurrido_ms() const;

    // Retorna el tiempo transcurrido en nanosegundos
    long long transcurrido_ns() const;

    // Reinicia el temporizador
    void reiniciar();
};

// Valida argumentos de entrada y retorna el nombre del archivo
// Termina el programa con código 1 si los argumentos son inválidos
std::string validar_archivo_entrada(int argc, char** argv);

// Obtiene el tamaño de un archivo en megabytes
double obtener_tamano_archivo_mb(const std::string& nombre_archivo);

// Escribe una línea en el CSV de construcción
// Formato: archivo_original,tamano_original_mb,tiempo_construccion_ms,tamano_estructura_mb
void escribir_csv_construccion(const std::string& nombre_csv,
                                const std::string& archivo_entrada,
                                double tamano_original_mb,
                                long tiempo_construccion_ms,
                                double tamano_estructura_mb);

// Escribe una línea en el CSV de búsquedas
// Formato: archivo_original,tamano_patron,tiempo_busqueda_ns,ocurrencias
void escribir_csv_busqueda(const std::string& nombre_csv,
                           const std::string& archivo_entrada,
                           size_t tamano_patron,
                           long long tiempo_busqueda_ns,
                           size_t ocurrencias);

// Lee patrones desde un archivo en formato especial:
// - Una línea con el largo del patrón (número entero)
// - Exactamente ese número de bytes con el contenido del patrón
// - Se repite para cada patrón
// Retorna un vector con todos los patrones leídos
std::vector<std::string> leer_patrones_desde_archivo(const std::string& nombre_archivo);

#endif // UTILS_HPP
