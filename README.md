# Estructuras de Datos para Indexación de Secuencias Biológicas

Este proyecto implementa y compara tres estructuras de datos fundamentales para búsqueda de patrones en textos, con aplicaciones en bioinformática. Todas las implementaciones utilizan la biblioteca SDSL-lite (Succinct Data Structure Library).

## 1. FM-Index (Índice de Full-text Minute-space)

El **FM-Index** es una estructura de datos comprimida basada en la transformada de Burrows-Wheeler (BWT) que permite realizar búsquedas de patrones sin necesidad de descomprimir el texto original. Implementado usando `csa_wt<wt_int<>>` de SDSL, combina un arreglo de sufijos comprimido (CSA) con un wavelet tree para lograr un balance óptimo entre espacio y tiempo de consulta.

La búsqueda en FM-Index utiliza el concepto de _backward search_ sobre la BWT, reduciendo iterativamente el rango de sufijos que coinciden con el patrón. La estructura incluye muestreo del arreglo de sufijos (cada 512 posiciones por defecto) y del arreglo inverso (cada 1024 posiciones), permitiendo recuperar las posiciones exactas de las ocurrencias. Esta técnica es particularmente efectiva en genomas donde existen muchas repeticiones, logrando compresión superior al texto original.

### Análisis de Complejidad

**Complejidad Temporal:**
- **Construcción:** O(n) donde n es el tamaño del texto
- **Búsqueda (count):** O(m) donde m es la longitud del patrón
- **Localización (locate):** O(occ × s) donde occ es el número de ocurrencias y s es la densidad de muestreo del SA

**Complejidad Espacial:**
- **Índice:** Entre 0.5x y 2x el tamaño del texto original (típicamente ~1x para DNA)
- La compresión efectiva depende de la entropía del texto y el tipo de wavelet tree usado

---

## 2. Suffix Array (Arreglo de Sufijos)

El **Suffix Array (SA)** es un arreglo que almacena las posiciones iniciales de todos los sufijos del texto ordenados lexicográficamente. El algoritmo de construcción utiliza libdivsufsort, una implementación optimizada del algoritmo de Yuta Mori que opera en tiempo lineal.

La búsqueda se realiza mediante búsqueda binaria sobre el SA, comparando el patrón con los sufijos ordenados. Una vez encontrado el rango de sufijos que comienzan con el patrón, se extraen directamente las posiciones del SA. A diferencia del FM-Index, esta estructura mantiene el SA sin comprimir, lo que permite búsquedas más rápidas a costa de mayor consumo de memoria.

### Análisis de Complejidad

**Complejidad Temporal:**
- **Construcción SA:** O(n) usando libdivsufsort (implementación lineal optimizada)
- **Búsqueda:** O(m log n + occ) donde m es la longitud del patrón y occ el número de ocurrencias
  - m log n: búsqueda binaria con comparación de m caracteres en cada paso
  - occ: recuperación de posiciones del SA

**Complejidad Espacial:**
- **Durante construcción:** 5-10x el tamaño del texto (memoria de trabajo de libdivsufsort)
- **Estructura final:** ~4x el tamaño del texto
  - SA: 4 bytes por carácter (arreglo de enteros de 32 bits)
  - Texto original en memoria: 1 byte por carácter

---

## 3. Suffix Array + LCP (SA con Longest Common Prefix Array)

Esta estructura extiende el **SA** añadiendo el **LCP Array (Longest Common Prefix)**, que almacena la longitud del prefijo común más largo entre sufijos consecutivos en el SA. El LCP array se construye usando el algoritmo de Kasai, que opera en tiempo lineal O(n), comparando los sufijos de manera eficiente aprovechando la información del SA.

El LCP array permite optimizar las búsquedas al evitar comparaciones redundantes de caracteres ya verificados. Durante la búsqueda binaria, se puede utilizar información del LCP para determinar cuántos caracteres ya coinciden con el patrón en cada paso, reduciendo el número de comparaciones. Aunque la implementación actual usa búsqueda binaria estándar, la presencia del LCP facilita futuras optimizaciones como el algoritmo de búsqueda acelerada.

### Análisis de Complejidad

**Complejidad Temporal:**
- **Construcción SA:** O(n) usando libdivsufsort
- **Construcción LCP:** O(n) usando el algoritmo de Kasai
- **Búsqueda:** O(m log n + occ) similar al SA básico
  - Potencial mejora a O(m + log n + occ) con algoritmos avanzados que aprovechan el LCP

