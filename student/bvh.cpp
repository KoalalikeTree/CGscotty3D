
#include "../rays/bvh.h"

#include "debug.h"
#include <stack>
#include <cmath>
#include <iostream>


namespace PT {

template <typename Primitive>
void BVH<Primitive>::build(std::vector<Primitive> &&prims, size_t max_leaf_size) {

    // NOTE (PathTracer):
    // This BVH is parameterized on the type of the primitive it contains. This allows
    // us to build a BVH over any type that defines a certain interface. Specifically,
    // we use this to both build a BVH over triangles within each Tri_Mesh, and over
    // a variety of Objects (which might be Tri_Meshes, Spheres, etc.) in Pathtracer.
    //
    // The Primitive interface must implement these two functions:
    //      BBox bbox() const;
    //      Trace hit(const Ray& ray) const;
    // Hence, you may call bbox() and hit() on any value of type Primitive.
    //
    // Finally, also note that while a BVH is a tree structure, our BVH nodes don't
    // contain pointers to children, but rather indicies. This is because instead
    // of allocating each node individually, the BVH class contains a vector that
    // holds all of the nodes. Hence, to get the child of a node, you have to
    // look up the child index in this vector (e.g. nodes[node.l]). Similarly,
    // to create a new node, don't allocate one yourself - use BVH::new_node, which
    // returns the index of a newly added node.

    nodes.clear();
    primitives = std::move(prims);

    // TODO (PathTracer): Task 3
    // Construct a BVH from the given vector of primitives and maximum leaf
    // size configuration. The starter code builds a BVH with a
    // single leaf node (which is also the root) that encloses all the
    // primitives.

    BBox box;
    for (const Primitive &prim : primitives)
        box.enclose(prim.bbox());
    size_t parent_idx = new_node(box, 0, primitives.size(), 0, 0);

    std::vector<size_t> nodes_need_partition_idx;
    nodes_need_partition_idx.push_back(parent_idx);

    //recursively build
    max_leaf_size = 5;
    do{
        size_t cur_node_idx = nodes_need_partition_idx[0];
        // if the amount of primitive is allowed to be contained in one leaf, we're done!
        Node node = nodes[cur_node_idx];

        if (node.size < max_leaf_size){
            return;
            //Do we need to create a new node that l and r is point to itself?
        }

        // 1. what's the best dimension to divide the primitives?
        int bestDim;
        float max_expendLen;
        float left_boudary; // The projected left boudary of the all the buckets
        const int n_bucket = 8; // How many buckets we want to loop through;
        float len_of_a_bucket = 0.0f; // The length of a single bucket, use for classify the primitives
        

        bestDim = 0;
        max_expendLen = FLT_MIN;
        for (int dim=0; dim<3; dim++){
            // 1.1 In which dimension the extend is the largest? 
            float expendLen = node.bbox.max[dim] - node.bbox.min[dim];
            if (max_expendLen < expendLen ){
                max_expendLen = expendLen;
                bestDim = dim;
            }
        }
        left_boudary = node.bbox.min[bestDim];
        len_of_a_bucket = max_expendLen/(1.0f*n_bucket);
        
        // 2. what's the best bucket to divide the primitives?
        std::vector<std::vector<size_t>> idx_buckets_contain_primitives(n_bucket); // It contains the buckets which contains primitives
        std::vector<BBox> bbox_of_buckets(n_bucket); // The bounding box of each buckets, use for compute the area cost
        // 2.1 put all the primitive to different buckets
        for (size_t primi_idx=node.start; primi_idx<node.start+node.size; primi_idx++)
        {   
            // Get the center of the bounding box
            BBox bbox_of_primitive = primitives[primi_idx].bbox();
            float center_of_primitive; 
            center_of_primitive = bbox_of_primitive.center()[bestDim];

            // Calculate which bucket does this primitive belongs to and push it to the buckets vector
            auto bucket_idx = floor((center_of_primitive - left_boudary)/len_of_a_bucket);
            size_t bucket_idx_int = (size_t)bucket_idx;

            //if (bucket_idx_int == idx_buckets_contain_primitives.size()){
            if (bucket_idx_int == idx_buckets_contain_primitives.size()){
                // in case it's a vertical/ horizental plane on the boundary of the buckets
                bucket_idx_int = idx_buckets_contain_primitives.size() -1 ;
            }    
                
            idx_buckets_contain_primitives[bucket_idx_int].push_back(primi_idx); 

            // expand the bbox of the bucket
            bbox_of_buckets[bucket_idx_int].enclose(bbox_of_primitive); 
        }

        // 2.3 Let's use the bucket-primitive vector to calculate the cost
        std::vector<BBox> box_of_left_buckets(n_bucket); // The bounding box of the left part buckets, it contains 1/2/3/../max buckets in order (start from left)
        std::vector<BBox> box_of_right_buckets(n_bucket); // it contains 1/2/3/../max buckets (start from right)
        //The above two vector should always add to the whole nodes

        std::vector<size_t> left_buckets_box_num(n_bucket); // How many box is in the left part?
        std::vector<size_t> right_buckets_box_num(n_bucket);
        left_buckets_box_num[0] = 0; // 0 boxes is in the zero th bucket since it divide all the box to the right
        right_buckets_box_num[0] = 0; // 0 boxes is in the zero th bucket since it divide all the box to the right

        for (size_t i=1; i<n_bucket; i++){
            // we extend the left part with the box of the current buckets (from the last one)
            // box_of_left_buckets[1] is the most left buckets
            // bbox_of_buckets[0] is the most left bucktes
            box_of_left_buckets[i] = box_of_left_buckets[i - 1];
            box_of_left_buckets[i].enclose(bbox_of_buckets[i-1]);
            box_of_right_buckets[i] = box_of_right_buckets[i - 1];
            box_of_right_buckets[i].enclose(bbox_of_buckets[n_bucket-i]);

            //the idx for two vector is different
            // left_buckets_box_num[1] is the most left buckets
            // buckets_contain_primitives[0] is the most left bucktes
            left_buckets_box_num[i] = left_buckets_box_num[i-1]+idx_buckets_contain_primitives[i-1].size();
            // right_buckets_box_num[1] is the most right buckets
            // buckets_contain_primitives[8-1] is the most right buckets
            right_buckets_box_num[i] = right_buckets_box_num[i-1]+idx_buckets_contain_primitives[n_bucket-i].size();
        }

        // set the min_cost to the max number so it can be replaced
        float min_cost = FLT_MAX;
        size_t bestParti_left_num = 0;
        // 2.4 find the best bucket partition with less cost
        for (size_t n_left_buck=1; n_left_buck<n_bucket; n_left_buck++){
            auto left_sa = box_of_left_buckets[n_left_buck].surface_area();
            auto right_sa = box_of_right_buckets[n_bucket - n_left_buck].surface_area();
            //cost = left:number * left:area + right:number * right:area
            auto cost = left_sa * left_buckets_box_num[n_left_buck] + right_sa*right_buckets_box_num[n_bucket - n_left_buck];
            if (cost < min_cost){
                bestParti_left_num = n_left_buck;
                min_cost = cost;
            }
        }
        
        // 3. divide it
        std::vector<size_t> idx_leftPartition;
        std::vector<size_t> idx_rightPartition;
        for (size_t i=0; i<bestParti_left_num; i++){
            for (auto idx_primitive:idx_buckets_contain_primitives[i]){
                idx_leftPartition.push_back(idx_primitive);
            }
        }
        for (size_t i=bestParti_left_num; i<n_bucket; i++){
            for (auto idx_primitive:idx_buckets_contain_primitives[i]){
                idx_rightPartition.push_back(idx_primitive);
            }
        }

        std::vector<Primitive> left_temp;
        std::vector<Primitive> right_temp;
        for (auto i:idx_leftPartition){
            left_temp.push_back(std::move(primitives[i]));
        }
        for (auto j:idx_rightPartition){
            right_temp.push_back(std::move(primitives[j]));
        }
        
        primitives.erase(primitives.begin()+node.start, primitives.begin()+node.start+node.size);
        
        for(size_t j=0; j<left_temp.size() ; j++){
            primitives.insert(primitives.begin()+node.start, std::move(left_temp[j]));
        }
        for(size_t j=0; j<right_temp.size() ; j++){
            primitives.insert(primitives.begin()+node.start+left_temp.size(), std::move(right_temp[j]));
        }

        //sort(idx_leftPartition.begin(), idx_leftPartition.end()); 
        //sort(idx_rightPartition.begin(), idx_rightPartition.end()); 
        //for(size_t idx:idx_leftPartition){
        //    std::swap(primitives[idx], primitives[swap_primitive_idx]);
        //    swap_primitive_idx++;
        //}


        // 3.1 reorder the primitive to left and right
        //auto divide_line = bestParti_left_num*len_of_a_bucket;
        //auto it = std::partition(primitives.begin() + node.start,
        //                            primitives.begin() + node.start + node.size,
        //                            [divide_line, bestDim](const Primitive &p) {
        //                                return p.bbox().center()[bestDim] < divide_line;
                                    //});
        //auto it_idx = std::find(primitives.begin(), primitives.end(), it);
        size_t leftPart_start = node.start;
        size_t leftPart_size = idx_leftPartition.size();
        size_t rightPart_start = node.start+leftPart_size;
        size_t rightPart_size = idx_rightPartition.size();
        
        //left node
        //BBox leftBox;
        //for (size_t i=leftPart_start; i<leftPart_start+leftPart_size; i++){
        //    leftBox.enclose(primitives[i].bbox());
        //}
        

        //size_t l_idx = new_node(leftBox, leftPart_start, leftPart_size, NULL, NULL);
        size_t l_idx = new_node(box_of_left_buckets[bestParti_left_num], leftPart_start, leftPart_size, NULL, NULL);
        //size_t l_idx = new_node(validation_left, leftPart_start, leftPart_size, NULL, NULL);
        nodes[cur_node_idx].l = l_idx;
        
        
        //right node
        //BBox rightBox;
        //for (size_t i=rightPart_start; i<rightPart_start+rightPart_size; i++){
        //    rightBox.enclose(primitives[i].bbox());
        //}

        //size_t r_idx = new_node(rightBox, rightPart_start, rightPart_size, NULL, NULL);
        size_t r_idx = new_node(box_of_right_buckets[n_bucket-bestParti_left_num], rightPart_start, rightPart_size, NULL, NULL);
        //size_t r_idx = new_node(validation_right, rightPart_start, rightPart_size, NULL, NULL);
        nodes[cur_node_idx].r = r_idx;

        // if the amount of primitives is larger than the max_leaf_size, we need to further divide it
        nodes_need_partition_idx.erase(nodes_need_partition_idx.begin());
        if (leftPart_size>max_leaf_size){
            nodes_need_partition_idx.push_back(l_idx);
            
        }

        if (rightPart_size>max_leaf_size){
            nodes_need_partition_idx.push_back(r_idx);
        }

    }while(!nodes_need_partition_idx.empty());
}

template <typename Primitive> Trace BVH<Primitive>::hit(const Ray &ray) const {

    // TODO (PathTracer): Task 3
    // Implement ray - BVH intersection test. A ray intersects
    // with a BVH aggregate if and only if it intersects a primitive in
    // the BVH that is not an aggregate.

    // The starter code simply iterates through all the primitives.
    // Again, remember you can use hit() on any Primitive value.

    Trace ret;
    
    if (nodes.size()<8){
        for (const Primitive &prim : primitives) {
            Trace hit = prim.hit(ray);
            ret = Trace::min(ret, hit);
        }
        return ret;
    }

    //for (const Primitive &prim : primitives) {
    //        Trace hit = prim.hit(ray);
    //        ret = Trace::min(ret, hit);
    //    }
    //return ret;
    
    
    size_t node_idx = 0;
    ret = this->find_closest_hit(ray, node_idx);

    return ret;
    
}

template <typename Primitive> Trace BVH<Primitive>::find_closest_hit(const Ray &ray, size_t node_idx) const{
    Trace ret;
    //ret.time = FLT_MAX;
    
    bool isleaf = nodes[node_idx].is_leaf();
    if (isleaf){
        for (size_t i = nodes[node_idx].start; i<nodes[node_idx].start+nodes[node_idx].size; i++) 
        {
            Trace hit_pri = primitives[i].hit(ray);
            //std::cout << "hit with the "<< i <<"th leaf node ? " << hit.hit << std::endl;
            if (hit_pri.hit){
                ret = Trace::min(ret, hit_pri);
                return ret;
            }
        }
    }else{
        Vec2 left_time;
        size_t left_node_idx = nodes[node_idx].l;
        BBox box_left = nodes[left_node_idx].bbox;
        bool hit_left = box_left.hit(ray, left_time);

        Vec2 right_time;
        size_t right_node_idx = nodes[node_idx].r;
        BBox box_right = nodes[right_node_idx].bbox;
        bool hit_right = box_right.hit(ray, right_time);

        //if(hit_left){
        //    if(hit_right){
        //            size_t first_node_idx = (left_time[0]<right_time[0]) ? left_node_idx:right_node_idx;
        //            size_t second_node_idx = (left_time[0]<right_time[0]) ? right_node_idx:left_node_idx;

        //            ret = find_closest_hit(ray, first_node_idx);
        //            if(right_time[0]<ret.time){
        //                ret = find_closest_hit(ray, second_node_idx);
        //        }
        //        return ret;
        //    }else{
        //        ret = find_closest_hit(ray, left_node_idx);
        //        return ret;
        //    }
        //}else if(hit_right){
        //    ret = find_closest_hit(ray, right_node_idx);
        //    return ret;
        //}else{
        //    return ret;
        //}

        if (hit_left && !hit_right){
            node_idx = nodes[node_idx].l;
            ret = find_closest_hit(ray, node_idx);
            return ret;
        }

        // intersect with the right node?
        if (hit_right && !hit_left){
            node_idx = nodes[node_idx].r;
            ret = find_closest_hit(ray, node_idx);
            return ret;
        }
        
        // intersect with the left & right node?
        if(hit_right && hit_left){
            size_t node_first_idx = 0;
            size_t node_second_idx = 0;
            float node_second_time;
            
            // which one is more close?
            if (left_time[0] <= right_time[0]){
                node_first_idx = nodes[node_idx].l;
            }else{node_first_idx = nodes[node_idx].r;}
            
            if (left_time[0] <= right_time[0]){
                node_second_idx = nodes[node_idx].r;
                node_second_time = right_time[0];
            }else{
                node_second_idx = nodes[node_idx].l;
                node_second_time = left_time[0];
            }

            ret = find_closest_hit(ray, node_first_idx);
            if (node_second_time<ret.time || (ret.hit==false)){
            //if (right_time[0]<ret.time){
                Trace ret2 = find_closest_hit(ray, node_second_idx);
                return Trace::min(ret,ret2);
            }
        }
    }
    return ret;
}

template <typename Primitive>
BVH<Primitive>::BVH(std::vector<Primitive> &&prims, size_t max_leaf_size) {
    build(std::move(prims), max_leaf_size);
}

template <typename Primitive> bool BVH<Primitive>::Node::is_leaf() const {
    return l == 0 && r == 0;
}

template <typename Primitive>
size_t BVH<Primitive>::new_node(BBox box, size_t start, size_t size, size_t l, size_t r) {
    Node n;
    n.bbox = box;
    n.start = start;
    n.size = size;
    n.l = l;
    n.r = r;
    nodes.push_back(n);
    return nodes.size() - 1;
}

template <typename Primitive> BBox BVH<Primitive>::bbox() const { 
    return nodes[0].bbox; 
}

template <typename Primitive> std::vector<Primitive> BVH<Primitive>::destructure() {
    nodes.clear();
    return std::move(primitives);
}

template <typename Primitive> void BVH<Primitive>::clear() {
    nodes.clear();
    primitives.clear();
}

template <typename Primitive>
size_t BVH<Primitive>::visualize(GL::Lines &lines, GL::Lines &active, size_t level,
                                 const Mat4 &trans) const {

    std::stack<std::pair<size_t, size_t>> tstack;
    tstack.push({0, 0});
    size_t max_level = 0;

    if (nodes.empty())
        return max_level;

    while (!tstack.empty()) {

        auto [idx, lvl] = tstack.top();
        max_level = std::max(max_level, lvl);
        const Node &node = nodes[idx];
        tstack.pop();

        Vec3 color = lvl == level ? Vec3(1.0f, 0.0f, 0.0f) : Vec3(1.0f);
        GL::Lines &add = lvl == level ? active : lines;

        BBox box = node.bbox;
        box.transform(trans);
        Vec3 min = box.min, max = box.max;

        auto edge = [&](Vec3 a, Vec3 b) { add.add(a, b, color); };

        edge(min, Vec3{max.x, min.y, min.z});
        edge(min, Vec3{min.x, max.y, min.z});
        edge(min, Vec3{min.x, min.y, max.z});
        edge(max, Vec3{min.x, max.y, max.z});
        edge(max, Vec3{max.x, min.y, max.z});
        edge(max, Vec3{max.x, max.y, min.z});
        edge(Vec3{min.x, max.y, min.z}, Vec3{max.x, max.y, min.z});
        edge(Vec3{min.x, max.y, min.z}, Vec3{min.x, max.y, max.z});
        edge(Vec3{min.x, min.y, max.z}, Vec3{max.x, min.y, max.z});
        edge(Vec3{min.x, min.y, max.z}, Vec3{min.x, max.y, max.z});
        edge(Vec3{max.x, min.y, min.z}, Vec3{max.x, max.y, min.z});
        edge(Vec3{max.x, min.y, min.z}, Vec3{max.x, min.y, max.z});

        if (node.l)
            tstack.push({node.l, lvl + 1});
        if (node.r)
            tstack.push({node.r, lvl + 1});

        if (!node.l && !node.r) {
            for (size_t i = node.start; i < node.start + node.size; i++) {
                size_t c = primitives[i].visualize(lines, active, level - lvl, trans);
                max_level = std::max(c, max_level);
            }
        }
    }
    return max_level;
}

} // namespace PT
