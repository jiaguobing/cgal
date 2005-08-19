// Copyright (c) 2005  INRIA (France).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org); you may redistribute it under
// the terms of the Q Public License version 1.0.
// See the file LICENSE.QPL distributed with CGAL.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $Source$
// $Revision$
// $Name$
//
// Author(s)     : Laurent Saboret, Pierre Alliez


#ifndef MESHADAPTORPOLYHEDRONEX_H
#define MESHADAPTORPOLYHEDRONEX_H

#include <CGAL/iterator.h>
#include <CGAL/circulator.h>
#include <CGAL/Convertible_iterator_project.h>
#include <CGAL/Convertible_circulator_project.h>

#include "Polyhedron_ex.h"

#include <list>
#include <cassert>


// Class Mesh_adaptor_polyhedron_ex
// Model of MeshAdaptor_3 concept, whose purpose is to allow
// the parameterization package to access meshes on an uniform manner.
//
// Mesh_adaptor_polyhedron_ex is an adaptor class to access to a Polyhedron_ex
// 3D mesh using MeshAdaptor_3 interface.
//
// The input mesh can be of any genus.
// It can have have any number of boundaries. Its "main border"
// will be the mesh's longest boundary (if there is at least one boundary).
//
// Design pattern:
// Mesh_adaptor_polyhedron_ex is an Adaptor (see [GOF95]): it changes the
// Polyhedron_ex interface to match the MeshAdaptor_3 concept

class Mesh_adaptor_polyhedron_ex
{
// Forward references

private:
    struct                                  Project_halfedge_vertex;
    struct                                  Project_vertex_handle_vertex;
    struct                                  Project_opposite_halfedge_vertex;

// Private types
private:

    // Halfedge
    typedef Polyhedron_ex::Halfedge         Halfedge;
    typedef Polyhedron_ex::Halfedge_handle  Halfedge_handle;
    typedef Polyhedron_ex::Halfedge_const_handle
                                            Halfedge_const_handle;
    typedef Polyhedron_ex::Halfedge_iterator Halfedge_iterator;
    typedef Polyhedron_ex::Halfedge_const_iterator
                                            Halfedge_const_iterator;
    typedef Polyhedron_ex::Halfedge_around_vertex_circulator
                                            Halfedge_around_vertex_circulator;
    typedef Polyhedron_ex::Halfedge_around_vertex_const_circulator
                                            Halfedge_around_vertex_const_circulator;
    typedef Polyhedron_ex::Halfedge_around_facet_circulator
                                            Halfedge_around_facet_circulator;
    typedef Polyhedron_ex::Halfedge_around_facet_const_circulator
                                            Halfedge_around_facet_const_circulator;


// Public types
public:

    //******************************************************************
    // LEVEL 1 INTERFACE:
    // for "normal" classes that do not deal with virtual seams
    // Example: all parameterization methods
    //******************************************************************

    // Number type
    typedef Polyhedron_ex::Traits::FT       NT;
    // Points and vectors
    typedef Polyhedron_ex::Traits::Point_2  Point_2;
    typedef Polyhedron_ex::Traits::Point_3  Point_3;
    typedef Polyhedron_ex::Traits::Vector_2 Vector_2;
    typedef Polyhedron_ex::Traits::Vector_3 Vector_3;

    // Facet
    typedef Polyhedron_ex::Facet            Facet;
    typedef Polyhedron_ex::Facet_handle     Facet_handle;
    typedef Polyhedron_ex::Facet_const_handle Facet_const_handle;
    // Iterator over all mesh facets
    typedef Polyhedron_ex::Facet_iterator   Facet_iterator;
    typedef Polyhedron_ex::Facet_const_iterator
                                            Facet_const_iterator;

