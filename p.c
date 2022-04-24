#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "Queue.h"

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

void split_in4(Node* root, Color** img, double prag, int depth) {

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
    newNode1->q = 1;
    newNode1->s_size = newNode1->w_end - newNode1->w_start;

    newNode2->h_start = root->h_start;
    newNode2->h_end = root->h_end - root->s_size / 2;
    newNode2->w_start = root->w_start + root->s_size / 2;
    newNode2->w_end = root->w_end;
    newNode2->q = 2;
    newNode2->s_size = root->s_size / 2;

    newNode3->h_start = root->h_start + root->s_size / 2;
    newNode3->h_end = root->h_end;
    newNode3->w_start = root->w_start + root->s_size / 2;
    newNode3->w_end = root->w_end;
    newNode3->q = 3;
    newNode3->s_size = root->s_size / 2;

    newNode4->h_start = root->h_start + root->s_size / 2;
    newNode4->h_end = root->h_end;
    newNode4->w_start = root->w_start;
    newNode4->w_end = root->w_end - root->s_size / 2;
    newNode4->q = 4;
    newNode4->s_size = root->s_size / 2;



    newNode1->mean = compute_mean(newNode1, img);
    if (newNode1->mean > prag) {
        newNode1->isLeaf = 0;
        split_in4(newNode1, img, prag, depth + 1);
    } else {
        newNode1->isLeaf = 1;
        *(root->nr_leafs) += 1;
    }

    
    
    newNode2->mean = compute_mean(newNode2, img);
    if (newNode2->mean > prag) {
        newNode2->isLeaf = 0;
        split_in4(newNode2, img, prag, depth + 1);
    } else {
        newNode2->isLeaf = 1;
        *(root->nr_leafs) += 1;
    }

    newNode3->mean = compute_mean(newNode3, img);
    if (newNode3->mean > prag) {
        newNode3->isLeaf = 0;
        split_in4(newNode3, img, prag, depth + 1);
    } else {
        newNode3->isLeaf = 1;
        *(root->nr_leafs) += 1;
    }

    newNode4->mean = compute_mean(newNode4, img);
    if (newNode4->mean > prag) {
        newNode4->isLeaf = 0;
        split_in4(newNode4, img, prag, depth + 1);
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
    Queue* q = createQueue();
    enqueue(q, root);

    Node* t;
    int i = 0;
    while(!isQueueEmpty(q)) {
        t = front(q);
        dequeue(q);
        vec[i].area = t->s_size * t->s_size;
        vec[i].red = t->pixel.r;
        vec[i].green = t->pixel.g;
        vec[i].blue = t->pixel.b;
        if (!t->isLeaf) {
            vec[i].top_left = 4*i + 1;
            vec[i].top_right = 4*i + 2;
            vec[i].bottom_right = 4*i + 3;
            vec[i].bottom_left = 4*i + 4;
            i++;
        } else {
            vec[i].top_left = -1;
            vec[i].top_right = -1;
            vec[i].bottom_right = -1;
            vec[i].bottom_left = -1;
            i++;
        }

        if (t->isLeaf != 1) {
            enqueue(q, t->q1);
            enqueue(q, t->q2);
            enqueue(q, t->q3);
            enqueue(q, t->q4);
        }

    }
}

int main(int argc, char** argv) {
    if (strcmp(argv[1], "-c") == 0) {
        double prag = atoi(argv[2]);
        FILE* fp = fopen(argv[3], "r");
        char* line = malloc(50);
        fscanf(fp, "%s", line);
        int width = -1, height = -1, maxColorVal;
        fscanf(fp, "%d %d %d\n", &width, &height, &maxColorVal);

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


        int size_of_tree = 1;
        int nr_leafs = 0;
        root->size_of_tree = &size_of_tree;
        root->nr_leafs = &nr_leafs;

        split_in4(root, img, prag, 0);

        CompressedFile* compressedFile = malloc(1 * sizeof(CompressedFile));
        compressedFile->numar_culori = *root->nr_leafs;
        compressedFile->numar_noduri = *root->size_of_tree;
        compressedFile->vector = calloc(compressedFile->numar_noduri, sizeof(QuadtreeNode));
        
        populate_vec(compressedFile->vector, root);

        for (int i = 0; i < compressedFile->numar_noduri; i++) {
            printf("%d %d\n", compressedFile->vector[i].area, compressedFile->vector[i].top_left);
        }
        print_tree(root, 0);

    }
    return 0;
}