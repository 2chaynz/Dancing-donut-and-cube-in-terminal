/*LINK that have helped me :  https://www.a1k0n.net/2011/07/20/donut-math.html
                              https://youtu.be/sW9npZVpiMI?si=N9HuN4UTYlazzBsL    
*/

// Libraries needed

#include <stdio.h>
#include <string.h>
#include <math.h>


/* Couple of things that I didn't know about this project that I wish I read somewhere:

- What is a Buffer?: It is like a storage box, a sponge memory that stores data

- How is it used here?: Here 2 buffers: z[1760]: z-buffer (depth) and b[1760]: rendering buffer.
  Ex: → b stores the character that needs to be shown (space ' ', point '.', '@', etc.)
  Moreover b is a list of chars (so 1 byte per place) and 1 char = 1 byte.
  Ex: → z stores the depth at a certain point = a 3D technique for rendering objects in 2D
  z is a list of floats, with a size of 1760 boxes.

  Careful!: In memory a float weighs 4 bytes. So memset(z, 0, 7040) puts all the bytes of this list to 0 (0.0f for a float: 32 bits) 0.0 is a double (64 bits).

- Why 1760 as a size for the buffer?:
  Here my terminal is 80 columns × 22 lines = 1760. (Write stty size in your terminal)
  Terminal = window, a grid of characters

- About the z buffer, why?: Imagine 2 3D points projected at the same point on the screen, the closest must be shown. So we compare the distance D

- What is putchar()?:
  putchar(...) prints a character into the terminal. It converts c into an unsigned character:
  putchar('A');  // prints A
  putchar(10);   // prints a line jump.
  Inconveniences: only one character at a time.

  Pros: Faster than printf.
*/

/*
-- THE IDEA --

The 2 loops phi and theta are enough to configure a torus.
A big angle and a small one cf: https://www.a1k0n.net/2011/07/20/donut-math.html

We are going to map every reachable points:

Starting on a surface (z=0 because easier) we are creating the torus with polar coordinates and then with 2
rotational matrices we are going to build every points on the surface of the torus:
Again: https://www.a1k0n.net/2011/07/20/donut-math.html

Then we consider a point (x,y,z) in space (the loops are going to do the work) of the torus and we rotate it with 2 matrices again (A and B)

Then we project on 2D and we calculate an "inverted depth" (go further) D for the z buffer.
Finally the luminance for the ASCII.

Typical path in the loop: 

Calculate the position, the luminance, take the cursor back at the top, render the buffers,
increment the angles and repeat.
*/

/*
The code = matrices product, here it is implemented with maths so it's easier to read while doing the maths I think.
Just follow https://www.a1k0n.net/2011/07/20/donut-math.html steps 

IF YOU WANT TO SEE MORE MATHS ALSO IMPLEMENTED GO SEE MY CODE FOR 
THE SPINNING CUBE :) #DIY
 */

int k;

