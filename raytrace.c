#include <stdio.h>
#include <math.h>

#define PI 3.141592654

typedef struct point{
    float x;
    float y;
    float z;
} point;

typedef struct vector{
    float x;
    float y;
    float z;
} vector;

typedef struct color{
    int r;
    int g;
    int b;
} color;

typedef struct ellipsoid{
    float x;
    float y;
    float z;
    float a;
    float b;
    float c;
} ellipsoid;

typedef struct sphere{
    float x;
    float y;
    float z;
    float r;
} sphere;

point AddToPoint(point p, vector v, float magnitude);
vector vectorTo(point from, point to);
void normalizeVector(vector* v);
void movePoint(point* p, vector v, float magnitude);
void setPoint(point* p, point s);
color getColor(vector v);

int main(){
    //TODO get fill in these values from file
    point camPos = {0, 0, 0};
    vector camDir = {0, 0, -1};
    float viewDist = 1;
    float vfov = 70;
    int imageHeight = 4;
    int imageWidth = 8;
    color bgColor = {0, 0, 0};

    //find hfov
    float hfov = atan( (tan(vfov/2)/(imageWidth/imageHeight)) ) * 2;

    //convert vfov to radians
    float vfovRad = (vfov * PI) / 180;
    float hfovRad = (hfov * PI) / 180;

    //locate window corners
    float vertDist = 2/cos(vfovRad);
    float horiDist = 2/cos(hfovRad);

    //half of the window height
    float windowHHeight = sqrt(viewDist * viewDist + vertDist* vertDist);
    //half of the window width
    float windowHWidth = sqrt(viewDist * viewDist + horiDist * horiDist);

    point tr = {windowHHeight, windowHWidth, viewDist};
    point tl = {-windowHHeight, windowHWidth, viewDist};
    point br = {windowHHeight, -windowHWidth, viewDist};
    point bl = {-windowHHeight, -windowHWidth, viewDist};

    //TODO create PPM file.
    /* Format:
    P3
    # filename
    width height
    255
    r g b
    r g b
    r g b
    .
    .
    .
    */
    FILE *fp;
    fp = fopen ("image.ppm", "w+");
    fprintf(fp, "P3\n# image\n%d %d\n%d\n", imageWidth, imageHeight, 255);

    /*get pixel length vector by finding vector between adjacent corners and dividing by the corresponding resolution
    the resulting vector would be 1 pixel long, so divide that by half for each row/col to start in the middle of a pixel
    then add the vector to jump to the next pixels center*/
    vector hIncr = {(tr.x - tl.x)/imageWidth, (tr. y - tl. y)/imageWidth, (tr.z - tl.z)/imageWidth};
    vector vIncr = {(bl.x - tl.x)/imageHeight, (bl. y - tl. y)/imageHeight, (bl.z - tl.z)/imageHeight};
    printf("vIncr: %f, %f, %f\n", vIncr.x, vIncr.y, vIncr.z);
    printf("hIncr: %f, %f, %f\n", hIncr.x, hIncr.y, hIncr.z);
    //loop through each pixel
    //Target point is the center of the current pixel to be checked. start position is center of top left pixel
    point target = AddToPoint(tl, vIncr, 0.5);
    target = AddToPoint(target, hIncr, 0.5);
    point targetTemp = target;
    for(int i = 0; i < imageHeight; i++){
        for(int j = 0; j < imageWidth; j++){
            vector traceme = vectorTo(camPos, targetTemp);
            normalizeVector(&traceme);
            printf("target: %f, %f, %f\n", targetTemp.x, targetTemp.y, targetTemp.z);
            printf("trace: %f, %f, %f\n", traceme.x, traceme.y, traceme.z);
            //get color
            color c = getColor(traceme);
            fprintf(fp, "%d %d %d\n", c.r, c.b, c.g);
            //set target to next pixel in row
            movePoint(&targetTemp, hIncr, 1);
        }
        printf("\n");
        //set target to first pixel in next column
        //setPoint(&target, {});
        movePoint(&target, vIncr, 1);
        targetTemp = target;
    }
    fclose(fp);

    printf("tr: %f, %f, %f\ntl: %f, %f, %f\nbr: %f, %f, %f\nbl: %f, %f, %f\n", 
        tr.x,tr.y,tr.z, tl.x,tl.y,tl.z, br.x,br.y,br.z, bl.x,bl.y,bl.z);

    
    return 1;
}

point AddToPoint(point p, vector v, float magnitude){
    point newp = {p.x + (magnitude * v.x), p.y + (magnitude * v.y), p.z + (magnitude * v.z)};
    return newp;
}

vector vectorTo(point from, point to){
    vector v = {to.x - from.x, to.y - from.y, to.z - from.z};
    return v;
}

void normalizeVector(vector* v){
    float length = sqrt(v->x*v->x + v->y*v->y + v->z*v->z);
    v->x /= length;
    v->y /= length;
    v->z /= length;
}

void movePoint(point* p, vector v, float magnitude){
    p->x += (v.x * magnitude);
    p->y += (v.y * magnitude);
    p->z += (v.z * magnitude);
}

void setPoint(point* p, point s){
    p->x = s.x;
    p->y = s.y;
    p->z = s.z;
}

color getColor(vector v){
    color c = {0, 0, 0};
    return c;
}