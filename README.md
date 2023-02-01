# Borg ER-3
A portable (SDL) audio synthesizer / tone generator inspired by the Korg ER-1

[![Image of the Borg-ER3](https://dashboard.snapcraft.io/site_media/appmedia/2023/01/borger3_eyBQLNu.png)](https://www.youtube.com/watch?v=gLB91cO6RaE "Borg ER-3 FART Oscillators Demonstration Video")

Read more about this project over at the official Medium article:<br>
https://james-william-fletcher.medium.com/the-borg-er-3-46641d5784be

<details>
    <summary>Details</summary>
    <b>FART</b> = Frequency, Amplitude, Resolution, Transition
    <b>Resolution</b> = How many additive sinusoids are combined to make the final waveshape.
    <b>Transition</b> = Selects which wave shape to output but also allows blending between the shapes.
    <b>Wave shape order:</b> Sine, Slanted Sine, Square, Saw, Triangle, Impulse, Violin.
    <b>Adjust the dials by left clicking and dragging or hovering and scrolling mouse 3 in the Y axis.</b>

    <b>Binds to play audio:</b> spacebar, mouse3, mouse4
    <b>Reset envelope:</b> right click on it
    <b>Scroll dial sensitivity selection:</b> right click, three sensitvity options

    <b>BIQUADS are executed from left to right, first BIQUAD 1, then 2, then 3.</b>

    <b>You can use the Load button to reset any changes since your last Save.</b>
    
    <b>You can mouse 3 Y axis scroll zoom the oscilloscope.</b>
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
