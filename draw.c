#include <stdio.h>
#include <stdlib.h>

#include "ml6.h"
#include "display.h"
#include "draw.h"
#include "matrix.h"
#include "math.h"

/*======== void add_polygon() ==========
Inputs:   struct matrix *surfaces
double x0
double y0
double z0
double x1
double y1
double z1
double x2
double y2
double z2
Returns:
Adds the vertices (x0, y0, z0), (x1, y1, z1)
and (x2, y2, z2) to the polygon matrix. They
define a single triangle surface.
====================*/
void add_polygon( struct matrix *polygons, double x0, double y0, double z0, double x1, double y1, double z1, double x2, double y2, double z2 ) {

	add_point(polygons,x0,y0,z0);
	add_point(polygons,x1,y1,z1);
	add_point(polygons,x2,y2,z2);


}

/*======== void draw_polygons() ==========
Inputs:   struct matrix *polygons
screen s
color c
Returns:
Goes through polygons 3 points at a time, drawing
lines connecting each points to create bounding
triangles

Uses helper function face
====================*/
double face(double x0,double y0, double x1,double y1, double x2,double y2){

	//ax = p1x - p0x
	double ax = x1 - x0;
	//bx = p2x - p0x
	double bx = x2 - x0;
	//ay = p1y - p0y
	double ay = y1 - y0;
	//by = p2y - p0y
	double by = y2 - y0;

	//Nz = ax*by - ay*bx
	double nz = (ax*by) - (ay*bx);

	return nz;
}

void draw_polygons( struct matrix *polygons, screen s, color c ) {

	double ** m = polygons->m;

	int i;

	for (i = 0; i + 2 < polygons->lastcol; i+=3){

		double nz = face(m[0][i],m[1][i], m[0][i+1],m[1][i+1], m[0][i+2],m[1][i+2]);

		if (nz > 0){

			draw_line(m[0][i], m[1][i], m[0][i+1], m[1][i+1], s, c);
			draw_line(m[0][i+1], m[1][i+1], m[0][i+2], m[1][i+2], s, c);
			draw_line(m[0][i+2], m[1][i+2], m[0][i], m[1][i], s, c);

		}

	}

}


/*======== void add_box() ==========
Inputs:   struct matrix * edges
double x
double y
double z
double width
double height
double depth
Returns:

add the points for a rectagular prism whose
upper-left corner is (x, y, z) with width,
height and depth dimensions.
====================*/
void add_box( struct matrix * edges, double x, double y, double z, double width, double height, double depth ) {

	double x0, y0, z0, x1, y1, z1;
	x0 = x;
	x1 = x+width;
	y0 = y;
	y1 = y-height;
	z0 = z;
	z1 = z-depth;

	//front
	add_polygon(edges, x0,y0,z0, x0,y1,z0, x1,y0,z0);
	add_polygon(edges, x0,y1,z0, x1,y1,z0, x1,y0,z0);

	//right
	add_polygon(edges, x1,y1,z0, x1,y0,z1, x1,y0,z0);
	add_polygon(edges, x1,y1,z0, x1,y1,z1, x1,y0,z1);

	//left
	add_polygon(edges, x0,y0,z0, x0,y1,z1, x0,y1,z0);
	add_polygon(edges, x0,y0,z0, x0,y0,z1, x0,y1,z1);

	//top
	add_polygon(edges, x1,y0,z0, x1,y0,z1, x0,y0,z1);
	add_polygon(edges, x1,y0,z0, x0,y0,z1, x0,y0,z0);

	//bottom
	add_polygon(edges, x1,y1,z0, x0,y1,z0, x0,y1,z1);
	add_polygon(edges, x1,y1,z0, x0,y1,z1, x1,y1,z1);

	//back
	add_polygon(edges, x1,y0,z1, x1,y1,z1, x0,y1,z1);
	add_polygon(edges, x1,y0,z1, x0,y1,z1, x0,y0,z1);

}

/*======== void add_sphere() ==========
Inputs:   struct matrix * points
double cx
double cy
double cz
double r
double step
Returns:

adds all the points for a sphere with center
(cx, cy, cz) and radius r.

should call generate_sphere to create the
necessary points
====================*/
void add_sphere( struct matrix * edges, double cx, double cy, double cz, double r, double step ) {

	struct matrix *points = generate_sphere(cx, cy, cz, r, step);
	double ** m = points->m;


	int i;
	int num_steps = (int)(1/step) + 1;

	//stop there bc i + num_steps + 1 is as far as we go
	for (i = 0; i < points->lastcol - num_steps - 1; i++){

		add_polygon(
			edges,
			//point i
			m[0][i], m[1][i], m[2][i],
			//i + pts per slice + 1
			m[0][i+num_steps+1], m[1][i+num_steps+1], m[2][i+num_steps+1],
			//i + pts per slice
			m[0][i+num_steps], m[1][i+num_steps], m[2][i+num_steps]
		);

		add_polygon(
			edges,
			//point i
			m[0][i], m[1][i], m[2][i],
			//i + 1
			m[0][i+1], m[1][i+1], m[2][i+1],
			//i + pts per slice [num_steps] + 1
			m[0][i+num_steps+1], m[1][i+num_steps+1], m[2][i+num_steps+1]
		);

	}
	free_matrix(points);
}


