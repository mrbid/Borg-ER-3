# Borg ER-3
A portable (SDL) audio synthesizer / tone generator inspired by the Korg ER-1

[![Image of the Borg-ER3](https://dashboard.snapcraft.io/site_media/appmedia/2023/01/borger3_eyBQLNu.png)](https://www.youtube.com/watch?v=gLB91cO6RaE "Borg ER-3 FART Oscillators Demonstration Video")

Read more about this project over at the official Medium article:<br>
https://james-william-fletcher.medium.com/the-borg-er-3-46641d5784be

<details>
    <summary>Details</summary>
    FART = Frequency, Amplitude, Resolution, Transition
    Resolution = How many additive sinusoids are combined to make the final waveshape.
    Transition = Selects which wave shape to output but also allows blending between the shapes.
    Wave shape order: Sine, Slanted Sine, Square, Saw, Triangle, Impulse, Violin.

    Adjust the dials by left clicking and dragging or hovering and scrolling mouse 3 in the Y axis.

    Binds to play audio: spacebar, mouse3, mouse4
    Reset envelope: right click on it
    Scroll dial sensitivity selection: right click, three sensitvity options

    BIQUADS are executed from left to right, first BIQUAD 1, then 2, then 3.

    You can use the Load button to reset any changes since your last Save.
    
    You can mouse 3 Y axis scroll zoom the oscilloscope.
</details>

## Flathub
https://flathub.org/apps/details/com.voxdsp.Borg

## Snapcraft
https://snapcraft.io/borger3

## Latest Release
*Windows, Linux, Deb, AppImage, etc*<br>
https://github.com/mrbid/Borg-ER-3/releases/tag/1.0

## Build Instructions
```
sudo ./getdeps.sh
./compile.sh
```
