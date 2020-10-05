# monstructor, a stand-alone installer generator based on mamba

Monstructor generates standalone installers (similar to `constructor` for conda) that uses `conda` packages and the C++ of `mamba` to install the packages.

The `monstructor` project is currently a Work-In-Progress. The current idea is to have a installer layout (for Linux) that looks like the following:

- monstructor binary
- JSON description of payload (list of repodata snippets including channel, URL, SHA256, file size)
- concatenated binary tar.bz2 or .conda files