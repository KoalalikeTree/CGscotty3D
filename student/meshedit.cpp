
#include <queue>
#include <set>
#include <unordered_map>

#include "../geometry/halfedge.h"
#include "debug.h"
#include <iostream>
#include <vector>

/* Note on local operation return types:

        The local operations all return a std::optional<T> type. This is used so that your
        implementation can signify that it does not want to perform the operation for
        whatever reason (e.g. you don't want to allow the user to erase the last vertex).

        An optional can have two values: std::nullopt, or a value of the type it is
        parameterized on. In this way, it's similar to a pointer, but has two advantages:
        the value it holds need not be allocated elsewhere, and it provides an API that
        forces the user to check if it is null before using the value.

        In your implementaiton, if you have successfully performed the operation, you can
        simply return the required reference:

                ... collapse the edge ...
                return collapsed_vertex_ref;

        And if you wish to deny the operation, you can return the null optional:

                return std::nullopt;

        Note that the stubs below all reject their duties by returning the null optional.
*/

/*
        This method should replace the given vertex and all its neighboring
        edges and faces with a single face, returning the new face.
 */
std::optional<Halfedge_Mesh::FaceRef> Halfedge_Mesh::erase_vertex(Halfedge_Mesh::VertexRef v) {

    (void)v;
    
    return std::nullopt;
}

/*
        This method should erase the given edge and return an iterator to the
        merged face.
 */
std::optional<Halfedge_Mesh::FaceRef> Halfedge_Mesh::erase_edge(Halfedge_Mesh::EdgeRef e) {
    
    (void)e;
    if (e -> on_boundary()) {
		return std::nullopt;
	}
    if(e==edges.end()){
        return std::nullopt;
    }
    //if it's a single line

  //  if ((e->halfedge()->next() == e->halfedge()->twin()) || 
  //      (e->halfedge()->twin()->next() == e->halfedge())||
  //          ){
  //      //TODO how to remove a single line? what should I return?
  //      Halfedge_Mesh::erase(e->halfedge()->twin()->vertex());
		//Halfedge_Mesh::erase(e->halfedge()->vertex());
		//Halfedge_Mesh::erase(e->halfedge()->face());
		//Halfedge_Mesh::erase(e->halfedge()->twin());
		//Halfedge_Mesh::erase(e->halfedge());
		//Halfedge_Mesh::erase(e);
		//return std::nullopt;
  //  }

    
    std::cout << e->halfedge()->twin()->face()->id() << std::endl;
    std::cout << e->halfedge()->face()->id() << std::endl;

    Halfedge_Mesh::HalfedgeRef left_he = e->halfedge();
    Halfedge_Mesh::HalfedgeRef right_he = left_he->twin();
    
    Halfedge_Mesh::HalfedgeRef left_prev = left_he;
    for(Halfedge_Mesh::HalfedgeRef edge=left_he; edge->next()!=left_he; edge=edge->next()){
        left_prev=left_prev->next();
    }
    Halfedge_Mesh::HalfedgeRef left_next = left_he->next();

    Halfedge_Mesh::HalfedgeRef right_prev = right_he;
    Halfedge_Mesh::HalfedgeRef right_next = right_he->next();
    for(Halfedge_Mesh::HalfedgeRef edge=right_he; edge->next()!=right_he; edge=edge->next()){
        right_prev=right_prev->next();
    }

    Halfedge_Mesh::VertexRef v_left = left_he->vertex();
    
    Halfedge_Mesh::VertexRef v_right = right_he->vertex();

    Halfedge_Mesh::FaceRef f_left = left_he->face();
    Halfedge_Mesh::FaceRef f_right = right_he->face();
    
    if (v_left->halfedge() == left_he){
        v_left->halfedge() = right_next;
    }
    if (v_right->halfedge() == right_he){
        v_right->halfedge() = left_next;
    }
    if (f_left->halfedge() == left_he){
        f_left->halfedge() = left_prev;
    }
    
    
    for(Halfedge_Mesh::HalfedgeRef edge=right_next; edge!=right_he; edge=edge->next()){
        edge->face()=f_left;
    }

    left_prev->next()= right_next;
    right_prev->next()= left_next;

    Halfedge_Mesh::erase(e);
    Halfedge_Mesh::erase(left_he);
    Halfedge_Mesh::erase(right_he);
    Halfedge_Mesh::erase(f_right);
    return f_left;
    //return std::nullopt;
}

