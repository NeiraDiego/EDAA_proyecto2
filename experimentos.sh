#!/bin/bash

set -e  # Salir si hay errores

# Archivos de entrada (datasets)
ARCHIVOS=(
    "sources"
    "dna"
    "proteins"
    "genomic.fna"  # Comentado: archivo muy grande (3.2GB), causa std::bad_alloc con SA/SA-LCP
)

# Archivos de patrones correspondientes a cada dataset
PATRONES=(
    "patrones-sources"
    "patrones-dna"
    "patrones-proteins"
    "patrones-genomic"  # Comentado: corresponde a genomic.fna
)

# Programas a ejecutar
PROGRAMAS=("./FM" "./SA" "./SA-LCP")

# Número de veces que se construye la estructura
NUM_CONSTRUCCIONES=5

# Número de veces que se busca cada patrón
NUM_BUSQUEDAS_POR_PATRON=30

# ==============================================================================
# FUNCIONES AUXILIARES
# ==============================================================================

# Función para mostrar progreso
mostrar_progreso() {
    local programa=$1
    local archivo=$2
    local iteracion=$3
    local total=$4
    echo ""
    echo "=========================================="
    echo "Programa: $programa"
    echo "Archivo: $archivo"
    echo "Construcción: $iteracion/$total"
    echo "=========================================="
}

# Función para verificar que existen los archivos necesarios
verificar_archivos() {
    echo "Verificando que existen los archivos necesarios..."

    # Verificar ejecutables
    for prog in "${PROGRAMAS[@]}"; do
        if [ ! -x "$prog" ]; then
            echo "ERROR: El programa $prog no existe o no es ejecutable"
            echo "Por favor, ejecuta 'make all' primero"
            exit 1
        fi
    done

    # Verificar archivos de entrada
    for i in "${!ARCHIVOS[@]}"; do
        archivo="${ARCHIVOS[$i]}"
        patron="${PATRONES[$i]}"

        if [ ! -f "$archivo" ]; then
            echo "ADVERTENCIA: El archivo de entrada '$archivo' no existe"
        fi

        if [ ! -f "$patron" ]; then
            echo "ADVERTENCIA: El archivo de patrones '$patron' no existe"
        fi
    done

    echo "Verificación completada."
    echo ""
}

# Función para ejecutar construcción sin búsquedas
ejecutar_solo_construccion() {
    local programa=$1
    local archivo=$2

    # Simplemente envía "exit" para terminar después de la construcción
    echo "exit" | $programa "$archivo" > /dev/null
}

# Función para ejecutar construcción + búsquedas múltiples
ejecutar_con_busquedas() {
    local programa=$1
    local archivo=$2
    local archivo_patrones=$3

    if [ ! -f "$archivo_patrones" ]; then
        echo "ADVERTENCIA: Archivo de patrones '$archivo_patrones' no encontrado"
        echo "Ejecutando solo construcción..."
        ejecutar_solo_construccion "$programa" "$archivo"
        return
    fi

    # Leer todos los patrones del archivo
    mapfile -t patrones < "$archivo_patrones"

    # Crear entrada para el programa: cada patrón 30 veces, luego "exit"
    {
        for patron in "${patrones[@]}"; do
            # Ignorar líneas vacías
            if [ -n "$patron" ]; then
                for ((busqueda=1; busqueda<=NUM_BUSQUEDAS_POR_PATRON; busqueda++)); do
                    echo "$patron"
                done
            fi
        done
        echo "exit"
    } | $programa "$archivo" > /dev/null
}

# ==============================================================================
# SCRIPT PRINCIPAL
# ==============================================================================

echo "=========================================="
echo "  SCRIPT DE EXPERIMENTOS AUTOMATIZADO"
echo "=========================================="
echo ""
echo "Configuración:"
echo "  - Archivos de entrada: ${ARCHIVOS[*]}"
echo "  - Construcciones por archivo: $NUM_CONSTRUCCIONES"
echo "  - Búsquedas por patrón (última construcción): $NUM_BUSQUEDAS_POR_PATRON"
echo "  - Programas: ${PROGRAMAS[*]}"
echo ""

# Verificar que todo está listo
verificar_archivos

# Preguntar si desea limpiar los CSV existentes
#read -p "¿Desea eliminar los archivos CSV existentes para empezar desde cero? (s/n): " -n 1 -r
#echo ""
#if [[ $REPLY =~ ^[Ss]$ ]]; then
#    echo "Eliminando archivos CSV existentes..."
#    rm -f exp-*.csv
#    echo "Archivos CSV eliminados."
#fi

echo ""
echo "Iniciando experimentos..."
echo ""

# Hora de inicio
inicio=$(date +%s)

# Iterar sobre cada archivo de entrada
for i in "${!ARCHIVOS[@]}"; do
    archivo="${ARCHIVOS[$i]}"
    patron="${PATRONES[$i]}"

    # Verificar que el archivo existe
    if [ ! -f "$archivo" ]; then
        echo "ADVERTENCIA: Saltando '$archivo' porque no existe"
        continue
    fi

    echo ""
    echo "######################################################################"
    echo "  PROCESANDO ARCHIVO: $archivo"
    echo "######################################################################"
    echo ""

    # Iterar sobre cada programa
    for programa in "${PROGRAMAS[@]}"; do

        # Ejecutar 29 construcciones solo (sin búsquedas)
        for ((iter=1; iter<NUM_CONSTRUCCIONES; iter++)); do
            mostrar_progreso "$programa" "$archivo" "$iter" "$NUM_CONSTRUCCIONES"
            ejecutar_solo_construccion "$programa" "$archivo"
        done

        # Última construcción con búsquedas
        mostrar_progreso "$programa" "$archivo" "$NUM_CONSTRUCCIONES" "$NUM_CONSTRUCCIONES"
        echo ">>> Esta es la última construcción: ejecutando búsquedas de patrones <<<"
        ejecutar_con_busquedas "$programa" "$archivo" "$patron"

    done

    echo ""
    echo "✓ Completado el procesamiento de '$archivo'"
    echo ""
done

# Hora de fin
fin=$(date +%s)
duracion=$((fin - inicio))

echo ""
echo "=========================================="
echo "  EXPERIMENTOS COMPLETADOS"
echo "=========================================="
echo ""
echo "Tiempo total de ejecución: $duracion segundos"
echo ""
echo "Archivos CSV generados:"
ls -lh exp-*.csv 2>/dev/null || echo "  (No se generaron archivos CSV)"
echo ""
echo "Para analizar los resultados, revisa los archivos CSV:"
echo "  - exp-FM-construccion.csv / exp-FM-busquedas.csv"
echo "  - exp-SA-construccion.csv / exp-SA-busquedas.csv"
echo "  - exp-SA-LCP-construccion.csv / exp-SA-LCP-busquedas.csv"
echo ""
