Readme - Force Field Shaders
_________________________


Important : Because the normal map are UV dependent, in some situation like for spherical meshes, the seams can be too visible. If you want to remove the seams completely you will need 
           to set the Angle1 and Angle2 to 0. In the tiling options of the normal map, set the X value to 0.(cf demo scene Pedestal2) 
           You can also choose a 90 degree angle but in this case you will need to keep a whole number for the Y tiling value. (cf demo scene Pedestal1)
           
            
          

Force Field Additive :


     
     - Main Properties -

Control the color, specularity and the normal appearance.


     - Additional Color - 

Add details color with a fresnel and a random color controled by the normal map selected above.


     - Reflection Properties -

Contoled by a good old Cubemap. Improve significantly the general look of the effect.


     - Caustic Properties - 

I called it Caustic but you can actually use any grayscale map you want.


     - Linear Dissolve -

Linear spread control the color thickness.
Switch Y to Z axis depending on you model (cf video tuto)
Linear Dissolve : Change this value to animate the linear wipe effect
Wave amout : increase the number of wave depending on the map above 
Wave amplitude : the lower the value is, larger the amplitude will be
Wave speed control the amplitude rate
Smooth edge will smooth the wave mask


     - Custom Dissolve -

Projection depending on the world coordinates. 


     - General Dissolve Properties -

Control the tint and intensity of the disintegration effect.
With the distortion properties you can add color variation to the burning edges. This colors are controled by the Wave Mask in the Linear Dissolve section


  

Force Field Transparent+Refraction :


This shader is not additive. It looks less bright. It's including all the properties of the Additive Shader + the refraction and the transparency parameters





_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _

If you are happy with the package please consider add a review or simply rate the package. This will help a lot.
Thank you.


Contact :

If you have any questions, please feel free to contact us : contact@ciconia-studio.com

More news on our work? Follow us on Twitter : twitter.com/CiconiaStudio