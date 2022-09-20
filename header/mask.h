#ifndef INDEXATIONIMAGE_MASK_H
#define INDEXATIONIMAGE_MASK_H

const int impulse_response[3][3] = {
        {1,1,1},
        {1,1,1},
        {1,1,1}
};

const int horizontal_gradient[3][3] = {
        {-1,0,1},
        {-2,0,2},
        {-1,0,1}
};

const int vertical_gradient[3][3] = {
        {-1,-2,-1},
        {0,0,0},
        {1,2,1}
};

#endif //INDEXATIONIMAGE_MASK_H
