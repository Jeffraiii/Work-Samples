/* **************************
 * CSCI 420
 * Assignment 3 Raytracer
 * Name: Tianmu Lei
 * *************************
*/

#ifdef WIN32
  #include <windows.h>
#endif

#if defined(WIN32) || defined(linux)
  #include <GL/gl.h>
  #include <GL/glut.h>
#elif defined(__APPLE__)
  #include <OpenGL/gl.h>
  #include <GLUT/glut.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef WIN32
  #define strcasecmp _stricmp
#endif

#include <imageIO.h>
#include <glm/glm.hpp>
#include <algorithm>
#include <cmath>
#include <vector>

#define MAX_TRIANGLES 20000
#define MAX_SPHERES 100
#define MAX_LIGHTS 100
# define M_PI 3.14159265358979323846  /* pi */

char * filename = NULL;

//different display modes
#define MODE_DISPLAY 1
#define MODE_JPEG 2

int mode = MODE_DISPLAY;

//you may want to make these smaller for debugging purposes
#define WIDTH 640
#define HEIGHT 480

//the field of view of the camera
#define fov 60.0

unsigned char buffer[HEIGHT][WIDTH][3];

struct Vertex
{
  double position[3];
  double color_diffuse[3];
  double color_specular[3];
  double normal[3];
  double shininess;
};

struct Triangle
{
  Vertex v[3];
};

struct Sphere
{
  double position[3];
  double color_diffuse[3];
  double color_specular[3];
  double shininess;
  double radius;
};

struct Light
{
  double position[3];
  double color[3];
};

struct Ray
{
	glm::dvec3 origin;
	glm::dvec3 direction;
};

class Color
{
public:
	Color(double x, double y, double z) {
		r = x;
		g = y;
		b = z;
	}
	double r;
	double g;
	double b;
};

Triangle triangles[MAX_TRIANGLES];
Sphere spheres[MAX_SPHERES];
Light lights[MAX_LIGHTS];
double ambient_light[3];

int num_triangles = 0;
int num_spheres = 0;
int num_lights = 0;

void plot_pixel_display(int x,int y,unsigned char r,unsigned char g,unsigned char b);
void plot_pixel_jpeg(int x,int y,unsigned char r,unsigned char g,unsigned char b);
void plot_pixel(int x,int y,unsigned char r,unsigned char g,unsigned char b);

double a = (double)WIDTH / (double)HEIGHT;
glm::dvec3 TL = glm::dvec3(-a * tan(glm::degrees((double)fov / 2.0)), tan(glm::degrees((double)fov / 2.0)), -1.0);
glm::dvec3 TR = glm::dvec3(a * tan(glm::degrees((double)fov / 2.0)), tan(glm::degrees((double)fov / 2.0)), -1.0);
glm::dvec3 BL = glm::dvec3(-a * tan(glm::degrees((double)fov / 2.0)), -tan(glm::degrees((double)fov / 2.0)), -1.0);
glm::dvec3 BR = glm::dvec3(a * tan(glm::degrees((double)fov / 2.0)), -tan(glm::degrees((double)fov / 2.0)), -1.0);
double unitX = (TR.x - TL.x) / (double) WIDTH;
double unitY = (BL.y - TL.y) / (double)HEIGHT;
glm::dvec3 CAMERA_POS = glm::dvec3(0.0, 0.0, 0.0);

Ray generate_ray(int x, int y) {
	//CITE: https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-generating-camera-rays/generating-camera-rays
	double delta = tan(((double)fov / 2.0) * (M_PI/180.0));
	double NDC_x = (x + 0.5) / WIDTH;
	double NDC_y = (y + 0.5) / HEIGHT;
	double screen_x = 2 * NDC_x - 1;
	double screen_y = 2 * NDC_y - 1;
	double camera_x = screen_x * a * delta;
	double camera_y = screen_y * delta;

	//glm::dvec3 dir = TL + glm::dvec3((double)x * unitX, (double)y * unitY, -1.0);
	glm::dvec3 dir = glm::dvec3(camera_x, camera_y, -1.0);
	Ray r;
	r.origin = CAMERA_POS;
	r.direction = glm::normalize(dir - CAMERA_POS);
	return r;
}