    // Vertex
    typedef Polyhedron_ex::Vertex           Vertex;
    typedef Polyhedron_ex::Vertex_handle    Vertex_handle;
    typedef Polyhedron_ex::Vertex_const_handle Vertex_const_handle;
    // Iterator over all mesh vertices
    typedef Polyhedron_ex::Vertex_iterator  Vertex_iterator;
    typedef Polyhedron_ex::Vertex_const_iterator
                                            Vertex_const_iterator;
    // Iterator over mesh boundary vertices
    typedef CGAL::Convertible_iterator_project<std::list<Vertex_handle>::iterator,
                                               Project_vertex_handle_vertex,
                                               Vertex_const_handle,
                                               Vertex_handle>
                                            Border_vertex_iterator;
    typedef CGAL::Convertible_iterator_project<std::list<Vertex_handle>::const_iterator,
                                               Project_vertex_handle_vertex,
                                               Vertex_const_handle>
                                            Border_vertex_const_iterator;
    // Counter-clockwise circulator over a facet's vertices
    typedef CGAL::Convertible_circulator_project<Halfedge_around_facet_circulator,
                                                 Project_halfedge_vertex,
                                                 Vertex&,
                                                 Vertex*,
                                                 Vertex_const_handle,
                                                 Vertex_handle>
                                            Vertex_around_facet_circulator;
    typedef CGAL::Convertible_circulator_project<Halfedge_around_facet_const_circulator,
                                                 Project_halfedge_vertex,
                                                 const Vertex&,
                                                 const Vertex*,
                                                 Vertex_const_handle>
                                            Vertex_around_facet_const_circulator;
    // Clockwise circulator over the vertices incident to a vertex
    typedef CGAL::Convertible_circulator_project<Halfedge_around_vertex_circulator,
                                                 Project_opposite_halfedge_vertex,
                                                 Vertex&,
                                                 Vertex*,
                                                 Vertex_const_handle,
                                                 Vertex_handle>
                                            Vertex_around_vertex_circulator;
    typedef CGAL::Convertible_circulator_project<Halfedge_around_vertex_const_circulator,
                                                 Project_opposite_halfedge_vertex,
                                                 const Vertex&,
                                                 const Vertex*,
                                                 Vertex_const_handle>
                                            Vertex_around_vertex_const_circulator;


// Public operations
public:

    //******************************************************************
    // INTERFACE SPECIFIC TO Polyhedron_ex
    //******************************************************************

    // Create an adaptator for an existing Polyhedron_ex mesh.
    // The input mesh can be of any genus.
    // It can have have any number of boundaries. Its "main border"
    // will be the mesh's longest boundary (if there is at least one boundary).
    Mesh_adaptor_polyhedron_ex(Polyhedron_ex* mesh)
    {
        assert(mesh != NULL);

        // Store adapted mesh pointer
        m_polyhedron = mesh;

        // Extract mesh's longest boundary
        m_main_border = extract_longest_boundary(mesh);

#ifndef NDEBUG
        // Index vertices right away to ease debugging
        index_mesh_vertices();
#endif
    }

    // Default destructor, copy constructor and operator =() are fine

    // Get the adapted mesh
    Polyhedron_ex*       get_adapted_mesh()       { return m_polyhedron; }
    const Polyhedron_ex* get_adapted_mesh() const { return m_polyhedron; }

    // Get halfedge from source and target vertices
    // Will assert if such an halfedge doesn't exist
    Halfedge_const_handle get_halfedge(
        Vertex_const_handle source, Vertex_const_handle target) const
    {
        assert(source != NULL);
        assert(target != NULL);

        Halfedge_around_vertex_const_circulator cir     = target->vertex_begin(),
                                                cir_end = cir;
        CGAL_For_all(cir, cir_end)
            if (cir->opposite()->vertex() == source)
                return cir;

#ifdef DEBUG_TRACE
        fprintf(stderr, "      get_halfedge(%d->%d): error\n", source->index(), target->index());
#endif
        assert(false);              // error if we reach this point
        return NULL;
    }
    Halfedge_handle get_halfedge(Vertex_handle source, Vertex_handle target)
    {
        Halfedge_const_handle halfedge = get_halfedge((Vertex_const_handle)source,
                                                      (Vertex_const_handle)target);
        return const_cast<Halfedge*>(&*halfedge);
    }

