#ifndef OCTREE_H
#define OCTREE_H

#include "hlm.h"

struct Point{
    hlm::vec3 pos, norm, col;
    float r;
    Point(const hlm::vec3& p, const hlm::vec3& n, const hlm::vec3& c, float _r)
    : pos(p), norm(n), col(c), r(_r){};
};

typedef Point DataType;

struct OctNode{
    hlm::vec3 center;
    OctNode* children;
    DataType* data;
    float half_len;
    OctNode(const hlm::vec3& _center, float _half_len)
    : center(_center), children(nullptr), data(nullptr), half_len(_half_len){};
    ~OctNode(){ delete data; free(children); };
    inline char getChildIdx(hlm::vec3& location){
        char i = 0;
        i |= location.x >= center.x ? 4 : 0;
        i |= location.y >= center.y ? 2 : 0;
        i |= location.z >= center.z ? 1 : 0;
        return i;
    }
    inline void makeChildren(){
        children = (OctNode*)malloc(sizeof(OctNode) * 8);
        float q_len = half_len * 0.5f;
        for(unsigned char i=0; i<8; i++){
            hlm::vec3 n_c(center);
            n_c.x += i&4 ? q_len : -q_len;
            n_c.y += i&2 ? q_len : -q_len;
            n_c.z += i&1 ? q_len : -q_len;
            *(children+i) = OctNode(n_c, q_len);
        }
    }
    inline bool isLeaf(){ return !data && !children;}
    inline void insert(DataType* d){
        if(isLeaf()){
            data = d;
            return;
        }
        if(!children)
            makeChildren();
        char i = getChildIdx(data->pos);
        (children+i)->insert(data);
        data = nullptr;
        i = getChildIdx(d->pos);
        (children+i)->insert(d);
    }
};


struct AABB{
    hlm::vec3 bounds[2];
    AABB(OctNode& o){
        bounds[0] = o.center - o.half_len;
        bounds[1] = o.center + o.half_len;
    }
};


struct Ray{
    hlm::vec3 origin, direction, inv_dir;
    unsigned char sign[3];
    Ray(hlm::vec3& o, hlm::vec3& d) : origin(o), direction(d) {
        inv_dir = 1.0f / d;
        sign[0] = inv_dir.x < 0 ? 1 : 0;
        sign[1] = inv_dir.y < 0 ? 1 : 0;
        sign[2] = inv_dir.z < 0 ? 1 : 0;
    }
    inline float intersect(AABB& box){
        hlm::vec3 min, max;
        min.x = inv_dir.x * ( box.bounds[sign[0]].x - origin.x );
        max.x = inv_dir.x * ( box.bounds[1 - sign[0]].x - origin.x );
        min.y = inv_dir.y * ( box.bounds[sign[0]].y - origin.y ); 
        max.y = inv_dir.y * ( box.bounds[1 - sign[0]].y - origin.y );
        min.z = inv_dir.z * ( box.bounds[sign[0]].z - origin.z );
        max.z = inv_dir.z * ( box.bounds[1 - sign[0]].z - origin.z );
        min.x = hlm::max(min.x, hlm::max(min.y, min.z));
        max.x = hlm::min(max.x, hlm::min(max.y, max.z));
        return min.x > max.x ? -1.0f : min.x;
    }
};

#endif
