# Interval Arithmetic Beam Bisection

This is an application for rendering complex implicit equations in real-time. 
This application uses Interval Arithmetic to both define and evaluate complex surfaces and equations.
 

The basic principle of Interval Arithmetic is that a valid IA function must return the minimum and maximum possible values, as an interval. For instance, if f(x) = -x then f([5, 10]) must return [-10, -5]. 
As a result of this requirement, one is able to do binary search on 3D spaces. If one were to take the minimum and maximum values of x, y, and z of the view frustum, one could create a 3D interval defining the axis aligned box which contains all visible things. Now consider the equation of a sphere: x^2 + y^2 + z^2 - r^2 = 0. This equation takes in a 3D vector, and a radius, and returns the squared distance to its surface. However, when Interval Arithmetic is substituted in the equation, the function returns the minimum and maximum squared distance to its surface. Now, one can input a 3D vector interval and have returned a 1 Dimensional interval. If the returned interval does not span 0, then the input 3D vector interval, an axis aligned box, does not span the function which represents a 3D shape. Using this property one can now do binary search upon a 3D space by checking the entire view frustum, the near half, the far half, and so on, until a nearness threshold is met. Once that threshold is met, one can subdivide the view frustum into sub-frustums. This process can continue until each pixel is its own sub-frustum. Next, the depth buffer is written to by these pixel frustums. Finally, in the fragment shader normals and positions are derived from the depth buffer and used in lighting.

__[YouTube Video](https://www.youtube.com/watch?v=phZ94Mc2Grs)__

__Dependencies:__
* OpenGL 4.3
* glew
* glfw3
* glm
* clang
* make
  
__Building:__
* make -j release

__Running:__
* make run

![alt tag](http://i.imgur.com/UQfzSRx.jpg)
