# LENS

Videojuego de plataformas 2D estilo *Sonic*, escrito en C++ puro sobre WinAPI (sin motor externo). Proyecto educativo desarrollado como práctica de programación de sistemas / bajo nivel.

> ⚠️ **Proyecto fan-made / educativo.** Usa sprites y pistas de audio de *Sonic the Hedgehog* (SEGA) y otros juegos con fines de aprendizaje. No es un proyecto comercial, no está afiliado a SEGA ni a los titulares de esos derechos, y no debe distribuirse con fines comerciales. Ver [Créditos](#créditos-y-assets-de-terceros).

## Descripción

LENS es un plataformero 2D con scroll horizontal donde el jugador corre, salta, esquiva enemigos y pinchos, y recolecta anillos contra un contador de tiempo. Al terminar la partida se muestra el puntaje final y, si califica, se puede registrar el nombre en una tabla de los 3 mejores puntajes persistida en disco.

## Características técnicas

- **Renderizado por software**: framebuffer propio (`int*`) pintado a mano, sin GPU/DirectX — el blitting de sprites (escalado, espejado, transparencia por color clave) está implementado desde cero.
- **Físicas simples**: gravedad y salto con detección de colisión suelo/objeto vía `RECT`/`IntersectRect`.
- **Carga de bitmaps** vía GDI+.
- **Audio** con la librería de terceros [libzplay](http://libzplay.sourceforge.net/) para música y efectos.
- **Persistencia de puntajes** en un archivo de texto plano (`Puntajes.txt`), con top 3 ordenado y entrada de nombre estilo arcade (3 letras).
- Spawns de enemigos y anillos con posición y temporización semi-aleatorias.

## Controles (teclado)

| Tecla | Acción |
|---|---|
| `W` / `↑` | Saltar |
| `A` / `←` | Moverse a la izquierda |
| `D` / `→` | Moverse a la derecha |
| `Enter` | Avanzar/confirmar en menús (inicio, pantalla de derrota) |
| `Z` / `Space` | Confirmar letra al registrar nombre (pantalla de récord) |
| `F1` / `F2` | Pausar/reanudar música |

## Requisitos

- **Visual Studio 2022** con el workload *"Desarrollo para el escritorio con C++"*.
- Plataforma de compilación: **Win32 (x86)** — Debug o Release. No hay configuración x64.

## Cómo compilar y ejecutar

1. Abrir `Plantilla LENS 2023 EX.sln` en Visual Studio 2022.
2. Seleccionar configuración `Debug|Win32` o `Release|Win32`.
3. Compilar (F7) y ejecutar (F5).
4. `libzplay.dll` debe quedar en la misma carpeta que el `.exe` para que el audio funcione (ya se copia junto al build en `Debug/`).

## Estructura del proyecto

```
Plantilla LENS 2023 EX/
├── main.cpp                  # Lógica principal del juego (ventana, input, físicas, render, puntajes)
├── gdipload.h                # Carga de bitmaps vía GDI+
├── Librerias/
│   ├── Dibujar bitmaps/      # Helper de carga de imágenes
│   ├── Gamepad/               # Wrapper de XInput (soporte de control, no usado por defecto)
│   └── Musica/                # libzplay (dll, lib, headers)
├── Recursos/                  # Sprites (.bmp/.png) y audio (.mp3)
└── Puntajes.txt                # Tabla de puntajes (se genera/actualiza en tiempo de ejecución)
```

## Créditos y assets de terceros

- Sprites y/o pistas de audio pertenecen a sus respectivos creadores/franquicias originales (*Sonic the Hedgehog* — SEGA, entre otros) y se incluyen únicamente con fines educativos y de práctica de programación.
- Librería de audio: [libzplay](http://libzplay.sourceforge.net/).

## Licencia

El código propio de este repositorio se comparte con fines educativos. Los assets de terceros (sprites, audio) conservan los derechos de sus autores originales y **no** están cubiertos por ninguna licencia de este repositorio.
