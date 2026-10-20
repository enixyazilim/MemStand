<div align="center">

# ⚡ MemStand

### Temporizador de sistema de alta precisión y optimizador inteligente de memoria para Windows

<p align="center">
  <a href="README.md">English</a> • <a href="README.tr.md">Türkçe</a> • <a href="README.fr.md">Français</a> • <a href="README.de.md">Deutsch</a> • <b>Español</b> • <a href="README.it.md">Italiano</a> • <a href="README.pt-BR.md">Português (Brasil)</a> • <a href="README.ru.md">Русский</a>
</p>

[![Release](https://img.shields.io/github/v/release/enixyazilim/MemStand?color=3b82f6&logo=github&style=flat-square)](https://github.com/enixyazilim/MemStand/releases)
[![Build Status](https://img.shields.io/github/actions/workflow/status/enixyazilim/MemStand/ci.yml?branch=main&label=CI&logo=githubactions&logoColor=white&style=flat-square)](https://github.com/enixyazilim/MemStand/actions)
[![Language](https://img.shields.io/badge/C%2B%2B-20%20(ISO)-00599C?logo=c%2B%2B&style=flat-square)](https://isocpp.org/)
[![Platform](https://img.shields.io/badge/Platform-Windows%2010%20%7C%2011%20(x64)-0078D4?logo=windows&style=flat-square)](https://microsoft.com/windows)
[![Dependencies](https://img.shields.io/badge/Dependencies-Zero%20(Pure%20Win32)-10b981?style=flat-square)](#-especificaciones-técnicas)
[![Binary Size](https://img.shields.io/badge/Binary%20Size-%3C%20500%20KB-success?style=flat-square)](#-especificaciones-técnicas)
[![RAM Footprint](https://img.shields.io/badge/RAM%20Usage-~1.9%20MB-success?style=flat-square)](#-especificaciones-técnicas)
[![License](https://img.shields.io/badge/License-MIT-8b5cf6?style=flat-square)](LICENSE)

<br/>

**MemStand** es un optimizador de sistema para Windows moderno, ultraligero e independiente, diseñado en **C++20** puro por **[Enix Yazılım](https://github.com/enixyazilim)**. Bloquea la resolución global del temporizador del kernel de Windows a **0,50 ms** para eliminar la latencia de entrada del ratón, los microtirones y las variaciones en el tiempo de fotograma en el juego competitivo, ofreciendo al mismo tiempo una gestión de memoria automatizada y adaptada al hardware.

Construido bajo una estricta filosofía de **cero dependencias externas**, MemStand no requiere el entorno de ejecución .NET, ni WebView2, ni Electron, ni servicios en segundo plano. Funciona silenciosamente en la bandeja del sistema con un conjunto de trabajo en reposo de solo **~1,9 MB** y un uso de CPU del **0,00 %**.

</div>

---

## ⚡ Características principales

| Función | Descripción |
| :--- | :--- |
| **[▶] Interruptor principal** | Inicio/parada instantáneo del motor en segundo plano con un solo clic |
| **[⚡] Temporizador de 0,50 ms** | Bloqueo de resolución del kernel por debajo del milisegundo + guardián |
| **[🧠] Perfilado automático** | Calcula dinámicamente los umbrales óptimos a partir de la RAM física |
| **[🎯] Purga de la reserva** | Purga segura de la caché de la lista de reserva sin congelar los juegos |
| **[📊] Bandeja del sistema dinámica** | Porcentaje de RAM en tiempo real codificado por colores en la barra de tareas |
| **[🛡️] Inicio sin UAC** | Inicio de sesión administrativo silencioso a través de la API del Programador de tareas |
| **[🌐] Actualizador nativo** | Comprueba las versiones de GitHub directamente mediante WinHTTP |
| **[🌍] i18n modular** | Traducciones de la comunidad intercambiables en caliente (basadas en JSON) |

---

## 🥊 ¿Por qué MemStand? (Comparación con herramientas tradicionales)

| Función | Herramientas tradicionales (ej. ISLC) | Suites OEM (Razer, etc.) | **MemStand (Enix Yazılım)** |
| :--- | :---: | :---: | :---: |
| **Lenguaje de programación** | C# / .NET Framework | Electron / Chromium | **C++20 puro (estándar ISO)** |
| **Dependencias externas** | Runtime .NET 4.x/6.x | Node.js, Chromium, servicios | **Ninguna (Win32 nativo y API NT)** |
| **Tamaño del ejecutable** | ~3 - 10 MB | 150 MB - 500 MB+ | **< 500 KB** |
| **Memoria en reposo** | 40 MB - 80 MB | 200 MB - 800 MB | **~1,9 MB** |
| **Umbrales adaptados al hardware** | ❌ Valores estáticos fijos | ❌ Limpiezas agresivas a ciegas | **✅ Perfil dinámico (RAM / 2)** |
| **Guardián del temporizador** | ⚠️ Sondeo básico | ❌ Ninguno | **✅ Guardián que reafirma la resolución del kernel** |
| **Inicio elevado sin UAC** | ⚠️ Creación de tareas incoherente | ❌ Inicia un servicio pesado | **✅ Integración limpia con el Programador de tareas** |
| **Modo oscuro nativo moderno** | ❌ WinForms estilo años 90 | ⚠️ Renderizado web / Pesado | **✅ Win32 Deep Slate dibujado a mano** |
| **Comprobador de actualizaciones integrado** | ❌ Búsqueda manual en la web | ⚠️ Actualizador automático en segundo plano | **✅ Comprobador de versiones nativo mediante WinHTTP** |

---

## 🚀 Análisis en detalle: funcionalidades clave

### 1. ⚡ Perfilado de memoria adaptado al hardware
A diferencia de las herramientas tradicionales que requieren que el usuario adivine valores arbitrarios en megabytes, MemStand analiza la RAM física instalada al iniciar y calcula los umbrales óptimos:
- **Umbral de reserva (MB):** Establecido automáticamente en el 50 % de la memoria instalada (`RAM / 2`). Esto preserva la caché útil del sistema de archivos de Windows durante el funcionamiento normal, al tiempo que garantiza que las páginas de reserva infladas nunca dejen sin memoria a los juegos en ejecución.
- **Margen de seguridad para la RAM libre:** Se adapta dinámicamente según la gama de hardware (8 GB, 16 GB, 24 GB, 32 GB, 64 GB+) para evitar el agotamiento de la memoria.
- **Botón `⚡ Calcular automáticamente`:** Restaura o recalcula en cualquier momento los parámetros recomendados para tu hardware, con un solo clic.

### 2. 🎮 Temporizador de kernel submilisegundo a 0,50 ms + guardián
- Invoca las rutinas nativas del kernel NT (`NtSetTimerResolution`) junto con períodos de temporizador multimedia, reduciendo la resolución de los 15,6 ms o 1,00 ms estándar a **0,50 ms** (5.000 unidades con precisión de 100 ns).
- Reduce significativamente la variación del tiempo de fotograma, minimiza la inestabilidad del sondeo del ratón y mejora la capacidad de respuesta en el registro de impactos en títulos competitivos (*Valorant, CS2, Apex Legends, Warzone, Fortnite*).
- **Guardián activo:** Reafirma continuamente la resolución en segundo plano para evitar que Windows o el software en segundo plano la restablezcan de forma silenciosa al valor predeterminado. Objetivos personalizables (0,50 ms, 0,75 ms, 1,00 ms, 15,62 ms).

### 3. 🧠 Limpieza de memoria adaptativa multinivel
- **Nivel 1 (carga normal / en juego):** Solo se purga de forma segura la lista de reserva. Los conjuntos de trabajo de los procesos activos se mantienen en caché, lo que garantiza cero microtirones o caídas de fotogramas.
- **Nivel 2 (alta presión de memoria):** Cuando la RAM total del sistema supera el umbral configurado (predeterminado: **85 %**), los conjuntos de trabajo y la caché del sistema se optimizan de forma gradual para evitar bloqueos por falta de memoria.

### 4. 📊 Icono dinámico en la bandeja del sistema
- Muestra el porcentaje de uso de RAM en tiempo real directamente sobre el icono de notificación de la barra de tareas, con doble búfer GDI personalizado.
- Codificación de colores a simple vista:
  - 🟢 **Verde esmeralda:** Carga normal (< 70 %)
  - 🟡 **Naranja ámbar:** Carga moderada (70 % - 85 %)
  - 🔴 **Rojo carmesí:** Alta presión (> 85 %)
- El menú contextual de la bandeja permite realizar purgas manuales con un clic, alternar el temporizador y comprobar actualizaciones sin abrir la ventana principal.

### 5. 🛡️ Inicio elevado sin UAC
- Registra una tarea programada nativa de Windows (`schtasks.exe` con nivel de ejecución `HIGHEST`).
- Se inicia automáticamente al iniciar sesión con privilegios de administrador, eliminando por completo las solicitudes de UAC de Windows al arrancar.

### 6. 🔄 Comprobador nativo de versiones y actualizaciones
- Un comprobador de actualizaciones ligero e integrado consulta la API de GitHub Releases mediante `winhttp.dll`.
- Comprueba las etiquetas de versión de forma asíncrona, sin bloquear la interfaz de usuario ni requerir bibliotecas de terceros.
- Acceso con un clic a la descarga de nuevas versiones y a la consulta de las notas de cambios.

---

## 📥 Descarga e instalación

MemStand es **100 % portátil**: no requiere instalador ni modificaciones en el registro.

1. Descarga el archivo `MemStand-vX.X.X-windows-x64.zip` más reciente desde [GitHub Releases](https://github.com/enixyazilim/MemStand/releases/latest).
2. Extrae el archivo en la carpeta que prefieras (por ejemplo, `C:\Tools\MemStand` o en el `Escritorio`).
3. Ejecuta `MemStand.exe`.
4. *(Opcional)* En la pestaña **Ajustes**, marca **Iniciar con Windows (sin UAC)** e **Iniciar minimizado en la bandeja** para un funcionamiento silencioso en segundo plano.

### Verificación (SHA-256)
Cada versión incluye un archivo `checksums.txt`. Puedes verificar la integridad de la descarga con PowerShell:
```powershell
Get-FileHash -Path MemStand.exe -Algorithm SHA256
```

---

## 💻 Interfaz de línea de comandos (CLI)

MemStand admite parámetros de línea de comandos para scripts, lanzadores de juegos y procesos por lotes automatizados:

| Argumento | Descripción |
| :--- | :--- |
| `MemStand.exe -minimized` | Se inicia silenciosamente en la bandeja del sistema sin mostrar la ventana. |
| `MemStand.exe -purge` | Purga inmediatamente la lista de reserva y se cierra silenciosamente. |
| `MemStand.exe -clean` | Ejecuta una limpieza completa de la memoria (reserva, conjuntos de trabajo, caché) y se cierra silenciosamente. |

---

## ⌨️ Atajo global

- **`Ctrl + Shift + M`**: Activa una purga inmediata de la memoria desde cualquier juego o aplicación a pantalla completa, sin pasar al escritorio. Se puede activar y desactivar en la pestaña Ajustes.

### 🎹 Atajo reasignable

No estás obligado a usar `Ctrl + Shift + M`. Marca **"Atajo global de limpieza"** en la pestaña Ajustes y haz clic en el botón situado al lado: el grabador se activa (el borde cambia a ámbar y la etiqueta pasa a *Grabando...*); luego presiona la combinación que prefieras.

- Pulsa **`Esc`** durante la grabación para cancelar, o **`Supr`** para restaurar el atajo integrado `Ctrl + Shift + M`.
- Se requiere al menos uno entre `Ctrl`, `Alt`, `Mayús` o `Win`, de modo que la aplicación nunca capture una sola tecla suelta.
- La selección solo se aplica cuando pulsas **Guardar ajustes** y se conserva tras reiniciar.
- Si otra aplicación ya utiliza esa combinación, MemStand te avisará en lugar de fallar en silencio, permitiéndote elegir otra distinta.
- La repetición automática de teclas está desactivada, por lo que mantener pulsada la combinación no acumulará purgas reiteradas en cola.

---

## 🌐 Traducciones de la comunidad (i18n)

MemStand se distribuye con **8 idiomas de interfaz** — English, Türkçe, Deutsch, Español, Français, Italiano, Português (Brasil) y Русский — y admite un número ilimitado de traducciones de la comunidad mediante archivos JSON modulares.

La lista está ordenada alfabéticamente según el nombre nativo del idioma, y cualquier archivo en la carpeta `languages/` se detecta automáticamente al iniciar la aplicación.

1. Copia `languages/template.json` a `languages/<tu-idioma>.json` (por ejemplo, `languages/french.json`).
2. Traduce los valores en el bloque `"strings"`. Mantén las etiquetas cortas: la pantalla de ajustes utiliza columnas de ancho fijo, por lo que una etiqueta demasiado larga se solapará con el campo contiguo.
3. Reinicia MemStand: tu idioma aparecerá de inmediato en el menú **Idioma**.
4. ¡Abre una Pull Request para compartir tu traducción con la comunidad!

---

## 📊 Especificaciones técnicas

| Parámetro | Especificación |
| :--- | :--- |
| **Lenguaje y estándar** | C++ puro (ISO C++20) |
| **Arquitectura** | Nativa de 64 bits (`x86_64`) |
| **APIs utilizadas** | API Win32, API nativa NT (`ntdll.dll`), WinHTTP (`winhttp.dll`) |
| **Dependencias externas** | **Ninguna** (cero dependencias en tiempo de ejecución, binario independiente) |
| **Tamaño del binario** | **~500 KB** (`MemStand.exe`) |
| **Consumo de memoria** | **~1,9 MB** de conjunto de trabajo en reposo |
| **Uso de CPU** | **0,00 %** entre dos ciclos de sondeo de un segundo |
| **Sistemas operativos compatibles** | Windows 10 (todas las ediciones desde 1809), Windows 11 (todas las ediciones) |

---

## 🛠️ Compilar desde el código fuente

### Requisitos previos
- Windows 10 o Windows 11 (x64)
- Visual Studio 2022 o Visual Studio 2026 con la carga de trabajo *«Desarrollo para el escritorio con C++»*
- CMake 3.20 o posterior (opcional, si se compila con CMake)

### Método 1: Script de compilación con un solo clic
Ejecuta el script de compilación en la carpeta raíz:
```cmd
build.bat
```
El ejecutable independiente compilado se genera en `bin\MemStand.exe`.

### Método 2: CMake
```cmd
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

---

## 🤝 Contribuir

¡Las contribuciones, informes de errores y sugerencias son bienvenidos! Consulta [CONTRIBUTING.md](CONTRIBUTING.md) para conocer las directrices detalladas sobre el estilo de código, convenciones de arquitectura y el flujo de trabajo de las Pull Requests.

---

## 📄 Licencia

Este proyecto está distribuido bajo la licencia [MIT](LICENSE).  
Copyright © 2026 **[Enix Yazılım](https://github.com/enixyazilim)** y los colaboradores de MemStand.
