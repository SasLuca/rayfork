# Todo list

- Add allocators and io callback parameters to every function that needs them.
- Review error handling. Maybe make error checking functions for structs (eg: rf_image_is_valid(rf_image))
- Review all string literals, especially shaders and uniform names
- Make default allocator and io callbacks.
- Check all `todo` and `note` comments 
- Update examples.
- Make dependencies optional.
- Group functions and check their order.
- Rewrite readme and add documentation.
- Investigate `spacing` argument for `rf_draw_text` and other text functions.
- Merge `_rf_measure_height_of_wrapped_text` with `rf_measure_text`
- use new tinyobjloader with io callbacks.
- review cgltf io callbacks situation.  
- Rename everything from camelCase to snake_case.
- Add clang tidy maybe???
- Add a clang format file if that was a thing.
- Add individual build scripts or make files for every example.
