// Inverse Transforms

typedef struct
{
    int a;
    int b;
    int c;
    int d;
    int e;
    int f;
    int g;
    int h;
    int i;
    int j;
    int k;
    int l;
    int m;
    int n;
    int o;
    int p;
} Grid;

static inline void swapInt(int* pa, int* pb)
{
    const int temp = *pa;
    *pa = *pb;
    *pb = temp;
}

static inline void rotateScale(int* pa, int* pb)
{
    int a = *pa;
    int b = *pb;

    a -= (b + 1) >> 1;
    b -= (3 * a + 2) >> 2;
    a += (b + 1) >> 1;
    b >>= 1; // inverse lbt, divide

    *pa = a;
    *pb = b;
}

static inline void butterflyDown(int* pa, int* pb, int* pc, int* pd)
{
    int a = *pa;
    int b = *pb;
    int c = *pc;
    int d = *pd;

    d = a - d;
    a -= d >> 1;
    c = b - c;
    b -= c >> 1;

    *pa = a;
    *pb = b;
    *pc = c;
    *pd = d;
}

static inline void butterflyUp(int* pa, int* pb, int* pc, int* pd)
{
    int a = *pa;
    int b = *pb;
    int c = *pc;
    int d = *pd;

    b += c >> 1;
    c = b - c;
    a += d >> 1;
    d = a - d;

    *pa = a;
    *pb = b;
    *pc = c;
    *pd = d;
}

static inline void invTHH(int* pa, int* pb, int* pc, int* pd)
{
    int a = *pa;
    int b = *pb;
    int c = *pc;
    const int D = *pd;

    a += b;
    c -= D;
    const int t = (a - c) >> 1;
    const int d = t - b;
    b = d + c;
    c = t - D;
    a -= c;

    *pa = a;
    *pb = b;
    *pc = c;
    *pd = d;
}

static inline void invTHR(int* pa, int* pb, int* pc, int* pd)
{
    int a = *pa;
    int b = *pb;
    int c = *pc;
    int d = *pd;

    const int t1 = a + ((b + 1) >> 1);
    const int t2 = d - ((c + 1) >> 1);
    d = b - ((5 * t1 + 2) >> 2);
    a = c + ((5 * t2 + 2) >> 2);
    c = t1 + ((d + 1) >> 1) + (a >> 1);
    b = t2 - ((a + 1) >> 1) - (d >> 1);
    a -= c;
    d += b;

    *pa = a;
    *pb = b;
    *pc = c;
    *pd = d;
}

static inline void invTRR(int* pa, int* pb, int* pc, int* pd)
{
    int a = *pa;
    int b = *pb;
    int c = *pc;
    int d = *pd;

    const int t1 = a - d;
    const int t2 = c + b;
    b -= t2 >> 1;
    d += t1 >> 1;
    c = (t1 >> 1) - t2;
    a = c - t1;
    c -= a >> 1;
    b += c >> 1;
    c -= b;
    d -= a >> 1;
    a += d;

    *pa = a;
    *pb = b;
    *pc = c;
    *pd = d;
}

static inline void downscale(int* row0, int* row1, int* row2, int* row3)
{
    // This algorithm has more writes than reads, so
    // just perform inline instead of unpacking
    row0[2] = (row0[2] + 2) >> 2;
    row0[3] = 0;
    row1[1] = (3 * row1[1] + 2) >> 2;
    row1[2] = (row1[2] + 2) >> 2;
    row1[3] = 0;
    row2[0] = (row2[0] + 2) >> 2;
    row2[1] = (row2[1] + 2) >> 2;
    row2[2] = (row2[2] + 4) >> 3;
    row2[3] = 0;
    row3[0] = 0;
    row3[1] = 0;
    row3[2] = 0;
    row3[3] = 0;
}

