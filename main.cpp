#include "octree.h"
#include <iostream>
#include "time.h"
#include <random>

using namespace std;
using namespace hlm;

#define PLM printf("%d in %s\n", __LINE__, __FILE__);

int main(){
    srand(time(NULL));
    OctNode root({0.0f}, 1000.0f);
    for(unsigned i = 0; i < 25; i++){
        float n[10];
        for(unsigned j = 0; j < 10; j++){
            n[j] = (float)(rand()%2000) - 1000.0f;
            cout << n[j] << " ";
        }
        cout << endl;
        Point* pt = new Point({n[0],n[1],n[2]}, {n[3],n[4],n[5]}, {n[6],n[7],n[8]}, n[9]);
        root.insert(pt);
    }
    return 0;
}
