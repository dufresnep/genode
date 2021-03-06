#
# \brief  Build config for parts of core that depend on Trustzone status
# \author Stefan Kalkowski
# \author Martin Stein
# \date   2012-10-24
#

# add include paths
INC_DIR += $(REP_DIR)/src/core/imx53

# add C++ sources
SRC_CC += pic.cc

# include less specific library parts
include $(REP_DIR)/lib/mk/core-trustzone.inc