bool intersect_sphere(Ray ray, Sphere sphere, glm::dvec3& intersection) {
	double b = 2.0 * (ray.direction.x * (ray.origin.x - sphere.position[0]) + ray.direction.y * (ray.origin.y - sphere.position[1]) + ray.direction.z * (ray.origin.z - sphere.position[2]));
	double c = pow(ray.origin.x - sphere.position[0], 2) + pow(ray.origin.y - sphere.position[1], 2) + pow(ray.origin.z - sphere.position[2], 2) - pow(sphere.radius, 2);
	double discriminant = b * b - 4 * c;
	if (discriminant < 0.0) {
		intersection = glm::dvec3(0.0, 0.0, 0.0);
		return false;
	}
	double t0 = (-b + sqrt(discriminant)) / 2;
	double t1 = (-b - sqrt(discriminant)) / 2;
	double t;
	if (t0 > 0.0 && t1 > 0.0) {
		t = glm::min(t0, t1);
		intersection = ray.origin + t * ray.direction;
		return true;
	}
	else if (t0 < 0.0 && t1 < 0.0) {
		intersection = glm::dvec3(0.0, 0.0, 0.0);
		return false;
	}
	else {
		t = glm::max(t0, t1);
		intersection = ray.origin + t * ray.direction;
		return true;
	}
}

bool intersect_triangle(Ray ray, Triangle tri, glm::dvec3& intersection) {
	glm::dvec3 vA = glm::dvec3(tri.v[0].position[0], tri.v[0].position[1], tri.v[0].position[2]);
	glm::dvec3 vB = glm::dvec3(tri.v[1].position[0], tri.v[1].position[1], tri.v[1].position[2]);
	glm::dvec3 vC = glm::dvec3(tri.v[2].position[0], tri.v[2].position[1], tri.v[2].position[2]);
	glm::dvec3 normal = glm::normalize(glm::cross(vB - vA, vC - vA));
	if (glm::dot(ray.direction, normal) == 0.0) {
		intersection = glm::dvec3(0.0, 0.0, 0.0);
		return false;
	}
	double t = -(glm::dot((ray.origin - vA), normal) / (glm::dot(normal, ray.direction)));

	if (t < 0.0) {
		intersection = glm::dvec3(0.0, 0.0, 0.0);
		return false;
	}
	intersection = ray.origin + t * ray.direction;
	glm::dvec3 winding;
	winding = glm::normalize(glm::cross(vB - vA, intersection - vA));
	if (glm::dot(winding, normal) < 0.0) return false;
	winding = glm::normalize(glm::cross(vC - vB, intersection - vB));
	if (glm::dot(winding, normal) < 0.0) return false;
	winding = glm::normalize(glm::cross(vA - vC, intersection - vC));
	if (glm::dot(winding, normal) < 0.0) return false;
	return true;
}

bool check_shadow_ray(Ray shadow, Sphere& sphere, Light l) {
	bool hitSphere = false;
	bool hitTri = false;
	double dist = glm::length(glm::dvec3(l.position[0], l.position[1], l.position[2]) - shadow.origin);
	for (int j = 0; j < num_spheres; j++) {
		glm::dvec3 temp;
		if (spheres + j != &sphere) {
			if (intersect_sphere(shadow, spheres[j], temp)) {
				if (glm::length(temp - shadow.origin) < dist) {
					hitSphere = true;
				}
			}
		}

	}
	for (int j = 0; j < num_triangles; j++) {
		glm::dvec3 temp;
		if (intersect_triangle(shadow, triangles[j], temp)) {
			if (glm::length(temp - shadow.origin) < dist) {
				hitTri = true;
			}
		}
	}
	return hitSphere || hitTri;
}

