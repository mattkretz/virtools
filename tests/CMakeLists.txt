set(all "ALL")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/virtest")
if(DEFINED ENV{APPVEYOR})
   include_directories("C:/Libraries/boost")
   set(all)
endif()

function(vir_add_test name)
   set(target ${name})
   add_executable(${target} ${name}.cpp)
   if(MSVC)
      #set_target_properties(${target} PROPERTIES COMPILE_FLAGS "-std=c++11 -Wall -Wextra")
   else()
      set_target_properties(${target} PROPERTIES COMPILE_FLAGS "-std=c++1y -Wall -Wextra")
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