/*
        This method should collapse the given edge and return an iterator to
        the new vertex created by the collapse.
*/
std::optional<Halfedge_Mesh::VertexRef> Halfedge_Mesh::collapse_edge(Halfedge_Mesh::EdgeRef e) {

    (void)e;
    Halfedge_Mesh::HalfedgeRef h0= e->halfedge();
    Halfedge_Mesh::HalfedgeRef a0= h0->twin();
    Halfedge_Mesh::VertexRef leftVtx= h0->vertex();
    Halfedge_Mesh::VertexRef rightVtx= a0->vertex();

    Halfedge_Mesh::HalfedgeRef h1 = h0->next();
    Halfedge_Mesh::HalfedgeRef a1 = a0->next();
    Halfedge_Mesh::HalfedgeRef h2 = h1->twin();
    Halfedge_Mesh::HalfedgeRef a2 = a1->twin();

    Halfedge_Mesh::HalfedgeRef h3 = h1;
    for(Halfedge_Mesh::HalfedgeRef edge=h1; edge->next()!=h0; edge=edge->next()){
        h3=h3->next();
    }
    Halfedge_Mesh::HalfedgeRef a3 = a1;
    for(Halfedge_Mesh::HalfedgeRef edge=a1; edge->next()!=a0; edge=edge->next()){
        a3=a3->next();
    }

    
    //triangle
    if (h1->next() == h3){
        Halfedge_Mesh::EdgeRef e1 = h1->edge();
        Halfedge_Mesh::FaceRef f1 = h1->face();
    	Halfedge_Mesh::VertexRef v2 = h2->vertex();
    	Halfedge_Mesh::FaceRef f_inner = h2->face();
    	Halfedge_Mesh::HalfedgeRef h2_next = h2->next();
    	Halfedge_Mesh::HalfedgeRef h2_prev = h2_next;

        for(Halfedge_Mesh::HalfedgeRef edge=h2_next; edge->next()!=h2; edge=edge->next()){
            h2_prev=h2_prev->next();
        }
        
        //remove all edge connect to h2/h1
        h0->next() = h2_next;
    	h2_prev->next() = h3;

        //remove all vertices connect to h2/h1
    	rightVtx->halfedge() = a0;
        v2->halfedge() = h3;
        //remove all face connect to h2/h1
    	f_inner->halfedge() = h2_next;

        //remove all face connect to f1
    	h0->face() = f_inner;
    	h3->face() = f_inner;

        Halfedge_Mesh::erase(e1);
        Halfedge_Mesh::erase(f1);
        Halfedge_Mesh::erase(h2);
        Halfedge_Mesh::erase(h1);
    }

    if (a1->next() == a3) {
    	Halfedge_Mesh::EdgeRef e2 = a1->edge();
        Halfedge_Mesh::FaceRef f2 = a1->face();
    	Halfedge_Mesh::VertexRef v2 = a2->vertex();
        Halfedge_Mesh::FaceRef f_inner = a2->face();
        Halfedge_Mesh::HalfedgeRef h2_next = a2->next();
        Halfedge_Mesh::HalfedgeRef h2_prev = h2_next;

        for(Halfedge_Mesh::HalfedgeRef edge=h2_next; edge->next()!=a2; edge=edge->next()){
            h2_prev=h2_prev->next();
        }

        a0->next() = h2_next;
        h2_prev->next()=a3;

    	v2->halfedge() = a3;

    	// face
    	f_inner->halfedge() = h2_next;
        
    	a0->face() = f_inner;
    	a3->face() = f_inner;

    	// delete
    	Halfedge_Mesh::erase(e2);
    	Halfedge_Mesh::erase(f2);
    	Halfedge_Mesh::erase(a2);
        Halfedge_Mesh::erase(a1);
    }

    Halfedge_Mesh::FaceRef f1 = h0->face();
    Halfedge_Mesh::FaceRef f2 = a0->face();
    Vec3 centroid = e->center();

    Halfedge_Mesh::HalfedgeRef currentH = h0->next();
    do{
        currentH->vertex()=leftVtx;
        currentH = currentH->twin()->next();
    }while(currentH!=a0);
    h3->next()=h0->next();
    a3->next()=a0->next();

    //vertex
    leftVtx->halfedge() = h3->twin();
    leftVtx->pos = centroid;

    f1->halfedge() = h3;
    f2->halfedge() = a3;

    Halfedge_Mesh::erase(rightVtx);
    Halfedge_Mesh::erase(e);
    Halfedge_Mesh::erase(h0);
    Halfedge_Mesh::erase(a0);

    return leftVtx;
}

/*
        This method should collapse the given face and return an iterator to
        the new vertex created by the collapse.
*/
std::optional<Halfedge_Mesh::VertexRef> Halfedge_Mesh::collapse_face(Halfedge_Mesh::FaceRef f) {

    (void)f;
    // points name
    
    


    return std::nullopt;
}

/*
        This method should flip the given edge and return an iterator to the
        flipped edge.
*/
std::optional<Halfedge_Mesh::EdgeRef> Halfedge_Mesh::flip_edge(Halfedge_Mesh::EdgeRef e) {

    (void)e;
    Halfedge_Mesh::HalfedgeRef left_he = e->halfedge();
    Halfedge_Mesh::HalfedgeRef right_he = left_he->twin();

    Halfedge_Mesh::HalfedgeRef left_next = left_he->next();
    Halfedge_Mesh::HalfedgeRef left_next_next = left_next->next();
    Halfedge_Mesh::HalfedgeRef right_next = right_he->next();
    Halfedge_Mesh::HalfedgeRef right_next_next = right_next->next();

    Halfedge_Mesh::HalfedgeRef left_prev = left_he;
    for(Halfedge_Mesh::HalfedgeRef edge=left_he; edge->next()!=left_he; edge=edge->next()){
        left_prev=left_prev->next();
    }
    Halfedge_Mesh::HalfedgeRef right_prev = right_he;
    for(Halfedge_Mesh::HalfedgeRef edge=right_prev; edge->next()!=right_he; edge=edge->next()){
        right_prev=right_prev->next();
    }

    // vertices
    Halfedge_Mesh::VertexRef v_left = left_he->vertex();
    Halfedge_Mesh::VertexRef v_right = right_he->vertex();
    Halfedge_Mesh::VertexRef v_left_next_next = left_next_next->vertex();
    Halfedge_Mesh::VertexRef v_right_next_next = right_next_next->vertex();

    // faces
    Halfedge_Mesh::FaceRef f_left = left_he->face();
    Halfedge_Mesh::FaceRef f_right = right_he->face();

    // Assign edge
    left_he->next() = left_next_next;
    left_he->vertex() = v_right_next_next;

    left_next->next() = right_he;
    left_next->face() = f_right;

    left_prev->next() = right_next;

    right_he->next() = right_next_next;
    right_he->vertex() = v_left_next_next;

    right_next->next() = left_he;
    right_next->face() = f_left;

    right_prev->next() = left_next;
    
    v_left->halfedge() = right_next;
    v_right->halfedge() = left_next;

    f_left->halfedge() = left_he;
    f_right->halfedge() = right_he;
    
    return e;
}