**Complejidad Espacial:**
- **Durante construcción:** 5-10x el tamaño del texto (dominado por libdivsufsort)
- **Estructura final:** ~8x el tamaño del texto
  - SA: 4 bytes por carácter
  - LCP: 4 bytes por carácter (enteros de 32 bits)
  - Texto en memoria: 1 byte por carácter

---

## Tabla Comparativa

| Característica | FM-Index | SA | SA + LCP |
|----------------|----------|----------|----------------|
| **Construcción (Tiempo)** | O(n) | O(n) | O(n) SA + O(n) LCP |
| **Búsqueda (Tiempo)** | O(m) count<br>O(occ × s) locate | O(m log n + occ) | O(m log n + occ)<br>O(m + log n + occ)* |
| **Espacio en Memoria** | 0.5x - 2x texto<br>(~1x típico) | ~4x texto | ~8x texto |
| **Compresión** | ✅ Sí (CSA + WT) | ❌ No (SA sin comprimir) | ❌ No (SA + LCP sin comprimir) |
| **Velocidad de Búsqueda** | Rápida<br>(proporcional a m) | Muy rápida<br>(acceso directo a SA) | Muy rápida<br>(potencial de optimización) |
| **Uso de Memoria** | **Bajo** 🟢 | **Alto** 🔴 | **Muy Alto** 🔴 |
| **Escalabilidad** | **Excelente**<br>(genomas completos) | **Moderada**<br>(limitada por RAM) | **Limitada**<br>(textos pequeños/medianos) |
| **Caso de Uso Ideal** | Búsquedas en genomas grandes, patrones cortos | Búsquedas frecuentes, RAM abundante | Análisis LCP, aplicaciones algorítmicas |
| **Recuperación de Contexto** | Requiere muestreo | Directa desde SA | Directa desde SA |

\* Con algoritmos avanzados que aprovechan el LCP (no implementado en esta versión).
\*\*\* Estas características dependen del tamaño del texto y patrones específicos.

### Notas sobre la Comparación

- **FM-Index** es la estructura más eficiente en espacio y escalable para textos grandes (genomas, colecciones de documentos), ideal cuando la memoria es limitada.
- **SA** ofrece las búsquedas más directas a costa de mayor uso de memoria, apropiado para aplicaciones donde el rendimiento de búsqueda es crítico y hay RAM disponible.
- **SA + LCP** es principalmente educativo en esta implementación, pero el LCP array es fundamental para algoritmos avanzados como encontrar repeticiones maximales, calcular árboles de sufijos implícitos, y aplicaciones de ensamblaje de genomas.

### Recomendaciones de Uso

| Escenario | Método Recomendado | Justificación |
|-----------|-------------------|---------------|
| Genoma humano completo (3 GB) | **FM-Index** | Compresión esencial, memoria limitada |
| Servidor con 64+ GB RAM, búsquedas frecuentes | **SA** | Máximo rendimiento de búsqueda |
| Análisis de repeticiones/similitudes | **SA + LCP** | LCP necesario para algoritmos de análisis |
| Secuencias de DNA/proteínas (<100 MB) | **Cualquiera** | Todos son viables, FM-Index más versátil |
| Construcción una sola vez, millones de búsquedas | **SA** | Amortiza el costo de memoria |

---

## Compilación y Ejecución
### Compilación

```bash
make all          # Compila todos los programas
make FM           # Solo FM-Index
make SA           # Solo Suffix Array
make SA-LCP       # Solo SA + LCP
```

### Uso

```bash
# FM-Index
./FM <archivo_entrada>

# Suffix Array
./SA <archivo_entrada>

# Suffix Array + LCP
./SA-LCP <archivo_entrada>
```

Cada programa genera archivos CSV con métricas de rendimiento para análisis experimental.

---

## Referencias

- **FM-Index:** Ferragina, P., & Manzini, G. (2000). "Opportunistic data structures with applications"
- **Suffix Array:** Manber, U., & Myers, G. (1993). "Suffix arrays: A new method for on-line string searches"
- **LCP Array:** Kasai, T., et al. (2001). "Linear-Time Longest-Common-Prefix Computation"
- **SDSL-lite:** Gog, S., et al. (2014). "From Theory to Practice: Plug and Play with Succinct Data Structures"