/*======== void generate_sphere() ==========
Inputs:   double cx
double cy
double cz
double r
double step
Returns: Generates all the points along the surface
of a sphere with center (cx, cy, cz) and
radius r.
Returns a matrix of those points
====================*/
struct matrix * generate_sphere(double cx, double cy, double cz, double r, double step ) {

	struct matrix * ret = new_matrix(4, 4);
	int n = 1 / step;

	int i, j;

	for (i = 0; i <= n; i++) {

		double t = 2 * M_PI * step * i;

		for (j = 0; j <= n; j++) {

			double f = M_PI * step * j;

			double x = r * cos(f) + cx;
			double y = r * sin(f) * cos(t) + cy;
			double z = r * sin(f) * sin(t) + cz;

			add_point( ret, x, y, z );

		}

	}

	return ret;
}


/*======== void add_torus() ==========
Inputs:   struct matrix * points
double cx
double cy
double cz
double r1
double r2
double step
Returns:

adds all the points required to make a torus
with center (cx, cy, cz) and radii r1 and r2.

should call generate_torus to create the
necessary points
====================*/
void add_torus( struct matrix * edges, double cx, double cy, double cz, double r1, double r2, double step ) {

	struct matrix * points = generate_torus( cx, cy, cz, r1, r2, step );

	int num_steps = 1 / step;
	int i;

	for (i = 0; i < pow(num_steps,2) + num_steps; i++){

		add_polygon( edges,
			points->m[0][i], points->m[1][i], points->m[2][i],
			points->m[0][i + num_steps + 1], points->m[1][i + num_steps + 1], points->m[2][i + num_steps + 1],
			points->m[0][i + num_steps], points->m[1][i + num_steps], points->m[2][i + num_steps]
		);

		add_polygon( edges,
			points->m[0][i], points->m[1][i], points->m[2][i],
			points->m[0][i + 1], points->m[1][i + 1], points->m[2][i + 1],
			points->m[0][i + num_steps + 1], points->m[1][i + num_steps + 1], points->m[2][i + num_steps + 1]
		);
	}
}


/*======== void generate_torus() ==========
Inputs:   struct matrix * points
double cx
double cy
double cz
double r
double step
Returns: Generates all the points along the surface
of a torus with center (cx, cy, cz) and
radii r1 and r2.
Returns a matrix of those points
====================*/
struct matrix * generate_torus( double cx, double cy, double cz, double r1, double r2, double step ) {

	struct matrix * ret = new_matrix(4, 4);

	int n = 1 / step;
	int i, j;

	for (i = 0; i <= n; i++) {

		double t = 2 * M_PI * step * i;

		for (j = 0; j <= n; j++) {

			double f = 2 * M_PI * step * j;
			double x = cos(t) * ( r1 * cos(f) + r2) + cx;
			double y = r1 * sin(f) +  cy;
			double z = -1 * sin(t) * ( r1 * cos(f) + r2 ) + cz;

			add_point( ret, x, y, z );
		}

	}

	return ret;
}


/*======== void add_circle() ==========
Inputs:   struct matrix * points
double cx
double cy
double r
double step
Returns:

Adds the circle at (cx, cy) with radius r to edges
====================*/
void add_circle( struct matrix * edges, double cx, double cy, double cz, double r, double step ) {

	double x0, y0, x1, y1, t;

	x0 = r + cx;
	y0 = cy;
	for (t=step; t <= 1.00001; t+= step) {

		x1 = r * cos(2 * M_PI * t) + cx;
		y1 = r * sin(2 * M_PI * t) + cy;

		add_edge(edges, x0, y0, cz, x1, y1, cz);
		x0 = x1;
		y0 = y1;
	}
}

