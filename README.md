# visual_debugger

A simple visual debugger using OpenGL, ImGui and a shared memory connection, to receive and visualize data from a client.

## build
Use cmake
```
mkdir build
cd build
cmake ..
```

## run

First start the visual debugger executable, and leave it running. In a separate program, run the shared_memory_client to send data.

![](https://raw.githubusercontent.com/erwincoumans/visual_debugger/main/visual_debugger.png)
