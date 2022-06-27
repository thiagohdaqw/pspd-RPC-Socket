struct params {
    float data<>;
};

struct result {
    float min;
    float max;
};

program PROG {
 version VERSAO {
 result MINMAX(params) = 1;
 } = 100;
} = 55555555;
