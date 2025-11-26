# Guía de Experimentos Automatizados

Este documento explica cómo usar el script `experimentos.sh` para realizar experimentos automatizados con los tres programas de indexación de cadenas.

## Descripción

El script `experimentos.sh` automatiza la ejecución de experimentos sobre múltiples archivos de entrada, ejecutando:

1. **30 construcciones** de cada estructura (FM-Index, SA, SA-LCP) para cada archivo
2. En la **última construcción** de cada archivo: **30 búsquedas** de cada patrón definido

Todos los resultados se acumulan en los archivos CSV correspondientes para análisis posterior.

## Requisitos Previos

1. **Compilar los programas:**
   ```bash
   make all
   ```

2. **Preparar archivos de entrada:**
   - `sources` - Archivo de texto fuente
   - `dna` - Secuencia de ADN
   - `proteins` - Secuencia de proteínas
   - `genomic.fna` - Archivo genómico humano

3. **Preparar archivos de patrones:**
   - `patrones-sources` - Patrones para buscar en `sources`
   - `patrones-dna` - Patrones para buscar en `dna`
   - `patrones-proteins` - Patrones para buscar en `proteins`
   - `patrones-genomic` - Patrones para buscar en `genomic.fna`

## Formato de Archivos de Patrones

Los archivos de patrones usan un formato binario especial:

```
<largo_patron_1>
<bytes_del_patron_1>
<largo_patron_2>
<bytes_del_patron_2>
...
```

Donde:
- Cada patrón comienza con una línea que contiene el largo del patrón (número entero)
- Le siguen exactamente ese número de bytes con el contenido del patrón
- Los patrones pueden contener saltos de línea y caracteres especiales
- Este formato permite búsqueda de patrones arbitrarios incluyendo secuencias binarias

**Ejemplo visual:**
```
10
ATCGATCGAT
5
GCTAT
15
TACCG
AAGGCCTT
```

### Generación de Patrones

Para generar archivos de patrones, usa el script `run_select_patrones.sh`:

```bash
chmod +x run_select_patrones.sh
./run_select_patrones.sh
```

Este script usa el programa `select_patrones` para extraer patrones aleatorios de diferentes tamaños desde los archivos de entrada:

```bash
./select_patrones <archivo_fuente> <archivo_salida> <cantidad> <tamaño>
```

Por ejemplo:
```bash
# Extraer 100 patrones de tamaño 1000 desde dna y guardarlos en patrones-dna
./select_patrones dna patrones-dna 100 1000
```

**Nota:** El script buscará cada patrón 30 veces en la última construcción de cada archivo.

## Uso

### Uso de los programas individuales

Cada programa (FM, SA, SA-LCP) acepta dos modos de operación:

**Modo interactivo (sin archivo de patrones):**
```bash
./FM <archivo_entrada>
./SA <archivo_entrada>
./SA-LCP <archivo_entrada>
```
En este modo, el programa solicita patrones por entrada estándar hasta que se ingresa "exit".

**Modo batch (con archivo de patrones):**
```bash
./FM <archivo_entrada> <archivo_patrones>
./SA <archivo_entrada> <archivo_patrones>
./SA-LCP <archivo_entrada> <archivo_patrones>
```
En este modo, el programa lee todos los patrones desde el archivo y ejecuta las búsquedas automáticamente sin output verboso.

**Ejemplo:**
```bash
# Construir índice y buscar patrones desde archivo
./FM dna patrones-dna

# Modo interactivo
./FM dna
# Luego ingresar patrones manualmente...
```

### Ejecución básica del script de experimentos

```bash
./experimentos.sh
```

### Flujo de ejecución

1. El script verifica que existan los ejecutables y archivos necesarios
2. Pregunta si deseas eliminar los CSV existentes (para empezar desde cero)
3. Para cada archivo de entrada:
   - Ejecuta cada programa (FM, SA, SA-LCP):
     - 4 veces solo construcción (sin búsquedas)
     - 1 vez construcción + 30 búsquedas por patrón
4. Muestra el tiempo total de ejecución

### Ejemplo de salida

```
==========================================
  SCRIPT DE EXPERIMENTOS AUTOMATIZADO
==========================================

Configuración:
  - Archivos de entrada: sources dna proteins genomic.fna
  - Construcciones por archivo: 30
  - Búsquedas por patrón (última construcción): 30
  - Programas: ./FM ./SA ./SA-LCP

¿Desea eliminar los archivos CSV existentes? (s/n): s

######################################################################
  PROCESANDO ARCHIVO: sources
######################################################################

==========================================
Programa: ./FM
Archivo: sources
Construcción: 1/30
==========================================
...
```

