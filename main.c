#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>


#define ROWS 100
#define COLUMNS 100


typedef struct{
    int width;
    int height;
    int max_color;
    unsigned char* data; // Store pixel data (RGB values)
} PPMImage;


void skipComments(FILE* file){
    int ch;
    while ((ch = fgetc(file)) != EOF){
        if (ch == '#'){
            // Skip to the end of the comment line
            while ((ch = fgetc(file)) != EOF && ch != '\n');
        } else if (!isspace(ch)){
            // If it's not whitespace, put the character back and break
            ungetc(ch, file);
            break;
        }
    }
}


PPMImage* readPPM(const char* filename){
    FILE* file = fopen(filename, "rb");
    if (!file) {
        perror("fopen");
        return NULL;
    }

    char format[3];
    if (!fgets(format, sizeof(format), file)){
        perror("fgets");
        fclose(file);
        return NULL;
    }

    // Check the PPM format (P3 or P6)
    if (format[0] != 'P' || (format[1] != '3' && format[1] != '6')){
        fprintf(stderr, "Unsupported PPM format: %s\n", format);
        fclose(file);
        return NULL;
    }

    // Skip comments and whitespace
    skipComments(file);

    int width, height, max_color;
    if (fscanf(file, "%d %d", &width, &height) != 2){
        fprintf(stderr, "Failed to read image dimensions.\n");
        fclose(file);
        return NULL;
    }

    skipComments(file);

    if (fscanf(file, "%d", &max_color) != 1){
        fprintf(stderr, "Failed to read max color value.\n");
        fclose(file);
        return NULL;
    }

    // Skip the newline character after max_color
    fgetc(file);

    // Allocate memory for the image
    PPMImage* image = (PPMImage*)malloc(sizeof(PPMImage));
    image->width = width;
    image->height = height;
    image->max_color = max_color;
    image->data = (unsigned char*)malloc(3 * width * height);

    if (format[1] == '3') {
        // P3 format (plain text)
        for (int i = 0; i < width * height * 3; i++) {
            int value;
            if (fscanf(file, "%d", &value) != 1) {
                fprintf(stderr, "Failed to read pixel data.\n");
                free(image->data);
                free(image);
                fclose(file);
                return NULL;
            }
            image->data[i] = (unsigned char)value;
        }
    } else if (format[1] == '6') {
        // P6 format (binary)
        if (fread(image->data, 1, 3 * width * height, file) != (size_t)(3 * width * height)) {
            fprintf(stderr, "Failed to read binary pixel data.\n");
            free(image->data);
            free(image);
            fclose(file);
            return NULL;
        }
    }

    fclose(file);
    return image;
}


void freePPM(PPMImage* image){
    if (image) {
        free(image->data);
        free(image);
    }
}


void convertPpmToGrid(int **outputGrid, PPMImage* inputImage){
    if(inputImage->width != COLUMNS || inputImage->height != ROWS){ printf("Image dimensions do not match requirement"); return; }
    for(int c = 0; c < COLUMNS; c++){
        for(int r = 0; r < ROWS; r++){
            //indexing is done so that the red value of each pixel is read. (r*ROWS wraps the index based on r).
            if(inputImage->data[(3*r*ROWS)+(3*c)] == 0) { outputGrid[r][c] = 1; }
        }
    }
}


void clearAllGrids(int** evaluationGrid, int** printGrid){
    //Iterate through every index within range ROWS*COLUMNS, set all to 0
    for(int r = 0; r < ROWS; r++){
        for(int c = 0; c < COLUMNS; c++){
            evaluationGrid[r][c] = 0;
            printGrid[r][c] = 0;
        }
    }
}


void printGrids(int** evaluationGrid){
    //prints an "X " if there is a pixel there and "  " if not.
    for(int r = 0; r < ROWS; r++){
        for(int c = 0; c < COLUMNS; c++){
            if(evaluationGrid[r][c] == 1) { printf("X "); }
            else { printf("  "); }
        }
        printf("\n");
    }
}


int main()
{

    //create necessary grids
    int **evaluationGrid = (int**)calloc(ROWS, sizeof(int*));
    int **printGrid = (int**)calloc(ROWS, sizeof(int*));

    if (evaluationGrid == NULL || printGrid == NULL){
        perror("Failed to allocate memory");
        return 1;
    }

    for(int i = 0; i < COLUMNS; i++){
        evaluationGrid[i] = (int*)calloc(COLUMNS, sizeof(int));
        printGrid[i] = (int*)calloc(COLUMNS, sizeof(int));

        if (evaluationGrid[i] == NULL || printGrid[i] == NULL){
            perror("Failed to allocate memory");
            return 1;
        }
    }

    //get the image and load it into the grid
    const char* filename = "/Users/efakselrud/projects/personal/cTesting/image.ppm";
    PPMImage* image = readPPM(filename);

    if (!image){
        fprintf(stderr, "Failed to read PPM file.\n");
        return 1;
    }

    convertPpmToGrid(evaluationGrid, image);
    freePPM(image);

    //main loop
    int running = 1;
    int neighborCount;
    while(running == 1){

        //unix sleep the thread (for now)
        usleep(50000);

        system("clear");

        //evaluate the grid not including the sides (for now)
        for(int r = 1; r < ROWS - 1; r++){
            for(int c = 1; c < COLUMNS - 1; c++){

                neighborCount = 0;

                neighborCount += evaluationGrid[r-1][c]; //top
                neighborCount += evaluationGrid[r-1][c+1]; //top right
                neighborCount += evaluationGrid[r][c+1]; //right
                neighborCount += evaluationGrid[r+1][c+1]; //bottom right
                neighborCount += evaluationGrid[r+1][c]; //bottom
                neighborCount += evaluationGrid[r+1][c-1]; //bottom left
                neighborCount += evaluationGrid[r][c-1]; //left 
                neighborCount += evaluationGrid[r-1][c-1]; //top left


                if(evaluationGrid[r][c] == 1){ //handle living cells
                    if(neighborCount <= 1 || neighborCount >= 4) { printGrid[r][c] = 0; }
                    if(neighborCount == 2 || neighborCount == 3) { printGrid[r][c] = 1; }
                }
                else{ //handle dead cells
                    if(neighborCount == 3) { printGrid[r][c] = 1; }
                }
            }
        }

        //print the grid to console
        printGrids(printGrid);

        int** temp = evaluationGrid;
        evaluationGrid = printGrid;
        printGrid = temp;

        for(int r = 0; r < ROWS; r++){
            for(int c = 0; c < COLUMNS; c++) { printGrid[r][c] = 0; }
            printf("\n");
        }
    }

    //free used memory
    for(int i = 0; i < COLUMNS; i++)
    {
        free(evaluationGrid[i]);
        free(printGrid[i]);
    }
    free(evaluationGrid);
    free(printGrid);

}