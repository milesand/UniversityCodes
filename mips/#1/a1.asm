# Since I do not understand what you mean by v0 reads in Celsius and stuff,
# This program just reads in a 32-bit float from console, consider it a celsius and
# turns it into a fahrenheit value, then prints it.
# Or is it that you wanted an imprecise one that uses integer instructions?

.data
A: .float 1.8   # 9 / 5
B: .float 32.0

.text
.globl main
.ent main
main:
    li $v0, 6
    syscall          # Read a float
    mov.s $f12, $f0  # Float that was read is in $f0. Move it to $f12 so we can pass it to c2f
    jal c2f          # Call c2f
                     # Yup, inline it, get rid of those mov.s stuff, all viable.
                     # But I wanted to do a function call thing here. So here it is.
    mov.s $f12, $f0  # Result from c2f is in $f0. Move it to $f12 so we can print it
    li $v0, 2
    syscall          # Print that float
    li $v0, 10
    syscall          # Terminate
.end main
    
c2f:
    l.s $f0, A            # A = 1.8
    l.s $f1, B            # B = 32.0
    mul.s $f0, $f0, $f12  # result = A * input
    add.s $f0, $f0, $f1   # result = result + B
    jr $ra                # return
.end c2f
