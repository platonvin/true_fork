**C++ version of Lum is abandoned - project was [rewritten to Rust](https://github.com/platonvin/lum-rs). It might be revived later.**\
I also rewrote it to use modules

# Lum
**Lum** is a simple voxel\* renderer\*\* built with Vulkan

##### Some demo footage
https://github.com/user-attachments/assets/ce7883c4-a706-406f-875c-fbf23d68020d

#### Some (demo) benchmarks:
 * Intel Iris XE (integrated gpu!) - <7 mspf in FullHD
 * NVIDIA 1660s - ~1.6 mspf in FullHD 

## Building 

#### you will need following
  - Clang 20+ compiler   
  - CMake 4.0.2 or newer  
  - Ninja  
  - Vulkan support


```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -GNinja
cmake --build build
./build/demo
```

### Demo controls
- WASD for camera movement
- Arrows for robot movement
- Enter for shooting particles
- 0 to remove block underneath
- 1-9 and F1-F5 to place matching blocks (btw world is saved to a file)
- "<" and ">" to rotate camera
- "Page Up" and "Page Down" to zoom in/out
- Esc to close demo