/*
        This method should split the given edge and return an iterator to the
        newly inserted vertex. The halfedge of this vertex should point along
        the edge that was split, rather than the new edges.
*/
std::optional<Halfedge_Mesh::VertexRef> Halfedge_Mesh::split_edge(Halfedge_Mesh::EdgeRef e) {

    (void)e;
    std::cout<<"Is e on boundary? " << e->on_boundary() << std::endl;
    if (!e->on_boundary()){
        //triangle
        Halfedge_Mesh::HalfedgeRef h1 = e->halfedge();
        Halfedge_Mesh::HalfedgeRef h1_twin = e->halfedge()->twin();
        Halfedge_Mesh::FaceRef f1 = h1->face();
        Halfedge_Mesh::FaceRef f4 = h1_twin->face();
        Halfedge_Mesh::HalfedgeRef b0 = h1->next();
        Halfedge_Mesh::HalfedgeRef b1 = b0->next();
        Halfedge_Mesh::HalfedgeRef b2 = h1_twin->next();
        Halfedge_Mesh::HalfedgeRef b3 = b2->next();
        Halfedge_Mesh::VertexRef v1 = b0->vertex();
        Halfedge_Mesh::VertexRef v2 = b1->vertex();
        Halfedge_Mesh::VertexRef v3 = b2->vertex();
        Halfedge_Mesh::VertexRef v4 = b3->vertex();

        if(b1->next()==h1 && b3->next()==h1_twin){
            std::cout<<"Both side of this edge are triangle"<<std::endl;
            
            // set the new vertice
            Halfedge_Mesh::VertexRef v = new_vertex();
            v->pos = e->center();

            Halfedge_Mesh::EdgeRef e2 = new_edge();
            Halfedge_Mesh::EdgeRef e3 = new_edge();
            Halfedge_Mesh::EdgeRef e4 = new_edge();
            
            Halfedge_Mesh::FaceRef f2 = new_face();
            Halfedge_Mesh::FaceRef f3 = new_face();
            
            Halfedge_Mesh::HalfedgeRef h2 = new_halfedge();
            Halfedge_Mesh::HalfedgeRef h2_twin = new_halfedge();
            Halfedge_Mesh::HalfedgeRef h3 = new_halfedge();
            Halfedge_Mesh::HalfedgeRef h3_twin = new_halfedge();
            Halfedge_Mesh::HalfedgeRef h4 = new_halfedge();
            Halfedge_Mesh::HalfedgeRef h4_twin = new_halfedge();

            // f1 and its related edge
            f1->halfedge()=h1;
            f2->halfedge()=h2;
            f3->halfedge()=h3;
            f4->halfedge()=h4;
            
            v->halfedge()=h1;
            v2->halfedge()=h2_twin;
            v4->halfedge()=h4_twin;
            v3->halfedge()=h3_twin;

            e2->halfedge()=h2;
            e3->halfedge()=h3;
            e4->halfedge()=h4;

            // f1 edges
            h1->vertex()=v;
            h1_twin->next()=h4;
            h1_twin->twin()=h1;
            h1_twin->face()=f4;
            h1_twin->vertex()=v1;
            
            b0->next()=h2_twin;

            h2_twin->next()=h1;
            h2_twin->twin()=h2;
            h2_twin->edge()=e2;
            h2_twin->face()=f1;
            h2_twin->vertex()=v2;

            //f2 edges
            b1->next()=h3_twin;
            b1->face()=f2;

            h2->next()=b1;
            h2->twin()=h2_twin;
            h2->edge()=e2;
            h2->face()=f2;
            h2->vertex()=v;

            h3_twin->next()=h2;
            h3_twin->twin()=h3;
            h3_twin->edge()=e3;
            h3_twin->face()=f2;
            h3_twin->vertex()=v3;
            
            // f3 edges
            h3->next()=b2;
            h3->twin()=h3_twin;
            h3->edge()=e3;
            h3->face()=f3;
            h3->vertex()=v;

            b2->next()=h4_twin;
            b2->face()=f3;

            h4_twin->next()=h3;
            h4_twin->twin()=h4;
            h4_twin->edge()=e4;
            h4_twin->face()=f3;
            h4_twin->vertex()=v4;

            // f4
            h4->next()=b3;
            h4->twin()=h4_twin;
            h4->edge()=e4;
            h4->face()=f4;
            h4->vertex()=v;

            b3->next()=h1_twin;
            b3->face()=f4;

            return v;
            
        }
    }
    return std::nullopt;
}

/* Note on the beveling process:

        Each of the bevel_vertex, bevel_edge, and bevel_face functions do not represent
        a full bevel operation. Instead, they should only update the _connectivity_ of
        the mesh, _not_ the positions of newly created vertices. In fact, you should set
        the positions of new vertices to be exactly the same as wherever they "started from."

        When you click on a mesh element while in bevel mode, one of those three functions
        is called. But, because you may then adjust the distance/offset of the newly
        beveled face, we need another method of updating the positions of the new vertices.

        This is where bevel_vertex_positions, bevel_edge_positions, and
        bevel_face_positions come in: these functions are called repeatedly as you
        move your mouse, the position of which determins the normal and tangent offset
        parameters. These functions are also passed an array of the original vertex
        positions: for  bevel_vertex, it has one element, the original vertex position,
        for bevel_edge,  two for the two vertices, and for bevel_face, it has the original
        position of each vertex in halfedge order. You should use these positions, as well
        as the normal and tangent offset fields to assign positions to the new vertices.

        Finally, note that the normal and tangent offsets are not relative values - you
        should compute a particular new position from them, not a delta to apply.
*/

/*
        This method should replace the vertex v with a face, corresponding to
        a bevel operation. It should return the new face.  NOTE: This method is
        responsible for updating the *connectivity* of the mesh only---it does not
        need to update the vertex positions.  These positions will be updated in
        Halfedge_Mesh::bevel_vertex_positions (which you also have to
        implement!)
*/
std::optional<Halfedge_Mesh::FaceRef> Halfedge_Mesh::bevel_vertex(Halfedge_Mesh::VertexRef v) {

    (void)v;
    return std::nullopt;
}

/*
        This method should replace the edge e with a face, corresponding to a
        bevel operation. It should return the new face. NOTE: This method is
        responsible for updating the *connectivity* of the mesh only---it does not
        need to update the vertex positions.  These positions will be updated in
        Halfedge_Mesh::bevel_edge_positions (which you also have to
        implement!)
*/
std::optional<Halfedge_Mesh::FaceRef> Halfedge_Mesh::bevel_edge(Halfedge_Mesh::EdgeRef e) {

    (void)e;
    return std::nullopt;
}

