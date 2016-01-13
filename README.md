# 3DImageVolumeToMovie
Generate projection frames of a 3D image volume with a given rotation matrix

## Software Summary

- The C code takes in all images of a 3D volume data set and applies rotation matrix to generate the movie frames at the desired rotation angle
- See the function ZXrotate() in the [C subroutine](spinsub.cpp) for the 3D matrix rotation and maximum intentisty projection was implemented to generate the movie frames
- One can generate a movie via ImageJ or Matlab with the output frames (.avi)
- The code was written in C and compiled with a 64-bit Windows system

Movie sample
--------------------

- See [here](AVIs/sample.avi) for an sample movie generated from a contrast-enhanced 3D breast CT dataset
- See the Appendix of [my UC Davis disseration work](Disseration_Huang.pdf)for detailed description of movie frame generation
- The .avi was generated via ImageJ by loading up all the frames and save-as .avi (jpeg)

