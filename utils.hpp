#include <iostream>
#include <cstdio>
#include <string>
#include <memory>
#include "mvIMPACT_CPP/mvIMPACT_acquire.h"
#include "mvIMPACT_CPP/mvIMPACT_acquire_GenICam.h"

#include  "opencv2/core/core.hpp"
#include "opencv2/imgcodecs.hpp"


auto getPixelFormat(mvIMPACT::acquire::Request* pRequest) -> int;
auto print_statistics(mvIMPACT::acquire::Device* pDev, mvIMPACT::acquire::Statistics stats, unsigned int count) -> void;
auto capture(mvIMPACT::acquire::Device* pDev, bool isSingleShot, std::string imageDir, int numImages) -> void;
