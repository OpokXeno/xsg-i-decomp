# Project Headers

`xeno/` is the project include namespace. Its domain directories mirror `src/`-

See [source organization](../src/README.md) for responsibilities, original-symbol
evidence and ownership boundaries. Keep private headers beside their source;
do not create speculative types or empty API declarations to fill a folder.
The `.gitkeep` files only retain the planned directories in Git.

`xeno/audio/ssd/` and `xeno/audio/rssd/` keep the two IOP modules' internal
interfaces distinct. Shared EE/IOP wire definitions require validated widths and
layouts, not reuse of an EE type by assumption.