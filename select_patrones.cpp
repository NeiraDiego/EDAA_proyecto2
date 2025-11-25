#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <random>
#include <ctime>

// Función para leer el archivo completo en un string
std::string leerArchivo(const std::string& nombreArchivo) {
  std::string file_path = "/home/dataset/"+nombreArchivo;
  std::ifstream archivo(file_path);
  if (!archivo.is_open()) {
    std::cerr << "Error: No se pudo abrir el archivo " << file_path << std::endl;
    exit(EXIT_FAILURE);
  }

  std::stringstream buffer;
  buffer << archivo.rdbuf();
  return buffer.str();
}

// Función que busca un patrón y retorna la cantidad de ocurrencias
int contarOcurrencias(const std::string& texto, const std::string& patron) {
  if (patron.empty()) return 0;

  int contador = 0;
  size_t pos = texto.find(patron, 0); // Primera búsqueda

  while (pos != std::string::npos) {
    contador++;
    // Buscar la siguiente ocurrencia avanzando un caracter
    pos = texto.find(patron, pos + 1);
  }
  return contador;
}

int main() {
  std::string nombreArchivo;
  int occ; // Cantidad de patrones a buscar
  int m;   // Largo del patrón (en el prompt mencionaste "n", asumo que es el largo)

  // 1. Entrada de datos por parte del usuario
  std::cout << "--- Buscador de Patrones Aleatorios ---" << std::endl;
  std::cout << "Ingrese nombre del archivo de texto (ej: libro.txt): ";
  std::cin >> nombreArchivo;

  std::cout << "Ingrese la cantidad de patrones a buscar (occ): ";
  std::cin >> occ;

  std::cout << "Ingrese el largo de los patrones (m): ";
  std::cin >> m;

  // 2. Cargar el texto
  std::cout << "Leyendo archivo..." << std::endl;
  std::string texto = leerArchivo(nombreArchivo);
  long n = texto.length();

  if (n < m) {
    std::cerr << "Error: El archivo es mas corto que el largo del patron solicitado." << std::endl;
    return 1;
  }

  std::cout << "Archivo leido. Tamano del texto: " << n << " caracteres." << std::endl;
  std::cout << "----------------------------------------------------" << std::endl;

  std::ofstream archivoSalida("patrones_"+nombreArchivo+".txt");
  // 3. Configuración de aleatoriedad
  // Usamos mt19937 para mejor calidad aleatoria que rand()
  std::mt19937 rng(std::time(nullptr)); 
  // Distribución: indices válidos donde puede empezar un patrón de largo m
  std::uniform_int_distribution<long> dist(0, n - m);

  // 4. Bucle principal de generación y búsqueda
  for (int i = 0; i < occ; ++i) {
    // A. Seleccionar una posición aleatoria y extraer el patrón
    long indiceAleatorio = dist(rng);
    std::string patron = texto.substr(indiceAleatorio, m);

    // B. Buscar cuántas veces aparece ese patrón en el texto
    // Nota: Al menos aparecerá 1 vez (la que acabamos de extraer)
    int totalOcurrencias = contarOcurrencias(texto, patron);

    // C. Reportar
    archivoSalida << patron << ";" << totalOcurrencias << std::endl;
    //std::cout << "Patron " << (i + 1) << ": [" << patron << "] "
    //  << " -> Encontrado " << totalOcurrencias << " veces." << std::endl;
  }

  std::cout << "--- Fin del programa ---" << std::endl;

  return 0;
}
