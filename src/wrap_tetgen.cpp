#include "tetgen.h"
#include <boost/python.hpp>
#include <boost/lexical_cast.hpp>
#include <vector>
#include <stdexcept>
#include <iostream>
#include "foreign_array_wrap.hpp"




using namespace boost::python;
using namespace std;




struct tMeshInfo : public tetgenio, public boost::noncopyable
{
  public:
    tForeignArray<REAL>		        Points; // in/out
    tForeignArray<REAL>		        PointAttributes; // in/out
    tForeignArray<REAL>		        AdditionalPoints; // out
    tForeignArray<REAL>		        AdditionalPointAttributes; // out
    tForeignArray<int>		        PointMarkers; // in/out

    tForeignArray<int>		        Elements; // in/out
    tForeignArray<REAL>		        ElementAttributes; // in/out
    tForeignArray<REAL>		        ElementVolumes; // out
    tForeignArray<int>		        Neighbors; // out

    tForeignArray<tetgenio::facet>	Facets;
    tForeignArray<int>                  FacetMarkers;

    tForeignArray<REAL>                 Holes;
    tForeignArray<REAL>                 Regions;

    tForeignArray<REAL>                 FacetConstraints;
    tForeignArray<REAL>                 SegmentConstraints;
    tForeignArray<REAL>                 NodeConstraints;

    tForeignArray<pbcgroup>             PBCGroups;

    tForeignArray<int>                  Faces;
    tForeignArray<int>                  AdjacentElements;
    tForeignArray<int>                  FaceMarkers;

    tForeignArray<int>                  Edges;
    tForeignArray<int>                  EdgeMarkers;
  public:
    tMeshInfo()
      : Points(pointlist, numberofpoints, 3),
        PointAttributes(pointattributelist, numberofpoints, 0, &Points),
        AdditionalPoints(addpointlist, numberofaddpoints, 3),
        AdditionalPointAttributes(addpointattributelist, 
            numberofaddpoints, 0, &AdditionalPoints),
	PointMarkers(pointmarkerlist, numberofpoints, 1, &Points),

	Elements(tetrahedronlist, numberoftetrahedra, 4),
	ElementAttributes(tetrahedronattributelist, 
            numberoftetrahedra, 0, &Elements),
	ElementVolumes(tetrahedronvolumelist, numberoftetrahedra, 1, &Elements),
	Neighbors(neighborlist, numberoftetrahedra, 4, &Elements),

        Facets(facetlist, numberoffacets),
        FacetMarkers(facetmarkerlist, numberoffacets, 1, &Facets),

	Holes(holelist, numberofholes, 3),

	Regions(regionlist, numberofregions, 5),

        FacetConstraints(facetconstraintlist, numberoffacetconstraints, 2),
        SegmentConstraints(facetconstraintlist, numberofsegmentconstraints, 3),
        NodeConstraints(nodeconstraintlist, numberofnodeconstraints, 2),

        PBCGroups(pbcgrouplist, numberofpbcgroups),

        Faces(trifacelist, numberoftrifaces, 3),
        AdjacentElements(adjtetlist, numberoftrifaces, 2, &Faces),
        FaceMarkers(trifacemarkerlist, numberoftrifaces, 2, &Faces),

        Edges(edgelist, numberofedges, 2),
        EdgeMarkers(edgemarkerlist, numberofedges, 1, &Edges)
    {
    } 
    
    unsigned numberOfPointAttributes() const
    {
      return numberofpointattributes;
    }

    unsigned numberOfElementAttributes() const
    {
      return numberoftetrahedronattributes;
    }

    void setNumberOfPointAttributes(unsigned attrs)
    {
      PointAttributes.setUnit(attrs);
      numberofpointattributes = attrs;
    }

    void setNumberOfElementAttributes(unsigned attrs)
    {
      ElementAttributes.setUnit(attrs);
      numberoftetrahedronattributes = attrs;
    }

    /*
    tTriangulationParameters &operator=(const tTriangulationParameters &src)
    {
      numberofpointattributes = src.numberofpointattributes ;
      numberofcorners = src.numberofcorners;
      numberoftriangleattributes = src.numberoftriangleattributes;

      Points = src.Points;
      PointAttributes = src.PointAttributes;
      PointMarkers = src.PointMarkers;

      Triangles = src.Triangles;
      TriangleAttributes = src.TriangleAttributes;
      TriangleAreas = src.TriangleAreas;
      Neighbors = src.Neighbors;

      Segments = src.Segments;
      SegmentMarkers = src.SegmentMarkers;

      Holes = src.Holes;

      Regions = src.Regions;

      Edges = src.Edges;
      EdgeMarkers = src.EdgeMarkers;
      Normals = src.Normals;

      return *this;
    }
    */
};




