# Connect to remote QEMU instance
target remote :1234

# Display source window at the top and make command window active for scrolling
layout src
focus cmd

# Break at useful functions
break _kmain
break kernel::_assert_failure(char const*, char const*, lib::usize, char const*)
break kernel::_panic(char const*, char const*, lib::usize, char const*)
