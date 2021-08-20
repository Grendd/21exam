#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int str_error(char *str){
    printf("%s\n", str);
    return 1;
}

typedef struct s_zone {
    int width, height;
    char color;
}               t_zone;

typedef struct	s_shape{
    char cmd, color;
    float width, height;
    float x, y;
}           t_shapes;

int get_zone(FILE *file, t_zone *zone, char **image) {
    if(fscanf(file, "%i %i %c\n", &zone->width, &zone->height, &zone->color) != 3)
        return 0;
    if(zone->width <= 0 || zone->height <= 0 || zone->height > 300 || zone->width > 300)
        return 0;
    if (!(*image = malloc(zone->width * zone->height * sizeof(char))))
        return 0;
    for (int i = 0; i < zone->height * zone->width; i++)
        (*image)[i] = zone->color;
    return 1;
}

int is_in_rectangle(float x, float y, t_shapes *shape){
    if (x < shape->x || x > (shape->x + shape->width) || y < shape->y || y > (shape->y + shape->height))
        return 0;    
    if ((x - shape->x < 1.00000000) || (shape->x + shape->width - x < 1.00000000) || (y - shape->y < 1.00000000) || (shape->y + shape->height - y < 1.00000000))
        return 2;    
    return 1;
}
void update_image(t_shapes shape, char **image, int width, int height){
    int ret;

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            ret = is_in_rectangle(j, i, &shape);
            if((ret == 2 && shape.cmd == 'r') || (ret && shape.cmd == 'R'))
                (*image)[i * width + j] = shape.color;
        }
    }
}

int get_shapes(FILE *file, char **image, int width, int height){
    int ret;
    t_shapes shape;

    while ((ret = fscanf(file, "%c %f %f %f %f %c\n", &shape.cmd, &shape.x, &shape.y, &shape.width, &shape.height, &shape.color)) == 6){
        if (!((shape.cmd == 'r' || shape.cmd == 'R') && shape.width > 0 && shape.height > 0))
            return 0;        
        update_image(shape, image, width, height);        
    }
    if (ret != -1)
		return (0);
    return 1;
}

void print_zone(char *image, int width, int height){
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            printf("%c", image[i * width + j]);
            if (j == width - 1)
                printf("\n");            
        }
    }
}

int main(int ac, char **av){
    FILE *file;
    t_zone zone;
    char *image = NULL;

    if (ac != 2)
        return str_error("Error: argument");
    if ((!(file = fopen(av[1], "r"))))
        return str_error("Error: Operation file corrupted");
    if (!get_zone(file, &zone, &image))
        return str_error("Error: Operation file corrupted");
    if (!get_shapes(file, &image, zone.width, zone.height))
        return str_error("Error: Operation file corrupted");
    print_zone(image, zone.width, zone.height);
    free(image);
    fclose(file);
    return 0;
}