/*
tTriangulationParameters *copyTriangulationParameters(const tTriangulationParameters &src)
{
  auto_ptr<tTriangulationParameters> copy(new tTriangulationParameters);
  *copy = src;
  return copy.release();
}
*/




void tetrahedralizeWrapper(tetgenbehavior &bhv, tMeshInfo &in, tMeshInfo &out)
{
  try
  {
    tetrahedralize(&bhv, &in, &out);
  }
  catch (int &i)
  {
    throw runtime_error("TetGen runtime error code "+boost::lexical_cast<string>(i));
  }
}




template <std::size_t owner_arg = 1, class Base = default_call_policies>
struct manage_new_internal_reference
    : with_custodian_and_ward_postcall<0, owner_arg, Base>
{
   typedef manage_new_object result_converter;
};




tForeignArray<tetgenio::polygon> *facet_get_polygons(tetgenio::facet &self)
{
  return new tForeignArray<tetgenio::polygon>(
      self.polygonlist, self.numberofpolygons);
}




tForeignArray<REAL> *facet_get_holes(tetgenio::facet &self)
{
  return new tForeignArray<REAL>(self.holelist, self.numberofholes);
}





tForeignArray<int> *polygon_get_vertices(tetgenio::polygon &self)
{
  return new tForeignArray<int>(self.vertexlist, self.numberofvertices);
}





REAL pbcgroup_get_transmat_entry(tetgenio::pbcgroup &self, long i, long j)
{
  if (i < 0) i += 4;
  if (j < 0) j += 4;

  if (i < 0 || i >= 4 || j < 0 || j >= 4)
    PYTHON_ERROR(IndexError, "transform matrix index out of bounds");
  return self.transmat[i][j];
}





void pbcgroup_set_transmat_entry(tetgenio::pbcgroup &self, long i, long j, REAL value)
{
  if (i < 0) i += 4;
  if (j < 0) j += 4;

  if (i < 0 || i >= 4 || j < 0 || j >= 4)
    PYTHON_ERROR(IndexError, "transform matrix index out of bounds");
  self.transmat[i][j] = value;
}





tForeignArray<int> *pbcgroup_get_pointpairs(tetgenio::pbcgroup &self)
{
  return new tForeignArray<int>(self.pointpairlist, self.numberofpointpairs);
}





