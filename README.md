# Fractal Generator

A collection of historical C++ fractal visualization experiments by Garrett
Miller. The source explores complex quadratic maps, recursive midpoint
displacement, L-systems, and multiple reduction copy machines (MRCM). It also
contains region-based CPU threading and shader experiments, including a
logistic-map bifurcation shader. Some work is unfinished or experimental.

The MRCM experiments were inspired in part by *Chaos and Fractals: New
Frontiers of Science*, by Heinz-Otto Peitgen, Hartmut Jürgens and Dietmar
Saupe. The underlying mathematical ideas are credited to their sources.

## Contents

- `Fractal Generator_SDI/`: earlier single-window project source.
- `Fractal Generator/`: later multi-window project source, including alternate
  source and shader copies preserved from the archive.

## Status

Archived source samples, not verified modern builds. The projects reference
legacy Windows/MFC, Direct3D and Garrett's GLib framework. The exact compatible
versions of the dependencies and toolchain have not been established.

## Staging changes

Executables, libraries, generated build output, logs, IDE user files, images,
icons, other media and nested archives are omitted. Original MFC resource
scripts are included but may refer to omitted image or icon assets, so the
projects are not ready to build without restoring and reviewing dependencies
and resources. Hard-coded user-directory include paths in the older project
file were replaced with `$(GLIB_INCLUDE_DIR)`; no application C++ or shader
source was changed. Verify third-party notices and assets before publication.

No open-source license has been selected for this staging copy.
