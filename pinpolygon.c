/*
 * =====================================================================================
 *
 *       Filename:  pinpolygon.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  Saturday 11 November 2017 01:55:01  IST
 *       Revision:  1.1
 *       Compiler:  gcc
 *
 *         Author:  Jagdish Prajapati (JP), prajapatijagdish@gmail.com
 *        Company:  Emsoftronic
 *
 * =====================================================================================
 */


#include <stdio.h>

#define INF 10000.00f
#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
#endif

#ifndef MAX
#define MAX(a, b)   (((a) > (b))? (a) : (b))
#endif

#ifndef MIN
#define MIN(a, b)   (((a) < (b))? (a) : (b))
#endif

// Given three colinear points p, q, r, the function checks if
// point q lies on line segment 'pr'
int onSegment(float *p, float *q, float *r)
{
    if (*q <= MAX(*p, *r) && *q >= MIN(*p, *r) &&
            *(q+1) <= MAX(*(p+1), *(r+1)) && *(q+1) >= MIN(*(p+1), *(r+1)))
        return true;
    return false;
}

// To find orientation of ordered triplet (p, q, r).
// The function returns following values
// 0 --> p, q and r are colinear
// 1 --> Clockwise
// 2 --> Counterclockwise
int orientation(float *p, float *q, float *r)
{
    int val = (*(q+1) - *(p+1)) * (*r - *q) -
              (*q - *p) * (*(r+1) - *(q+1));

    if (val == 0) return 0;  // colinear
    return (val > 0)? 1: 2; // clock or counterclock wise
}

// The function that returns true if line segment 'p1q1'
// and 'p2q2' intersect.
int doIntersect(float *p1, float *q1, float *p2, float *q2)
{
    // Find the four orientations needed for general and
    // special cases
    int o1 = orientation(p1, q1, p2);
    int o2 = orientation(p1, q1, q2);
    int o3 = orientation(p2, q2, p1);
    int o4 = orientation(p2, q2, q1);

    // General case
    if (o1 != o2 && o3 != o4)
        return true;

    // Special Cases
    // p1, q1 and p2 are colinear and p2 lies on segment p1q1
    if (o1 == 0 && onSegment(p1, p2, q1)) return true;

    // p1, q1 and p2 are colinear and q2 lies on segment p1q1
    if (o2 == 0 && onSegment(p1, q2, q1)) return true;

    // p2, q2 and p1 are colinear and p1 lies on segment p2q2
    if (o3 == 0 && onSegment(p2, p1, q2)) return true;

     // p2, q2 and q1 are colinear and q1 lies on segment p2q2
    if (o4 == 0 && onSegment(p2, q1, q2)) return true;

    return false; // Doesn't fall in any of the above cases
}

// Returns true if the point (x,y) lies inside the polygon[] with n vertices and
// next vertex element offset as stride

int isInside(float *polygon, int n, int stride, float x, float y)
{
    // There must be at least 3 vertices in polygon[]
    if (n < 3)  return false;

    // Create a point for line segment from p to infinite
    float extreme[2] = {INF, y};
    float p[2] = {x,y};

    // Count intersections of the above line with sides of polygon
    int count = 0, i = 0, next = 0;
    do
    {
        next = (i+1)%n;

        // Check if the line segment from 'p' to 'extreme' intersects
        // with the line segment from 'polygon[i]' to 'polygon[next]'
        if (doIntersect(polygon+(stride*i), polygon+(stride*next), p, extreme))
        {
            // If the point 'p' is colinear with line segment 'i-next',
            // then check if it lies on segment. If it lies, return true,
            // otherwise false
            if (orientation(polygon+(stride*i), p, polygon+(stride*next)) == 0)
               return onSegment(polygon+(stride*i), p, polygon+(stride*next));

            count++;
        }
        i = next;
    } while (i != 0);

    // Return true if count is odd, false otherwise
    return count&1;  // Same as (count%2 == 1)
}

// Driver program to test above functions
int main()
{
    float polygon1[] = {0, 0, 10, 0, 6, 3, 10, 10, 5, 7, 0, 10, 3, 3};
    int n = (sizeof(polygon1)/sizeof(polygon1[0]))/2;
    float p[] = {5, 8};
    printf("%s\n", isInside(polygon1, n,2, p[0], p[1])? "Yes" : "No");

    p[0] = 5; p[1] = 5;
    printf("%s\n", isInside(polygon1, n,2, p[0], p[1])? "Yes" : "No");

    p[0] = 3; p[1] = 3;
    printf("%s\n", isInside(polygon1, n,2, p[0], p[1])? "Yes" : "No");

    p[0] = 5; p[1] = 1;
    printf("%s\n", isInside(polygon1, n,2, p[0], p[1])? "Yes" : "No");

    p[0] = 8; p[1] = 4;
    printf("%s\n", isInside(polygon1, n,2, p[0], p[1])? "Yes" : "No");

    p[0] = 2; p[1] = 3;
    printf("%s\n", isInside(polygon1, n,2, p[0], p[1])? "Yes" : "No");

    return 0;
}