#define DEF_RW_MEMBER(NAME) \
    def_readwrite(#NAME, &cl::NAME)
#define DEF_METHOD(NAME) \
    def(#NAME, &cl::NAME)

BOOST_PYTHON_MODULE(_tetgen)
{
  def("tetrahedralize", tetrahedralizeWrapper);

  {
    typedef tMeshInfo cl;
    class_<cl, boost::noncopyable>
      ("MeshInfo", init<>())
      .def_readonly("points", &cl::Points)
      .def_readonly("point_attributes", &cl::PointAttributes)
      .def_readonly("additional_points", &cl::AdditionalPoints)
      .def_readonly("additional_point_attributes", &cl::AdditionalPointAttributes)
      .def_readonly("point_markers", &cl::PointMarkers)

      .def_readonly("elements", &cl::Elements)
      .def_readonly("element_attributes", &cl::ElementAttributes)
      .def_readonly("element_volumes", &cl::ElementVolumes)
      .def_readonly("neighbors", &cl::Neighbors)


      .def_readonly("facets", &cl::Facets)
      .def_readonly("facet_markers", &cl::FacetMarkers)

      .def_readonly("holes", &cl::Holes)

      .def_readonly("regions", &cl::Regions)

      .def_readonly("facet_constraints", &cl::FacetConstraints)
      .def_readonly("segment_constraints", &cl::SegmentConstraints)
      .def_readonly("node_constraints", &cl::NodeConstraints)

      .def_readonly("pbc_groups", &cl::PBCGroups)

      .def_readonly("faces", &cl::Faces)
      .def_readonly("adjacent_elements", &cl::AdjacentElements)
      .def_readonly("face_markers", &cl::FaceMarkers)

      .def_readonly("edges", &cl::Edges)
      .def_readonly("edge_markers", &cl::EdgeMarkers)

      .add_property("number_of_point_attributes", 
          &cl::numberOfPointAttributes,
          &cl::setNumberOfPointAttributes)
      .add_property("number_of_element_attributes", 
          &cl::numberOfElementAttributes,
          &cl::setNumberOfElementAttributes)

      .DEF_METHOD(save_nodes)
      .DEF_METHOD(save_elements)
      .DEF_METHOD(save_faces)
      .DEF_METHOD(save_edges)
      .DEF_METHOD(save_neighbors)
      .DEF_METHOD(save_poly)

      .DEF_METHOD(load_node)
      .DEF_METHOD(load_addnodes)
      .DEF_METHOD(load_pbc)
      .DEF_METHOD(load_var)
      .DEF_METHOD(load_mtr)
      .DEF_METHOD(load_poly)
      .DEF_METHOD(load_off)
      .DEF_METHOD(load_ply)
      .DEF_METHOD(load_stl)
      .DEF_METHOD(load_medit)
      .DEF_METHOD(load_plc)
      .DEF_METHOD(load_tetmesh)

      /*
         .def("copy", &copyTriangulationParameters,
         return_value_policy<manage_new_object>())
         */
      //.enable_pickling()
      ;
  }

  {
    typedef tetgenio::facet cl;
    class_<cl, boost::noncopyable>("Facet", no_init)
      .def("get_polygons", facet_get_polygons, manage_new_internal_reference<>())
      .def("get_holes", facet_get_holes, manage_new_internal_reference<>())
      ;
  }

  {
    typedef tetgenio::polygon cl;
    class_<cl, boost::noncopyable>("Polygon", no_init)
      .def("get_vertices", polygon_get_vertices, manage_new_internal_reference<>())
      ;
  }

  {
    typedef tetgenio::pbcgroup cl;
    class_<cl, boost::noncopyable>("PBCGroup", no_init)
      .def_readwrite("facet_marker_1", &cl::fmark1)
      .def_readwrite("facet_marker_2", &cl::fmark2)
      .def("get_transmat_entry", pbcgroup_get_transmat_entry)
      .def("set_transmat_entry", pbcgroup_set_transmat_entry)
      .def("get_point_pairs", pbcgroup_get_pointpairs, manage_new_internal_reference<>())
      ;
  }

  {
    typedef tetgenbehavior cl;
    class_<cl, boost::noncopyable>("Options", init<>())
      .DEF_RW_MEMBER(plc)
      .DEF_RW_MEMBER(refine)
      .DEF_RW_MEMBER(quality)
      .DEF_RW_MEMBER(smooth)
      .DEF_RW_MEMBER(metric)
      .DEF_RW_MEMBER(bgmesh)
      .DEF_RW_MEMBER(varvolume)
      .DEF_RW_MEMBER(fixedvolume)
      .DEF_RW_MEMBER(insertaddpoints)
      .DEF_RW_MEMBER(regionattrib)
      .DEF_RW_MEMBER(offcenter)
      .DEF_RW_MEMBER(conformdel)
      .DEF_RW_MEMBER(diagnose)
      .DEF_RW_MEMBER(zeroindex)
      .DEF_RW_MEMBER(order)
      .DEF_RW_MEMBER(facesout)
      .DEF_RW_MEMBER(edgesout)
      .DEF_RW_MEMBER(neighout)
      .DEF_RW_MEMBER(meditview)
      .DEF_RW_MEMBER(gidview)
      .DEF_RW_MEMBER(geomview)
      .DEF_RW_MEMBER(nobound)
      .DEF_RW_MEMBER(nonodewritten)
      .DEF_RW_MEMBER(noelewritten)
      .DEF_RW_MEMBER(nofacewritten)
      .DEF_RW_MEMBER(noiterationnum)
      .DEF_RW_MEMBER(nomerge)
      .DEF_RW_MEMBER(nobisect)
      .DEF_RW_MEMBER(noflip)
      .DEF_RW_MEMBER(nojettison)
      .DEF_RW_MEMBER(steiner)
      .DEF_RW_MEMBER(fliprepair)
      .DEF_RW_MEMBER(docheck)
      .DEF_RW_MEMBER(quiet)
      .DEF_RW_MEMBER(verbose)
      .DEF_RW_MEMBER(tol)
      .DEF_RW_MEMBER(useshelles)
      .DEF_RW_MEMBER(minratio)
      .DEF_RW_MEMBER(goodratio)
      .DEF_RW_MEMBER(minangle)
      .DEF_RW_MEMBER(goodangle)
      .DEF_RW_MEMBER(maxvolume)
      .DEF_RW_MEMBER(maxdihedral)
      .DEF_RW_MEMBER(alpha1)
      .DEF_RW_MEMBER(alpha2)
      .DEF_RW_MEMBER(alpha3)
      .DEF_RW_MEMBER(epsilon)
      .DEF_RW_MEMBER(epsilon2)

      .def("parse_switches", (bool (tetgenbehavior::*)(char *)) &cl::parse_commandline)
      ;
  }

  exposePODForeignArray<REAL>("RealArray");
  exposePODForeignArray<int>("IntArray");
  exposeStructureForeignArray<tetgenio::facet>("FacetArray");
  exposeStructureForeignArray<tetgenio::polygon>("PolygonArray");
  exposeStructureForeignArray<tetgenio::pbcgroup>("PBCGroupArray");
}
