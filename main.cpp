#include "octree.h"
#include <iostream>

using namespace std;
using namespace hlm;

int main(){
    OctNode root({0.0f}, 1000.0f);
    Point* pt = new Point({1.1f}, {2.3}, {1.0f}, 1.0f); 
    root.insert(pt);
    return 0;
}