// Block order
// r[a] r[b] r[c] r[d]
// r[e] r[f] r[g] r[h]
// r[i] r[j] r[k] r[l]
// r[m] r[n] r[o] r[p]
void invDCT(Grid* r)
{
    // reorder EO blocks
    swapInt(&r->b, &r->c);
    swapInt(&r->i, &r->e);
    swapInt(&r->l, &r->h);
    swapInt(&r->k, &r->f);
    swapInt(&r->j, &r->g);
    swapInt(&r->o, &r->n);

    // Hadamard THH
    invTHH(&r->a, &r->b, &r->e, &r->f);

    // Rotation THR - clockwise
    invTHR(&r->h, &r->g, &r->d, &r->c);

    // Rotation THR - counterclockwise
    invTHR(&r->n, &r->j, &r->m, &r->i);

    // Rotation TRR - in place
    invTRR(&r->k, &r->l, &r->o, &r->p);

    invTHH(&r->a, &r->d, &r->m, &r->p);
    invTHH(&r->b, &r->c, &r->n, &r->o);
    invTHH(&r->e, &r->h, &r->i, &r->l);
    invTHH(&r->f, &r->g, &r->j, &r->k);
}

// Lossless, figure 10
void invOverlap(Grid* r)
{
    // 4X4 iPOT Hadamard Phase 1
    invTHH(&r->a, &r->m, &r->d, &r->p);
    invTHH(&r->b, &r->n, &r->c, &r->o);
    invTHH(&r->e, &r->i, &r->h, &r->l);
    invTHH(&r->f, &r->j, &r->g, &r->k);

    // These are different (simpler) than iPOT
    // Phase 2, 3 Scaling + Rotation combined
    rotateScale(&r->m, &r->i);
    rotateScale(&r->n, &r->j);
    rotateScale(&r->o, &r->k);
    rotateScale(&r->p, &r->l);
    rotateScale(&r->d, &r->c);
    rotateScale(&r->h, &r->g);
    rotateScale(&r->l, &r->k);
    rotateScale(&r->p, &r->o);

    // 4X4 iPOT Hadamard Phase 4
    invTHH(&r->a, &r->d, &r->m, &r->p);
    invTHH(&r->b, &r->c, &r->n, &r->o);
    invTHH(&r->e, &r->h, &r->i, &r->l);
    invTHH(&r->f, &r->g, &r->j, &r->k);
}

void gridReadStage1(Grid* grid, const int* row0, const int* row1, const int* row2, const int* row3)
{
    grid->a = row0[0];
    grid->b = row0[1];
    grid->c = row0[2];
    grid->d = row0[3];
    grid->e = row1[0];
    grid->f = row1[1];
    grid->g = row1[2];
    grid->h = row1[3];
    grid->i = row2[0];
    grid->j = row2[1];
    grid->k = row2[2];
    grid->l = row2[3];
    grid->m = row3[0];
    grid->n = row3[1];
    grid->o = row3[2];
    grid->p = row3[3];
}

void gridWriteStage1(Grid* grid, int* row0, int* row1, int* row2, int* row3)
{
    row0[0] = grid->a;
    row0[1] = grid->b;
    row0[2] = grid->c;
    row0[3] = grid->d;
    row1[0] = grid->e;
    row1[1] = grid->f;
    row1[2] = grid->g;
    row1[3] = grid->h;
    row2[0] = grid->i;
    row2[1] = grid->j;
    row2[2] = grid->k;
    row2[3] = grid->l;
    row3[0] = grid->m;
    row3[1] = grid->n;
    row3[2] = grid->o;
    row3[3] = grid->p;
}

void invDCTStage1(int* row0, int* row1, int* row2, int* row3)
{
    Grid grid;
    gridReadStage1(&grid, row0, row1, row2, row3);
    invDCT(&grid);
    gridWriteStage1(&grid, row0, row1, row2, row3);
}

void invOverlapStage1(int* row0, int* row1, int* row2, int* row3)
{
    Grid grid;
    gridReadStage1(&grid, row0, row1, row2, row3);
    invOverlap(&grid);
    gridWriteStage1(&grid, row0, row1, row2, row3);
}

