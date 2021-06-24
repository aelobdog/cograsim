# Changelog

## 2.0.1 

* Improved the opengl -> povray mapping by setting povray's assumed_gamma to 2.2
* Fixed a bug that would prevent the fps counter from sometimes showing up on the screen
* Fixed a bug that would make the code segfault whenever a DNA topology file would contain fewer lines than what stated in the header line

## 2.0.0

* Ported the OpenGL engine to *modern* OpenGL. The performance have been greatly enhanced.
* Added an option to display/hide the axes keys on the bottom left of the viewport (k key)
* Added an option to display particle ids on screen (l key, works only in perspective mode)
* The fps counter is now rendered in the viewport
* Improved the OpenGL -> povray mapping (especially the lighting)