int main()
{
  {
  float A = 0, B = 0, theta, phi, z[1760]; // The angles of rotation A and B, theta and phi for constructing the torus in the following and the z buffer for depth.
  char b[1760];                            // The buffer b for rendering.
  
  // For cleaning the screen of the terminal at the beginning, \x1b = character ESC (value 27 in ASCII), [2J ANSI sequence = delete everything on the screen.
  // Without this the torus would be showed on top of what's already there.

  printf("\x1b[2J");

  for (;;) // Infinit loop
  {
    memset(b, 32, 1760);
    memset(z, 0, 7040);

/* 
- About memset(..., 32, 1760) :
 It puts the value 32 in every bytes→ or 32 in ASCII = ' ' (space).
 It fills the buffer with spaces before drawing again

It initialize the z buffer before calculating anything.
At the beginning we want : no pixels drew that's why

Because D > 0, to initialize at 0 means everything calculated will be closer than 0 so we will write on top of the screen.
*/

    int K2 = 5; // K2, the distance from the viewer to the donut, was chosen to be 5.
    float R1 = 1;   // small radius
    float R2 = 2;   // Big radius

    //The factor 30 and 15 are for the scale on the screen ; 40 and 12 are offsets for centering the image (80/2 = 40 , 22/2 ≈ 11 → ~12).

    for (phi = 0; 2*M_PI > phi; phi += 0.07)
      for(theta = 0; 2*M_PI > theta; theta += 0.02)

      {
        float z1 = (R2 + R1*cos(theta))*sin(phi)*cos(A) - R1*sin(theta)*sin(A);; //z1 because z is already taken

        float D =  1 / (z1 + K2);

        // D = 1 / (z + K2); it is inverted depth  (1/(length screen/point) ) with K2 as a setting (arbitrary).
        // D is convenient : we calculate D for projecting x and y and it's used for the z buffer

        // For projecting : The idea is x' = K1 * X / (Z + K2) and y' = K1' * Y / (Z + K2)
        // Here D is 1/(Z+K2).

        float x = 40 + 30 * D * ((R2 + R1*cos(theta))*cos(phi)*cos(B) 
      + (R1*sin(theta)*cos(A) + (R2 + R1*cos(theta))*sin(phi)*sin(A))*sin(B));

        float y = 12 + 15 * D * (-(R2 + R1*cos(theta))*cos(phi)*sin(B) 
      + (R1*sin(theta)*cos(A) + (R2 + R1*cos(theta))*sin(phi)*sin(A))*cos(B));
        // Every position of the screen is transformed into a single index o; for ex with i x = 5 and y = 2 : o = 5 + 80*2 = 165.
        // Located column 5, ligne 2.

        int o = (int)x + 80 * (int)y;

        // o transforms the 2D coordinate into an number that gos into the buffers.

        // Go to the Latex file for all the calculus for the luminance Lu (here *8 for having more integers for ".,-~:;=!*#$@"[L > 0 ? L : 0];)
        
        int L = 8 * ( (cos(A) - sin(A)) * (cos(theta) * cos(phi) * sin(B) + sin(theta) * cos(B)) - (cos(A) + sin(A)) * (cos(theta) * sin(phi)) );

        // If the new D > z[o], we replace b[o] and z[o].

        if (22 > y && y > 0 && x > 0 && 80 > x && D > z[o]) //  Check if (x,y) is inside the screen window (80×22).
                                                            //  D > z[o] → test the z buffer.
                                                            // If D >z[o] it means that the current point is closer than the one rendered 
                                                            // -> We need to replace it and update the z buffer
        {
          z[o] = D;
          b[o] =  ".,-~:;=!*#$@"[L > 0 ? L : 0];

/*
What is the syntax b[o] = ".,-~:;=!*#$@"[N > 0 ? N : 0]; ?

It is a C shortcut.
In C, a chain "abc"  is in reality a list of char ended by \0
So ".,-~:;=!*#$@" = a list :
--> chars[] = {'.', ',', '-', '~', ':', ';', '=', '!', '*', '#', '$', '@', '\0'};

[N > 0 ? N : 0] is an index applied to the chain.
If N > 0 → we take N
Else → we take 0 : it is just a security measure.

So :
if N = 0 → b[o] = '.'
if N = 1 → b[o] = ','
if N = 11 → b[o] = '@'
if N < 0 → b[o] = '.'

It is a way to implement a link btw the luminance and the ASCII character.
*/

/*
The light comes from [0,1,-1] (arbitrary).

Then the result is multipled by 8 so that the the values are scattered in a int scale so that ultimatly we can use it with the luminance :
 ".,-~:;=!*#$@"[N > 0 ? N : 0]; 
 The char chain has 12 lvl of intensit, N goes to 0 (dark) to 11 (bright)
*/

        }
      }
    printf("\x1b[H"); //Puts the cursor back at the top whitout deleting the screen
                      //It allows to draw on top of each frames so that the movement seems flowing.

/*
k goes to each index of the buffers btw 0 and 1760 

What does the condition does ? :

Every 80 columns we want to go to the next line.
If k % 80 != 0 → we print the buffer's character b[k]
If k % 80 == 0 → we print \n (jump of line) in order to go to the next one

→ The screen is exactly the buffer b
*/

    for (k = 0; 1761 > k; k++)
        if (k % 80 == 0) {
            putchar('\n');
        } else {
            putchar(b[k]);
        }
    A += 0.01;    // Increment the angles so that the donut rotates
    B += 0.005;
  }
}
return 0;  
}


