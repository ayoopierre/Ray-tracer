#include <stdio.h>
#include <SDL2/SDL.h>
#include <cmath>
#include "Screen.h"
#include "Screen.cpp"
#include "vector.h"
#include "color.h"
#include "ray.h"
#include "Sphere.h"
using std::sqrt;
using std::pow;

const int image_height = 800;
const int image_width = 800;
int offset = 100;
int number_of_jumps = 2;
point3 origin(0.5*image_width,0.5*image_height, -offset);


double hit_sphere(const Sphere& object, const ray& r) {
    vec3 oc = r.origin() - object.get_center();
    auto a = r.direction().length_squared();
    auto half_b = dot(oc, r.direction());
    auto c = oc.length_squared() - object.get_radius()*object.get_radius();
    auto discriminant = half_b*half_b - a*c;

    if (discriminant < 0) {
        return -1.0;
    } else {
        return (-half_b - sqrt(discriminant) ) / a;
    }
}

Uint32 map_color_to_uint32(color col){
    Uint32 ret = 0;
    ret += col.x() * 256;
    ret = ret << 8;
    ret += col.y() * 256;
    ret = ret << 8; 
    ret += col.z() * 256;
    ret = ret << 8; 
    ret += 0xFF; 
    return ret;
}

vec3 uint32_to_rgb(Uint32 color){
    Uint32 copy = color;
    copy = copy >> 8;
    double blue = (copy & (0xff));
    blue = blue / 255;
    copy = copy >> 8;
    double green = (copy & (0xff));
    green = green / 255;
    copy = copy >> 8;
    double red = (copy & (0xff));
    red = red /255;
    return vec3(red, green, blue);
}

ray ray_form_a_to_b(const point3& a, const point3& b){
    return ray(a, b-a);
}

ray reflected_ray(const vec3& normal, const vec3& in,const point3& intersection){
    return ray(intersection, in - 2*normal*dot(in, normal));
}

ray refracted_ray(const vec3& normal, const vec3& in, const point3& intersetion, double u){// u is ref_index1/ref_index2
    return ray(intersetion, u*in + normal*sqrt(1-u*u*dot(normal,in)*dot(normal,in)) - u * dot(normal, in) * normal);
}