## Archivos CSV Generados

El script genera 6 archivos CSV con los resultados:

### Construcción
- `exp-FM-construccion.csv` - Métricas de construcción del FM-Index
- `exp-SA-construccion.csv` - Métricas de construcción del Suffix Array
- `exp-SA-LCP-construccion.csv` - Métricas de construcción del SA + LCP

### Búsquedas
- `exp-FM-busquedas.csv` - Métricas de búsqueda con FM-Index
- `exp-SA-busquedas.csv` - Métricas de búsqueda con Suffix Array
- `exp-SA-LCP-busquedas.csv` - Métricas de búsqueda con SA + LCP

## Personalización

Puedes modificar las siguientes variables en el script:

```bash
# Número de construcciones por archivo
NUM_CONSTRUCCIONES=30

# Número de búsquedas por patrón
NUM_BUSQUEDAS_POR_PATRON=30

# Archivos de entrada
ARCHIVOS=("sources" "dna" "proteins" "genomic.fna")

# Archivos de patrones
PATRONES=("patrones-sources" "patrones-dna" "patrones-proteins" "patrones-genomic")
```

## Análisis de Resultados

Después de ejecutar el script, puedes analizar los CSV para comparar:

1. **Tiempo de construcción** entre las tres estructuras
2. **Uso de espacio** (tamaño de las estructuras)
3. **Tiempo de búsqueda** entre las tres estructuras
4. **Variabilidad** entre las 30 ejecuciones

Ejemplo de análisis con herramientas de línea de comandos:

```bash
# Ver estadísticas de construcción del FM-Index
column -t -s, exp-FM-construccion.csv | head

# Contar búsquedas realizadas
wc -l exp-*-busquedas.csv

# Calcular promedio de tiempos de construcción (requiere herramientas adicionales)
# Puedes usar Python, R, o Excel para análisis estadístico
```

## Consejos

1. **Memoria:** Los experimentos con SA y SA-LCP pueden requerir mucha memoria para archivos grandes
2. **Tiempo:** El script puede tardar varias horas dependiendo del tamaño de los archivos
3. **Espacio en disco:** Asegúrate de tener suficiente espacio para los CSV acumulados
4. **Interrupción:** Puedes detener el script con Ctrl+C. Los datos en CSV ya escritos se preservan

## Resolución de Problemas

### Error: "El programa ./FM no existe o no es ejecutable"
```bash
make all
chmod +x experimentos.sh
```

### Error: "Archivo de patrones no encontrado"
Crea los archivos de patrones correspondientes con un patrón por línea.

### Los CSV están vacíos
Verifica que los archivos de entrada existan y no estén vacíos.

## Ejemplo Completo

```bash
# 1. Compilar programas
make all

# 2. Crear archivo de entrada pequeño para prueba
echo "mississippi" > test.txt

# 3. Crear archivo de patrones
cat > patrones-test.txt << EOF
issi
ippi
ssi
EOF

# 4. Modificar el script temporalmente para usar test.txt
# (o crear una versión de prueba del script)

# 5. Ejecutar experimentos
./experimentos.sh

# 6. Verificar resultados
cat exp-FM-construccion.csv
cat exp-FM-busquedas.csv
```

## Herramientas Adicionales

### lectura_patrones - Verificar archivos de patrones

El programa `lectura_patrones` permite verificar el contenido de los archivos de patrones:

```bash
# Compilar (si es necesario)
g++ -std=c++11 -O3 lectura_patrones.cpp -o lectura_patrones

# Usar
./lectura_patrones <archivo_patrones>
```

Este programa lee y muestra todos los patrones contenidos en un archivo, útil para:
- Verificar que los patrones se generaron correctamente
- Depurar problemas con el formato de archivos
- Inspeccionar el contenido antes de ejecutar experimentos

**Ejemplo de salida:**
```
--- Iniciando lectura de patrones ---
Patron #1 (Largo: 10 bytes):
CONTENIDO: [->managed_]
----------------------------------------
Patron #2 (Largo: 10 bytes):
CONTENIDO: [hat you al]
----------------------------------------
Lectura finalizada. Total patrones leidos: 2
```

## Contacto

Para preguntas o problemas con el script, consulta la documentación principal del proyecto en `CLAUDE.md`.
