# Connect to remote QEMU instance
target remote :1234

# Display source window at the top and make command window active for scrolling
layout src
focus cmd

# Break at useful functions
break kmain
break _kernel_on_assert
break _kernel_on_panic
