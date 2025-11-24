# Makefile para compilar los tres métodos de búsqueda de patrones
# Uso:
#   make FM           -> compila FM-index.cpp -> ejecutable FM
#   make SA           -> compila sa.cpp -> ejecutable SA
#   make SA-LCP       -> compila sa-lcp.cpp -> ejecutable SA-LCP
#   make all          -> compila los tres programas
#   make clean        -> elimina todos los ejecutables y archivos objeto

# Compilador
CXX = g++

# Flags generales
CXXFLAGS = -Wall -Wextra -std=c++11 -O3 -DNDEBUG -I ~/include -L ~/lib

# Librerías SDSL
LIBS = -lsdsl -ldivsufsort -ldivsufsort64

# Archivos de utilidades compartidas
UTILS_SRC = utils.cpp
UTILS_OBJ = utils.o
UTILS_HDR = utils.hpp

# =========================
#  PROGRAMA 1: FM-INDEX
# =========================
FM_SRC = FM-index.cpp
FM_OUT = FM

# =========================
#  PROGRAMA 2: SA
# =========================
SA_SRC = sa.cpp
SA_OUT = SA

# =========================
#  PROGRAMA 3: SA-LCP
# =========================
SA_LCP_SRC = sa-lcp.cpp
SA_LCP_OUT = SA-LCP

# =========================
#  REGLAS
# =========================

all: FM SA SA-LCP

# Compilar archivo de utilidades (objeto reutilizable)
$(UTILS_OBJ): $(UTILS_SRC) $(UTILS_HDR)
	$(CXX) $(CXXFLAGS) -c $(UTILS_SRC) -o $(UTILS_OBJ)

# Compilar FM-index
FM: $(FM_SRC) $(UTILS_OBJ) $(UTILS_HDR)
	$(CXX) $(CXXFLAGS) $(FM_SRC) $(UTILS_OBJ) -o $(FM_OUT) $(LIBS)

# Compilar SA
SA: $(SA_SRC) $(UTILS_OBJ) $(UTILS_HDR)
	$(CXX) $(CXXFLAGS) $(SA_SRC) $(UTILS_OBJ) -o $(SA_OUT) $(LIBS)

# Compilar SA-LCP
SA-LCP: $(SA_LCP_SRC) $(UTILS_OBJ) $(UTILS_HDR)
	$(CXX) $(CXXFLAGS) $(SA_LCP_SRC) $(UTILS_OBJ) -o $(SA_LCP_OUT) $(LIBS)

# Limpiar ejecutables, archivos objeto y CSVs
clean:
	rm -f $(FM_OUT) $(SA_OUT) $(SA_LCP_OUT) $(UTILS_OBJ)

# Limpiar también los archivos CSV de experimentos
clean-csv:
	rm -f exp-*.csv

# Limpiar todo
clean-all: clean clean-csv

.PHONY: all clean clean-csv clean-all
