if(NOT WIN32)
  string(ASCII 27 Esc)
  set(ColourReset "${Esc}[m")
  set(ColourBold  "${Esc}[1m")
  set(Red         "${Esc}[31m")
  set(Green       "${Esc}[32m")
  set(Yellow      "${Esc}[33m")
  set(Blue        "${Esc}[34m")
  set(Magenta     "${Esc}[35m")
  set(Cyan        "${Esc}[36m")
  set(White       "${Esc}[37m")
  set(BoldRed     "${Esc}[1;31m")
  set(BoldGreen   "${Esc}[1;32m")
  set(BoldYellow  "${Esc}[1;33m")
  set(BoldBlue    "${Esc}[1;34m")
  set(BoldMagenta "${Esc}[1;35m")
  set(BoldCyan    "${Esc}[1;36m")
  set(BoldWhite   "${Esc}[1;37m")
endif()

message("
======================================================${Magenta}
                 __        __
                /\\ \\__  __/\\ \\__
       __    ___\\ \\  _\\/\\_\\ \\  _\\  __  __
     / __ \\/  _  \\ \\ \\/\\/\\ \\ \\ \\/ /\\ \\/\\ \\
    /\\  __//\\ \\/\\ \\ \\ \\_\\ \\ \\ \\ \\_\\ \\ \\_\\ \\  __
    \\ \\____\\ \\_\\ \\_\\ \\__\\\\ \\_\\ \\__\\\\ \\____ \\/\\_\\
     \\/____/\\/_/\\/_/\\/__/ \\/_/\\/__/ \\/___/  \\/_/
                                       /\\___/
                                       \\/__/${ColourReset}
======================================================
`entity` has been configured with the following options:
")

# message("This is normal")
# message("${Red}This is Red${ColourReset}")
# message("${Green}This is Green${ColourReset}")
# message("${Yellow}This is Yellow${ColourReset}")
# message("${Blue}This is Blue${ColourReset}")
# message("${Magenta}This is Magenta${ColourReset}")
# message("${Cyan}This is Cyan${ColourReset}")
# message("${White}This is White${ColourReset}")
# message("${BoldRed}This is BoldRed${ColourReset}")
# message("${BoldGreen}This is BoldGreen${ColourReset}")
# message("${BoldYellow}This is BoldYellow${ColourReset}")
# message("${BoldBlue}This is BoldBlue${ColourReset}")
# message("${BoldMagenta}This is BoldMagenta${ColourReset}")
# message("${BoldCyan}This is BoldCyan${ColourReset}")
# message("${BoldWhite}This is BoldWhite\n\n${ColourReset}")

# message()