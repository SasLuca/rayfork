# Todo list

- Decouple rf_gl in order to allow people to make their own backends.
- Maybe switch to handmade math, if not just add SIMD support to rf_math.
- Make versions that don't allocate for all functions that do allocate. Might not be possible for functions that use stb libs since those just use malloc and free.
- Investigate `spacing` argument for `rf_draw_text` and other text functions.
- Rename everything from camelCase to snake_case.
- Add clang tidy maybe???
- Add a clang format file if that was a thing.
- Add individual build scripts or make files for every example.
- Add more assertions.
- Merge `_rf_measure_height_of_text_in_container` with `rf_measure_text`
- tinyobjloader uses fopen io functions which we dont want, so review that 