    //******************************************************************
    // LEVEL 1 INTERFACE:
    // for "normal" classes that do not deal with virtual seams
    // Example: all parameterization methods
    //******************************************************************

    // MESH INTERFACE

    // Get iterator over first vertex of mesh
    Vertex_iterator  mesh_vertices_begin() {
        return m_polyhedron->vertices_begin();
    }
    Vertex_const_iterator  mesh_vertices_begin() const {
        return m_polyhedron->vertices_begin();
    }

    // Get iterator over past-the-end vertex of mesh
    Vertex_iterator  mesh_vertices_end() {
        return m_polyhedron->vertices_end();
    }
    Vertex_const_iterator  mesh_vertices_end() const {
        return m_polyhedron->vertices_end();
    }

    // Count the number of vertices of the mesh
    int  count_mesh_vertices() const {
        int index = 0;
        for (Vertex_const_iterator it=mesh_vertices_begin(); it!=mesh_vertices_end(); it++)
            index++;
        return index;
    }

    // Index vertices of the mesh from 0 to count_mesh_vertices()-1
    void  index_mesh_vertices ()
    {
        fprintf(stderr,"  index Mesh_adaptor_polyhedron vertices:\n");
        int index = 0;
        for (Vertex_iterator it=mesh_vertices_begin(); it!=mesh_vertices_end(); it++)
        {
            Point_3 position = get_vertex_position(it);
#ifdef DEBUG_TRACE
            fprintf(stderr, "    %d=(%f,%f,%f)\n",
                            index,
                            (float)position.x(),
                            (float)position.y(),
                            (float)position.z());
#endif
            set_vertex_index(it, index++);
        }
        fprintf(stderr,"    ok\n");
    }

    // Get iterator over first vertex of mesh's main border
    Border_vertex_iterator  mesh_main_border_vertices_begin() {
        return Border_vertex_iterator(m_main_border.begin());
    }
    Border_vertex_const_iterator  mesh_main_border_vertices_begin() const {
        return Border_vertex_const_iterator(m_main_border.begin());
    }

    // Get iterator over past-the-end vertex of mesh's main border
    Border_vertex_iterator  mesh_main_border_vertices_end() {
        return Border_vertex_iterator(m_main_border.end());
    }
    Border_vertex_const_iterator  mesh_main_border_vertices_end() const {
        return Border_vertex_const_iterator(m_main_border.end());
    }

    // Return the boundary containing seed_vertex
    // Return an empty list if not found
    std::list<Vertex_handle> get_boundary(Vertex_handle seed_vertex)
    {
        std::list<Vertex_handle> boundary;  // returned list

        Halfedge_around_vertex_circulator pHalfedge = seed_vertex->vertex_begin();
        Halfedge_around_vertex_circulator end       = pHalfedge;

        // if isolated vertex
        if (pHalfedge == NULL) {
            boundary.push_back(seed_vertex);
            return boundary;
        }

        // Get seed_vertex' border halfedge
        Halfedge_handle  seed_halfedge = NULL;
        CGAL_For_all(pHalfedge,end) {
            if(pHalfedge->is_border()) {
                seed_halfedge = pHalfedge;
                break;
            }
        }

        // if inner vertex
        if (seed_halfedge == NULL)
            return boundary;                // return empty list

        std::cerr << "  find one closed boundary...";

        // Add seed vertex
        boundary.push_back(seed_vertex);

        // fill boundary
        int size = 1;
        Halfedge_handle current_halfedge = seed_halfedge;
        do
        {
            // Stop if end of loop
            Halfedge_handle next_halfedge = current_halfedge->next();
            Vertex_handle next_vertex = next_halfedge->vertex();
            if(next_vertex == seed_vertex)
                break;

            // Add vertex
            boundary.push_back(next_vertex);

            current_halfedge = next_halfedge;
            size++;
        }
        while(1);

        std::cerr << size << " vertices" << std::endl;

        return boundary;
    }

