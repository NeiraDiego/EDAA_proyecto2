#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <random>
#include <ctime>
#include <sstream>

std::string sanitizarPatron(const std::string& entrada) {
    std::stringstream ss;
    for (char c : entrada) {
        if (c == '\n') {
            ss << "\\n"; // Reemplaza salto de linea por los caracteres \ y n
        } else if (c == '\r') {
            ss << "\\r"; // Importante si trabajas con archivos de Windows
        } else if (c == '\t') {
            ss << "\\t"; // Opcional: para tabulaciones
        } else {
            ss << c;
        }
    }
    return ss.str();
}

// Imprime ayuda si los argumentos no son correctos
void mostrarUso(const char* nombrePrograma) {
    std::cerr << "Uso: " << nombrePrograma << " <input_file> <output_file> <occ> <m>" << std::endl;
    std::cerr << "Ejemplo: " << nombrePrograma << " libro.txt resultados.txt 100 5" << std::endl;
}

std::string leerArchivo(const std::string& nombreArchivo) {
  std::string file_path = "/home/dataset/"+nombreArchivo;
    std::ifstream archivo(file_path);
    if (!archivo.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo de entrada '" << file_path << "'" << std::endl;
        exit(EXIT_FAILURE);
    }
    std::stringstream buffer;
    buffer << archivo.rdbuf();
    return buffer.str();
}

int contarOcurrencias(const std::string& texto, const std::string& patron) {
    if (patron.empty()) return 0;
    int contador = 0;
    size_t pos = texto.find(patron, 0);
    while (pos != std::string::npos) {
        contador++;
        pos = texto.find(patron, pos + 1);
    }
    return contador;
}

int main(int argc, char* argv[]) {
    // 1. Validar cantidad de argumentos
    // Se esperan 5: [0]programa, [1]input, [2]output, [3]occ, [4]m
    if (argc != 5) {
        mostrarUso(argv[0]);
        return 1;
    }

    // 2. Parsear argumentos
    std::string nombreArchivoEntrada = argv[1];
    std::string nombreArchivoSalida = argv[2];
    int occ = 0;
    int m = 0;

    try {
        occ = std::stoi(argv[3]);
        m = std::stoi(argv[4]);
    } catch (...) {
        std::cerr << "Error: 'occ' y 'm' deben ser numeros enteros validos." << std::endl;
        return 1;
    }

    if (occ <= 0 || m <= 0) {
        std::cerr << "Error: 'occ' y 'm' deben ser mayores a 0." << std::endl;
        return 1;
    }

    // 3. Lógica principal (Carga y búsqueda)
    std::cout << "[INFO] Leyendo archivo: " << nombreArchivoEntrada << "..." << std::endl;
    std::string texto = leerArchivo(nombreArchivoEntrada);
    long n = texto.length();
    std::cout << "[INFO] Leído archivo de " << n << " caracteres." << std::endl;

    if (n < m) {
        std::cerr << "Error: El archivo es mas corto que el largo del patron (m)." << std::endl;
        return 1;
    }

    std::ofstream archivoSalida(nombreArchivoSalida, std::ios::app);
    if (!archivoSalida.is_open()) {
        std::cerr << "Error: No se pudo crear el archivo de salida '" << nombreArchivoSalida << "'" << std::endl;
        return 1;
    }

    // Configuración aleatoria
    std::mt19937 rng(std::time(nullptr));
    std::uniform_int_distribution<long> dist(0, n - m);

    std::cout << "[INFO] Buscando " << occ << " patrones de largo " << m << "..." << std::endl;

    for (int i = 0; i < occ; ++i) {
        long indiceAleatorio = dist(rng);
        std::string patron = sanitizarPatron(texto.substr(indiceAleatorio, m));
        //int totalOcurrencias = contarOcurrencias(texto, patron);
        
        // Escribir al archivo
        //archivoSalida << patron << " ; " << totalOcurrencias << "\n";
        archivoSalida << patron << "\n";
    }

    archivoSalida.close();
    std::cout << "[EXITO] Resultados guardados en: " << nombreArchivoSalida << std::endl;

    return 0;
}
