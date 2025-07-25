## Updating the Grapa Version Number

To update the Grapa version number (e.g., for a new release):

1. Edit `setup.py` and change `grapapy_version = "<new_version>"`.
2. Edit `source/mainpy.cpp` and update `m.attr("__version__") = "<new_version>";`.
3. Edit `source/grapa/GrapaLink.h` and update `#define grapa_version "<new_version>"`.

Be sure to commit all three changes together for consistency. 