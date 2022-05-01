#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "Queue.h"
// #include "Stack.h"



int ind = 0;

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

void populate_img(Node* root, Color** img) {
    if (root == NULL)
        return;

    if (root->isLeaf) {
        for (int i = root->h_start; i < root->h_end; i++) {
            for (int j = root->w_start; j < root->w_end; j++) {
                img[i][j].r = root->pixel.r;
                img[i][j].g = root->pixel.g;
                img[i][j].b = root->pixel.b;
            }
        }
    } else {
        populate_img(root->q1, img);
        populate_img(root->q2, img);
        populate_img(root->q3, img);
        populate_img(root->q4, img);
    }
}

void decompress_image(Node* root, Color** img, FILE* fp_out) {
    if (root == NULL)
        return;

    populate_img(root, img);

    fwrite("P6\n", sizeof(char), 3, fp_out);
    fprintf(fp_out, "%d %d\n", root->s_size, root->s_size);
    fprintf(fp_out, "%d\n", 255);
    
    for (int i = 0; i < root->s_size; i++) {
        for (int j = 0; j < root->s_size; j++) {
            fprintf(fp_out, "%c%c%c", img[i][j].b, img[i][j].g, img[i][j].r);
        } 
    }
}


int compute_mean(Node* node, Color** img) {
    long long r_med = 0;
    long long g_med = 0;
    long long b_med = 0;
    for (int i = node->h_start; i < node->h_end; i++) {
        for (int j = node->w_start; j < node->w_end; j++) {
            r_med += img[i][j].r;
            g_med += img[i][j].g;
            b_med += img[i][j].b;
        }
    }

    long long size = node->s_size;

    r_med /= size * size;
    g_med /= size * size;
    b_med /= size * size;

    long long sum = 0;
    for (int i = node->h_start; i < node->h_end; i++) {
        for (int j = node->w_start; j < node->w_end; j++) {
            sum +=  (r_med - img[i][j].r) * (r_med - img[i][j].r) +
                    (g_med - img[i][j].g) * (g_med - img[i][j].g) +
                    (b_med - img[i][j].b) * (b_med - img[i][j].b);
        }
    }

    long long mean = sum / (3 * size * size);

    node->pixel.r = r_med;
    node->pixel.g = g_med;
    node->pixel.b = b_med;

    return mean;
}

void createTree(Node* root, CompressedFile* compressedFile) {
    QuadtreeNode rt = compressedFile->vector[root->ind];

    if (rt.top_left != -1) {
        Node* newNode1 = calloc(1, sizeof(Node));
        Node* newNode2 = calloc(1, sizeof(Node));
        Node* newNode3 = calloc(1, sizeof(Node));
        Node* newNode4 = calloc(1, sizeof(Node));

        newNode1->size_of_tree = root->size_of_tree;
        newNode2->size_of_tree = root->size_of_tree;
        newNode3->size_of_tree = root->size_of_tree;
        newNode4->size_of_tree = root->size_of_tree;

        newNode1->nr_leafs = root->nr_leafs;
        newNode2->nr_leafs = root->nr_leafs;
        newNode3->nr_leafs = root->nr_leafs;
        newNode4->nr_leafs = root->nr_leafs;

        newNode1->h_start = root->h_start;
        newNode1->h_end = root->h_end - root->s_size / 2;
        newNode1->w_start = root->w_start;
        newNode1->w_end = root->w_end - root->s_size / 2;
        newNode1->s_size = newNode1->w_end - newNode1->w_start;

        newNode2->h_start = root->h_start;
        newNode2->h_end = root->h_end - root->s_size / 2;
        newNode2->w_start = root->w_start + root->s_size / 2;
        newNode2->w_end = root->w_end;
        newNode2->s_size = root->s_size / 2;

        newNode3->h_start = root->h_start + root->s_size / 2;
        newNode3->h_end = root->h_end;
        newNode3->w_start = root->w_start + root->s_size / 2;
        newNode3->w_end = root->w_end;
        newNode3->s_size = root->s_size / 2;

        newNode4->h_start = root->h_start + root->s_size / 2;
        newNode4->h_end = root->h_end;
        newNode4->w_start = root->w_start;
        newNode4->w_end = root->w_end - root->s_size / 2;
        newNode4->s_size = root->s_size / 2;

        QuadtreeNode rt1 = compressedFile->vector[rt.top_left];
        QuadtreeNode rt2 = compressedFile->vector[rt.top_right];
        QuadtreeNode rt3 = compressedFile->vector[rt.bottom_right];
        QuadtreeNode rt4 = compressedFile->vector[rt.bottom_left];

        newNode1->pixel.r = rt1.red;
        newNode1->pixel.g = rt1.green;
        newNode1->pixel.b = rt1.blue;

        newNode2->pixel.r = rt2.red;
        newNode2->pixel.g = rt2.green;
        newNode2->pixel.b = rt2.blue;

        newNode3->pixel.r = rt3.red;
        newNode3->pixel.g = rt3.green;
        newNode3->pixel.b = rt3.blue;

        newNode4->pixel.r = rt4.red;
        newNode4->pixel.g = rt4.green;
        newNode4->pixel.b = rt4.blue;

        newNode1->ind = rt.top_left;
        newNode2->ind = rt.top_right;
        newNode3->ind = rt.bottom_right;
        newNode4->ind = rt.bottom_left;


        *(root->size_of_tree) += 4;

        root->q1 = newNode1;
        root->q2 = newNode2;
        root->q3 = newNode3;
        root->q4 = newNode4;

        root->isLeaf = 0;

        createTree(root->q1, compressedFile);
        createTree(root->q2, compressedFile);
        createTree(root->q3, compressedFile);
        createTree(root->q4, compressedFile);
    } else {
        root->isLeaf = 1;
        *(root->nr_leafs) += 1;
    }
    
}

