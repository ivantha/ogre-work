#include "PartA.h"
#include "PartB.h"

int main(int argc, char *argv[])
{
    PartA partA;
    partA.initApp();
    partA.getRoot()->startRendering();
    partA.closeApp();

    PartB partB;
    partB.initApp();
    partB.getRoot()->startRendering();
    partB.closeApp();
    return 0;
}