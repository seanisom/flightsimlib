# PTCLib
Implementation of Microsoft's Progressive Transform Codec (PTC) in ANSI C


## Introduction
PTCLib implements a decoder for PTC compressed images (encoder TBD).

PTC is a proprietary lossy image codec produced by Microsoft Research that can
be viewed as a more advanced JPEG, conceptually similar to JPEG2000.

It was used on several historical Microsoft products, and has since been 
superseded by the technically superior and open source JPEG-XR.

This supports the latest known iteration of PTC, v4.30, from 2007. If you have
access to the Xbox 360 SDK, this is very similar to PTC v4.10 as implemented by
PTCDecode.exe / PTCEncode.exe, with only the following known changes:
* A FLOAT32 single channel mode (for elevation data, but never used)
* The addition of 8 bytes to the frame header to support float mode
	* This renders those images binary incompatible without header fix ups
* Support for 24 bit max depth (previously 16)
	* This eases support for 24 bit RGB bitmaps
	* Although previously could write as 8X3 through a trivial conversion
* Support for custom destination formats and row writing
	* Allows RGBA > ARGB and custom packing like 565 RGB
	* Supports streaming pixels to arbitrary, non-file destinations

FSX+ was the only known client for PTC v4.30, as the above was all custom logic
specifically for the simulator. These images can still be written today in
BGL files produced by resample.exe in P3D's SDK. 


## Documentation
Documentation is almost nonexistent. See BglDecompressor.cpp to get an idea of
the interface. Implementation is fairly straightforward if you are familiar with
DCT block-based image codecs. I hope to improve the documentation over time.

I am fairly well versed in image coding technologies and history of PTC, so
feel free to email me or file an issue if you have any questions.

The best descriptions of the underlying technologies can be found in:
* [The most recent patent embodiment of PTC / Windows Photo HD](https://patents.google.com/patent/US7006699B2/en?oq=7006699)
* [Various research documents from Henrique Malvar to 1997](https://www.microsoft.com/en-us/research/people/malvar/publications/)

There are some peculiarities not always well-documented or that deviate from 
the specifications, including but not limited to:
* Fixed Peano (AC) and Spatial (DC) block reordering
	* Covered in the linked patent
	* Differs from Malvar's papers
	* Fixed reordering and no prediction / residual scheme
	* Ping-pong alternating macroblock grouping
* Quantization scaled by 2 for all but first channel of YCoCg 
	* Covered in a later YCoCg Paper for JXR
* Actual (H)LBT / DCT Transforms are not documented
	* Close to lifting scheme in patent
	* 2D version close to PCT/POT in JXR
	* Actual Biorthogonal shears/lifts not defined anywhere
		* Taken from PTCEncode.exe / PTCDecode.exe produced images
	* HH uses a separate, also undocumented TRR transform
	* I don't understand the point of using an "almost DCT" / Hadamard
		* JXR also does, although it uses slightly different lifting
	* While the derivations are beyond me, these transforms are 100% correct
* Entropy is not well documented
	* RLGR uses an arbitrary calculation for initial kr with a static k
	* RLGR is otherwise documented in RemoteFX SDK with minor differences
	* RLGR and BPC use different U0,D0,U1,D1 constants than in the papers
	* BLC has different and missing implementation details than in the paper
* Strange spatial hierarchy
	* Blocks are 4X4, macroblocks are 16X16
	* "Chunks" are an arbitrary encoder width setting, multiples of 32 pixels
	* "Tiles" are stripes of image width, macroblock (16) high
	* Thus if image width > chunk width, there are multiple chunks per tile
	* FSX smartly sets chunk width to image width of 256
		* Thus 16 tiles (stripes) of 256X16 pixels each
* It's not really a progressive codec 
	* Progressive resolution comes from mipmaps, quality from bit planes
	* As implemented, all bitplanes are always decoded
	* Mips can only be extracted 1 by 1 with coefficient scalability
	* There is no way to refine / stream resolutions without decoding again


## Testing
I have basic unit test image(s) from the Terrain SDK samples I will port over.

Note that only the code paths used by FSX have been extensively tested. This
encompasses no mips, no subregions, no advanced image flags, only 256X256
images, only PT16 or PT1555 formats, and only BLC or RLGR entropy.

This library has not been appropriately fuzzed and should be considered only
for hobby use cases.


## Roadmap
* Sample code / integration
* Tests
* Docs
* Standalone lib / nuget package
* Encoder / full codec
* Implement missing features (mips)
* CLI


## Patents
Note that this technology may be covered under patents owned by Microsoft.

PTC is an old technology that has long since evolved into JPEG-XR, which
is an ITU open standard and comes with a permissive open-source implementation.
Microsoft has a covenant not to sue for infringement for any JXR-covered 
tecnologies.

PTC is no longer used or revenue generating, most technologies are 
covered under JXR, and there are no known active patents with decoder claims.

That said, I am not a lawyer, nothing in this document or code constitutes 
legal advice, and if you have any concerns regarding patents you should contact 
a lawyer.