void split_in4(Node* root, Color** img, double prag) {

    Node* newNode1 = calloc(1, sizeof(Node));
    Node* newNode2 = calloc(1, sizeof(Node));
    Node* newNode3 = calloc(1, sizeof(Node));
    Node* newNode4 = calloc(1, sizeof(Node));

    // every node points to the size of tree
    newNode1->size_of_tree = root->size_of_tree;
    newNode2->size_of_tree = root->size_of_tree;
    newNode3->size_of_tree = root->size_of_tree;
    newNode4->size_of_tree = root->size_of_tree;

    newNode1->nr_leafs = root->nr_leafs;
    newNode2->nr_leafs = root->nr_leafs;
    newNode3->nr_leafs = root->nr_leafs;
    newNode4->nr_leafs = root->nr_leafs;

    newNode1->h_start = root->h_start;
    newNode1->h_end = root->h_end - root->s_size / 2;
    newNode1->w_start = root->w_start;
    newNode1->w_end = root->w_end - root->s_size / 2;
    newNode1->s_size = newNode1->w_end - newNode1->w_start;

    newNode2->h_start = root->h_start;
    newNode2->h_end = root->h_end - root->s_size / 2;
    newNode2->w_start = root->w_start + root->s_size / 2;
    newNode2->w_end = root->w_end;
    newNode2->s_size = root->s_size / 2;

    newNode3->h_start = root->h_start + root->s_size / 2;
    newNode3->h_end = root->h_end;
    newNode3->w_start = root->w_start + root->s_size / 2;
    newNode3->w_end = root->w_end;
    newNode3->s_size = root->s_size / 2;

    newNode4->h_start = root->h_start + root->s_size / 2;
    newNode4->h_end = root->h_end;
    newNode4->w_start = root->w_start;
    newNode4->w_end = root->w_end - root->s_size / 2;
    newNode4->s_size = root->s_size / 2;

    newNode1->mean = compute_mean(newNode1, img);
    if (newNode1->mean > prag) {
        newNode1->isLeaf = 0;
        split_in4(newNode1, img, prag);
    } else {
        newNode1->isLeaf = 1;
        *(root->nr_leafs) += 1;
    }

    
    
    newNode2->mean = compute_mean(newNode2, img);
    if (newNode2->mean > prag) {
        newNode2->isLeaf = 0;
        split_in4(newNode2, img, prag);
    } else {
        newNode2->isLeaf = 1;
        *(root->nr_leafs) += 1;
    }

    newNode3->mean = compute_mean(newNode3, img);
    if (newNode3->mean > prag) {
        newNode3->isLeaf = 0;
        split_in4(newNode3, img, prag);
    } else {
        newNode3->isLeaf = 1;
        *(root->nr_leafs) += 1;
    }

    newNode4->mean = compute_mean(newNode4, img);
    if (newNode4->mean > prag) {
        newNode4->isLeaf = 0;
        split_in4(newNode4, img, prag);
    } else {
        newNode4->isLeaf = 1;
        *(root->nr_leafs) += 1;
    }

    *(root->size_of_tree) += 4;

    root->q1 = newNode1;
    root->q2 = newNode2;
    root->q3 = newNode3;
    root->q4 = newNode4;
}