void gridReadStage2(Grid* grid, const int* row0, const int* row1, const int* row2, const int* row3)
{
    grid->a = row0[0];
    grid->b = row0[4];
    grid->c = row0[8];
    grid->d = row0[12];
    grid->e = row1[0];
    grid->f = row1[4];
    grid->g = row1[8];
    grid->h = row1[12];
    grid->i = row2[0];
    grid->j = row2[4];
    grid->k = row2[8];
    grid->l = row2[12];
    grid->m = row3[0];
    grid->n = row3[4];
    grid->o = row3[8];
    grid->p = row3[12];
}

void gridWriteStage2(Grid* grid, int* row0, int* row1, int* row2, int* row3)
{
    row0[0] = grid->a;
    row0[4] = grid->b;
    row0[8] = grid->c;
    row0[12] = grid->d;
    row1[0] = grid->e;
    row1[4] = grid->f;
    row1[8] = grid->g;
    row1[12] = grid->h;
    row2[0] = grid->i;
    row2[4] = grid->j;
    row2[8] = grid->k;
    row2[12] = grid->l;
    row3[0] = grid->m;
    row3[4] = grid->n;
    row3[8] = grid->o;
    row3[12] = grid->p;
}

void invOverlapStage2(int* row0, int* row1, int* row2, int* row3)
{
    Grid grid;
    gridReadStage2(&grid, row0, row1, row2, row3);
    invOverlap(&grid);
    gridWriteStage2(&grid, row0, row1, row2, row3);
}

void invDCTStage2(int* row0, int* row1, int* row2, int* row3)
{
    Grid grid;
    gridReadStage2(&grid, row0, row1, row2, row3);
    invDCT(&grid);
    gridWriteStage2(&grid, row0, row1, row2, row3);
}

void invOverlapVerticalStage1(int* row0, int* row1, int* row2, int* row3)
{
    butterflyDown(&row0[0], &row1[0], &row2[0], &row3[0]);
    rotateScale(&row3[0], &row2[0]);
    butterflyUp(&row0[0], &row1[0], &row2[0], &row3[0]);

    butterflyDown(&row0[1], &row1[1], &row2[1], &row3[1]);
    rotateScale(&row3[1], &row2[1]);
    butterflyUp(&row0[1], &row1[1], &row2[1], &row3[1]);
}

void invOverlapHorizontalStage1(int* row0, int* row1)
{
    butterflyDown(&row0[0], &row0[1], &row0[2], &row0[3]);
    rotateScale(&row0[3], &row0[2]);
    butterflyUp(&row0[0], &row0[1], &row0[2], &row0[3]);

    butterflyDown(&row1[0], &row1[1], &row1[2], &row1[3]);
    rotateScale(&row1[3], &row1[2]);
    butterflyUp(&row1[0], &row1[1], &row1[2], &row1[3]);
}

void invOverlapVerticalStage2(int* row0, int* row1, int* row2, int* row3)
{
    butterflyDown(&row0[0], &row1[0], &row2[0], &row3[0]);
    rotateScale(&row3[0], &row2[0]);
    butterflyUp(&row0[0], &row1[0], &row2[0], &row3[0]);

    butterflyDown(&row0[4], &row1[4], &row2[4], &row3[4]);
    rotateScale(&row3[4], &row2[4]);
    butterflyUp(&row0[4], &row1[4], &row2[4], &row3[4]);
}

void invOverlapHorizontalStage2(int* row0, int* row1)
{
    butterflyDown(&row0[0], &row0[4], &row0[8], &row0[12]);
    rotateScale(&row0[12], &row0[8]);
    butterflyUp(&row0[0], &row0[4], &row0[8], &row0[12]);

    butterflyDown(&row1[0], &row1[4], &row1[8], &row1[12]);
    rotateScale(&row1[12], &row1[8]);
    butterflyUp(&row1[0], &row1[4], &row1[8], &row1[12]);
}