    //// Compute the genus of the mesh
    //int  get_mesh_genus() const {
    //    return m_polyhedron->genus();
    //}

    //// Count the number of boundaries of the mesh
    //int  count_mesh_boundaries() const {
    //    return m_polyhedron->nb_boundaries();
    //}

    // Get iterator over first facet of mesh
    Facet_iterator  mesh_facets_begin() {
        return m_polyhedron->facets_begin();
    }
    Facet_const_iterator  mesh_facets_begin() const {
        return m_polyhedron->facets_begin();
    }

    // Get iterator over past-the-end facet of mesh
    Facet_iterator  mesh_facets_end() {
        return m_polyhedron->facets_end();
    }
    Facet_const_iterator  mesh_facets_end() const {
        return m_polyhedron->facets_end();
    }

    // Count the number of facets of the mesh
    int  count_mesh_facets() const {
        int index = 0;
        for (Facet_const_iterator it=mesh_facets_begin(); it!=mesh_facets_end(); it++)
            index++;
        return index;
    }

    // Return true of all mesh's facets are triangles
    bool  is_mesh_triangular() const {
        for (Facet_const_iterator it = mesh_facets_begin(); it != mesh_facets_end(); it++)
            if (count_facet_vertices(it) != 3)
                return false;
        return true;            // mesh is triangular if we reach this point
    }

    // Count the number of halfedges of the mesh
    int  count_mesh_halfedges() const {
        int index = 0;
        for (Halfedge_iterator pHalfedge = m_polyhedron->halfedges_begin();
             pHalfedge != m_polyhedron->halfedges_end();
             pHalfedge++)
        {
            index++;
        }
        return index;
    }

    // FACET INTERFACE

    // Get circulator over facet's vertices
    Vertex_around_facet_circulator  facet_vertices_begin(Facet_handle facet) {
        return Vertex_around_facet_circulator(facet->facet_begin());
    }
    Vertex_around_facet_const_circulator  facet_vertices_begin(Facet_const_handle facet) const {
        return Vertex_around_facet_const_circulator(facet->facet_begin());
    }

    // Count the number of vertices of a facet
    int  count_facet_vertices(Facet_const_handle facet) const {
        int index = 0;
        Vertex_around_facet_const_circulator cir     = facet_vertices_begin(facet),
                                             cir_end = cir;
        CGAL_For_all(cir, cir_end)
            index++;
        return index;
    }

    // VERTEX INTERFACE

    // Get the 3D position of a vertex
    Point_3 get_vertex_position(Vertex_const_handle vertex) const {
        return vertex->point();
    }

    // Get/set the 2D position (u/v pair) of a vertex. Default value is undefined.
    // (stored in halfedges sharing the same vertex)
    Point_2  get_vertex_uv(Vertex_const_handle vertex) const {
        return get_corners_uv(vertex, NULL, NULL);
    }
    void  set_vertex_uv(Vertex_handle vertex, const Point_2& uv) {
        set_corners_uv(vertex, NULL, NULL, uv);
    }

    // Get/set "is parameterized" field of vertex. Default value is undefined.
    // (stored in halfedges sharing the same vertex)
    bool  is_vertex_parameterized(Vertex_const_handle vertex) const {
        return are_corners_parameterized(vertex, NULL, NULL);
    }
    void  set_vertex_parameterized(Vertex_handle vertex, bool parameterized) {
        set_corners_parameterized(vertex, NULL, NULL, parameterized);
    }

    // Get/set vertex index. Default value is undefined.
    // (stored in Polyhedron vertex for debugging purpose)
    int  get_vertex_index(Vertex_const_handle vertex) const {
        //return get_corners_index(vertex, NULL, NULL);
        return vertex->index();
    }
    void  set_vertex_index(Vertex_handle vertex, int index)
    {
        //set_corners_index(vertex, NULL, NULL, index);
        vertex->index(index);
    }

