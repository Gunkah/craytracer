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
    float r;
    float g;
    float b;
} color;

typedef struct ellipsoid{
    float x;
    float y;
    float z;
    float a;
    float b;
    float c;
} ellipsoid;

/*
typedef struct shape{
    int type;   
} shape;
*/

typedef struct sphere{
    //shape base;
    point pos;
    float r;
    color color;
} sphere;

typedef struct light{
    point pos;
    color power;
} light;

point AddToPoint(point p, vector v, float magnitude);
vector vectorTo(point from, point to);
vector normalizeVector(vector v);
void movePoint(point* p, vector v, float magnitude);
void setPoint(point* p, point s);
color getColor(point o, vector v, sphere s[], light l[], int scount, int lcount);
color getLighting(point s, vector n, light l);
float dotProd(vector a, vector b);
float checkSphere(point origin, vector dir, sphere s);
float distBetweenPoints(point a, point b);

int main(){
    //TODO get fill in these values from file
    point camPos = {0, 0, 0};
    vector camDir = {0, 0, -1};
    float viewDist = 1;
    float vfov = 70;
    int imageHeight = 500;
    int imageWidth = 1000;
    float aspect = imageWidth/imageHeight;
    color bgColor = {0, 0, 0};

    //convert fov to radians
    float vfovRad = (vfov * PI) / 180;

    //find hfov
    float hfovRad = atan( tan(vfovRad/2)*aspect ) * 2;
    float hfov = (hfovRad * 180) / PI;

    printf("vfovrad = %f\n", vfovRad);
    printf("vfovdeg = %f\n", vfov);
    printf("hfovrad = %f\n", hfovRad);
    printf("hfovdeg = %f\n\n", hfov);

    //locate window corners
    float vertDist = viewDist/cos(vfovRad/2);
    float horiDist = viewDist/cos(hfovRad/2);
    printf("Top edge dist = %f\n", vertDist);
    printf("Side edge dist = %f\n", horiDist);

    //half of the window height
    float windowHHeight = sqrt(vertDist*vertDist - (viewDist*viewDist));
    //half of the window width
    float windowHWidth = sqrt(horiDist*horiDist - (viewDist*viewDist));

    point tr = {windowHWidth, windowHHeight, viewDist};
    point tl = {-windowHWidth, windowHHeight, viewDist};
    point br = {windowHWidth, -windowHHeight, viewDist};
    point bl = {-windowHWidth, -windowHHeight, viewDist};

    //array holding the shapes in the scene
    int scount = 4;
    sphere sp = {{0, 0, 8}, 2, {255, 255, 255}};
    sphere sp2 = {{-3, -3, 7}, 1, {255, 255, 0}};
    sphere sp3 = {{-6, 3, 6}, 1, {255, 0, 255}};
    sphere sp4 = {{-4, 0, 7}, 0.5, {255, 255, 255}};
    sphere spheres[] = {sp, sp2, sp3, sp4};

    //array holding lights
    int lcount = 1;
    light l = {{-10, 0, 8}, {1, 1, 1}};
    light lights[] = {l};

    printf("%f, %f, %f, %f\n", sp.pos.x, sp.pos.y, sp.pos.z, sp.r);
    //create ppm file
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
    vector vIncr = {(bl.x - tl.x)/imageHeight, (bl.y - tl.y)/imageHeight, (bl.z - tl.z)/imageHeight};
    vector hIncr = {(tr.x - tl.x)/imageWidth, (tr.y - tl.y)/imageWidth, (tr.z - tl.z)/imageWidth};
    printf("vIncr: %f, %f, %f\n", vIncr.x, vIncr.y, vIncr.z);
    printf("hIncr: %f, %f, %f\n", hIncr.x, hIncr.y, hIncr.z);


    //loop through each pixel
    //Target point is the center of the current pixel to be checked. start position is center of top left pixel
    point target = AddToPoint(tl, vIncr, 0.5);
    target = AddToPoint(target, hIncr, 0.5);
    point targetTemp = target;

    for(int i = 0; i < imageHeight; i++){
        for(int j = 0; j < imageWidth; j++){
            vector traceme = normalizeVector(vectorTo(camPos, targetTemp));
            //printf("target: %f, %f, %f\n", targetTemp.x, targetTemp.y, targetTemp.z);
            //printf("trace: %f, %f, %f\n", traceme.x, traceme.y, traceme.z);
            //get color
            color c = getColor(camPos, traceme, spheres, lights, scount, lcount);
            fprintf(fp, "%d %d %d\n", (int)c.r, (int)c.b, (int)c.g);
            //set target to next pixel in row
            movePoint(&targetTemp, hIncr, 1);
        }
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

vector normalizeVector(vector v){
    float ln = sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
    vector nv = {v.x/ln, v.y/ln, v.z/ln};
    return nv;
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

color getColor(point o, vector v, sphere sp[], light l[], int scount, int lcount){
    //check for intersection
    int nearest = -1;  //index of nearest intersection
    float dist = -1; //distance of that intersection
    for(int i = 0; i < scount; i++){
       float distToSphere = checkSphere(o, v, sp[i]);
       //printf("t: %f\n", distToSphere);
       if(distToSphere>=0){
           if(dist<0||distToSphere<dist){
               nearest = i;
               dist = distToSphere;
           }
       }
    }
    color c = {0, 0, 0};
    if(nearest>=0){
        //calculate illumination
        point s = AddToPoint(o, v, dist);
        for(int i = 0; i < lcount; i++){

            //vectors to compare
            vector normal = normalizeVector(vectorTo(sp[nearest].pos, s));
            point lp;
            lp.x = l[i].pos.x;
            lp.y = l[i].pos.y;
            lp.z = l[i].pos.z;
            float distToLight = distBetweenPoints(s, l[i].pos);
            vector toLight = normalizeVector(vectorTo(s, l[i].pos));
            int blocked = 0;

            //check for objects in the way
            for(int j = 0; j < scount; j++){
                if(j == nearest){}//skip itself
                else{
                    float distToSphere = checkSphere(s, toLight, sp[j]);
                    
                    //printf("checking point: %f %f %f, vector: %f %f %f\n", s.x, s.y, s.z, toLight.x, toLight.y, toLight.z);
                    if(distToSphere>=0&&distToSphere<distToLight){
                        printf("%f %f\n", distToSphere, distToLight);
                        blocked = 1;
                        printf("%f %f %f blocked by sphere %d\n", s.x, s.y, s.z, j);
                        break;
                    }
                }
            }
            if(blocked==0){
                //cosine of angle between the vectors will give the light intensity
                float pow = dotProd(normal, toLight);
                if(pow>0){
                    c.r += (sp[nearest].color.r * pow * l[i].power.r);
                    c.g += (sp[nearest].color.g * pow * l[i].power.g);
                    c.b += (sp[nearest].color.b * pow * l[i].power.b);
                    //printf("Color updated: %f %f %f\n", c.r, c.g, c.b);
                }
            }
            else{
                c.r = 255;
            }
        }
    }
    //clamp colors
    if(c.r>=c.b&&c.r>=c.g){
        if(c.r>255){
            //printf("Clamping: %f %f %f to ", c.r, c.g, c.b);
            c.b /= c.r;
            c.g /= c.r;
            c.r /= c.r;
            c.b *= 255;
            c.g *= 255;
            c.r *= 255;
            //printf("%f %f %f\n", c.r, c.g, c.b);
        }
    }
    else if(c.b>=c.r&&c.b>=c.g){
        if(c.b>255){
            //printf("Clamping: %f %f %f to ", c.r, c.g, c.b);
            c.r /= c.b;
            c.g /= c.b;
            c.b /= c.b;
            c.b *= 255;
            c.g *= 255;
            c.r *= 255;
            //printf("%f %f %f\n", c.r, c.g, c.b);
        }
    }
    else if(c.g>=c.r&&c.g>=c.b){
        if(c.g>255){
            //printf("Clamping: %f %f %f to ", c.r, c.g, c.b);
            c.r /= c.g;
            c.b /= c.g;
            c.g /= c.g;
            c.b *= 255;
            c.g *= 255;
            c.r *= 255;
            //printf("%f %f %f\n", c.r, c.g, c.b);
        }
    }
    
    return c;
}

//Checks to see if line intersects with sphere, returns the distance from the origin to first intersection
float checkSphere(point origin, vector dir, sphere s){
    /*
    (origin.x + dir.x * t - s.x)^2 +
    (origin.y + dir.y * t - s.y)^2 +
    (origin.z + dir.z * t - s.z)^2 -
    s.r^2
    = 0
    */
   //ugly quadratic stuff
    float a = (dir.x*dir.x) + (dir.y*dir.y) + (dir.z*dir.z);
    float b = 2*origin.x*dir.x + 2*origin.y*dir.y + 2*origin.z*dir.z - 2*dir.x*s.pos.x - 2*dir.y*s.pos.y - 2*dir.z*s.pos.z;
    float c = origin.x*origin.x + origin.y*origin.y + origin.z*origin.z + s.pos.x*s.pos.x + s.pos.y*s.pos.y + s.pos.z*s.pos.z 
                - (2*origin.x*s.pos.x) - (2*origin.y*s.pos.y) - (2*origin.z*s.pos.z) - s.r*s.r;
    //printf("%f %f %f\n", a, b, c);

    //check discriminant
    float d = b*b - 4*a*c;
    float t = -1;

    //if 2 solutions
    if(d>0){
        
        float t1 = (-b+sqrt(d))/(2*a);
        float t2 = (-b-sqrt(d))/(2*a);
        if(t1>=0&&t2>=0){
            if(t1<t2){
                t = t1;
            }
            else{
                t = t2;
            }
        }
        else if(t1 >= 0){
            t = t1;
        }
        else if(t2 >= 0){
            t = t2;
        }
    }
    //if 1 solution
    else if(d==0){
        float t1 = (b*b)/(2*a);
        if( t1 >= 0){
            t = t1;
        }
    }
    return t;
}

float dotProd(vector a, vector b){
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

float distBetweenPoints(point a, point b){
    float d = sqrt((b.x - a.x)*(b.x - a.x) + (b.y - a.y)*(b.y - a.y) + (b.z - a.z)*(b.z - a.z));
    return d;
}