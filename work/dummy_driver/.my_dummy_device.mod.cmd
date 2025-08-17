savedcmd_my_dummy_device.mod := printf '%s\n'   my_dummy_device.o | awk '!x[$$0]++ { print("./"$$0) }' > my_dummy_device.mod
