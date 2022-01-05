file(REMOVE_RECURSE
  "../bin/hello"
  "../bin/hello.pdb"
  "CMakeFiles/hello.dir/main.c.o"
)

# Per-language clean rules from dependency scanning.
foreach(lang C)
  include(CMakeFiles/hello.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
