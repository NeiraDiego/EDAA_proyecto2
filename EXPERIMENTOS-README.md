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

Cada archivo de patrones debe contener un patrón por línea. Por ejemplo:

```
ATCG
GCTA
TACG
```

**Nota:** El script buscará cada patrón 30 veces en la última construcción de cada archivo.

## Uso

### Ejecución básica

```bash
./experimentos.sh
```

### Flujo de ejecución

1. El script verifica que existan los ejecutables y archivos necesarios
2. Pregunta si deseas eliminar los CSV existentes (para empezar desde cero)
3. Para cada archivo de entrada:
   - Ejecuta cada programa (FM, SA, SA-LCP):
     - 29 veces solo construcción (sin búsquedas)
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
- `exp-SA-LCP-construccion.csv` - Métricas de construcción del SA+LCP

### Búsquedas
- `exp-FM-busquedas.csv` - Métricas de búsqueda con FM-Index
- `exp-SA-busquedas.csv` - Métricas de búsqueda con Suffix Array
- `exp-SA-LCP-busquedas.csv` - Métricas de búsqueda con SA+LCP

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

## Contacto

Para preguntas o problemas con el script, consulta la documentación principal del proyecto en `CLAUDE.md`.
