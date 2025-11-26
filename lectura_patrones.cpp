#include <iostream>
#include <fstream>
#include <vector>
#include <string>

void mostrarUso(const char* nombrePrograma) {
    std::cerr << "Uso: " << nombrePrograma << " <archivo_patrones.txt>" << std::endl;
}

int main(int argc, char* argv[]) {
    // 1. Validación de argumentos
    if (argc != 2) {
        mostrarUso(argv[0]);
        return 1;
    }

    std::string nombreArchivo = argv[1];

    // 2. Abrir archivo en modo BINARIO
    // Es CRUCIAL usar binary para que std::string::length() coincida 
    // exactamente con los bytes en disco (especialmente en Windows).
    std::ifstream archivo(nombreArchivo, std::ios::binary);

    if (!archivo.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo '" << nombreArchivo << "'" << std::endl;
        return 1;
    }

    std::cout << "--- Iniciando lectura de patrones ---" << std::endl;

    size_t largoBytes;
    int contador = 0;

    // 3. Bucle de lectura
    // El operador >> salta automáticamente espacios en blanco (incluyendo saltos de línea previos)
    // hasta encontrar el número, por lo que maneja bien el espaciado entre registros.
    while (archivo >> largoBytes) {
        
        // A. Consumir el salto de línea '\n' que está justo después del número.
        // Si no hacemos esto, el siguiente archivo.read() leería ese '\n' como parte del dato.
        archivo.ignore(); 

        // B. Preparar buffer de lectura
        std::vector<char> buffer(largoBytes);

        // C. Leer los bytes exactos
        if (archivo.read(buffer.data(), largoBytes)) {
            // Convertir a string para usarlo
            std::string patron(buffer.begin(), buffer.end());
            
            contador++;
            
            // D. Mostrar resultado (usamos delimitadores [] para ver claramente dónde empieza y termina)
            std::cout << "Patron #" << contador << " (Largo: " << largoBytes << " bytes):" << std::endl;
            std::cout << "CONTENIDO: [" << patron << "]" << std::endl;
            std::cout << "----------------------------------------" << std::endl;
        } else {
            std::cerr << "Error: Se esperaba leer " << largoBytes << " bytes pero el archivo termino antes." << std::endl;
            break;
        }
    }

    archivo.close();
    std::cout << "Lectura finalizada. Total patrones leidos: " << contador << std::endl;

    return 0;
}