/*
        This method should replace the face f with an additional, inset face
        (and ring of faces around it), corresponding to a bevel operation. It
        should return the new face.  NOTE: This method is responsible for updating
        the *connectivity* of the mesh only---it does not need to update the vertex
        positions. These positions will be updated in
        Halfedge_Mesh::bevel_face_positions (which you also have to
        implement!)
*/
std::optional<Halfedge_Mesh::FaceRef> Halfedge_Mesh::bevel_face(Halfedge_Mesh::FaceRef f) {

    (void)f;
    std::cout<<"I am here " << f->is_boundary() << std::endl;

    if(!f->is_boundary())
    {
        Halfedge_Mesh::HalfedgeRef h = f->halfedge();
		std::vector<Halfedge_Mesh::HalfedgeRef> he;
		std::vector<Halfedge_Mesh::HalfedgeRef> newhefc;
		std::vector<Halfedge_Mesh::HalfedgeRef> newhefc_t;
		std::vector<Halfedge_Mesh::HalfedgeRef> newhe;
		std::vector<Halfedge_Mesh::HalfedgeRef> newhe_t;
		std::vector<Halfedge_Mesh::EdgeRef> newedg_hrz;
		std::vector<Halfedge_Mesh::EdgeRef> newedg_vtc;
		std::vector<Halfedge_Mesh::VertexRef> newvtx;
		std::vector<Halfedge_Mesh::VertexRef> origvtx;
		std::vector<Halfedge_Mesh::FaceRef> newfc;

        do
		{
		    he.push_back(h);
			origvtx.push_back(h->vertex());
			h = h->next();

            newedg_hrz.push_back(new_edge());
			newedg_vtc.push_back(new_edge());
			newvtx.push_back(new_vertex());
			newfc.push_back(new_face());
			newhefc.push_back(new_halfedge());
			newhefc_t.push_back(new_halfedge());
			newhe.push_back(new_halfedge());
			newhe_t.push_back(new_halfedge());
		}while(h != f->halfedge());

        size_t N = he.size();
	
		for(size_t i = 0; i < he.size(); i++)
		{
			he[i]->next() = newhe[i];
			he[i]->face() = newfc[i];
		    
            newhefc[i]->next()= newhefc[(i+1)%N];
			newhefc[i]->twin() = newhefc_t[i];
			newhefc[i]->face() = f;
			newhefc[i]->edge() = newedg_hrz[i];
			newhefc[i]->vertex() = newvtx[i];
		
			newhefc_t[i]->next() = newhe_t[(i+N-1) % N];
            newhefc_t[i]->twin() = newhefc[i];
			newhefc_t[i]->face() = newfc[i];
			newhefc_t[i]->edge() = newedg_hrz[i];
			newhefc_t[i]->vertex() = newvtx[(i+1) % N];

			newhe[i]->next() = newhefc_t[i];
			newhe[i]->twin() = newhe_t[i];
			newhe[i]->face() = newfc[i];
			newhe[i]->edge() = newedg_vtc[i];
			newhe[i]->vertex() = he[i]->twin()->vertex();
		
            newhe_t[i]->next() = he[(i+1)%N];
			newhe_t[i]->twin() = newhe[i];
			newhe_t[i]->edge() = newedg_vtc[i];
			i == he.size() - 1 ? newhe_t[i]->face() = newfc[0] : newhe_t[i]->face() = newfc[i+1];
			i == he.size() - 1 ? newhe_t[i]->vertex() = newvtx[0] : newhe_t[i]->vertex() = newvtx[i+1]; 
		
		
			newvtx[i]->halfedge() = newhefc[i];
			newvtx[i]->pos = he[i]->vertex()->pos;
			newedg_hrz[i]->halfedge() = newhefc[i];
			newedg_vtc[i]->halfedge() = newhe[i];
			newfc[i]->halfedge() = newhefc[i]->twin();	
		 
		}
        f->halfedge() = newhefc[0];
        std::cout<<"Hello world"<<std::endl;
    }

    return f;
}

/*
        Compute new vertex positions for the vertices of the beveled vertex.

        These vertices can be accessed via new_halfedges[i]->vertex()->pos for
        i = 1, ..., new_halfedges.size()-1.

        The basic strategy here is to loop over the list of outgoing halfedges,
        and use the original vertex position and its associated outgoing edge
        to compute a new vertex position along the outgoing edge.
*/
void Halfedge_Mesh::bevel_vertex_positions(const std::vector<Vec3> &start_positions,
                                           Halfedge_Mesh::FaceRef face, float tangent_offset) {

    std::vector<HalfedgeRef> new_halfedges;
    auto h = face->halfedge();
    do {
        new_halfedges.push_back(h);
        h = h->next();
    } while (h != face->halfedge());

    (void)new_halfedges;
    (void)start_positions;
    (void)face;
    (void)tangent_offset;
}

/*
        Compute new vertex positions for the vertices of the beveled edge.

        These vertices can be accessed via new_halfedges[i]->vertex()->pos for
        i = 1, ..., new_halfedges.size()-1.

        The basic strategy here is to loop over the list of outgoing halfedges,
        and use the preceding and next vertex position from the original mesh
        (in the orig array) to compute an offset vertex position.

        Note that there is a 1-to-1 correspondence between halfedges in
        newHalfedges and vertex positions
        in orig.  So, you can write loops of the form

        for(size_t i = 0; i < new_halfedges.size(); i++)
        {
                Vector3D pi = start_positions[i]; // get the original vertex
                position corresponding to vertex i
        }
*/
void Halfedge_Mesh::bevel_edge_positions(const std::vector<Vec3> &start_positions,
                                         Halfedge_Mesh::FaceRef face, float tangent_offset) {

    std::vector<HalfedgeRef> new_halfedges;
    auto h = face->halfedge();
    do {
        new_halfedges.push_back(h);
        h = h->next();
    } while (h != face->halfedge());

    (void)new_halfedges;
    (void)start_positions;
    (void)face;
    (void)tangent_offset;
}