    // Get/set vertex' all purpose tag. Default value is undefined.
    // (stored in halfedges sharing the same vertex)
    int  get_vertex_tag(Vertex_const_handle vertex) const {
        return get_corners_tag(vertex, NULL, NULL);
    }
    void set_vertex_tag(Vertex_handle vertex, int tag) {
        set_corners_tag(vertex, NULL, NULL, tag);
    }

    // Return true if a vertex belongs to ANY mesh's boundary
    bool  is_vertex_on_border(Vertex_const_handle vertex) const {
        return m_polyhedron->is_border(vertex);
    }

    // Return true if a vertex belongs to the UNIQUE mesh's main boundary,
    // ie the mesh's LONGEST boundary
    bool  is_vertex_on_main_border(Vertex_const_handle vertex) const {
        return std::find(m_main_border.begin(), 
                         m_main_border.end(), 
                         (Vertex*)&*vertex) != m_main_border.end();
    }

    // Get circulator over the vertices incident to 'vertex'
    // 'start_position' defines the optional initial position of the circulator
    Vertex_around_vertex_circulator vertices_around_vertex_begin(
                            Vertex_handle vertex,
                            Vertex_handle start_position = NULL)
    {
        if (start_position == NULL)
            return Vertex_around_vertex_circulator(vertex->vertex_begin());
        else
            return Vertex_around_vertex_circulator(
            		Halfedge_around_vertex_circulator(
                        	get_halfedge(start_position, vertex)));
    }
    Vertex_around_vertex_const_circulator vertices_around_vertex_begin(
                            Vertex_const_handle vertex,
                            Vertex_const_handle start_position = NULL) const
    {
        if (start_position == NULL)
            return Vertex_around_vertex_const_circulator(vertex->vertex_begin());
        else
            return Vertex_around_vertex_const_circulator(
            		Halfedge_around_vertex_const_circulator(
                        	get_halfedge(start_position, vertex)));
    }

    //******************************************************************
    // LEVEL 2 INTERFACE:
    // for classes that deal with virtual seams
    // Example: Mesh_adaptor_patch_3
    //******************************************************************

    // VERTEX INTERFACE

    // Get/set vertex seaming flag. Default value is undefined.
    int  get_vertex_seaming(Vertex_const_handle vertex) const {
        return vertex->seaming();
    }
    void set_vertex_seaming(Vertex_handle vertex, int seaming) {
        vertex->seaming(seaming);
    }

    // EDGE INTERFACE

    // Get/set oriented edge's seaming flag, ie position of the oriented edge
    // wrt to the UNIQUE main boundary
    int  get_halfedge_seaming(Vertex_const_handle source, Vertex_const_handle target) const {
        return get_halfedge(source, target)->seaming();
    }
    void set_halfedge_seaming(Vertex_handle source, Vertex_handle target, int seaming) {
        get_halfedge(source, target)->seaming(seaming);
    }

    // CORNER INTERFACE

