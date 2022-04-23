#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

typedef struct QuadtreeNode {
    unsigned char blue, green, red ;
    /*nr de pixeli din blocul descris de nod*/
    uint32_t area;
    int32_t top_left , top_right ;
    int32_t bottom_left , bottom_right ;
} __attribute__ ( ( packed ) ) QuadtreeNode ;


typedef struct CompressedFile {
    /*nr de blocuri, cu informaţie utilă,
    utilizate în cadrul procesului de compresie 
    (numărul de frunze ale arborelui cuaternar de compresie) */
    uint32_t numar_culori;

    /*nr total de noduri ale arborelui cuaternar*/
    uint32_t numar_noduri;

    /*un vector care conţine număr_noduri elemente de tip structură QuadtreeNode,
    fiecare element reprezentând un nod din arborele de compresie creat */
    int *vector;
} CompressedFile;

typedef struct Color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} Color;

typedef struct Node {
    Color pixel;
    uint32_t s_size;
    struct Node* q1;
    struct Node* q2;
    struct Node* q3;
    struct Node* q4;
    int isLeaf;
    int offset;

    int q;
    int h_start;
    int h_end;
    int w_start;
    int w_end;
    int64_t mean;
} Node;

void print_tree(Node* root, int depth) {
    if (root == NULL)
        return;

    for (int i = 0; i < depth; i++) {
        printf("\t");
    }
    printf("d=%d ", depth);
    printf("(%d -> %d; %d -> %d) m=%ld, r=%d,g=%d,b=%d\n", root->h_start, root->h_end, root->w_start, root->w_end, root->mean,  root->pixel.r, root->pixel.g, root->pixel.b);

    print_tree(root->q1, depth + 1);
    print_tree(root->q2, depth + 1);
    print_tree(root->q3, depth + 1);
    print_tree(root->q4, depth + 1);
}

int64_t compute_mean(Node* node, Color** img) {
    int64_t r_med = 0;
    int64_t g_med = 0;
    int64_t b_med = 0;
    for (int i = node->h_start; i < node->h_end; i++) {
        for (int j = node->w_start; j < node->w_end; j++) {
            r_med += img[i][j].r;
            g_med += img[i][j].g;
            b_med += img[i][j].b;
        }
    }

    int64_t size = node->s_size;

    r_med /= size * size;
    g_med /= size * size;
    b_med /= size * size;

    int64_t sum = 0;
    for (int i = node->h_start; i < node->h_end; i++) {
        for (int j = node->w_start; j < node->w_end; j++) {
            sum +=  (r_med - img[i][j].r) * (r_med - img[i][j].r) +
                    (g_med - img[i][j].g) * (g_med - img[i][j].g) +
                    (b_med - img[i][j].b) * (b_med - img[i][j].b);
        }
    }

    int64_t mean = sum / (3 * size * size);

    node->pixel.r = r_med;
    node->pixel.g = g_med;
    node->pixel.b = b_med;

    return mean;
}