/*
        Compute new vertex positions for the vertices of the beveled face.

        These vertices can be accessed via new_halfedges[i]->vertex()->pos for
        i = 1, ..., new_halfedges.size()-1.

        The basic strategy here is to loop over the list of outgoing halfedges,
        and use the preceding and next vertex position from the original mesh
        (in the start_positions array) to compute an offset vertex
        position.

        Note that there is a 1-to-1 correspondence between halfedges in
        new_halfedges and vertex positions
        in orig. So, you can write loops of the form

        for(size_t i = 0; i < new_halfedges.size(); hs++)
        {
                Vec3 pi = start_positions[i]; // get the original vertex
                position corresponding to vertex i
        }
*/
void Halfedge_Mesh::bevel_face_positions(const std::vector<Vec3> &start_positions,
                                         Halfedge_Mesh::FaceRef face, float tangent_offset,
                                         float normal_offset) {

    if (flip_orientation)
        normal_offset = -normal_offset;
    std::vector<HalfedgeRef> new_halfedges;
    auto h = face->halfedge();
    do {
        new_halfedges.push_back(h);
        h = h->next();
    } while (h != face->halfedge());

    (void)new_halfedges;
    (void)start_positions;
    (void)face;
    (void)tangent_offset;
    (void)normal_offset;


    size_t N = new_halfedges.size();
        
    //cross product get normal vector

	Vec3 pa1 = start_positions[0];
	Vec3 pb1 = start_positions[1];
	Vec3 pc1 = start_positions[2];

    Vec3 norm_left = pa1-pb1;
    Vec3 norm_right = pc1-pb1;
		
	Vec3 faceNormal = -cross(norm_left, norm_right);

	faceNormal.normalize();
	faceNormal = faceNormal*normal_offset;
	    
    std::cout << new_halfedges.size() << std::endl;
	for( size_t i = 0; i < N; i++)
	{
		size_t a = (i + N - 1) % N;
		size_t b = i;
		size_t c = (i+1) % N;
		
		Vec3 pa = start_positions[a];
		Vec3 pb = start_positions[b];
		Vec3 pc = start_positions[c];
		
		Vec3 newvect = (pa - pb) + (pc - pb);
		newvect.normalize();
        std::cout <<i<<"th unit tangent vector is "<< newvect <<std::endl;
		newvect = newvect*tangent_offset;
		std::cout <<i<<"th tangent vector is "<< newvect <<std::endl;
            
		new_halfedges[i]->vertex()->pos += newvect;
        if(!isnan(faceNormal[0])){
            new_halfedges[i]->vertex()->pos -= faceNormal;
        }
	
	}
    
    
}

/*
        Splits all non-triangular faces into triangles.
*/
void Halfedge_Mesh::triangulate() {
    
    // For each face...
    for(auto& f : faces)
    {
      //if it's not a triangle
      if (f.degree()!=3)
      {
          std::cout<<"This is not a triangle, lets add edge to it"<<std::endl; 
          // create a new halfedge and vertice list
          // hes = [he1, he2, he3, ...]
          // vtxs = [vtx1, vtx2, vtx3, ...]
          std::vector<HalfedgeRef> hes;
          std::vector<VertexRef> vtxs;

          Halfedge_Mesh::HalfedgeRef ori_h=f.halfedge();
          Halfedge_Mesh::HalfedgeRef he=f.halfedge();
          
          int i=0;
          do{
              i++;
              hes.push_back(he);
              vtxs.push_back(he->vertex());
              he=he->next();
              std::cout<<"add " << i << "th vertex/halfedge to the list"<<std::endl;
          }while(he!=ori_h);
          
          // if its a square , we need to add 4-3=1 edge
          // if its a pentagon, we need to add 5-3=2 edge
          Size n_vtx = f.degree();
          std::cout<<"It has " << n_vtx << " edges, we need to add " 
              << n_vtx-3 << " more" << std::endl;

          for(int idx=0; idx<n_vtx-3; idx++)
          {
              std::cout << "Start drawing the " << idx <<"th new edge" << std::endl;
              Halfedge_Mesh::HalfedgeRef h = new_halfedge();
              Halfedge_Mesh::HalfedgeRef h_t = new_halfedge();

              Halfedge_Mesh::FaceRef f_n = new_face();
              f_n->halfedge()=h;
              ori_h->face()->halfedge()=h_t;

              Halfedge_Mesh::EdgeRef e_n = new_edge();

              Size left_he_idx = n_vtx-2-idx;
              std::cout << "The left half edge idx is " << left_he_idx << std::endl;
              Size right_he_idx = n_vtx-3-idx;
              std::cout << "The right half edge idx is " << right_he_idx << std::endl;

              h->twin() = h_t;
              h->vertex() = vtxs.front();
              h->next() = hes[left_he_idx];
              hes[left_he_idx]->next()->next() = h;
              

              h_t->twin() = h;
              h_t->vertex() = vtxs[left_he_idx];
              h_t->face() = ori_h->face();
              h_t->next() = hes.front();
              hes[right_he_idx]->next() = h_t;

              //edge
              e_n->halfedge()=h;
              h->edge()=e_n;
              h_t->edge()=e_n;

              //face 
              hes[left_he_idx]->face() = f_n;
              hes[left_he_idx]->next()->face() = f_n;
              h->face() = f_n;

              f_n->halfedge() = h;
              ori_h->face()->halfedge()=h_t;
          }
     
      }

    }
}

/* Note on the quad subdivision process:

        Unlike the local mesh operations (like bevel or edge flip), we will perform
        subdivision by splitting *all* faces into quads "simultaneously."  Rather
        than operating directly on the halfedge data structure (which as you've
        seen is quite difficult to maintain!) we are going to do something a bit nicer:
           1. Create a raw list of vertex positions and faces (rather than a full-
              blown halfedge mesh).
           2. Build a new halfedge mesh from these lists, replacing the old one.
        Sometimes rebuilding a data structure from scratch is simpler (and even
        more efficient) than incrementally modifying the existing one.  These steps are
        detailed below.

  Step I: Compute the vertex positions for the subdivided mesh.
        Here we're going to do something a little bit strange: since we will
        have one vertex in the subdivided mesh for each vertex, edge, and face in
        the original mesh, we can nicely store the new vertex *positions* as
        attributes on vertices, edges, and faces of the original mesh. These positions
        can then be conveniently copied into the new, subdivided mesh.
        This is what you will implement in linear_subdivide_positions() and
        catmullclark_subdivide_positions().

  Steps II-IV are provided (see Halfedge_Mesh::subdivide()), but are still detailed
  here:

  Step II: Assign a unique index (starting at 0) to each vertex, edge, and
        face in the original mesh. These indices will be the indices of the
        vertices in the new (subdivided mesh).  They do not have to be assigned
        in any particular order, so long as no index is shared by more than one
        mesh element, and the total number of indices is equal to V+E+F, i.e.,
        the total number of vertices plus edges plus faces in the original mesh.
        Basically we just need a one-to-one mapping between original mesh elements
        and subdivided mesh vertices.

  Step III: Build a list of quads in the new (subdivided) mesh, as tuples of
        the element indices defined above. In other words, each new quad should be
        of the form (i,j,k,l), where i,j,k and l are four of the indices stored on
        our original mesh elements.  Note that it is essential to get the orientation
        right here: (i,j,k,l) is not the same as (l,k,j,i).  Indices of new faces
        should circulate in the same direction as old faces (think about the right-hand
        rule).

  Step IV: Pass the list of vertices and quads to a routine that clears
        the internal data for this halfedge mesh, and builds new halfedge data from
        scratch, using the two lists.
*/