    // Get/set the 2D position (= (u,v) pair) of corners at the "right"
    // of the prev_vertex -> vertex -> next_vertex line.
    // Default value is undefined.
    // (stored in incident halfedges)
    Point_2 get_corners_uv(Vertex_const_handle vertex,
                           Vertex_const_handle prev_vertex,
                           Vertex_const_handle next_vertex) const
    {
        // if inner vertex
        if (prev_vertex == NULL && next_vertex == NULL)
        {
            // get (u,v) pair from any incident halfedge
            return Point_2(vertex->halfedge()->u(), vertex->halfedge()->v());
        }
        else // if seam vertex
        {
            assert(prev_vertex != NULL);
            assert(next_vertex != NULL);

            // get (u,v) pair from first inner halfedge (clockwise)
            Halfedge_around_vertex_const_circulator cir(
                                get_halfedge(next_vertex, vertex) );
            return Point_2(cir->u(), cir->v());
        }
    }
    void set_corners_uv(Vertex_handle vertex,
                        Vertex_const_handle prev_vertex,
                        Vertex_const_handle next_vertex,
                        const Point_2& uv)
    {
        // if inner vertex
        if (prev_vertex == NULL && next_vertex == NULL)
        {
            // Loop over all incident halfedges
            Halfedge_around_vertex_circulator cir     = vertex->vertex_begin(),
                                              cir_end = cir;
            CGAL_For_all(cir, cir_end)
                cir->uv(uv.x(), uv.y());
        }
        else // if seam vertex
        {
            assert(prev_vertex != NULL);
            assert(next_vertex != NULL);

            // first inner halfedge (for a clockwise rotation)
            Halfedge_around_vertex_circulator cir(
                                get_halfedge((Vertex*)&*next_vertex, vertex) );

            // past-the-end inner halfedge (for a clockwise rotation)
            Halfedge_around_vertex_circulator cir_end(
                                get_halfedge((Vertex*)&*prev_vertex, vertex) );

            // Loop over incident halfedges at the "right"
            // of the prev_vertex -> vertex -> next_vertex line
            CGAL_For_all(cir, cir_end)
                cir->uv(uv.x(), uv.y());
        }
    }

    // Get/set "is parameterized" field of corners at the "right"
    // of the prev_vertex -> vertex -> next_vertex line.
    // Default value is undefined.
    // (stored in incident halfedges)
    bool are_corners_parameterized(Vertex_const_handle vertex,
                                   Vertex_const_handle prev_vertex,
                                   Vertex_const_handle next_vertex) const
    {
        // if inner vertex
        if (prev_vertex == NULL && next_vertex == NULL)
        {
            // get "is parameterized" field from any incident halfedge
            return vertex->halfedge()->is_parameterized();
        }
        else // if seam vertex
        {
            assert(prev_vertex != NULL);
            assert(next_vertex != NULL);

            // get "is parameterized" field from first inner halfedge (clockwise)
            Halfedge_around_vertex_const_circulator cir(
                                get_halfedge(next_vertex, vertex) );
            return cir->is_parameterized();
        }
    }
    void set_corners_parameterized(Vertex_handle vertex,
                                   Vertex_const_handle prev_vertex,
                                   Vertex_const_handle next_vertex,
                                   bool parameterized)
    {
        // if inner vertex
        if (prev_vertex == NULL && next_vertex == NULL)
        {
            // Loop over all incident halfedges
            Halfedge_around_vertex_circulator cir     = vertex->vertex_begin(),
                                              cir_end = cir;
            CGAL_For_all(cir, cir_end)
                cir->is_parameterized(parameterized);
        }
        else // if seam vertex
        {
            assert(prev_vertex != NULL);
            assert(next_vertex != NULL);

            // first inner halfedge (for a clockwise rotation)
            Halfedge_around_vertex_circulator cir(
                                get_halfedge((Vertex*)&*next_vertex, vertex) );

            // past-the-end inner halfedge (for a clockwise rotation)
            Halfedge_around_vertex_circulator cir_end(
                                get_halfedge((Vertex*)&*prev_vertex, vertex) );

            // Loop over incident halfedges at the "right"
            // of the prev_vertex -> vertex -> next_vertex line
            CGAL_For_all(cir, cir_end)
                cir->is_parameterized(parameterized);
        }
    }