void populate_vec(QuadtreeNode* vec, Node* root) {
    if (root == NULL)
        return;

    Queue* q = createQueue();
    enqueue(q, root);

    Node* t;
    int i = 0;
    int it = 0;
    while(!isQueueEmpty(q)) {
        t = front(q);
        dequeue(q);
        vec[it].area = t->s_size * t->s_size;
        vec[it].red = t->pixel.r;
        vec[it].green = t->pixel.g;
        vec[it].blue = t->pixel.b;
        if (!t->isLeaf) {
            vec[it].top_left = 4*i + 1;
            vec[it].top_right = 4*i + 2;
            vec[it].bottom_right = 4*i + 3;
            vec[it].bottom_left = 4*i + 4;

            enqueue(q, t->q1);
            enqueue(q, t->q2);
            enqueue(q, t->q3);
            enqueue(q, t->q4);

            i++;

        } else {
            vec[it].top_left = -1;
            vec[it].top_right = -1;
            vec[it].bottom_right = -1;
            vec[it].bottom_left = -1;
        }
        it++;
    }

    destroyQueue(q);
}

void free_tree(Node* root) {
    if (root == NULL)
        return;

    Node* toBeDeleted = root;
    if (!root->isLeaf) {
        free_tree(root->q1);
        free_tree(root->q2);
        free_tree(root->q3);
        free_tree(root->q4);
    }
    
    free(toBeDeleted);
}

void buildTree(Node** root, Color*** img, int *width, int* height, double prag, FILE* fp) {
        int maxColorVal;
        char* line = malloc(50);
        fscanf(fp, "%s\n", line);
        free(line);
        fscanf(fp, "%d %d\n", width, height);
        fscanf(fp, "%d\n", &maxColorVal);

        *img = calloc(*height, sizeof(Color*));
        for (int i = 0; i < *height; i++) {
            (*img)[i] = calloc(*width, sizeof(Color));
        }

        unsigned char r;
        unsigned char g;
        unsigned char b;
        
        int64_t r_med = 0;
        int64_t g_med = 0;
        int64_t b_med = 0;

        for (int i = 0; i < *height; i++) {
            for (int j = 0; j < *width; j++) {
                fread(&b, sizeof(unsigned char), 1, fp);
                fread(&g, sizeof(unsigned char), 1, fp);
                fread(&r, sizeof(unsigned char), 1, fp);

                (*img)[i][j].r = r;
                (*img)[i][j].g = g;
                (*img)[i][j].b = b;

                r_med += (*img)[i][j].r;
                g_med += (*img)[i][j].g;
                b_med += (*img)[i][j].b;
            }
        }

        r_med /= *height * *width;
        g_med /= *height * *width;
        b_med /= *height * *width;

        
        (*root)->pixel.r = r_med;
        (*root)->pixel.g = g_med;
        (*root)->pixel.b = b_med;
        (*root)->s_size = *width;
        (*root)->offset = 0;
        (*root)->h_start = 0;
        (*root)->h_end = *height;
        (*root)->w_start = 0;
        (*root)->w_end = *width;
        (*root)->mean = 1;

        split_in4(*root, *img, prag);
}