bool check_shadow_ray(Ray shadow, Triangle& tri, Light l) {
	bool hitSphere = false;
	bool hitTri = false;
	double dist = glm::length(glm::dvec3(l.position[0], l.position[1], l.position[2]) - shadow.origin);
	for (int j = 0; j < num_spheres; j++) {
		glm::dvec3 temp;
		if (intersect_sphere(shadow, spheres[j], temp)) {
			if (glm::length(temp - shadow.origin) < dist) {
				hitSphere = true;
			}
		}
	}
	for (int j = 0; j < num_triangles; j++) {
		glm::dvec3 temp;
		if (triangles + j != &tri) {
			if (intersect_triangle(shadow, triangles[j], temp)) {
				if (glm::length(temp - shadow.origin) < dist) {
					hitTri = true;
				}
			}
		}
	}
	return hitSphere || hitTri;
}


glm::vec3 barycentric_normal(double alpha, double beta, double gamma, Triangle& tri) {
	glm::dvec3 n0 = glm::dvec3(tri.v[0].normal[0], tri.v[0].normal[1], tri.v[0].normal[2]);
	glm::dvec3 n1 = glm::dvec3(tri.v[1].normal[0], tri.v[1].normal[1], tri.v[1].normal[2]);
	glm::dvec3 n2 = glm::dvec3(tri.v[2].normal[0], tri.v[2].normal[1], tri.v[2].normal[2]);
	//return alpha * n0 + beta * n1 + gamma * n2;
	return gamma * n0 + alpha * n1 + beta * n2;

}

glm::vec3 barycentric_diffuse(double alpha, double beta, double gamma, Triangle& tri) {
	glm::dvec3 n0 = glm::dvec3(tri.v[0].color_diffuse[0], tri.v[0].color_diffuse[1], tri.v[0].color_diffuse[2]);
	glm::dvec3 n1 = glm::dvec3(tri.v[1].color_diffuse[0], tri.v[1].color_diffuse[1], tri.v[1].color_diffuse[2]);
	glm::dvec3 n2 = glm::dvec3(tri.v[2].color_diffuse[0], tri.v[2].color_diffuse[1], tri.v[2].color_diffuse[2]);
	return gamma * n0 + alpha * n1 + beta * n2;
}

glm::vec3 barycentric_specular(double alpha, double beta, double gamma, Triangle& tri) {
	glm::dvec3 n0 = glm::dvec3(tri.v[0].color_specular[0], tri.v[0].color_specular[1], tri.v[0].color_specular[2]);
	glm::dvec3 n1 = glm::dvec3(tri.v[1].color_specular[0], tri.v[1].color_specular[1], tri.v[1].color_specular[2]);
	glm::dvec3 n2 = glm::dvec3(tri.v[2].color_specular[0], tri.v[2].color_specular[1], tri.v[2].color_specular[2]);
	return gamma * n0 + alpha * n1 + beta * n2;
}

double barycentric_shininess(double alpha, double beta, double gamma, Triangle& tri) {
	//return alpha * tri.v[0].shininess + beta * tri.v[1].shininess + gamma * tri.v[2].shininess;
	return gamma * tri.v[0].shininess + alpha * tri.v[1].shininess + beta * tri.v[2].shininess;
}

Color illuminate_sphere(Sphere& sphere, glm::dvec3 intersection) {
	Color result(0.0,0.0,0.0);
	glm::dvec3 center = glm::dvec3(sphere.position[0], sphere.position[1], sphere.position[2]);
	glm::dvec3 normal = glm::normalize(intersection - center);
	for (int i = 0; i < num_lights; i++) {
		Ray shadow;
		shadow.origin = intersection;
		shadow.direction = glm::normalize(glm::dvec3(lights[i].position[0], lights[i].position[1], lights[i].position[2]) - shadow.origin);
		
		if (!check_shadow_ray(shadow, sphere, lights[i])) {
			glm::dvec3 l = shadow.direction;
			glm::dvec3 r = -glm::reflect(l, normal);
			glm::dvec3 v = glm::normalize(CAMERA_POS - shadow.origin);
			result.r += lights[i].color[0] * (sphere.color_diffuse[0] * glm::max(glm::dot(l, normal), 0.0) + sphere.color_specular[0] * pow(glm::max(glm::dot(r, v), 0.0), sphere.shininess));
			result.g += lights[i].color[1] * (sphere.color_diffuse[1] * glm::max(glm::dot(l, normal), 0.0) + sphere.color_specular[1] * pow(glm::max(glm::dot(r, v), 0.0), sphere.shininess));
			result.b += lights[i].color[2] * (sphere.color_diffuse[2] * glm::max(glm::dot(l, normal), 0.0) + sphere.color_specular[2] * pow(glm::max(glm::dot(r, v), 0.0), sphere.shininess));
		}
	}
	result.r += ambient_light[0];
	result.g += ambient_light[1];
	result.b += ambient_light[2];
	result.r = glm::min(result.r, 1.0);
	result.g = glm::min(result.g, 1.0);
	result.b = glm::min(result.b, 1.0);
	return result;
}

