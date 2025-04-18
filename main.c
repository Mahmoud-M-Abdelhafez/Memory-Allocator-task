#include <stdio.h>
#include <stddef.h>
#include <string.h>

#define MEMORY_SIZE 1024
#define ALIGN4(s) (((((s) - 1) >> 2) << 2) + 4)

static char memory[MEMORY_SIZE];

typedef struct block {
    size_t size;
    int free;
    struct block *next;
} block_t;

#define BLOCK_SIZE sizeof(block_t)
static block_t *free_list = (void *)memory;

void initialize() {
    free_list->size = MEMORY_SIZE - BLOCK_SIZE;
    free_list->free = 1;
    free_list->next = NULL;
}

void split(block_t *fitting_slot, size_t size) {
    block_t *new_block = (void *)((void *)fitting_slot + size + BLOCK_SIZE);
    new_block->size = fitting_slot->size - size - BLOCK_SIZE;
    new_block->free = 1;
    new_block->next = fitting_slot->next;

    fitting_slot->size = size;
    fitting_slot->free = 0;
    fitting_slot->next = new_block;
}

void *my_malloc(size_t no_of_bytes) {
    block_t *curr, *prev;
    void *result;

    if (!(free_list->size)) {
        initialize();
    }

    size_t size = ALIGN4(no_of_bytes);
    curr = free_list;
    while ((((curr->size) < size) || ((curr->free) == 0)) && (curr->next != NULL)) {
        prev = curr;
        curr = curr->next;
    }

    if ((curr->size) == size) {
        curr->free = 0;
        result = (void *)(++curr);
        return result;
    } else if ((curr->size) > (size + BLOCK_SIZE)) {
        split(curr, size);
        result = (void *)(++curr);
        return result;
    } else {
        return NULL;
    }
}

void merge() {
    block_t *curr = free_list;
    while ((curr != NULL) && (curr->next != NULL)) {
        if (curr->free && curr->next->free) {
            curr->size += (curr->next->size) + BLOCK_SIZE;
            curr->next = curr->next->next;
        } else {
            curr = curr->next;
        }
    }
}

void my_free(void *ptr) {
    if (((void *)memory <= ptr) && (ptr <= (void *)(memory + MEMORY_SIZE))) {
        block_t *curr = ptr;
        --curr;
        curr->free = 1;
        merge();
    }
}

void print_blocks() {
    printf("\n[Memory Blocks Status]\n");
    block_t *curr = free_list;
    int i = 0;
    while (curr) {
        printf("Block %d - Size: %zu, Free: %d\n", i++, curr->size, curr->free);
        curr = curr->next;
    }
    printf("\n");
}

// -----------------------------
// Test Functions
// -----------------------------
void test_int_allocation() {
    printf("Test: Integer Allocation\n");
    int *p = (int *)my_malloc(sizeof(int));
    *p = 10;
    printf("Allocated int: %d\n", *p);
    my_free(p);
    print_blocks();
}

void test_string_allocation() {
    printf("Test: String Allocation\n");
    char *str = (char *)my_malloc(10 * sizeof(char));
    strcpy(str, "Hello");
    printf("Allocated string: %s\n", str);
    my_free(str);
    print_blocks();
}

void test_multiple_allocations() {
    printf("Test: Multiple Allocations\n");
    int *a = (int *)my_malloc(sizeof(int));
    int *b = (int *)my_malloc(sizeof(int));
    *a = 5;
    *b = 15;
    printf("a = %d, b = %d\n", *a, *b);
    my_free(a);
    my_free(b);
    print_blocks();
}

void test_coalescing() {
    printf("Test: Coalescing Free Blocks\n");
    int *a = (int *)my_malloc(50);
    int *b = (int *)my_malloc(50);
    int *c = (int *)my_malloc(50);
    my_free(b);
    my_free(a);
    my_free(c); // This should trigger merge
    print_blocks();
}

void test_out_of_memory() {
    printf("Test: Out of Memory\n");
    void *big = my_malloc(2000); // more than MEMORY_SIZE
    if (big == NULL) {
        printf("Out of memory handled correctly.\n");
    }
    print_blocks();
}

// -----------------------------
// Main Function
// -----------------------------
int main() {
    printf("Custom Memory Allocator - Modular Tests\n");

    test_int_allocation();
    test_string_allocation();
    test_multiple_allocations();
    test_coalescing();
    test_out_of_memory();

     return 0;
}