/*======== void add_curve() ==========
Inputs:   struct matrix *points
double x0
double y0
double x1
double y1
double x2
double y2
double x3
double y3
double step
int type
Returns:

Adds the curve bounded by the 4 points passsed as parameters
of type specified in type (see matrix.h for curve type constants)
to the matrix points
====================*/
void add_curve( struct matrix *edges, double x0, double y0, double x1, double y1, double x2, double y2, double x3, double y3, double step, int type ) {

	double t, x, y;
	struct matrix *xcoefs;
	struct matrix *ycoefs;

	xcoefs = generate_curve_coefs(x0, x1, x2, x3, type);
	ycoefs = generate_curve_coefs(y0, y1, y2, y3, type);

	/* print_matrix(xcoefs); */
	/* printf("\n"); */
	/* print_matrix(ycoefs); */

	for (t=step; t <= 1.000001; t+= step) {

		x = xcoefs->m[0][0] *t*t*t + xcoefs->m[1][0] *t*t+
		xcoefs->m[2][0] *t + xcoefs->m[3][0];
		y = ycoefs->m[0][0] *t*t*t + ycoefs->m[1][0] *t*t+
		ycoefs->m[2][0] *t + ycoefs->m[3][0];

		add_edge(edges, x0, y0, 0, x, y, 0);
		x0 = x;
		y0 = y;
	}

	free_matrix(xcoefs);
	free_matrix(ycoefs);
}


/*======== void add_point() ==========
Inputs:   struct matrix * points
int x
int y
int z
Returns:
adds point (x, y, z) to points and increment points.lastcol
if points is full, should call grow on points
====================*/
void add_point( struct matrix * points, double x, double y, double z) {

	if ( points->lastcol == points->cols )
	grow_matrix( points, points->lastcol + 100 );

	points->m[0][ points->lastcol ] = x;
	points->m[1][ points->lastcol ] = y;
	points->m[2][ points->lastcol ] = z;
	points->m[3][ points->lastcol ] = 1;
	points->lastcol++;
} //end add_point

/*======== void add_edge() ==========
Inputs:   struct matrix * points
int x0, int y0, int z0, int x1, int y1, int z1
Returns:
add the line connecting (x0, y0, z0) to (x1, y1, z1) to points
should use add_point
====================*/
void add_edge( struct matrix * points, double x0, double y0, double z0, double x1, double y1, double z1) {
	add_point( points, x0, y0, z0 );
	add_point( points, x1, y1, z1 );
}

/*======== void draw_lines() ==========
Inputs:   struct matrix * points
screen s
color c
Returns:
Go through points 2 at a time and call draw_line to add that line
to the screen
====================*/
void draw_lines( struct matrix * points, screen s, color c) {

	if ( points->lastcol < 2 ) {
		printf("Need at least 2 points to draw a line!\n");
		return;
	}

	int point;
	for (point=0; point < points->lastcol-1; point+=2)
	draw_line( points->m[0][point],
		points->m[1][point],
		points->m[0][point+1],
		points->m[1][point+1],
		s, c);
	}// end draw_lines

void draw_line(int x0, int y0, int x1, int y1, screen s, color c) {

	int x, y, d, A, B;
	//swap points if going right -> left
	int xt, yt;
	if (x0 > x1) {
		xt = x0;
		yt = y0;
		x0 = x1;
		y0 = y1;
		x1 = xt;
		y1 = yt;
	}

	x = x0;
	y = y0;
	A = 2 * (y1 - y0);
	B = -2 * (x1 - x0);

	//octants 1 and 8
	if ( abs(x1 - x0) >= abs(y1 - y0) ) {

		//octant 1
		if ( A > 0 ) {

			d = A + B/2;
			while ( x < x1 ) {
				plot( s, c, x, y );
				if ( d > 0 ) {
					y+= 1;
					d+= B;
				}
				x++;
				d+= A;
			} //end octant 1 while
			plot( s, c, x1, y1 );
		} //end octant 1

		//octant 8
		else {
			d = A - B/2;

			while ( x < x1 ) {
				//printf("(%d, %d)\n", x, y);
				plot( s, c, x, y );
				if ( d < 0 ) {
					y-= 1;
					d-= B;
				}
				x++;
				d+= A;
			} //end octant 8 while
			plot( s, c, x1, y1 );
		} //end octant 8
	}//end octants 1 and 8

	//octants 2 and 7
	else {

		//octant 2
		if ( A > 0 ) {
			d = A/2 + B;

			while ( y < y1 ) {
				plot( s, c, x, y );
				if ( d < 0 ) {
					x+= 1;
					d+= A;
				}
				y++;
				d+= B;
			} //end octant 2 while
			plot( s, c, x1, y1 );
		} //end octant 2

		//octant 7
		else {
			d = A/2 - B;

			while ( y > y1 ) {
				plot( s, c, x, y );
				if ( d > 0 ) {
					x+= 1;
					d+= A;
				}
				y--;
				d-= B;
			} //end octant 7 while
			plot( s, c, x1, y1 );
		} //end octant 7
	}//end octants 2 and 7
} //end draw_line
