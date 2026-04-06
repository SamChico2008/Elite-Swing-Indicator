# Changelog for Elite Swing Indicator

## v1.2.0 (Bug Fix & Stability Update)
- **Resolved CMake CI Error**: Explicitly provided $GEODE_SDK path for GitHub Actions.
- **Improved Field Access**: Refactored `main.cpp` using static helpers for safer memory management.
- **Color Compatibility**: Synchronized player color fetching with latest Geode SDK standards.
- **Performance**: Optimized indicator updates and setting refresh logic.

## v1.0.0 (Elite Release)
- **Initial Stable Release**: Finalized Elite Swing Indicator for Geometry Dash 2.2.
- **Elite Vector HUD**: High-performance vector rendering for gravity indicators.
- **Dual Support**: Independent indicators for both players in Dual mode.
- **Visual Effects**: Added gravity-flip shockwaves and velocity tracking bars.
- **Stability Fix**: Resolved critical null-pointer crashes during level exit/entry.
- **Build Optimized**: Modernized CMake build system with local environment fail-safes.
