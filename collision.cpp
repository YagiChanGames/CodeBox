//
//  collision.cpp
//  
//
//  Created by Chocomel on 2016/01/30.
//
//

#include"DirectXMath.h"
#include"DirectXCollision.h"

struct model{
    SimpleMath::Vector3 pos;
    SimpleMath::Color color;
    BoundingBox bounds;
    int index;
};

class OctTree{
    std::vector<OctTree> nodes;
    std::list<model> objects;
    BoundingBox bounds;
};

public:

OctTree(BoundingBox const& bounds):bounds(bounds)
{
}

void insert(model const& item,int level)
{
    if(level>0){
        if(nodes.empty()){
            nodes.reserve(8);
            XMFLOAT3 h(bounds.Extents.x/2.f,bounds.Extents.y/2.f,bounds.Extents.z/2.f);
            nodes.emplace_back(BoundingBox(
                                           XMFLOAT3(bounds.Center.x - h.x,bounds.Center.y - h.y,bounds.Center.z - h.z)));
            nodes.emplace_back(BoundingBox(
                                           XMFLOAT3(bounds.Center.x + h.x,bounds.Center.y - h.y,bounds.Center.z - h.z)));
            nodes.emplace_back(BoundingBox(
                                           XMFLOAT3(bounds.Center.x - h.x,bounds.Center.y + h.y,bounds.Center.z - h.z)));
            nodes.emplace_back(BoundingBox(
                                           XMFLOAT3(bounds.Center.x + h.x,bounds.Center.y + h.y,bounds.Center.z - h.z)));
            nodes.emplace_back(BoundingBox(
                                           XMFLOAT3(bounds.Center.x - h.x,bounds.Center.y - h.y,bounds.Center.z + h.z)));
            nodes.emplace_back(BoundingBox(
                                           XMFLOAT3(bounds.Center.x + h.x,bounds.Center.y - h.y,bounds.Center.z + h.z)));
            nodes.emplace_back(BoundingBox(
                                           XMFLOAT3(bounds.Center.x - h.x,bounds.Center.y + h.y,bounds.Center.z + h.z)));
            nodes.emplace_back(BoundingBox(
                                           XMFLOAT3(bounds.Center.x + h.x,bounds.Center.y + h.y,bounds.Center.z + h.z)));
        }
        for(auto& node: nodes){
            if(node.bounds.Contains(item.bounds)==CONTAINS){
                node.insert(item,level - 1);
                return;
            }
        }
    }
    objects.push_back(item);
}