#include <iostream>
#include <cstdint>
#include "a10p1.h"
#include "a10p2.h"

using namespace std;
int64_t free_space = arena()[0];
int64_t* s = arena();

int64_t *cons(int64_t a, int64_t *b) {
    if (free_space < 16) {
        return 0;
    }
    else {
        free_space -= 16;
        int64_t* result = s;
        if (s[1] == 0) {
            s += 2;
            result[0] = a;
            result[1] = (int64_t) b;
        }
        else {
            s = (int64_t*) s[1];
            result[0] = a;
            result[1] = (int64_t) b;
        }
        return result;
    }
}

int64_t *cdr(int64_t *p){
    return (int64_t*) p[1];
}

int64_t car(int64_t *p){
    return *p;
}

int64_t *setcar(int64_t *p, int64_t v){
    *p = v;
    return p;
}

int64_t *setcdr(int64_t *p, int64_t *v){
    p[1] = (int64_t) v;
    return p;
}

void snoc(int64_t *p) {
    free_space += 16;
    if (p) {
        p[0] = 0;
        p[1] = (int64_t) s;
        s = p;
    }
}