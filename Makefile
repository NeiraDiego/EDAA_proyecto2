# Makefile para compilar 
# Uso:
#   make FM           -> compila FM-index.cpp -> ejecutable FM
#   make SA           -> compila sa.cpp -> ejecutable SA
#   make all          -> compila los tres archivos

# Compilador
CXX = g++

# Flags generales
CXXFLAGS = -Wall -Wextra

# =========================
#  PROGRAMA 1: FM-INDEX
# =========================
FM_SRC = FM-index.cpp
FM_OUT = FM
FM_FLAGS = -std=c++11 -O3 -DNDEBUG -I ~/include -L ~/lib
FM_LIBS = -lsdsl -ldivsufsort -ldivsufsort64

# =========================
#  PROGRAMA 2: SA
# =========================
SA_SRC = sa.cpp
SA_OUT = SA
SA_FLAGS = -std=c++11 -O3 -DNDEBUG -I ~/include -L ~/lib
SA_LIBS = -lsdsl -ldivsufsort -ldivsufsort64

# =========================
#  REGLAS
# =========================

all: FM SA

# Compilar FM-index
FM: $(FM_SRC)
	$(CXX) $(CXXFLAGS) $(FM_FLAGS) $(FM_SRC) -o $(FM_OUT) $(FM_LIBS)

# Compilar SA
FM: $(SA_SRC)
	$(CXX) $(CXXFLAGS) $(SA_FLAGS) $(SA_SRC) -o $(SA_OUT) $(SA_LIBS)

# Limpiar
clean:
	rm -f $(FM_OUT)

