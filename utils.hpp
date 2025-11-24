#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
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
// Formato: archivo_original,patron,tamano_patron,tiempo_busqueda_ms,ocurrencias
void escribir_csv_busqueda(const std::string& nombre_csv,
                           const std::string& archivo_entrada,
                           const std::string& patron,
                           size_t tamano_patron,
                           long tiempo_busqueda_ms,
                           size_t ocurrencias);

#endif // UTILS_HPP
