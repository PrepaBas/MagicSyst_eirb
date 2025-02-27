#ifndef table_h
#define table_h


struct position {
    float x;
    float y;
    float theta;
};


struct table {
    struct position bl;
    struct position tl;
    struct position br;
    struct position tr;
    struct position extreme;
};


#endif