# PanosUI
The goal of this project is to create a basis for future Linux desktop development pertaining to Wayland. It should be modular enough to use any part of it alone.

# TODO:
1. Connect to a Wayland display to allocate buffers and subscribe to events
2. Create a scene graph with Skia
3. Get Skia rendering to the Wayland buffer
4. Implement event traversals on scene graph
5. Integrate layer shell protocol

# Planned library usage
Skia, libwayland, and wlr-layer-shell-unstable-v1-client.
