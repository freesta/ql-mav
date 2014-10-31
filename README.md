# QL Mouse Accel Visualizer

I have created a small tool to visualize how your mouse acceleration settings go into effect in Quake Live. This program takes your custom settings as an input, generates a mouse acceleration graph and finally shows where exactly on the accel curve you would be if you moved your mouse at the current speed.

Why does this exist?
When I played around with accel settings I never noticed shit while playing. I tried to figure out how and when the accel kicks in by endlessly playing around but never got a good feel of it. Now I can just move my mouse around, look at the graph and see what's happening to my sensitivity. The practical use of it might be limited, but I found it interesting to see how this accel stuff works in detail, and maybe you do too.

## Settings

- sens: cl_sensitivity
- accel: cl_mouseAccel
- offset: cl_mouseAccelOffset
- cap: cl_mouseSensCap
- power: cl_mouseAccelPower
- m_cpi: m_cpi
- fps: the max frames per second (if I am not mistaken, the fps actually have a weird and probably negligible impact - on mouse accel, that's why this is in)
- xScale: just the scale/zoom of the graph on the x-axis, this is not a QL setting
- yScale: same for y

### Not implemented:
- m_yaw/m_pitch: Those are are hardcoded to 0.022. If you use other values the program still produces useable results, but the units are incorrect then. I just didn't want to have too many input fields.
- m_filter: This is hardcoded to 0. Not sure if implementing this is worth it in the scope of what the tool aims to do.

## Requirements

- For accurate results, this program requires you to use raw mouse input ("in_mouse 2") in QL, but you're probably doing that anyway.
- Your computer needs to be fast enough to run this. It should be but I am a bad programmer so it might require a high end PC, who knows.
- Tested only on Windows 7 so far. Ubuntu/Wine didn't work for me due to raw mouse input.

## Disclaimer

This program comes with no warranties. I am not responsible for any damage using the program might cause.