void split_in4(Node* root, Color** img, double prag, int depth) {

    int offset_h = 0, offset_w = 0;
    if (root->q == 1) {
        offset_h = 0;
        offset_w = 0;
    } else if (root->q == 2) {
        offset_h = 0;
        offset_w = 0;
    } else if (root->q == 3) {
        offset_h = 0;
        offset_w = 0;
    } else if (root->q == 4) {
        offset_h = 0;
        offset_w = 0;
    }

    Node* newNode1 = calloc(1, sizeof(Node));

    newNode1->h_start = root->h_start + offset_h;
    newNode1->h_end = root->h_end - root->s_size / 2 + offset_h;
    newNode1->w_start = root->w_start + offset_w;
    newNode1->w_end = root->w_end - root->s_size / 2 + offset_w;
    newNode1->q = 1;
    newNode1->s_size = newNode1->w_end - newNode1->w_start;


    Node* newNode2 = calloc(1, sizeof(Node));
    newNode2->h_start = root->h_start + offset_h;
    newNode2->h_end = root->h_end - root->s_size / 2 + offset_h;
    newNode2->w_start = root->w_start + root->s_size / 2 + offset_w;
    newNode2->w_end = root->w_end + offset_w;
    newNode2->q = 2;
    newNode2->s_size = root->s_size / 2;


    Node* newNode3 = calloc(1, sizeof(Node));

    newNode3->h_start = root->h_start + root->s_size / 2 + offset_h;
    newNode3->h_end = root->h_end + offset_h;
    newNode3->w_start = root->w_start + root->s_size / 2 + offset_w;
    newNode3->w_end = root->w_end + offset_w;
    newNode3->q = 3;
    newNode3->s_size = root->s_size / 2;


    Node* newNode4 = calloc(1, sizeof(Node));

    newNode4->h_start = root->h_start + root->s_size / 2 + offset_h;
    newNode4->h_end = root->h_end + offset_h;
    newNode4->w_start = root->w_start + offset_w;
    newNode4->w_end = root->w_end - root->s_size / 2 + offset_w;
    newNode4->q = 4;
    newNode4->s_size = root->s_size / 2;

    newNode1->mean = compute_mean(newNode1, img);
    if (newNode1->mean > prag) {
        newNode1->isLeaf = 0;
        split_in4(newNode1, img, prag, depth + 1);
        root->q1 = newNode1;
    }
    else {
        newNode1->isLeaf = 1;
        root->q1 = newNode1;
    }
    
    newNode2->mean = compute_mean(newNode2, img);
    if (newNode2->mean > prag) {
        newNode2->isLeaf = 0;
        split_in4(newNode2, img, prag, depth + 1);
        root->q2 = newNode2;

    }
    else {
        newNode2->isLeaf = 1;
        root->q2 = newNode2;
    }

    newNode3->mean = compute_mean(newNode3, img);
    if (newNode3->mean > prag) {
        newNode3->isLeaf = 0;
        split_in4(newNode3, img, prag, depth + 1);
        root->q3 = newNode3;
    }  
    else {
        newNode3->isLeaf = 1;
        root->q3 = newNode3;
    }

    newNode4->mean = compute_mean(newNode4, img);
    if (newNode4->mean > prag) {
        newNode4->isLeaf = 0;
        split_in4(newNode4, img, prag, depth + 1);
        root->q4 = newNode4;
    }  
    else {
        newNode4->isLeaf = 1;
        root->q4 = newNode4;
    }
}

int main(int argc, char** argv) {
    if (strcmp(argv[1], "-c") == 0) {
        double prag = atoi(argv[2]);
        FILE* fp = fopen(argv[3], "r");
        char* line = malloc(50);
        fscanf(fp, "%s", line);
        int width = -1, height = -1, maxColorVal;
        fscanf(fp, "%d %d %d", &width, &height, &maxColorVal);

        Color** img = calloc(height, sizeof(Color*));
        for (int i = 0; i < height; i++) {
            img[i] = calloc(width, sizeof(Color));
        }

        int64_t r_med = 0;
        int64_t g_med = 0;
        int64_t b_med = 0;
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                unsigned char r, g, b;
                fscanf(fp, "%c%c%c", &r, &g, &b);
                img[i][j].r = r;
                img[i][j].g = g;
                img[i][j].b = b;

                r_med += img[i][j].r;
                g_med += img[i][j].g;
                b_med += img[i][j].b;
            }
        }


        r_med /= height * width;
        g_med /= height * width;
        b_med /= height * width;

        Node* root = calloc(1, sizeof(Node));
        root->pixel.r = r_med;
        root->pixel.g = g_med;
        root->pixel.b = b_med;
        root->s_size = width;
        root->offset = 0;
        root->q = 1;
        root->h_start = 0;
        root->h_end = height;
        root->w_start = 0;
        root->w_end = width;
        root->mean = 1;

        split_in4(root, img, prag, 0);

        print_tree(root, 0);

    }
    return 0;
}