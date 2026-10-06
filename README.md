# Rogue Vita 🎮

![Status](https://img.shields.io/badge/status-in%20development-orange)

Rogue Vita project aims to create an isometric rogue-lite game using a custom engine!

<p>
  <img src="showcases/rogue-vita-small-gameplay.gif" width="404">
  <img src="showcases/rogue-vita-animation.gif" width="400">
</p>
<p align="center"><i>Player controller with animations and gameplay using custom engine and <code>dvl</code> framework</i></p>

## dvl 🛠️

The core framework, `dvl`, is used and developed alongside the engine. It separates the game from low-level systems by providing graphics, input, time, logging, tweening, mathematics, animation and tools for assets.

It enables cross-platform development and currently supports the `PlayStation Vita` via `VitaGL` and `PC` via `OpenGL`. It can be extended to support more platforms, `SDKs` or graphics `APIs`.

## Implemented features

- 🎨 3D rendering with textured meshes and toon lighting
- 🧩 Entity-component and hybrid ECS framework
- 🎮 Simple player controller with basic gameplay systems
- 📦 Custom asset cookers
- 🧮 Skeletal animation
- 💻 PC and PS Vita support
- 💥 Physics queries support

## Planned Next Features 🚀

- 🎮 Improved gameplay
- 🧭 NavMesh pathfinding
- ✨ Particle system
- 🪵 Physics collision detection
- 👓 UI framework
- 🎵 Audio framework

## Build & Run

Game assets are not included.

VitaSDK, VitaGL, Make, Assimp, GLFW, GLEW, and a C++17 compiler are required.

- Vita: `make run`
- Vita emulator: `make emul`
- Desktop (Linux): `make desktop`
- Asset cooking: `make cook`
- Test running: `make test`