/*
        Compute new vertex positions for a mesh that splits each polygon
        into quads (by inserting a vertex at the face midpoint and each
        of the edge midpoints).  The new vertex positions will be stored
        in the members Vertex::new_pos, Edge::new_pos, and
        Face::new_pos.  The values of the positions are based on
        simple linear interpolation, e.g., the edge midpoints and face
        centroids.
*/
void Halfedge_Mesh::linear_subdivide_positions() {

    // For each vertex, assign Vertex::new_pos to
    // its original position, Vertex::pos.
    for (auto v = vertices.begin(); v!=vertices.end(); ++v){
        v->new_pos = v->pos;
    }

    // For each edge, assign the midpoint of the two original
    // positions to Edge::new_pos.
    for (auto e = edges.begin(); e!=edges.end(); ++e){
        e->new_pos = e->center();
    }
    // For each face, assign the centroid (i.e., arithmetic mean)
    // of the original vertex positions to Face::new_pos. Note
    // that in general, NOT all faces will be triangles!
    for (auto f=faces.begin(); f!=faces.end(); ++f){
        f->new_pos = f->center();
    }
}

/*
        Compute new vertex positions for a mesh that splits each polygon
        into quads (by inserting a vertex at the face midpoint and each
        of the edge midpoints).  The new vertex positions will be stored
        in the members Vertex::new_pos, Edge::new_pos, and
        Face::new_pos.  The values of the positions are based on
        the Catmull-Clark rules for subdivision.

        Note: this will only be called on meshes without boundary
*/
void Halfedge_Mesh::catmullclark_subdivide_positions() {

    // The implementation for this routine should be
    // a lot like Halfedge_Mesh:linear_subdivide_positions:(),
    // except that the calculation of the positions themsevles is
    // slightly more involved, using the Catmull-Clark subdivision
    // rules. (These rules are outlined in the Developer Manual.)

    // Faces
    for (auto f=faces.begin(); f!=faces.end(); ++f){
        f->new_pos = f->center();
    }
    // Edges
    for (auto e = edges.begin(); e!=edges.end(); ++e){
        e->new_pos = e->center();
        Vec3 v1 = e->center();
        Vec3 f1 =(e->halfedge()->face()->center()+e->halfedge()->twin()->face()->center())/2;
        e->new_pos = (v1+f1)/2;
    }
    // Vertices
    for (auto v = vertices.begin(); v!=vertices.end(); ++v){
        Vec3 Q;
        Vec3 R;
        Vec3 S = v->pos;
        
        Halfedge_Mesh::HalfedgeRef h = v->halfedge();

        do{
            Q = Q + h->face()->new_pos;
            R = R + h->edge()->new_pos;
            h = h->twin()->next();
        }while(h!=v->halfedge());

        float n = 1.0f*(v->degree());

        Q /= n;
        R /= n;

        v->new_pos = (Q+2*R+(n-3)*S)/n;
    }
}

/*
        This routine should increase the number of triangles in the mesh
        using Loop subdivision. Note: this is will only be called on triangle meshes.
*/
void Halfedge_Mesh::loop_subdivide() {

    // Compute new positions for all the vertices in the input mesh, using
    // the Loop subdivision rule, and store them in Vertex::new_pos.
    // -> At this point, we also want to mark each vertex as being a vertex of the
    //    original mesh. Use Vertex::is_new for this.
    // -> Next, compute the updated vertex positions associated with edges, and
    //    store it in Edge::new_pos.
    // -> Next, we're going to split every edge in the mesh, in any order.  For
    //    future reference, we're also going to store some information about which
    //    subdivided edges come from splitting an edge in the original mesh, and
    //    which edges are new, by setting the flat Edge::is_new. Note that in this
    //    loop, we only want to iterate over edges of the original mesh.
    //    Otherwise, we'll end up splitting edges that we just split (and the
    //    loop will never end!)
    // -> Now flip any new edge that connects an old and new vertex.
    // -> Finally, copy the new vertex positions into final Vertex::pos.

    // Each vertex and edge of the original surface can be associated with a
    // vertex in the new (subdivided) surface.
    // Therefore, our strategy for computing the subdivided vertex locations is to
    // *first* compute the new positions
    // using the connectivity of the original (coarse) mesh; navigating this mesh
    // will be much easier than navigating
    // the new subdivided (fine) mesh, which has more elements to traverse.  We
    // will then assign vertex positions in
    // the new mesh based on the values we computed for the original mesh.

    // Compute updated positions for all the vertices in the original mesh, using
    // the Loop subdivision rule.

    // Next, compute the updated vertex positions associated with edges.

    // Next, we're going to split every edge in the mesh, in any order. For
    // future reference, we're also going to store some information about which
    // subdivided edges come from splitting an edge in the original mesh, and
    // which edges are new.
    // In this loop, we only want to iterate over edges of the original
    // mesh---otherwise, we'll end up splitting edges that we just split (and
    // the loop will never end!)

    // Finally, flip any new edge that connects an old and new vertex.

    // Copy the updated vertex positions to the subdivided mesh.
}

/*
        Isotropic remeshing. Note that this function returns success in a similar
        manner to the local operations, except with only a boolean value.
        (e.g. you may want to return false if this is not a triangle mesh)
*/
bool Halfedge_Mesh::isotropic_remesh() {

    // Compute the mean edge length.
    // Repeat the four main steps for 5 or 6 iterations
    // -> Split edges much longer than the target length (being careful about
    //    how the loop is written!)
    // -> Collapse edges much shorter than the target length.  Here we need to
    //    be EXTRA careful about advancing the loop, because many edges may have
    //    been destroyed by a collapse (which ones?)
    // -> Now flip each edge if it improves vertex degree
    // -> Finally, apply some tangential smoothing to the vertex positions

    return false;
}

