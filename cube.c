
#include <stdio.h>
#include <string.h>
#include <math.h>

//Main function, same structure as donut.c so go back to this code for more detailed explanations.

int main() {
    int k;              // Counter for the rendering buffer loop 
    float A = 0, B = 0; // Angle of rotations
    float z[1760];      // Depth buffer
    char b[1760];       // Buffer for displaying the characters
    printf("\x1b[2J");  // Clean the screen via an ASCII command
    float K2 = 5;       // Distance from the observer (arbitrary)
    float K1 = 30;      // Scale factor here for a 80x22 terminal size ( write stty size in your terminal) (arbitrary)

    for (;;) {                //Infinit loop 
        memset(b, 32, 1760);  // Fill the buffer with spaces
        memset(z, 0, 7040);   // Initialize the Z buffer ("close" to the screen with 0)

        /* 
        The idea is still similar to the donut: we are going to map all the points that are reachable for a cube.

        For that we need to explore every case: every point of a face needs to be mapped, think of it as a coloring book.

        Here the cube is centered at 0.

        Just think about a point in space (x,y,z1) (NB: z1 here instead of z because the buffer is named z) and 
        good loops for mapping all the points then focus on your point (x,y,z1) for the following like rotations etc..
        Only this point, the loop will do the rest.

        */
        for (int face = 0; face < 6; face++) {
            for (float R1 = -1; R1 <= 1; R1 += 0.05) {
                for (float R2 = -1; R2 <= 1; R2 += 0.05) {  
                    float x, y, z1;

                    // Switch is a better alternative as if and elif etc...
                    // Here we are focusing on a point in space (x,y,z1) like i said previously

                    switch (face) {
                        case 0: x = R1; y = R2; z1 = +1; break; // Front
                        case 1: x = R1; y = R2; z1 = -1; break; // Back
                        case 2: x = -1; y = R1; z1 = R2; break; // Left face etc...
                        case 3: x = +1; y = R1; z1 = R2; break; 
                        case 4: x = R1; y = +1; z1 = R2; break; 
                        case 5: x = R1; y = -1; z1 = R2; break; 
                    }

                    // Rotation : I multipled my vector (x,y,z1) by two rotation matrices oriented around two different axis : x and y 
                    // Go to the Latex file for the maths :)

                    float x1 = x*cos(B) + z1*sin(B);
                    float y1 = x*sin(A)*sin(B) + y*cos(A) - z1*sin(A)*cos(B);
                    float z2 = -x*sin(B)*cos(A) + y*sin(A) + z1*cos(A)*cos(B);

                    // Projection : Go either to mine donut.c or to https://www.a1k0n.net/2011/07/20/donut-math.html , the results
                    // and ideas for the projection are the exact same than with the donut.

                    float Z = 1 / (z2 + K2);
                    int xp = (int)(40 + K1 * Z * x1);
                    int yp = (int)(12 + K1 * Z * y1);
                    int o = xp + 80 * yp;

                    // Calcul related to the norm, here no need to calculate the partial derivatives for x1,y1 and z1 and then do the vectorial
                    // product, the normal vector of the faces are well know, there a the unit vector. (easier than the donut)
                    // But the different cases remains an issue here (longer than with the donut).

                    float Nx=0, Ny=0, Nz=0;
                        switch(face) {
                            case 0: Nx=0; Ny=0; Nz=1; break;
                            case 1: Nx=0; Ny=0; Nz=-1; break;
                            case 2: Nx=-1; Ny=0; Nz=0; break;
                            case 3: Nx=1; Ny=0; Nz=0; break;
                            case 4: Nx=0; Ny=1; Nz=0; break;
                            case 5: Nx=0; Ny=-1; Nz=0; break;
                        }

                        float Lx=0, Ly=1/sqrt(2), Lz=-1/sqrt(2);     // I chose the direction of the light like that and normalized itso that L is btw -1 and 1 (helps for the following ) 
                        float L = Nx*Lx + Ny*Ly + Nz*Lz;             // Dot product for the luminance but not enough values reachable so... -->
                        int N = (int)(8*(L + 1)/2);                  // N is now appart of the set : 0,1,2,3,4..8 that allow us to use : b[o] = ".:*"[N] like the donut; 
                        


                    // Check if the point is in the screen and if it is closer than the previous one already showed
                    // The rest is the exact same as donut.c

                    if (yp > 0 && yp < 22 && xp > 0 && xp < 80 && Z > z[o]) {
                        z[o] = Z;
                        b[o] = ".,-~:;=!*#$@"[N];              
                    }
                }
            }
        }

        // Put back the cursos at the top of the screen ofr drawing again ( it's a loop )
        // ASCII coommand 

        printf("\x1b[H");

        // Renders the buffer
        for (k = 0; k < 1760; k++) {
            putchar(k % 80 ? b[k] : 10);
        }

        // Updates the angles
        A += 0.005;
        B += 0.0005;
    }

    return 0;  // Classic C command just in case :)
}
