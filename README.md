## ed3d9bo
**E**xternal **D3D9** **b**ase **o**verlay

## Requirements
* Visual Studio >= 2015
* DirectX Software Development Kit
* Windows 7 or above
* A windowed target (must not be fullscreen!)

## Setup
If you have installed DirectX in its usual location, then `$(DXSDK_DIR)` should have been created.  
If not, you need to manually adjust the environment variable to point to your installation, or, alternatively, manually configure the library and include paths in project settings.

## Usage
Look at the example files. Some interesting notes (not mentioned in the examples):
* Use `SetOverlayDimensions(int iWidth, int iHeight)` to set the dimensions for the overlay. By default, the overlay occupies the target's whole width*height, i.e.:  
```cpp
g_pOverlay::SetOverlayDimensions(300, 200); // overlay is now 300x200px
```
* Uncomment `CALC_FPS` and use `::GetFPS()` to retrieve the framerate. You would call this in your render, of course.
* You can also use `SetSleep` to specify how much miliseconds to wait between each render. By default this is 17, because we try to render 60FPS, (1/60)*1000.