/* Helper type for quadric simplification */
struct Edge_Record {

    Edge_Record() {}
    Edge_Record(std::unordered_map<Halfedge_Mesh::VertexRef, Mat4> &vertex_quadrics,
                Halfedge_Mesh::EdgeRef e)
        : edge(e) {

        // Compute the combined quadric from the edge endpoints.
        // -> Build the 3x3 linear system whose solution minimizes the quadric error
        Mat4 K = Mat4::Zero;
        K = K + vertex_quadrics[e->halfedge()->vertex()];
        K = K + vertex_quadrics[e->halfedge()->twin()->vertex()];

        Mat4 A = Mat4::I;
        A [0][0] = K[0][0];
        A [0][1] = K[0][1];
        A [0][2] = K[0][2];
        A [1][0] = K[1][0];
        A [1][1] = K[1][1];
        A [1][2] = K[1][2];
        A [2][0] = K[2][0];
        A [2][1] = K[2][1];
        A [2][2] = K[2][2];
        
        Vec3 b(K[0][3], K[1][3], K[2][3]);

        optimal = A.inverse() * b;  

        Vec4 homoX(optimal[0],optimal[1],optimal[2],1.0f);
        cost = dot(K*homoX,homoX);

        //    associated with these two endpoints.
        // -> Use this system to solve for the optimal position, and store it in
        //    Edge_Record::optimal.
        // -> Also store the cost associated with collapsing this edge in
        //    Edge_Record::cost.
    }
    
    Halfedge_Mesh::EdgeRef edge;
    Vec3 optimal;
    float cost;
};

bool operator<(const Edge_Record &r1, const Edge_Record &r2) {
    if (r1.cost != r2.cost) {
        return r1.cost < r2.cost;
    }
    Halfedge_Mesh::EdgeRef e1 = r1.edge;
    Halfedge_Mesh::EdgeRef e2 = r2.edge;
    return &*e1 < &*e2;
}

/** Helper type for quadric simplification
 *
 * A PQueue is a minimum-priority queue that
 * allows elements to be both inserted and removed from the
 * queue.  Together, one can easily change the priority of
 * an item by removing it, and re-inserting the same item
 * but with a different priority.  A priority queue, for
 * those who don't remember or haven't seen it before, is a
 * data structure that always keeps track of the item with
 * the smallest priority or "score," even as new elements
 * are inserted and removed.  Priority queues are often an
 * essential component of greedy algorithms, where one wants
 * to iteratively operate on the current "best" element.
 *
 * PQueue is templated on the type T of the object
 * being queued.  For this reason, T must define a comparison
 * operator of the form
 *
 *    bool operator<( const T& t1, const T& t2 )
 *
 * which returns true if and only if t1 is considered to have a
 * lower priority than t2.
 *
 * Basic use of a PQueue might look
 * something like this:
 *
 *    // initialize an empty queue
 *    PQueue<myItemType> queue;
 *
 *    // add some items (which we assume have been created
 *    // elsewhere, each of which has its priority stored as
 *    // some kind of internal member variable)
 *    queue.insert( item1 );
 *    queue.insert( item2 );
 *    queue.insert( item3 );
 *
 *    // get the highest priority item currently in the queue
 *    myItemType highestPriorityItem = queue.top();
 *
 *    // remove the highest priority item, automatically
 *    // promoting the next-highest priority item to the top
 *    queue.pop();
 *
 *    myItemType nextHighestPriorityItem = queue.top();
 *
 *    // Etc.
 *
 *    // We can also remove an item, making sure it is no
 *    // longer in the queue (note that this item may already
 *    // have been removed, if it was the 1st or 2nd-highest
 *    // priority item!)
 *    queue.remove( item2 );
 *
 */
template <class T> struct PQueue {
    void insert(const T &item) { queue.insert(item); }
    void remove(const T &item) {
        if (queue.find(item) != queue.end()) {
            queue.erase(item);
        }
    }
    //bool operator<( const T& t1, const T& t2 ){
    //    return t1.cost < t2.cost;
    //}
    const T &top(void) const { return *(queue.begin()); }
    void pop(void) { queue.erase(queue.begin()); }
    size_t size() { return queue.size(); }

    std::set<T> queue;
};