void mirror(Node* root, char c) {
    if (root->isLeaf)
        return;

    Node* n1 = root->q1;
    Node* n2 = root->q2;
    Node* n3 = root->q3;
    Node* n4 = root->q4;

    if (c == 'v') {
        // 1 cu 4
        // 2 cu 3

        n4->h_start = root->h_start;
        n4->h_end = root->h_end - root->s_size / 2;
        n4->w_start = root->w_start;
        n4->w_end = root->w_end - root->s_size / 2;
        n4->s_size = n4->w_end - n4->w_start;

        n1->h_start = root->h_start + root->s_size / 2;
        n1->h_end = root->h_end;
        n1->w_start = root->w_start;
        n1->w_end = root->w_end - root->s_size / 2;
        n1->s_size = root->s_size / 2;

        n3->h_start = root->h_start;
        n3->h_end = root->h_end - root->s_size / 2;
        n3->w_start = root->w_start + root->s_size / 2;
        n3->w_end = root->w_end;
        n3->s_size = root->s_size / 2;

        n2->h_start = root->h_start + root->s_size / 2;
        n2->h_end = root->h_end;
        n2->w_start = root->w_start + root->s_size / 2;
        n2->w_end = root->w_end;
        n2->s_size = root->s_size / 2;
    }

    if (c == 'h') {
        // 1 cu 2
        // 4 cu 3

        n2->h_start = root->h_start;
        n2->h_end = root->h_end - root->s_size / 2;
        n2->w_start = root->w_start;
        n2->w_end = root->w_end - root->s_size / 2;
        n2->s_size = n2->w_end - n2->w_start;

        n1->h_start = root->h_start;
        n1->h_end = root->h_end - root->s_size / 2;
        n1->w_start = root->w_start + root->s_size / 2;
        n1->w_end = root->w_end;
        n1->s_size = root->s_size / 2;

        n4->h_start = root->h_start + root->s_size / 2;
        n4->h_end = root->h_end;
        n4->w_start = root->w_start + root->s_size / 2;
        n4->w_end = root->w_end;
        n4->s_size = root->s_size / 2;

        n3->h_start = root->h_start + root->s_size / 2;
        n3->h_end = root->h_end;
        n3->w_start = root->w_start;
        n3->w_end = root->w_end - root->s_size / 2;
        n3->s_size = root->s_size / 2;

    }

    mirror(root->q1, c);
    mirror(root->q2, c);
    mirror(root->q3, c);
    mirror(root->q4, c);
    


}