int main(int argc, char* argv[]){
    bool check = false;
    Screen screen;
    if(screen.init()){
        printf("Window initialized...");
    }
    else{
        printf("SDL failed...");
    }
    screen.create_renderer();
    screen.set_skybox(0,0x82c1f5ff);//0x3070a6ff
    
    //setup objects in scene
    color white(1,1,1);
    color red(1,0,0);
    color violet(1,0,1);
    color green(0,1,0);
    double def_s = 0;
    double max_smoothness = 1;
    double def_refracitve_index = 1;
    double current_refractive_index = def_refracitve_index;
    color gorund_color(0.15625,0.859375,0.15625);
    //ground and spheres

    Sphere ground(point3(screen.WIDTH/2, -14850, 250), 15000, gorund_color,def_s,0,1);//Simply really wide ball

    point3 sphere_center(3*screen.WIDTH/4, screen.HEIGHT/2, 150);
    double sphere_radius = 100;
    Sphere object(sphere_center, sphere_radius,white,1,0,1); 

    point3 sphere_center2(screen.WIDTH/4, screen.HEIGHT/2, 150);
    double sphere_radius2 = 100;
    Sphere object2(sphere_center2, sphere_radius2, red, 0.006,0,1);

    point3 sphere_center3(screen.WIDTH/2, screen.HEIGHT/2, 350);
    double sphere_radius3 = 100;
    Sphere object3(sphere_center3, sphere_radius3, violet, def_s,0,1);

    //light sources postion

    int depth=-250;

    point3 light_source1(10,800,depth);
    point3 light_source2(0, 800, depth);
    point3 light_source3(30,800,depth);
    point3 light_source4(400,400,500);
    point3 light_source5(400,400,0);


    //lists of objects

    Sphere objects[] = {ground, object2, object};
    point3 light_sources[] = {light_source1, light_source2};

    //render code;
    for(int x=0; x<screen.WIDTH ; x++){
        for(int y=0; y<screen.HEIGHT ; y++){

            //setting up main ray origin(camera position) - > pixel on screen

            vec3 point_on_screen(x,y,10);
            vec3 direction = point_on_screen - origin; 
            ray r(origin, direction);
            color pixel = color(0,0,0);

            for(int i = 0; i < sizeof(objects)/sizeof(Sphere); i++){
                pixel = color(0,0,0);
                double res = hit_sphere(objects[i], r); //iterates thru all objects check for intersection with objects
                vec3 normal = unit_vector(r.at(res) - objects[i].get_center());

                if(res > 0){//if there are any real roots changes pixel color after all calculations

                    int light_sources_covered = 0;
                    for(int n = 0; n<sizeof(objects)/sizeof(Sphere);n++){
                        if(n == i){
                            continue;
                        }
                        for(int k = 0; k < sizeof(light_sources)/sizeof(point3); k++){
                            if(hit_sphere(objects[n],ray_form_a_to_b(light_sources[k],r.at(res)))>0){
                                light_sources_covered++;//iterates thru all light sources, and check whether route to is blocked or not
                            }
                        }
                    }

                    for (int l = 0; l < sizeof(light_sources)/sizeof(point3); l++){
                        vec3 to_light = light_sources[l] - r.at(res);
                        if(dot(normal, unit_vector(to_light))>=0){
                            pixel += dot(normal, unit_vector(to_light))*objects[i].get_color();//checks dot product of normal to the surface of a sphere and unit vector to the light source
                        }
                        else{
                            pixel += color(0,0,0);//if dot prod < 0 surface is not facing light source, so its not being lighted up
                        }
                    }
                    for(int m = 0; m<light_sources_covered;m++){
                        pixel = pixel - pixel/(sizeof(light_sources)/sizeof(point3));
                    }

                    //starting rendering of reflecting surfaces;
                    color reflected_color = uint32_to_rgb(screen.get_buffer_color(x, y));
                    ray reflected = reflected_ray(normal, r.direction(), r.at(res));

                    for(int j=0; j < sizeof(objects)/sizeof(Sphere); j++){
                        if(i==j){
                            continue;
                        }
                        //reflected_color = uint32_to_rgb(screen.get_buffer_color(x, y+10));
                        //reflected_color = color(0,0,0);
                        res = hit_sphere(objects[j], reflected);
                        normal = unit_vector(reflected.at(res) - objects[j].get_center());
                        double brightness = 0;
                        vec3 to_light_source;
                        if(res>0){
                            reflected_color = color(0,0,0);
                            for(int o = 0; o<sizeof(light_sources)/sizeof(point3);o++){
                                to_light_source = unit_vector(light_sources[o] - reflected.at(res));
                                if(dot(normal, to_light_source)>0){
                                    brightness += dot(normal, to_light_source)/(sizeof(light_sources)/sizeof(point3));
                                }
                            }
                            reflected_color += brightness*objects[j].get_color();
                            reflected_color = reflected_color/2;
                        }
                    }

                    //refraction rendering
                    color refracted_color(0,0,0);
                    ray refracted_ligh = refracted_ray(normal, r.direction(), r.at(res), current_refractive_index/objects[i].get_refractive_index());

                    

                    //finishing touches fixing color overflow or underflow fixing brightnesss
                    pixel = pixel/(sizeof(light_sources)/sizeof(point3));
                    pixel = (objects[i].get_smoothness()*reflected_color + (1-objects[i].get_smoothness())*pixel);

                    double boost = 2;
                    color finish(pow(pixel.x(), 1/boost), pow(pixel.x(), 1/boost), pow(pixel.x(), 1/boost));//taking sqrt of rgb values form 0 to 1 makes them brighter
                    //color finish(pow(pixel.x(), 1/boost), pow(pixel.y(), 1/boost), pow(pixel.z(), 1/boost));
                    screen.set_buffer(map_color_to_uint32(finish),x,y);
                }
            }
        }
    }

    screen.update();
    return 0;
}