/*
        Mesh simplification. Note that this function returns success in a similar
        manner to the local operations, except with only a boolean value.
        (e.g. you may want to return false if you can't simplify the mesh any
        further without destroying it.)
*/
bool Halfedge_Mesh::simplify() {

    std::unordered_map<VertexRef, Mat4> vertex_quadrics;
    std::unordered_map<FaceRef, Mat4> face_quadrics;
    std::unordered_map<EdgeRef, Edge_Record> edge_records;
    PQueue<Edge_Record> edge_queue;

    // Compute initial quadrics for each face by simply writing the plane equation
    // for the face in homogeneous coordinates. These quadrics should be stored
    // in face_quadrics 
    for (auto f = faces.begin(); f!=faces.end();f++)
    {
        // is the d the position of the surface or vertices?
        //float d = 1.0f * dot(f->normal(), f->halfedge()->vertex()->pos);
        float d = 1.0f * dot(f->normal(), f->center());
        Vec4 v(f->normal(), d);
        Mat4 m_face = Mat4::Zero;
        m_face = outer(v,v);
        face_quadrics.insert({f,m_face});
    }

    // -> Compute an initial quadric for each vertex as the sum of the quadrics
    //    associated with the incident faces, storing it in vertex_quadrics
    for (auto vtx = vertices.begin(); vtx!=vertices.end(); vtx++){
        Halfedge_Mesh::HalfedgeRef h = vtx->halfedge();
        Mat4 m_quadric = Mat4::Zero;

        do{
            m_quadric = m_quadric + face_quadrics[h->face()];
            h = h->twin()->next();
        }while(h!=vtx->halfedge());

        vertex_quadrics.insert({vtx,m_quadric});
    }
    
    // -> Build a priority queue of edges according to their quadric error cost,
    //    i.e., by building an Edge_Record for each edge and sticking it in the
    //    queue. You may want to use the above PQueue<Edge_Record> for this.
    for (auto e = edges.begin(); e!=edges.end(); e++){
        Edge_Record record(vertex_quadrics,e->halfedge()->edge());
        edge_records.insert({e->halfedge()->edge(),record});
        edge_queue.insert(record);
    }

    // -> Until we reach the target edge budget, collapse the best edge. Remember
    //    to remove from the queue any edge that touches the collapsing edge
    //    BEFORE it gets collapsed, and add back into the queue any edge touching
    //    the collapsed vertex AFTER it's been collapsed. Also remember to assign
    //    a quadric to the collapsed vertex, and to pop the collapsed edge off the
    //    top of the queue.
    auto targetnum_triangle = faces.size();
    targetnum_triangle/=4;

    for (int i=0; i<=targetnum_triangle; i++){

        //std::cout<<"total edge"<< faces.size() <<std::endl;
        std::cout<<" "<<std::endl;
        std::cout<<"Start"<<std::endl;
        std::cout<<""<< "vertex_quadrics length " <<vertex_quadrics.size()<<std::endl;
        std::cout<<""<< "face_quadrics length " <<face_quadrics.size()<<std::endl;
        std::cout<<""<< "edge_records length " <<edge_records.size()<<std::endl;
        std::cout<<""<< "edge_queue length " <<edge_queue.size()<<std::endl;

        // 获得cost最小的点
        Edge_Record be = edge_queue.top();

        // 从顶点-矩阵哈希表中删除两个和edge相关的顶点
        vertex_quadrics.erase(vertex_quadrics.find(be.edge->halfedge()->vertex()));
        vertex_quadrics.erase(vertex_quadrics.find(be.edge->halfedge()->twin()->vertex()));
        
        // 从 边-矩阵 哈希表中把边删掉
        edge_records.erase(edge_records.find(be.edge));

        // 把这个点的struct 从edge queue里面删掉
        edge_queue.pop();


        HalfedgeRef h1 = be.edge->halfedge()->next();
		HalfedgeRef h2 = be.edge->halfedge()->twin()->next();

        int count = 0;
        do
		{
            count ++;
            //std::cout<<"remove edge left " << h1->edge()->id() << " from Edge_Record "<< std::endl;
            // 从 分数表 中把边删掉
			edge_queue.remove(edge_records[h1->edge()]);
            // 从 面-矩阵 哈希表中把所有和两个顶点相接的面都删掉
			face_quadrics.erase(face_quadrics.find(h1->face()));
            // 从 边-矩阵 哈希表中把所有相关的边都删掉
            edge_records.erase(edge_records.find(h1->edge()));

			h1 = h1->twin()->next();
		}while(h1 != be.edge->halfedge()->twin());
		//std::cout<<"remove " << count << "edges connected to the left vertex"<< std::endl;
  
        count = 0;

		do
		{
            count ++;
            // 从 分数表 中把边删掉
			edge_queue.remove(edge_records[h2->edge()]);
            // 从 面-矩阵 哈希表中把所有和两个顶点相接的面都删掉
            face_quadrics.erase(face_quadrics.find(h2->face()));	
            // 从 边-矩阵 哈希表中把所有相关的边都删掉
            edge_records.erase(edge_records.find(h2->edge()));

			h2 = h2->twin()->next();
		}while(h2 != be.edge->halfedge());
        //std::cout<<"remove " << count << "edges connected to the right vertex"<< std::endl;

        std::optional<VertexRef> cv = collapse_edge(be.edge);
        VertexRef newV = cv.value();
        newV->pos = be.optimal;

        // 重新计算每个面的quadric
        HalfedgeRef new_he = newV->halfedge();
        count = 0;
        Mat4 newV_quadric = Mat4::Zero;
        do
		{
            count ++;
            FaceRef  f = new_he->face();
			float d = 1.0f * dot(f->normal(), f->center());
            Vec4 v(f->normal(), d);
            Mat4 m_face = Mat4::Zero;
            m_face = outer(v,v);
            
            // 把周围一圈面 加入 面-矩阵 哈希表中
            face_quadrics.insert({f,m_face});

            // 把这个面累加到这个点的quadric中
            newV_quadric = newV_quadric + face_quadrics[f];

            new_he = new_he ->twin()->next();
		}while(new_he != newV->halfedge());

        //增加一个新的点到 顶点-矩阵 哈希表中
        vertex_quadrics.insert({newV,newV_quadric});
        //std::cout<<"add " << count << "edges connected to the new vertex"<< std::endl;

        count = 0;
        new_he = newV->halfedge();
        //更新其他和删除边缘相接的点的 顶点-矩阵 哈希表
        do
		{
            count ++;
            // 对每一顶点，创建一个空矩阵
            Mat4 current_V_qua = Mat4::Zero;
            // 获取顶点
            VertexRef twinV = new_he->twin()->vertex();
            // 获取顶点的halfedge
            HalfedgeRef twinVhe = twinV->halfedge();
            //遍历与这些点相接的所有面
            do{
                current_V_qua = current_V_qua + face_quadrics[twinVhe->face()];
                twinVhe = twinVhe->twin()->next();
            }while(twinVhe != twinV->halfedge());
            //跟新这个顶点的 顶点-矩阵哈希表
            vertex_quadrics[twinV] = current_V_qua;

            new_he = new_he->twin()->next();
		}while(new_he != newV->halfedge());

        std::cout<<"update "<< count <<"vertexs' quadric"<<std::endl;
        
        count = 0;
        new_he = newV->halfedge();
        do{
            count ++;
            Edge_Record record(vertex_quadrics,new_he->edge());
            // 把新的边加入到 边-矩阵 哈希表中
            edge_records.insert({new_he->edge(), record});
            // 把新的边加入Queue中
            edge_queue.insert(record);
	    	new_he = new_he->twin()->next();
		}while(new_he != newV->halfedge());
        //std::cout<<"add " << count << "edges to edge record"<< std::endl;

        
        std::cout<<""<< "vertex_quadrics length " <<vertex_quadrics.size()<<std::endl;
        std::cout<<""<< "face_quadrics length " <<face_quadrics.size()<<std::endl;
        std::cout<<""<< "edge_records length " <<edge_records.size()<<std::endl;
        std::cout<<""<< "edge_queue length " <<edge_queue.size()<<std::endl;
        std::cout<<"End"<<std::endl;
    }

    

    return true;
}
