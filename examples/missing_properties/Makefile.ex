#
#	configuration variables for the example

## Main application file
MAIN = missing_properties
DEPH = $(EXSNAPADV)/lod/typedefs.h $(EXSNAPADV)/lod/rdf_parser.h $(EXSNAPADV)/lod/lod_utils.h $(EXSNAPADV)/lod/object_utils.h $(EXSNAPADV)/lod/cluster_utils.h $(EXSNAPADV)/lod/property_utils.h $(EXSNAPADV)/lod/similarity_utils.h
DEPCPP = 
CXXFLAGS += $(CXXOPENMP)