Color illuminate_triangle(Triangle& tri, glm::dvec3 intersection) {
	Color result(0.0, 0.0, 0.0);
	for (int i = 0; i < num_lights; i++) {
		Ray shadow;
		shadow.origin = intersection;
		shadow.direction = glm::normalize(glm::dvec3(lights[i].position[0], lights[i].position[1], lights[i].position[2]) - shadow.origin);
		if (!check_shadow_ray(shadow, tri, lights[i])) {
			//CITE: Christer Ericson's Real-Time Collision Detection (textbook for ITP 485)
			glm::dvec3 vA = glm::dvec3(tri.v[0].position[0], tri.v[0].position[1], tri.v[0].position[2]);
			glm::dvec3 vB = glm::dvec3(tri.v[1].position[0], tri.v[1].position[1], tri.v[1].position[2]);
			glm::dvec3 vC = glm::dvec3(tri.v[2].position[0], tri.v[2].position[1], tri.v[2].position[2]);
			glm::dvec3 v0 = vB - vA;
			glm::dvec3 v1 = vC - vA;
			glm::dvec3 v2 = intersection - vA;
			double d00 = glm::dot(v0, v0);
			double d01 = glm::dot(v0, v1);
			double d11 = glm::dot(v1, v1);
			double d20 = glm::dot(v2, v0);
			double d21 = glm::dot(v2, v1);
			double denom = d00 * d11 - d01 * d01;
			double alpha = (d11 * d20 - d01 * d21) / denom;
			double beta = (d00 * d21 - d01 * d20) / denom;
			double gamma = 1.0f - alpha - beta;

			glm::dvec3 normal = barycentric_normal(alpha, beta, gamma, tri);
			glm::dvec3 diffuse = barycentric_diffuse(alpha, beta, gamma, tri);
			glm::dvec3 specular = barycentric_specular(alpha, beta, gamma, tri);
			glm::dvec3 lc = glm::dvec3(lights[i].color[0], lights[i].color[1], lights[i].color[2]);
			glm::dvec3 l = shadow.direction;
			glm::dvec3 r = -glm::reflect(l, normal);
			glm::dvec3 v = glm::normalize(CAMERA_POS - shadow.origin);
			double shine = barycentric_shininess(alpha, beta, gamma, tri);
			result.r += lights[i].color[0] * (diffuse.x * glm::max(glm::dot(l, normal), 0.0) + specular.x * pow(glm::max(glm::dot(r, v), 0.0), shine));
			result.g += lights[i].color[1] * (diffuse.y * glm::max(glm::dot(l, normal), 0.0) + specular.y * pow(glm::max(glm::dot(r, v), 0.0), shine));
			result.b += lights[i].color[2] * (diffuse.z * glm::max(glm::dot(l, normal), 0.0) + specular.z * pow(glm::max(glm::dot(r, v), 0.0), shine));
		}
	}
	result.r += ambient_light[0];
	result.g += ambient_light[1];
	result.b += ambient_light[2];
	result.r = glm::min(result.r, 1.0);
	result.g = glm::min(result.g, 1.0);
	result.b = glm::min(result.b, 1.0);
	return result;
}

