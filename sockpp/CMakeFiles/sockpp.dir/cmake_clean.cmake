file(REMOVE_RECURSE
  "libsockpp.pdb"
  "libsockpp.so"
  "libsockpp.so.0"
  "libsockpp.so.0.7.0"
)

# Per-language clean rules from dependency scanning.
foreach(lang CXX)
  include(CMakeFiles/sockpp.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
