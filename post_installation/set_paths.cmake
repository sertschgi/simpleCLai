# InstallPath.cmake
function(ADD_PATH_TO_USER_BASHRC target_path_install)
    file(APPEND "$ENV{HOME}/.bashrc" "export PATH=${target_path_install}:\$PATH\n")
    if (EXISTS "$ENV{HOME}/.bash_profile")
        file(APPEND "$ENV{HOME}/.bash_profile" "export PATH=${target_path_install}:\$PATH\n")
    endif()
endfunction()