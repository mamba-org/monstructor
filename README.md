### monstructor, a stand-alone installer generator based on mamba

**Note: very early stage software**

Monstructor generates standalone installers (similar to `constructor` for conda) that uses `conda` packages and the C++ of `mamba` to install the packages.

The `monstructor` project is currently a Work-In-Progress. The current idea is to have a installer layout (for Linux) that looks like the following:

- monstructor binary
- JSON description of payload (list of repodata snippets including channel, URL, SHA256, file size)
- concatenated binary tar.bz2 or .conda files

### Usage

To use the monstructor, you have to have a conda environment. Currently, mamba needs to be installed from source (but with mamba 0.5.4 it will be enough to have `libmamba-static` installed!).

First, build monstructor:

- `mkdir build && cd build`
- `cmake .. -DCMAKE_INSTALL_PREFIX=$CONDA_PREFIX`
- `make -j4`

With the monstructor binary in place, you can generate new monstructors. For this, run the python script included in `example`. First, select (or create) an environment, and export it:

- `mamba create -n xtensor xtensor -c conda-forge`
- `conda activate xtensor`
- `mamba list --json > exported_pkgs.json`
- `python examples/test.py build/monstructor ./exported_pkgs.json`

This will place a new, bundled executable into the `build` folder (`monstructor.done`). You can execute this file:

- `./build/monstructor.done -p /some/target/prefix`

To recreate the environment you previously exported.