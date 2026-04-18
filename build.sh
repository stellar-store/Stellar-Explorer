#!/bin/bash
# Script para compilar CppExplorer (C++/Qt5) en Linux

set -e

APP="CppExplorer"
BUILD_DIR="build"

echo "🔍 Verificando dependencias del sistema..."

DEPS=(
    "cmake"
    "g++"
    "qtbase5-dev"
    "qttools5-dev"
    "libqt5widgets5"
)

for dep in "${DEPS[@]}"; do
    if ! dpkg -l 2>/dev/null | grep -q "^ii.*$dep"; then
        echo "📦 Instalando $dep..."
        sudo apt install -y "$dep"
    else
        echo "✅ $dep ya está instalado"
    fi
done

# Crear directorio de build
if [ ! -d "$BUILD_DIR" ]; then
    mkdir "$BUILD_DIR"
fi

cd "$BUILD_DIR"

echo "⚙️  Configurando con CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release

echo "🔨 Compilando..."
cmake --build . -- -j"$(nproc)"

cd ..

echo ""
echo "✅ Compilación exitosa."
echo "▶️  Para ejecutar:"
echo "    cd $BUILD_DIR && ./$APP"
echo ""
echo "💡 El binario necesita themes.css e icons/ en el mismo directorio."
echo "   Cópialos junto al binario si lo mueves de lugar."
