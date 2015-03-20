// opengl and context creation headers
#include <GL/glew.h>
#include <GLFW/glfw3.h>
// ogl headers c++ wrapper on opengl
#include <oglplus/all.hpp>
#include <oglplus/opt/smart_enums.hpp>
#include <oglplus/bound/texture.hpp>
#include <oglplus/buffer_usage.hpp>
// standard and stl library headers
#include <iostream>
#include <stdexcept>
#include <memory>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
// glm math library headers
#include <glm/glm.hpp>
#include <glm/common.hpp>
#include <glm/gtx/matrix_operation.hpp>
#include <glm/gtc/type_ptr.hpp>
// boost c++ general utils headers
#include <boost/version.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/math/constants/constants.hpp>
// parallel patterns library (ppl) from Windows for concurrency and parallelism
#include <ppl.h>
// namespace assignations
namespace logging = boost::log;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;
namespace expr = boost::log::expressions;