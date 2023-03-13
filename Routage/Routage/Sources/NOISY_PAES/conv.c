#include <stdio.h> 
#include <stdlib.h>
#include <math.h>

int main(int n, char *params[])  
{
    float angle, angle2, angle3, conversion;
    angle = atof(params[1]);
    angle2 = angle;
    if (angle < 0) angle2 = -angle;
    angle2 = floor(angle2)+(angle2-floor(angle2))*100.0/60.0;
    if (angle<0) angle2 = -angle2;
    angle3=angle2*M_PI/180.0;
    conversion=60*angle2*sin(angle3);
    printf("%f\n",conversion);

    return 0;           
}
