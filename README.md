# Surface-Modelling

## Bezier Surface Modelling
I have developed a Bezier Surface Model of an Elephant (Gumbo) .
The bezier surface is generated with 4x4 Bezier patches as the inputs. This model incorporates
specular,ambient and diffuse lighting. Depending on the camera view position you will get a
different tesselation level. The tesselation level is lower when you are further away from the
model, and increases as you get closer. The model is able to play an animation sequence which
explodes the bezier surface model.

![scene](/images/raytracer.png) 
  
## Terain Surface Modelling
I implemented a terrain mapping program that allows the user to
choose between two height maps. The maps contain dynamic levels of detail in which the
tesselation decreases as the camera moves further away and increases as the camera gets closer
to the terrain. The terrain is rendered with ambient and diffuse lighting. Four textures are
included in this terrain (Water, Grass , Rock and Snow) . Additionally this program prevents
cracking that can occur between different tesselation levels. This program also includes
adjustable snow and water levels.

#Controls 
## For Both Scenes:
Key | Description
------------ | -------------
Forwards Arrow Key | Move forwards
Backwards Arrow Key | Move backwards
W | Toggle Wire Mesh

## For Bezier Surface Scene:
Key | Description
------------ | -------------
Space Bar | Exectute Explosion

## For Terrain Surface Scene:
Key | Description
------------ | -------------
1 | Select Map 1
2 | Select Map 2
o | Increase Water Level
p | Decrease Water level
l | Increase Snow
k | Decrease Snow

# How to Build and Run the Program:
First ensure required libraries are installed.\
Navigate to the correct directory.\
Run the following command to build the program:\
```cmake .``` \
```make``` \
Then to run the program:\
```./RayTracer.o``` 

