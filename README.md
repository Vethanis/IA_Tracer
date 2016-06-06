# Interval Arithmetic Beam Bisection

This is an application for rendering complex implicit equations in real-time. 
This application uses Interval Arithmetic to both define and evaluate complex surfaces and equations.
 

The basic principle of Interval Arithmetic is that a valid IA function must return the minimum and maximum possible values, as an interval. For instance, if f(x) = -x then f([5, 10]) must return [-10, -5]. 
As a result of this requirement, one is able to do binary search on 3D spaces by substituting implicit surface equations with their Interval Arithmetic equivalent and checking for a zero crossing in the resulting interval.
 
Relevant research can be found __[here](http://w3.impa.br/~diego/publications/GanEtAl11.pdf)__.

__[YouTube Video](https://www.youtube.com/watch?v=phZ94Mc2Grs)__

__Linux Dependencies:__
* OpenGL 4.3
* glew
* glfw3
* glm
* clang
* make
  
__Linux Building:__
* make -j release

__Linux Running:__
* make run

__Windows Usage:__
* requires VS2015 installed at default location, or an x64 cmd environment
* loadenv.bat
* build.bat
* run.bat

![alt tag](http://i.imgur.com/UQfzSRx.jpg)
