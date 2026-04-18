# CppExplorer

Gestor de archivos para Linux en **C++/Qt5** — traducción fiel de PyExplorer (Python/PyQt5).

## Funcionalidades

Todas las funciones del original Python están presentes:

| Función | Descripción |
|---|---|
| Vista árbol + lista | Panel lateral de ubicaciones y vista central de archivos |
| Breadcrumbs | Barra de ruta navegable por botones |
| Pestañas | Múltiples carpetas abiertas al mismo tiempo |
| Atrás / Adelante / Arriba | Navegación con historial por pestaña |
| Nueva pestaña | Ctrl+T o botón en barra |
| Carpeta nueva | Crear carpeta en el directorio actual |
| Renombrar | F2 / botón barra |
| Eliminar a Papelera | Mueve a `~/.local/share/Trash` |
| Copiar / Cortar / Pegar | Ctrl+C / Ctrl+X / Ctrl+V con progreso |
| Refrescar | F5 |
| Búsqueda | Busca recursivamente en la carpeta actual |
| Menú contextual | Abrir, Abrir con…, Ejecutar en terminal, Mover a…, Propiedades |
| Papelera | Ver, restaurar y eliminar definitivamente |
| Dispositivos | Listar, montar y desmontar con pkexec/udisksctl |
| Red (GVFS) | Navegar recursos de red montados |
| Tamaño de iconos | Slider en la barra inferior |
| Modos de vista | Lista / Iconos / Iconos grandes |
| Temas | Claro / Oscuro / Sistema (con themes.css) |
| Color de acento | Selector de color en configuración |
| Persistencia | Geometría de ventana y preferencias con QSettings |
| Atajos de teclado | Ctrl+T/W/C/X/V, F5, Alt+←/→/↑, Ctrl+Q |

## Estructura del proyecto

```
CppExplorer/
├── CMakeLists.txt
├── build.sh
├── themes.css          ← mismo archivo que el proyecto Python
├── icons/              ← mismos iconos que el proyecto Python
└── src/
    ├── main.cpp
    ├── mainwindow.h / .cpp     ← QMainWindow con pestañas y barra de herramientas
    ├── filetab.h / .cpp        ← Widget de pestaña (árbol + vista + breadcrumbs)
    ├── breadcrumbs.h / .cpp    ← Widget de breadcrumbs
    ├── fileopworker.h / .cpp   ← QThread para copiar/mover con progreso
    ├── configdialog.h / .cpp   ← Diálogo de configuración (3 pestañas)
    └── utils.h / .cpp          ← humanSize, openWithDefault, openTerminalHere
```

## Equivalencia Python → C++

| Python (PyQt5) | C++ (Qt5) |
|---|---|
| `class FileOpWorker(QThread)` | `class FileOpWorker : public QThread` |
| `pyqtSignal` | `signals:` |
| `@pyqtSlot` / `connect` | `connect(...)` |
| `Path` (pathlib) | `QString` + `QDir` + `QFileInfo` |
| `shutil.copy2` / `shutil.move` | `QFile::copy` / `QFile::rename` |
| `os.walk` | `QDirIterator` |
| `send2trash` | Implementado manualmente con `.trashinfo` |
| `QFileSystemModel` | `QFileSystemModel` (mismo nombre) |
| `QSettings` | `QSettings` (mismo nombre) |
| `subprocess.Popen` | `QProcess::startDetached` |
| `json.loads` | `QJsonDocument::fromJson` |

## Compilar y ejecutar

```bash
chmod +x build.sh
./build.sh
cd build
./CppExplorer
```

### Requisitos

- CMake ≥ 3.16
- GCC/Clang con C++17
- Qt5 (qtbase5-dev)

```bash
sudo apt install cmake g++ qtbase5-dev
```

## Iconos

Coloca los mismos iconos del proyecto Python en la carpeta `icons/` junto al binario:

```
icons/
  icon.png          ← icono de la app
  atras.png
  adelante.png
  arriba.png
  new_tab.png
  new_folde.png
  escribir.png
  delete.png
  copiar.png
  cortar.png
  pegar.png
  refrescar.png
  configuracion.png
  about.png
```

Si los iconos no existen, Qt usa los iconos del tema del sistema (`QIcon::fromTheme`).