void invTransformInitial(int** coefficients, int tileWidth, int channel, int bOverlap, int bDownscale)
{
    int* row0 = coefficients[16] + channel;
    int* row1 = coefficients[20] + channel;
    int* row2 = coefficients[24] + channel;
    int* row3 = coefficients[28] + channel;

    for (int i = 0; i < ((tileWidth - 16) >> 4) + 1; ++i)
    {
        invDCTStage2(row0, row1, row2, row3);

        if (i && bOverlap)
            invOverlapHorizontalStage2(row0 - 8, row1 - 8);

        row0 += 16;
        row1 += 16;
        row2 += 16;
        row3 += 16;
    }

    row0 = coefficients[16] + channel;
    row1 = coefficients[17] + channel;
    row2 = coefficients[18] + channel;
    row3 = coefficients[19] + channel;

    for (int i = 0; i < ((tileWidth - 1) >> 2) + 1; ++i)
    {
        if (bDownscale)
            downscale(row0, row1, row2, row3);

        invDCTStage1(row0, row1, row2, row3);

        if (i && bOverlap)
            invOverlapHorizontalStage1(row0 - 2, row1 - 2);

        row0 += 4;
        row1 += 4;
        row2 += 4;
        row3 += 4;
    }
}

void invTransformMacroblock(int** coefficients, int tileWidth, int channel, int bOverlap, int bLast)
{
    int* row0 = coefficients[8] + channel;
    int* row1 = coefficients[12] + channel;
    int* row2 = coefficients[16] + channel;
    int* row3 = coefficients[20] + channel;
    int* row4 = coefficients[24] + channel;
    int* row5 = coefficients[28] + channel;

    for (int i = 0; i < ((tileWidth - 16) >> 4) + 1; ++i)
    {
        invDCTStage2(row2, row3, row4, row5);

        if (i == 0)
        {
            if (bOverlap)
                invOverlapVerticalStage2(row0, row1, row2, row3);

            row0 += 8;
            row1 += 8;
        }
        else
        {
            if (bOverlap)
                invOverlapStage2(row0, row1, row2 - 8, row3 - 8);

            if (bLast && bOverlap)
                invOverlapHorizontalStage2(row4 - 8, row5 - 8);

            row0 += 16;
            row1 += 16;
        }

        row2 += 16;
        row3 += 16;
        row4 += 16;
        row5 += 16;
    }

    if (bOverlap)
        invOverlapVerticalStage2(row0, row1, row2 - 8, row3 - 8);
}

void invTransformBlock(
    int** coefficients, int tileWidth, int channel, int bOverlap, int bDownscale, int bLast, int rowInMacroblock)
{
    int* row0 = coefficients[rowInMacroblock + 0] + channel;
    int* row1 = coefficients[rowInMacroblock + 1] + channel;
    int* row2 = coefficients[rowInMacroblock + 2] + channel;
    int* row3 = coefficients[rowInMacroblock + 3] + channel;
    int* row4 = coefficients[rowInMacroblock + 4] + channel;
    int* row5 = coefficients[rowInMacroblock + 5] + channel;

    for (int i = 0; i < ((tileWidth - 1) >> 2) + 1; ++i)
    {
        if (bDownscale)
            downscale(row2, row3, row4, row5);

        invDCTStage1(row2, row3, row4, row5);

        if (i == 0)
        {
            if (bOverlap)
                invOverlapVerticalStage1(row0, row1, row2, row3);

            row0 += 2;
            row1 += 2;
        }
        else
        {
            if (bOverlap)
                invOverlapStage1(row0, row1, row2 - 2, row3 - 2);

            if (bLast && bOverlap)
                invOverlapHorizontalStage1(row4 - 2, row5 - 2);

            row0 += 4;
            row1 += 4;
        }

        row2 += 4;
        row3 += 4;
        row4 += 4;
        row5 += 4;
    }

    if (bOverlap)
        invOverlapVerticalStage1(row0, row1, row2 - 2, row3 - 2);
}
