set(all "ALL")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/virtest")
if(DEFINED ENV{APPVEYOR})
   include_directories("C:/Libraries/boost")
   set(all)
endif()

include(CheckCXXCompilerFlag)
macro(try_stdflag flag)
   string(REGEX REPLACE "[-.+/:= ]" "_" flag_esc "${flag}")
   check_cxx_compiler_flag("-std=${flag}" supports_${flag_esc})
   if(supports_${flag_esc})
      set(STDFLAG "-std=${flag}")
   elseif(${ARGC} GREATER 1)
      try_stdflag(${ARGN})
   endif()
endmacro()

try_stdflag("c++17" "c++1z" "c++14" "c++1y")

function(vir_add_test name)
   set(target ${name})
   add_executable(${target} ${name}.cpp)
   if(MSVC)
      #set_target_properties(${target} PROPERTIES COMPILE_FLAGS "-std=c++11 -Wall -Wextra")
   else()
      set_target_properties(${target} PROPERTIES COMPILE_FLAGS "${STDFLAG} -Wall -Wextra")
      set_target_properties(${target} PROPERTIES LINK_FLAGS "-pthread")
   endif()
   add_test(NAME ${target} COMMAND ${target})

   if("${CMAKE_GENERATOR}" MATCHES "Visual Studio")
      # do nothing. This just clutters the solution explorer
   else()
      add_custom_target(run_${target} ${all}
         ${target} -v
         DEPENDS ${target}
         COMMENT "Execute ${target} test"
         VERBATIM
         )
   endif()
endfunction()

vir_add_test(extendlifetime)
vir_add_test(constexpr_if)
vir_add_test(struct_reflect)
vir_add_test(ratelimit)