    // Get/set index of corners at the "right"
    // of the prev_vertex -> vertex -> next_vertex line.
    // Default value is undefined.
    // (stored in incident halfedges)
    int get_corners_index(Vertex_const_handle vertex,
                          Vertex_const_handle prev_vertex,
                          Vertex_const_handle next_vertex) const
    {
        // if inner vertex
        if (prev_vertex == NULL && next_vertex == NULL)
        {
            // get index from any incident halfedge
            return vertex->halfedge()->index();
        }
        else // if seam vertex
        {
            assert(prev_vertex != NULL);
            assert(next_vertex != NULL);

            // get index from first inner halfedge (clockwise)
            Halfedge_around_vertex_const_circulator cir(
                                get_halfedge(next_vertex, vertex) );
            return cir->index();
        }
    }
    void set_corners_index(Vertex_handle vertex,
                           Vertex_const_handle prev_vertex,
                           Vertex_const_handle next_vertex,
                           int index)
    {
        // if inner vertex
        if (prev_vertex == NULL && next_vertex == NULL)
        {
            // Loop over all incident halfedges
            Halfedge_around_vertex_circulator cir     = vertex->vertex_begin(),
                                              cir_end = cir;
            CGAL_For_all(cir, cir_end)
                cir->index(index);
        }
        else // if seam vertex
        {
            assert(prev_vertex != NULL);
            assert(next_vertex != NULL);

            // first inner halfedge (for a clockwise rotation)
            Halfedge_around_vertex_circulator cir(
                                get_halfedge((Vertex*)&*next_vertex, vertex) );

            // past-the-end inner halfedge (for a clockwise rotation)
            Halfedge_around_vertex_circulator cir_end(
                                get_halfedge((Vertex*)&*prev_vertex, vertex) );

            // Loop over incident halfedges at the "right"
            // of the prev_vertex -> vertex -> next_vertex line
            CGAL_For_all(cir, cir_end)
                cir->index(index);
        }
    }

    // Get/set all purpose tag of corners at the "right"
    // of the prev_vertex -> vertex -> next_vertex line.
    // Default value is undefined.
    // (stored in incident halfedges)
    int get_corners_tag(Vertex_const_handle vertex,
                        Vertex_const_handle prev_vertex,
                        Vertex_const_handle next_vertex) const
    {
        // if inner vertex
        if (prev_vertex == NULL && next_vertex == NULL)
        {
            // get tag from any incident halfedge
            return vertex->halfedge()->tag();
        }
        else // if seam vertex
        {
            assert(prev_vertex != NULL);
            assert(next_vertex != NULL);

            // get tag from first inner halfedge (clockwise)
            Halfedge_around_vertex_const_circulator cir(
                                get_halfedge(next_vertex, vertex) );
            return cir->tag();
        }
    }
    void set_corners_tag(Vertex_handle vertex,
                         Vertex_const_handle prev_vertex,
                         Vertex_const_handle next_vertex,
                         int tag)
    {
        // if inner vertex
        if (prev_vertex == NULL && next_vertex == NULL)
        {
            // Loop over all incident halfedges
            Halfedge_around_vertex_circulator cir     = vertex->vertex_begin(),
                                              cir_end = cir;
            CGAL_For_all(cir, cir_end)
                cir->tag(tag);
        }
        else // if seam vertex
        {
            assert(prev_vertex != NULL);
            assert(next_vertex != NULL);

            // first inner halfedge (for a clockwise rotation)
            Halfedge_around_vertex_circulator cir(
                                get_halfedge((Vertex*)&*next_vertex, vertex) );

            // past-the-end inner halfedge (for a clockwise rotation)
            Halfedge_around_vertex_circulator cir_end(
                                get_halfedge((Vertex*)&*prev_vertex, vertex) );

            // Loop over incident halfedges at the "right"
            // of the prev_vertex -> vertex -> next_vertex line
            CGAL_For_all(cir, cir_end)
                cir->tag(tag);
        }
    }


// Private operations
private:

    // Extract mesh's longest boundary
    std::list<Vertex_handle> extract_longest_boundary(Polyhedron_ex* mesh)
    {
        std::list<Vertex_handle> longest_boundary;  // returned list
        double                   max_len = 0;       // length of longest_boundary

        assert(mesh != NULL);

        // Tag all vertices as unprocessed
        const int tag_free = 0;
        const int tag_done = 1;
        for (Vertex_iterator it=mesh_vertices_begin(); it!=mesh_vertices_end(); it++)
             set_vertex_tag(it, tag_free);

        // find all closed boundaries and keep longest one
        int nb = 0;
        while (1)
        {
            // Find a boundary tagged as "free" and tag it as "processed"
            std::list<Vertex_handle> boundary = find_free_boundary(tag_free, tag_done);
            if(boundary.empty())
                break;

            // compute  total len of 'boundary'
            double len = 0.0;
            std::list<Vertex_handle>::const_iterator it;
            for(it = boundary.begin(); it != boundary.end(); it++)
            {
                // Get next iterator (looping)
                std::list<Vertex_handle>::const_iterator next = it;
                next++;
                if (next == boundary.end())
                    next = boundary.begin();

                Vector_3 vect = get_vertex_position(*next) - get_vertex_position(*it);
                len += std::sqrt(vect*vect);
            }

            // Keep 'boundary' if longer
            if (len > max_len)
            {
                longest_boundary = boundary;
                max_len = len;
            }

            nb++;
        }

        std::cerr << "  " << nb << " boundary(ies) found" << std::endl;

        return longest_boundary;
    }

    // Find a boundary tagged as "free" and tag it as "processed"
    // Return an empty list if not found
    std::list<Vertex_handle> find_free_boundary(int tag_free, int tag_done)
    {
        std::list<Vertex_handle> boundary;  // returned list

        // get any border vertex with "free" tag
        Vertex_handle seed_vertex = NULL;
        for (Vertex_iterator pVertex = mesh_vertices_begin();
             pVertex != mesh_vertices_end();
             pVertex++)
        {
            if (is_vertex_on_border(pVertex) && get_vertex_tag(pVertex) == tag_free) {
                seed_vertex = pVertex;
                break;
            }
        }
        if (seed_vertex == NULL)
            return boundary;                // return empty list

        // Get the boundary containing seed_vertex
        boundary = get_boundary(seed_vertex);

        // Tag boundary vertices as "processed"
        std::list<Vertex_handle>::iterator it;
        for(it = boundary.begin(); it != boundary.end(); it++)
            set_vertex_tag(*it, tag_done);

        return boundary;
    }


// Fields
private:

    // The adapted mesh (cannot be NULL)
    Polyhedron_ex*              m_polyhedron;

    // Main boundary of a topological disc inside m_polyhedron (may be empty)
    std::list<Vertex_handle>    m_main_border;


// Private types
private:

    // Utility class to generate the Vertex_around_facet_circulator type
    struct Project_halfedge_vertex {
        typedef Halfedge                            argument_type;
        typedef Mesh_adaptor_polyhedron_ex::Vertex  Vertex;
        typedef Vertex                              result_type;
        typedef CGAL::Arity_tag<1>                  Arity;

        // Get the target vertex of a halfedge
        Vertex&       operator()(Halfedge& h)       const {
            return *(h.vertex());
        }
        const Vertex& operator()(const Halfedge& h) const {
            return *(h.vertex());
        }
    };

    // Utility class to generate the Border_vertex_iterator type
    struct Project_vertex_handle_vertex {
        typedef Vertex_handle                       argument_type;
        typedef Mesh_adaptor_polyhedron_ex::Vertex  Vertex;
        typedef Vertex                              result_type;
        typedef CGAL::Arity_tag<1>                  Arity;

        // Convert Vertex_handle to Vertex
        Vertex&       operator()(Vertex_handle& vh)       const { return *vh; }
        const Vertex& operator()(const Vertex_handle& vh) const { return *vh; }
    };

    // This class is used to generate the Vertex_around_vertex_circulator type
    struct Project_opposite_halfedge_vertex {
        typedef Halfedge                            argument_type;
        typedef Mesh_adaptor_polyhedron_ex::Vertex  Vertex;
        typedef Vertex                              result_type;
        typedef CGAL::Arity_tag<1>                  Arity;

        // Get the source vertex of a halfedge
        Vertex&       operator()(Halfedge& h)       const {
            return *(h.opposite()->vertex());
        }
        const Vertex& operator()(const Halfedge& h) const {
            return *(h.opposite()->vertex());
        }
    };

}; // Mesh_adaptor_polyhedron_ex


#endif //MESHADAPTORPOLYHEDRONEX_H

