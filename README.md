# QL Mouse Accel Visualizer

QL Mouse Accel Visualizer is a small tool to visualize how your mouse acceleration settings go into effect in Quake Live. This program takes your custom settings as an input, generates a mouse acceleration graph and finally shows where exactly on the accel curve you would be if you moved your mouse at the current speed. It also has a convert feature which translates your settings between the old sensitivity system (/m_cpi "0") and the new sensitivity system (/m_cpi "xyz"). 

## Why does this exist?
When I played around with accel settings I never noticed shit while playing. I tried to figure out how and when the accel kicks in by endlessly playing around but never got a good feel of it. Now I can just move my mouse around, look at the graph and see what's happening to my sensitivity. The practical use of it might be limited, but I found it interesting to see how this accel stuff works in detail, and maybe you do too.

## How to use the visualizer

- Enter the Quake Live CVARs into the according fields and move your mouse around.
- The x-axis correlates to the mouse speed.
- The y-axis correlates to the multiplier, which is the sensitivity after acceleration was applied. 

## How to use the convert feature

- Case 1: You are not using the m_cpi system and want to start using it. For that, enter your QL settings into the according edit boxes, click convert, enter your mouse cpi (you need to know that value) in the dialog box and hit enter. Now the edit boxes contain the new settings. If you use those in QL your mouse should behave exactly as before.
- Case 2: You want to play with the exact same settings as another player. Enter his QL settings into the edit boxes, click convert, enter *their* mouse cpi and hit enter. Now you need to change m_cpi editbox to *your* mouse cpi and all the other settings get adjusted accordingly. You can use these settings in QL now.
- Case 3: You want to revert to the old system. Just hit convert when the m_cpi box is not zero and the conversion will be done the other way.

## Requirements

- For accurate results, this program requires you to use raw mouse input ("in_mouse 2") in QL, but you're probably doing that anyway.
- Your computer needs to be fast enough to run this. It should be but I am a bad programmer so it might require a high end PC, who knows.
- Tested only on Windows 7 so far. Ubuntu/Wine didn't work for me due to raw mouse input.

## Not implemented:

- m_yaw/m_pitch: Those are are hardcoded to 0.022 at the moment.
- m_filter: This is hardcoded to 0. Not sure if implementing this is worth it in the scope of what the tool aims to do.
- While the game actually supports negative multipliers (you move your mouse to the left and the view shifts to the right), the graph in the program only displays values down to 0 right now.

## Disclaimer

This program comes with no warranties. I am not responsible for any damage using the program might cause.
