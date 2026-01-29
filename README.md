# Borg ER-3
A portable (SDL) audio synthesizer / tone generator inspired by the Korg ER-1

[![Image of the Borg-ER3](https://dashboard.snapcraft.io/site_media/appmedia/2023/01/borger3_eyBQLNu.png)](https://www.youtube.com/watch?v=gLB91cO6RaE "Borg ER-3 FART Oscillators Demonstration Video")

* **FlatHub:** [com.voxdsp.Borg](https://flathub.org/apps/details/com.voxdsp.Borg)
* **Snapcraft:** [borger3](https://snapcraft.io/borger3)
* **Latest Release:** [*Windows, Linux, Deb, AppImage, etc*](https://github.com/mrbid/Borg-ER-3/releases)

Fixed-state modular synthesizer using FART oscillators, if you can hold back the chuckles for a moment stands for Frequency, Amplitude, Resolution, and Transformation oscillators, the resolution regards harmonic resolution and transformation regards the wave-shape being produced by the oscillator.

The envelope can be clicked and dragged into any shape, the biquads can be manually configured on a per coefficient basis, they are executed in sequential order so BIQUAD 1 executes first.

On Windows exported files save to the local directory that the program was executed in, on Linux exported files save to: `~/EXPORTS/Borg_ER-3/`

### Technical details
* **FART** = Frequency, Amplitude, Resolution, Transition
* **Resolution** = How many additive sinusoids are combined to make the final waveshape.
* **Transition** = Selects which wave shape to output but also allows blending between the shapes.
* **Wave shape order:** Sine, Slanted Sine, Square, Saw, Triangle, Formant, Impulse.
* **Binds to play audio:** spacebar, mouse3, mouse4
* **Reset envelope:** right click on it
* **Scroll dial sensitivity selection:** right click, three sensitvity options
* Adjust dials by left click and drag or hover and scroll mouse 3 in the Y axis.
* BIQUADS are executed from left to right, first BIQUAD 1, then 2, then 3.
* You can use the **Load** button to reset any changes since your last **Save**.
* You can mouse scroll zoom the oscilloscope, right click to reset zoom.
* You can hold the space bar while turning the dials to hear and see their effect in real-time.
* Flip sign of dial by mouse3 or mouse4 clicking on it.
* Reset/disable multiple selection button: right click on button.

## Build Instructions
```
sudo ./getdeps.sh
./compile.sh
```