int main(int argc, char** argv) {
    if (strcmp(argv[1], "-c") == 0) {
        FILE* fp = fopen(argv[3], "r");
        fseek(fp, 0, SEEK_END);          
        rewind(fp);                   
        Node* root = calloc(1, sizeof(Node));;
        double prag = atoi(argv[2]);
        Color** img;


        int width = 0, height = 0; 

        int size_of_tree = 1;
        int nr_leafs = 0;
        root->size_of_tree = &size_of_tree;
        root->nr_leafs = &nr_leafs;

        buildTree(&root, &img, &width, &height, prag, fp);


        for (int i = 0; i < height; i++) {
            free(img[i]);
        }

        free(img);

        CompressedFile* compressedFile = malloc(1 * sizeof(CompressedFile));
        compressedFile->numar_culori = *root->nr_leafs;
        compressedFile->numar_noduri = *root->size_of_tree;
        compressedFile->vector = calloc(compressedFile->numar_noduri, sizeof(QuadtreeNode));
        
        populate_vec(compressedFile->vector, root);

        FILE* fp_out = fopen(argv[4], "w");
        fwrite(&compressedFile->numar_culori, sizeof(uint32_t), 1, fp_out);
        fwrite(&compressedFile->numar_noduri, sizeof(uint32_t), 1, fp_out);
        for (int i = 0; i < compressedFile->numar_noduri; i++) {
            fwrite(&compressedFile->vector[i].red , sizeof(unsigned char), 1, fp_out);
            fwrite(&compressedFile->vector[i].green , sizeof(unsigned char), 1, fp_out);
            fwrite(&compressedFile->vector[i].blue , sizeof(unsigned char), 1, fp_out);

            fwrite(&compressedFile->vector[i].area , sizeof(uint32_t), 1, fp_out);
            
            fwrite(&compressedFile->vector[i].top_left , sizeof(int32_t), 1, fp_out);
            fwrite(&compressedFile->vector[i].top_right , sizeof(int32_t), 1, fp_out);
            fwrite(&compressedFile->vector[i].bottom_left , sizeof(int32_t), 1, fp_out);
            fwrite(&compressedFile->vector[i].bottom_right , sizeof(int32_t), 1, fp_out);
        }

        free_tree(root);
        free(compressedFile->vector);
        free(compressedFile);
        fclose(fp);
        fclose(fp_out);
    }

    if (strcmp(argv[1], "-d") == 0) {
        // ./quadtree -d compress1_370.out decompress1.ppm
        FILE* fp = fopen(argv[2], "r");
        FILE* fp_out = fopen(argv[3], "w");
        long filelen;
        fseek(fp, 0, SEEK_END);          
        filelen = ftell(fp);
        rewind(fp);

        int colors, size;
        fread(&colors, sizeof(int), 1, fp);
        fread(&size, sizeof(int), 1, fp);

        unsigned char r;
        unsigned char g;
        unsigned char b;
        uint32_t area;
        int32_t q1, q2;
        int32_t q3, q4;

        CompressedFile* compressedFile = malloc(1 * sizeof(CompressedFile));
        compressedFile->numar_culori = colors;
        compressedFile->numar_noduri = size;
        compressedFile->vector = calloc(compressedFile->numar_noduri, sizeof(QuadtreeNode));

        for (int i = 0; i < size; i++) {
            fread(&r, sizeof(unsigned char), 1, fp);
            fread(&g, sizeof(unsigned char), 1, fp);
            fread(&b, sizeof(unsigned char), 1, fp);

            fread(&area, sizeof(uint32_t), 1, fp);

            fread(&q1, sizeof(int32_t), 1, fp);
            fread(&q2, sizeof(int32_t), 1, fp);
            fread(&q4, sizeof(int32_t), 1, fp);
            fread(&q3, sizeof(int32_t), 1, fp);

            compressedFile->vector[i].red = r;
            compressedFile->vector[i].green = g;
            compressedFile->vector[i].blue = b;

            compressedFile->vector[i].area = area;

            compressedFile->vector[i].top_left = q1;
            compressedFile->vector[i].top_right = q2;
            compressedFile->vector[i].bottom_right = q3;
            compressedFile->vector[i].bottom_left = q4;
        }

        Node* root = calloc(1, sizeof(Node));
        QuadtreeNode rt = compressedFile->vector[0];
        root->pixel.r = rt.red;
        root->pixel.g = rt.green;
        root->pixel.b = rt.blue;
        root->s_size = sqrt(rt.area);
        root->offset = 0;
        root->ind = 0;
        root->h_start = 0;
        root->h_end = root->s_size;
        root->w_start = 0;
        root->w_end = root->s_size;
        root->mean = 1;

        root->size_of_tree = &size;
        root->nr_leafs = &colors;

        createTree(root, compressedFile);
        print_tree(root, 0);

        Color** img = calloc(root->s_size, sizeof(Color*));
        for (int i = 0; i < root->s_size; i++) {
            img[i] = calloc(root->s_size, sizeof(Color));
        }

        decompress_image(root, img, fp_out);

        free(compressedFile->vector);
        free(compressedFile);

        for (int i = 0; i < root->s_size; i++) {
            free(img[i]);
        }

        free(img);

        free_tree(root);

        fclose(fp);
        fclose(fp_out);
    }

    if (strcmp(argv[1], "-m") == 0) {
        FILE* fp = fopen(argv[4], "r");
        FILE* fp_out = fopen(argv[5], "w");
        fseek(fp, 0, SEEK_END);          
        rewind(fp);                   
        Node* root = calloc(1, sizeof(Node));;
        double prag = atoi(argv[3]);
        Color** img;

        int width = 0, height = 0; 

        int size_of_tree = 1;
        int nr_leafs = 0;
        root->size_of_tree = &size_of_tree;
        root->nr_leafs = &nr_leafs;

        buildTree(&root, &img, &width, &height, prag, fp);

        char mirror_type = (char) argv[2][0];

        mirror(root, mirror_type);

        // print_tree(root, 0);


        decompress_image(root, img, fp_out);

        
    }

    return 0;
}