void trace(Ray ray, double& r, double& g, double& b) {
	Color c(0.0, 0.0, 0.0);
	double minSphere = -1.0, minTriangle = -1.0;
	glm::dvec3 minSIntersection, minTIntersection;
	int minSphereIndex = 0, minTriangleIndex = 0;
	bool hitSphere = false;
	bool hitTriangle = false;
	for (int i = 0; i < num_spheres; i++) {
		glm::dvec3 temp;
		if (intersect_sphere(ray, spheres[i], temp)) {
			hitSphere = true;
			if (minSphere == -1.0) {
				minSphere = glm::length(temp);
				minSIntersection = temp;
				minSphereIndex = i;
			}
			else {
				minSphere = glm::min(minSphere, glm::length(temp));
				if (minSphere == glm::length(temp)) {
					minSIntersection = temp;
					minSphereIndex = i;
				}
			}
		}
	}
	for (int i = 0; i < num_triangles; i++) {
		glm::dvec3 temp;
		if (intersect_triangle(ray, triangles[i], temp)) {
			hitTriangle = true;
			if (minTriangle == -1.0) {
				minTriangle = glm::length(temp);
				minTIntersection = temp;
				minTriangleIndex = i;
			}
			else {
				minTriangle = glm::min(minTriangle, glm::length(temp));
				if (minTriangle == glm::length(temp)) {
					minTIntersection = temp;
					minTriangleIndex = i;
				}
			}
		}
	}
	if (hitSphere && hitTriangle) {
		if (minSphere <= minTriangle) {
			c = illuminate_sphere(spheres[minSphereIndex], minSIntersection);
		}
		else if (minTriangle < minSphere) {
			c = illuminate_triangle(triangles[minTriangleIndex], minTIntersection);
		}
	}
	else if (hitSphere) {
		c = illuminate_sphere(spheres[minSphereIndex], minSIntersection);
	}
	else if (hitTriangle) {
		c = illuminate_triangle(triangles[minTriangleIndex], minTIntersection);
	}
	else if (!hitSphere && !hitTriangle) {
		r = 1.0;
		g = 1.0;
		b = 1.0;
		return;
	}
	r = c.r;
	g = c.g;
	b = c.b;
}


//MODIFY THIS FUNCTION
void draw_scene()
{
  //a simple test output
  for(unsigned int x=0; x<WIDTH; x++)
  {
    glPointSize(2.0);  
    glBegin(GL_POINTS);
    for(unsigned int y=0; y<HEIGHT; y++)
    {
		Ray ray = generate_ray(x, y);
		//ray.direction = glm::dvec3(0, 0, -1);
		double r, g, b;
		trace(ray, r, g, b);
		plot_pixel(x, y, r * 255, g * 255, b * 255);
    }
    glEnd();
    glFlush();
  }
  printf("Done!\n"); fflush(stdout);
}

void plot_pixel_display(int x, int y, unsigned char r, unsigned char g, unsigned char b)
{
  glColor3f(((float)r) / 255.0f, ((float)g) / 255.0f, ((float)b) / 255.0f);
  glVertex2i(x,y);
}

void plot_pixel_jpeg(int x, int y, unsigned char r, unsigned char g, unsigned char b)
{
  buffer[y][x][0] = r;
  buffer[y][x][1] = g;
  buffer[y][x][2] = b;
}

void plot_pixel(int x, int y, unsigned char r, unsigned char g, unsigned char b)
{
  plot_pixel_display(x,y,r,g,b);
  if(mode == MODE_JPEG)
    plot_pixel_jpeg(x,y,r,g,b);
}

void save_jpg()
{
  printf("Saving JPEG file: %s\n", filename);

  ImageIO img(WIDTH, HEIGHT, 3, &buffer[0][0][0]);
  if (img.save(filename, ImageIO::FORMAT_JPEG) != ImageIO::OK)
    printf("Error in Saving\n");
  else 
    printf("File saved Successfully\n");
}

void parse_check(const char *expected, char *found)
{
  if(strcasecmp(expected,found))
  {
    printf("Expected '%s ' found '%s '\n", expected, found);
    printf("Parse error, abnormal abortion\n");
    exit(0);
  }
}

