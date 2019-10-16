## Usage

Convert ffx to json: `DSFfxTool.exe SomeFile.ffx`

Convert json to ffx: `DSFfxTool.exe SomeFile.ffx.json`

Or just drag the file onto the exe


## Status

#### master branch (supports both PTDE and Remaster)
* All bytes of all ffx files are read exactly once
* All ffx files, when converted to json and back, are byte-identical
   * DSR is the exception because of the 0xcdcdcdcd issue (see below), they were changed to zeroes for my file comparison

#### DSR branch
* Merged into master


## Notes
* All of the wacky and wild names are a result of the reverse engineering thing where you give types and variables memorable names instead of using names like `Type1Type7Type55` for everything; as things become known their names will be changed to be more sensical
* Developed using Visual Studio Community 2017


## 0xcdcdcdcd

This won't be relevant to anyone but I have to get this out there:

The main difference between the PTD and DSR versions is that most 32-bit ints are now 64-bit ints, for literally no reason. The latter half of those 64-bit ints are zero, except for many of them which are 0xcdcdcdcd. 0xcd is used by Microsoft's C++ debugging runtime library to mark uninitialised heap memory. This contributes to bloating the files to be over double the size of the PTD versions. But at least they're all aligned to 8 (except the ones that aren't).
