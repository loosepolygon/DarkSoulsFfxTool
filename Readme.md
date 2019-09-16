## Usage

Convert ffx to json: `DSFfxTool.exe SomeFile.ffx`

Convert json to ffx: `DSFfxTool.exe SomeFile.ffx.json`

Or just drag the file onto the exe


## Status

#### PTDE branch (master):
* All bytes of all ffx files are read exactly once
* All ffx files, when converted to json and back, are byte-identical

#### DSR branch:
* In-progress


## Notes
* All of the wacky and wild names are a result of the reverse engineering thing where you give types and variables memorable names instead of using names like `Type1Type7Type55` for everything; as things become known their names will be changed to be more sensical
* Developed using Visual Studio Community 2017
