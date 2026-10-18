# Fake Zune simulator

Headless test harness for the High Boy Fake Zune front end. It compiles the **real**
`boot_ui.c`, `home_ui.c`, `media_list_ui.c` and `ui_manager.c` from `firmware_p4` against LVGL on the host
(hardware services are stubbed in `src/stubs.c`), drives them with scripted key presses and
checks the result. No ESP-IDF, SDL or hardware needed.

```bash
cmake -S tools/fakezune_sim -B build/fakezune     # first configure downloads LVGL
cmake --build build/fakezune -j
ctest --test-dir build/fakezune --output-on-failure

# Look at the frames (needs Pillow)
./build/fakezune/fakezune_sim build/fakezune/frames
python3 tools/fakezune_sim/ppm2png.py build/fakezune/frames

# Same run on the rotated (320x240) display
FZ_LANDSCAPE=1 ./build/fakezune/fakezune_sim build/fakezune/frames_landscape
```

What it checks:

- Boot screen shows exactly one label, "Loading Media", centred and inside the screen.
- Home hub lists music / videos / pictures / settings / high boy, none of the old High Boy
  menu items, with "high boy" below the fold until you scroll to it.
- UP/DOWN move the selection and clamp at both ends; each entry opens the right screen.
- The music / videos / pictures lists show every demo item with its subtitle and duration or
  date, truncate long text to one line, start with the last item below the fold, scroll to
  reveal it, clamp at both ends, ignore OK, and return to the hub on Back.
- BACK from a screen opened from the hub returns to the hub (with the entry still
  selected); BACK from a screen opened from the high boy carousel returns to the carousel.

This is separate from the upstream `tools/hle` simulator, which no longer builds against
the current firmware sources.
