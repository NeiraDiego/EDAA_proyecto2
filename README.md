# ALGORITMOS DE BUSQUEDA DE PATRONES CON MULTIPLES OCURRENCIAS EN TEXTOS

Este proyecto implementa y compara tres estructuras de datos para búsqueda de patrones en textos. Todas las implementaciones utilizan la biblioteca SDSL-lite (Succinct Data Structure Library).

## Compilación y Ejecución
### Compilación

```bash
make all          # Compila todos los programas
make FM           # Solo FM-Index
make SA           # Solo Suffix Array
make SA-LCP       # Solo SA + LCP
```

### Uso

Todos los programas soportan dos modos de operación:

#### Modo Interactivo
```bash
./FM <archivo_entrada>
./SA <archivo_entrada>
./SA-LCP <archivo_entrada>
```
En este modo, el programa solicita patrones por entrada estándar. Ingrese un patrón por línea y escriba `exit` para terminar.

#### Modo Batch (Archivos de Patrones)
```bash
./FM <archivo_entrada> <archivo_patrones> [repeticiones]
./SA <archivo_entrada> <archivo_patrones> [repeticiones]
./SA-LCP <archivo_entrada> <archivo_patrones> [repeticiones]
```
En este modo, el programa lee todos los patrones desde un archivo y ejecuta las búsquedas automáticamente.

**Parámetro de repeticiones (opcional):**
- Permite ejecutar cada búsqueda múltiples veces **sin reconstruir el índice**
- Ideal para obtener múltiples mediciones estadísticas
- Por defecto: 1 repetición
- Las estructuras se cargan una sola vez y se reutilizan para todas las repeticiones

**Formato de archivo de patrones:**
```
<largo_patron_1>
<bytes_del_patron_1>
<largo_patron_2>
<bytes_del_patron_2>
...
```

Cada patrón está precedido por una línea con su longitud en bytes, seguida de exactamente esa cantidad de bytes de contenido.

**Ejemplos:**
```bash
# Modo interactivo
./FM dna
# (ingresar patrones manualmente...)

# Modo batch - 1 búsqueda por patrón (por defecto)
./FM dna patrones-dna
./SA proteins patrones-proteins
./SA-LCP genomic.fna patrones-genomic

# Modo batch - 30 repeticiones por patrón
# Ventaja: construye el índice 1 sola vez, ejecuta 30 búsquedas por patrón
./FM dna patrones-dna 30
./SA proteins patrones-proteins 30
./SA-LCP genomic.fna patrones-genomic 30

# Ejemplo: 100 patrones × 30 repeticiones = 3000 búsquedas registradas
# Esto es mucho más eficiente que ejecutar el programa 30 veces
```

### Archivos CSV Generados

Cada programa genera archivos CSV con métricas de rendimiento:

**Construcción:**
- `exp-FM-construccion.csv` - Tiempo de construcción, tamaño original, tamaño del índice (en MB)
- `exp-SA-construccion.csv`
- `exp-SA-LCP-construccion.csv`

**Búsquedas:**
- `exp-FM-busquedas.csv` - Archivo, tamaño del patrón, **tiempo en nanosegundos (ns)**, ocurrencias
- `exp-SA-busquedas.csv`
- `exp-SA-LCP-busquedas.csv`

**Formato CSV de construcción:**
```csv
archivo_original,tamano_original_mb,tiempo_construccion_ms,tamano_estructura_mb
dna,385.216,109035,4478.13
dna,385.216,109007,4478.13
dna,385.216,109094,4478.13
dna,385.216,109140,4478.13
...
```

**Formato CSV de búsquedas:**
```csv
archivo_original,tamano_patron,tiempo_busqueda_ns,ocurrencias
dna,100,15234,42
dna,100,14987,42
dna,1000,89321,3
...
```

**Notas importantes:**
- Los tiempos de búsqueda se miden en **nanosegundos** para capturar con precisión operaciones muy rápidas
- El contenido del patrón **no se guarda** en el CSV (solo su tamaño) para reducir el tamaño de los archivos
- Cada repetición genera una línea independiente en el CSV

### Herramientas Auxiliares

#### Generación de Patrones

Para generar archivos de patrones automáticamente:

```bash
# Compilar select_patrones (si es necesario)
make select_patrones

# Extraer patrones de un archivo
./select_patrones <archivo_fuente> <archivo_salida> <cantidad> <tamaño>

# Ejemplo: extraer 100 patrones de tamaño 1000 desde dna
./select_patrones dna patrones-dna 100 1000

# Generar múltiples archivos de patrones automáticamente
chmod +x run_select_patrones.sh
./run_select_patrones.sh
```

El script `run_select_patrones.sh` genera patrones de tamaños variados (10, 100, 1000, 10000, 100000, 1000000 bytes) para todos los archivos de entrada.

#### Verificación de Patrones

Para verificar el contenido de un archivo de patrones:

```bash
# Compilar lectura_patrones
g++ -std=c++11 -O3 lectura_patrones.cpp -o lectura_patrones

# Verificar contenido
./lectura_patrones patrones-dna
```

Este programa muestra todos los patrones contenidos en el archivo, útil para depuración y verificación.

### Experimentos Automatizados

Para ejecutar experimentos se ejecuta el script `experimentos.sh`, editando según los experimentos que se deseen correr:

```bash
# Ejecutar suite completa de experimentos
./experimentos.sh
```
