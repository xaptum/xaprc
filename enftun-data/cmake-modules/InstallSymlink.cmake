macro(install_symlink filepath sympath)
  install(CODE "execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${filepath} \$ENV{DESTDIR}/${sympath})")
endmacro(install_symlink)
