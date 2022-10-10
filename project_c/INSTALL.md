Building and installing the SRR application
======================================================

Set the `SCIP_DEVELOP_PATH` (or `SCIP_MIRROR_PATH`, as specified in `Makefile`) environment variable to the SCIP project, and refer to the INSTALL_APPLICATIONS_EXAMPLES.md file in the scip directory. Minimal installation instructions are given at the end of this file.

The code depends on the LEMON Graph Library (designed for version 1.3.1).
It either needs to be installed system-wide, or locally in the dependencies/lemon-1.3.1 folder.
Assuming build tools including cmake, curl and tar are present, the following works:

```bash
cd dependencies

# Get the lemon sourcecode
curl http://lemon.cs.elte.hu/pub/sources/lemon-1.3.1.tar.gz | tar -zxvf -

cd lemon-1.3.1

cmake .
make -j

# Installing is not needed.
# make install
```

Minimal installation instructions
---

```bash
# SCIP installation. Assuming SCIP_MIRROR_PATH is set.
cd $SCIP_MIRROR_PATH
make OPT=opt LPS=cpx -j

# Install LEMON
# See above instructions.

# Make the project
# Navigate to this directory
cd .
make OPT=opt LPS=cpx -j
```