void parse_doubles(FILE* file, const char *check, double p[3])
{
  char str[100];
  fscanf(file,"%s",str);
  parse_check(check,str);
  fscanf(file,"%lf %lf %lf",&p[0],&p[1],&p[2]);
  printf("%s %lf %lf %lf\n",check,p[0],p[1],p[2]);
}

void parse_rad(FILE *file, double *r)
{
  char str[100];
  fscanf(file,"%s",str);
  parse_check("rad:",str);
  fscanf(file,"%lf",r);
  printf("rad: %f\n",*r);
}

void parse_shi(FILE *file, double *shi)
{
  char s[100];
  fscanf(file,"%s",s);
  parse_check("shi:",s);
  fscanf(file,"%lf",shi);
  printf("shi: %f\n",*shi);
}

int loadScene(char *argv)
{
  FILE * file = fopen(argv,"r");
  int number_of_objects;
  char type[50];
  Triangle t;
  Sphere s;
  Light l;
  fscanf(file,"%i", &number_of_objects);

  printf("number of objects: %i\n",number_of_objects);

  parse_doubles(file,"amb:",ambient_light);

  for(int i=0; i<number_of_objects; i++)
  {
    fscanf(file,"%s\n",type);
    printf("%s\n",type);
    if(strcasecmp(type,"triangle")==0)
    {
      printf("found triangle\n");
      for(int j=0;j < 3;j++)
      {
        parse_doubles(file,"pos:",t.v[j].position);
        parse_doubles(file,"nor:",t.v[j].normal);
        parse_doubles(file,"dif:",t.v[j].color_diffuse);
        parse_doubles(file,"spe:",t.v[j].color_specular);
        parse_shi(file,&t.v[j].shininess);
      }

      if(num_triangles == MAX_TRIANGLES)
      {
        printf("too many triangles, you should increase MAX_TRIANGLES!\n");
        exit(0);
      }
      triangles[num_triangles++] = t;
    }
    else if(strcasecmp(type,"sphere")==0)
    {
      printf("found sphere\n");

      parse_doubles(file,"pos:",s.position);
      parse_rad(file,&s.radius);
      parse_doubles(file,"dif:",s.color_diffuse);
      parse_doubles(file,"spe:",s.color_specular);
      parse_shi(file,&s.shininess);

      if(num_spheres == MAX_SPHERES)
      {
        printf("too many spheres, you should increase MAX_SPHERES!\n");
        exit(0);
      }
      spheres[num_spheres++] = s;
    }
    else if(strcasecmp(type,"light")==0)
    {
      printf("found light\n");
      parse_doubles(file,"pos:",l.position);
      parse_doubles(file,"col:",l.color);

      if(num_lights == MAX_LIGHTS)
      {
        printf("too many lights, you should increase MAX_LIGHTS!\n");
        exit(0);
      }
      lights[num_lights++] = l;
    }
    else
    {
      printf("unknown type in scene description:\n%s\n",type);
      exit(0);
    }
  }
  return 0;
}

void display()
{
}

void init()
{
  glMatrixMode(GL_PROJECTION);
  glOrtho(0,WIDTH,0,HEIGHT,1,-1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glClearColor(0,0,0,0);
  glClear(GL_COLOR_BUFFER_BIT);
}

void idle()
{
  //hack to make it only draw once
  static int once=0;
  if(!once)
  {
    draw_scene();
    if(mode == MODE_JPEG)
      save_jpg();
  }
  once=1;
}

int main(int argc, char ** argv)
{
  if ((argc < 2) || (argc > 3))
  {  
    printf ("Usage: %s <input scenefile> [output jpegname]\n", argv[0]);
    exit(0);
  }
  if(argc == 3)
  {
    mode = MODE_JPEG;
    filename = argv[2];
  }
  else if(argc == 2)
    mode = MODE_DISPLAY;

  glutInit(&argc,argv);
  loadScene(argv[1]);

  glutInitDisplayMode(GLUT_RGBA | GLUT_SINGLE);
  glutInitWindowPosition(0,0);
  glutInitWindowSize(WIDTH,HEIGHT);
  int window = glutCreateWindow("Ray Tracer");
  glutDisplayFunc(display);
  glutIdleFunc(idle);
  init();
  glutMainLoop();
}

