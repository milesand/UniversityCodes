# Structures:
    # FileWrapper:
        # size: 140 bytes
        # layout:
        #   0 ~ 3:    File descriptor
        #   4 ~ 7:    Index of next byte to be read on buffer, in bytes
        #   8 ~ 11:   Amount of bytes that were read in last syscall
        #   12 ~ 139: Buffer
    # StdinWrapper:
        # size: 132 bytes
        # layout:
        #   0 ~ 3: Index of next byte to be read on buffer, in bytes
        #   4 ~ 131: Buffer
    # SegmentTree:
        # size: 20 bytes
        # layout:
        #   0 ~ 2:  Padding
        #   3:      Depth of this tree
        #   4 ~ 7:  "Least distance stored in this tree"-ish thing
        #   8 ~ 19: Node
    # Node:
        # size: 12 bytes
        # maybe leaf or non-leaf, depending on depth
        # layout when leaf:
        #   0 ~ 11: Points
        # layout when non-leaf:
        #   0 ~ 3:  Pointer to a block of 2 Nodes. *this = left child, *this = right child.
        #   4 ~ 11: Empty.
        # Note that 0 ~ 3 holds a pointer in both cases; it is null if this node isn't quite there.
    # Points:
        # Dynamic array of Point
        # size: 12 bytes
        # layout:
        #   0 ~ 3:  Pointer to memory block holding points
        #   4 ~ 7:  Capacity of the memory block, in number of points
        #   8 ~ 11: Length of this array, in number of points
    # Point:
        # size: 8 bytes
        # layout:
        #   0 ~ 3: x-coordinate, signed 32 bit int
        #   4 ~ 7: y-coordinate, signed 32 bit int

.data
    filename: .asciiz "input.txt"
    file_parse_error: .asciiz "Failed to parse "
    stdin_parse_error: .asciiz "Invalid input\n"
    none: .asciiz "None\n"
    comma_space: .asciiz ", "
    close_nl: .asciiz ")\n"

.text

.globl main
.ent main
main:
    move $fp, $sp
    addi $sp, $sp, -160  # Stack allocation

    li $v0, 13           # Open file
    la $a0, filename     # named "input.txt"
    li $a1, 0            # in read mode
    li $a2, 0            # UNIX file mode is probably irrelevant
    syscall              # $v0 = file

    # Initialize FileWrapper
    sw $v0, -140($fp)    # fw.file = file
    sw $zero, -136($fp)  # fw.len = 0
    sw $zero, -132($fp)  # fw.cap = 0
                         # leave fw.buf uninitialized
    
    # Initialize the tree
    sb $zero, -157($fp)  # s.depth = 0
    sw $zero, -156($fp)  # s.start = 0
    sw $zero, -152($fp)  # s.mark = null

    # Parse the file.
    parse_many__start:
        addi $a0, $fp, -140
        jal skip_whitespace                 # $v0 = fw.skip_whitespace()
        li $t0, -1
        beq $v0, $t0, parse_many__end       # Exit if EOF
        li $t0, 40
        bne $v0, $t0, main__parse_error     # if $v0 != '(': Error out
        addi $a0, $fp, -140
        jal skip_whitespace                 # $v0 = fw.skip_whitespace()
        li $t0, 45
        bne $t0, $v0, parse_many__L1        # if $v0 == '-':
            li $s0, 0                       #     sign = 0
            addi $a0, $fp, -140
            jal next_byte                   #     $v0 = fw.next_byte()
            j parse_many__L2
        parse_many__L1:                     # else:
            li $s0, 1                       #     sign = 1
        parse_many__L2:
        addi $s1, $v0, -48                  # a = $v0 - '0'
        sltiu $t0, $s1, 10
        beq $t0, $zero, main__parse_error   # if a < 0 || 10 <= a: Error out
        parse_many__L3:                     # loop:
            addi $a0, $fp, -140
            jal next_byte                   #     $v0 = fw.next_byte()
            addi $t0, $v0, -48              #     $t0 = $v0 - '0'
            sltiu $t1, $t0, 10
            beq $t1, $zero, parse_many__L4  #     if $t0 < 0 || 10 <= $t0: break
            li $t1, 10
            mult $s1, $t1
            mflo $t2                        #     $t2 = 10 * a
            add $s1, $t0, $t2               #     a = $t0 + $t2
            j parse_many__L3
        parse_many__L4:
        slti $t0, $v0, 9
        bne $t0, $zero, parse_many__L6      # if ($v0 >= 9
        slti $t0, $v0, 14
        bne $t0, $zero, parse_many__L5      # && $v0 < 14)
        li $t0, 32
        beq $t0, $v0, parse_many__L5        # || $v0 == 32
        li $t0, 133
        beq $t0, $v0, parse_many__L5        # || $v0 == 133
        li $t0, 160
        bne $t0, $v0, parse_many__L6        # || $v0 == 160: ($v0 is ws)
        parse_many__L5:
            addi $a0, $fp, -140
            jal skip_whitespace             #    $v0 = fw.skip_whitespace()
        parse_many__L6:
        li $t0, 44
        bne $v0, $t0, main__parse_error     # if $v0 != ',': Error out
        bne $s0, $zero, parse_many__L7      # if sign == 0:
            neg $s1, $s1                    #     a = -a;
        parse_many__L7:
        addi $a0, $fp, -140
        jal skip_whitespace                 # $v0 = fw.skip_whitespace()
        li $t0, 45
        bne $t0, $v0, parse_many__L8        # if $v0 == '-':
            li $s0, 0                       #     sign = 0
            addi $a0, $fp, -140
            jal next_byte                   #     $v0 = fw.next_byte()
            j parse_many__L9
        parse_many__L8:                     # else:
            li $s0, 1                       #     sign = 1
        parse_many__L9:
        addi $s2, $v0, -48                  # b = $v0 - '0'
        sltiu $t0, $s2, 10
        beq $t0, $zero, main__parse_error   # if b < 0 || 10 <= b: Error out
        parse_many__L10:                    # loop:
            addi $a0, $fp, -140
            jal next_byte                   #     $v0 = fw.next_byte()
            addi $t0, $v0, -48              #     $t0 = $v0 - '0'
            sltiu $t1, $t0, 10
            beq $t1, $zero, parse_many__L11 #     if $t0 < 0 || 10 <= $t0: break
            li $t1, 10
            mult $s2, $t1
            mflo $t2                        #     $t2 = 10 * b
            add $s2, $t0, $t2               #     b = $t0 + $t2
            j parse_many__L10
        parse_many__L11:
        slti $t0, $v0, 9
        bne $t0, $zero, parse_many__L13     # if ($v0 >= 9
        slti $t0, $v0, 14
        bne $t0, $zero, parse_many__L12     # && $v0 < 14)
        li $t0, 32
        beq $t0, $v0, parse_many__L12       # || $v0 == 32
        li $t0, 133
        beq $t0, $v0, parse_many__L12       # || $v0 == 133
        li $t0, 160
        bne $t0, $v0, parse_many__L13       # || $v0 == 160: ($v0 is ws)
        parse_many__L12:
            addi $a0, $fp, -140
            jal skip_whitespace             #    $v0 = fw.skip_whitespace()
        parse_many__L13:
        li $t0, 41
        bne $v0, $t0, main__parse_error     # if $v0 != ')': Error out
        bne $s0, $zero, parse_many__L14     # if sign == 0:
            neg $s2, $s2                    #     b = -b;
        parse_many__L14:
        addi $a0, $fp, -160
        move $a1, $s1
        move $a2, $s2
        jal insert                          # s.insert(a, b)
        addi $a0, $fp, -140
        jal skip_whitespace                 # $v0 = fw.skip_whitespace()
        li $t0, -1
        beq $v0, $t0, parse_many__end       # Exit if EOF
        li $t0, 44
        bne $v0, $t0, main__parse_error     # if $v0 != ',': Error out
        j parse_many__start
    parse_many__end:

    li $v0, 16         # Close the file
    lw $a0, -140($fp)
    syscall
    sw $zero, -132($fp) # -140($fp) ~ 0($fp) is now unused, so we reuse it for buffering stdin
    sb $zero, -128($fp) 

    main__L1:          # Loop
        # Parse stdin.
        parse_once_start:
            addi $a0, $fp, -132
            jal skip_whitespace_stdin                # $v0 = sw.skip_whitespace()
            li $t0, 120
            beq $t0, $v0, main__L2                   # if $c0 == 'x': break
            li $t0, 40
            bne $t0, $v0, main__parse_once_error     # if $v0 != '(': Error out
            addi $a0, $fp, -132
            jal skip_whitespace_stdin                # $v0, sw.skip_whitespace()
            li $t0, 45
            bne $v0, $t0, parse_once__L1             # If $v0 == '-':
                li $s0, 0                            #     sign = 0
                addi $a0, $fp, -132
                jal next_byte_stdin                  #     $v0 = sw.next_byte()
                j parse_once__L2
            parse_once__L1:                          # else:
                li $s0, 1                            #     sign = 1
            parse_once__L2:
            addi $s1, $v0, -48                       # a = $v0 - '0'
            sltiu $t0, $s1, 10
            beq $t0, $zero, main__parse_once_error   # if a < 0 || 10 <= a: Error out
            parse_once__L3:                          # loop:
                addi $a0, $fp, -132
                jal next_byte_stdin                  #     $v0 = sw.next_byte()
                addi $t0, $v0, -48                   #     $t0 = $v0 - '0'
                sltiu $t1, $t0, 10
                beq $t1, $zero, parse_once__L4       #     if $t0 < 0 || $t0 >= 10: break
                li $t1, 10
                mult $t1, $s1
                mflo $t1                             #     $t1 = a * 10
                add $s1, $t0, $t1                   #     a = $t0 + $t1
                j parse_once__L3
            parse_once__L4:
            slti $t0, $v0, 9
            bne $t0, $zero, parse_once__L6           # if ($v0 >= 9
            slti $t0, $v0, 14
            bne $t0, $zero, parse_once__L5           # && $v0 < 14)
            li $t0, 32
            beq $t0, $v0, parse_once__L5             # || $v0 == 32
            li $t0, 133
            beq $t0, $v0, parse_once__L5             # || $v0 == 133
            li $t0, 160
            bne $t0, $v0, parse_once__L6             # || $v0 == 160: ($v0 is ws)
            parse_once__L5:
                addi $a0, $fp, -132
                jal skip_whitespace_stdin            #    $v0 = sw.skip_whitespace()
            parse_once__L6:
            li $t0, 44
            bne $v0, $t0, main__parse_once_error     # if $v0 != ',': Error out
            bne $s0, $zero, parse_once__L7           # if sign == 0:
                neg $s1, $s1                         #     a = -a;
            parse_once__L7:
            addi $a0, $fp, -132
            jal skip_whitespace_stdin                # $v0, sw.skip_whitespace()
            li $t0, 45
            bne $v0, $t0, parse_once__L8             # If $v0 == '-':
                li $s0, 0                            #     sign = 0
                addi $a0, $fp, -132
                jal next_byte_stdin                  #     $v0 = sw.next_byte()
                j parse_once__L9
            parse_once__L8:                          # else:
                li $s0, 1                            #     sign = 1
            parse_once__L9:
            addi $s2, $v0, -48                       # b = $v0 - '0'
            sltiu $t0, $s2, 10
            beq $t0, $zero, main__parse_once_error   # if b < 0 || 10 <= b: Error out
            parse_once__L10:                         # loop:
                addi $a0, $fp, -132
                jal next_byte_stdin                  #     $v0 = sw.next_byte()
                addi $t0, $v0, -48                   #     $t0 = $v0 - '0'
                sltiu $t1, $t0, 10
                beq $t1, $zero, parse_once__L11      #     if $t0 < 0 || $t0 >= 10: break
                li $t1, 10
                mult $t1, $s2
                mflo $t1                             #     $t1 = b * 10
                add $s2, $t0, $t1                   #     b = $t0 + $t1
                j parse_once__L10
            parse_once__L11:
            slti $t0, $v0, 9
            bne $t0, $zero, parse_once__L13          # if ($v0 >= 9
            slti $t0, $v0, 14
            bne $t0, $zero, parse_once__L12          # && $v0 < 14)
            li $t0, 32
            beq $t0, $v0, parse_once__L12            # || $v0 == 32
            li $t0, 133
            beq $t0, $v0, parse_once__L12            # || $v0 == 133
            li $t0, 160
            bne $t0, $v0, parse_once__L13            # || $v0 == 160: ($v0 is ws)
            parse_once__L12:
                addi $a0, $fp, -132
                jal skip_whitespace_stdin            #    $v0 = sw.skip_whitespace()
            parse_once__L13:
            li $t0, 41
            bne $t0, $v0, main__parse_once_error     # if $v0 != ')': Error out
            bne $s0, $zero, parse_once__L14          # if sign == 0:
                neg $s2, $s2                         #    b = -b
            parse_once__L14:
        parse_once_end:

        addi $a0, $fp, -160
        move $a1, $s1
        move $a2, $s2
        jal query

        bne $v0, $zero, main__L3  # if result == none:
            li $v0, 4
            la $a0, none
            syscall               #     print("None")
            j main__L1            #     continue
        main__L3:
        lw $s0, 0($v0)
        lw $s1, 4($v0)            # load the coordinates
        li $v0, 11
        li $a0, 40
        syscall                   # print '('
        li $v0, 1
        move $a0, $s0
        syscall                   # print x coordinate
        li $v0, 4
        la $a0, comma_space
        syscall                   # print ", "
        li $v0, 1
        move $a0, $s1
        syscall                   # print y coordinate
        li $v0, 4
        la $a0, close_nl
        syscall                   # print ")\n"
        j main__L1
    main__L2:

    li $v0, 10         # return 0
    syscall

    # Error-handlings from down here
    main__parse_error: # file-parsing was unsuccessful
        li $v0, 16         # Close the file
        lw $a0, -140($fp)
        syscall
        li $v0, 4          # Print error message
        la $a0, file_parse_error
        syscall
        li $v0, 4
        la $a0, filename
        syscall
        li $v0, 11         # Newline for error message
        li $a0, 10
        syscall
        li $v0, 17         # return 1
        li $a0, 1
        syscall
    
    main__parse_once_error: # stdin-parsing was unsuccessful.
        li $v0, 4           # Print error message
        la $a0, stdin_parse_error
        syscall
        j main__L1          # Re-enter loop

.end main

# Functions for calculating distance between points.
# Technically, these could be inlined easily to avoid function call cost.
# But I wrote this this way, and I'm too lazy to change that now.

distance_between:
    # Square of distance between two points p1 and p2.
    #
    # Arguments:
    #   $a0: x coordinate of p1
    #   $a1: y coordinate of p1
    #   $a2: x coordinate of p2
    #   $a3: y coordinate of p2
    #
    # Return value:
    #   $v0: square of distance between p1 and p2
    sub $t0, $a0, $a2
    mult $t0, $t0
    mflo $t0           # $t0 = ($a0 - $a2) ^ 2
    sub $t1, $a1, $a3
    mult $t1, $t1
    mflo $t1           # $t1 = ($a1 - $a3) ^ 2
    add $v0, $t0, $t1  # add them together
    jr $ra
.end distance_between

distance_to_origin:
    # Square of distance between given point and origin.
    #
    # Arguments:
    #   $a0: x coordinate of point
    #   $a1: y coordinate of point
    #
    # Return value:
    #   $v0: square of distance between this point and origin.

    mult $a0, $a0
    mflo $t0           # $t0 = $a0 ^ 2
    mult $a1, $a1
    mflo $t1           # $t1 = $a1 ^ 2
    add $v0, $t0, $t1  # add them together
    jr $ra
.end distance_to_origin

ceilsqrt:
    # Use Newton's method to calculate Ceil(Sqrt(x)).
    #
    # Argument:
    #   $a0: x, unsigned 32bit int.
    #
    # Return value:
    #   $v0: ceil(sqrt(x)), unsigned 32bit int.
    sltiu $t0, $a0, 2
    beq $t0, $zero, ceilsqrt__L1
        move $v0, $a0
        jr $ra
    ceilsqrt__L1:
    addiu $a0, $a0, -1
    li $v0, 65536
    ceilsqrt__L2:
        divu $a0, $v0
        mflo $t0
        add $t0, $t0, $v0
        srl $t0, $t0, 1
        slt $t1, $t0, $v0
        bne $t1, $zero, ceilsqrt__L3
            addi $v0, $v0, 1
            jr $ra
        ceilsqrt__L3:
        move $v0, $t0
        j ceilsqrt__L2
.end sqrt

# Functions for inserting into the tree,
insert:
    # Insert Point into the segment tree.
    #
    # Arguments:
    #   $a0: &mut SegmentTree
    #   $a1: x coordinate of point
    #   $a2: y coordinate of point

    addi $sp, $sp, -40  # Preserve registers
    sw $fp, 8($sp)
    sw $ra, 12($sp)
    sw $s0, 16($sp)
    sw $s1, 20($sp)
    sw $s2, 24($sp)
    sw $s3, 28($sp)
    sw $s4, 32($sp)
    sw $s5, 36($sp)
    addi $fp, $sp, 40

    move $s0, $a0           # Preserve arguments
    move $s1, $a1
    move $s2, $a2

    move $a0, $a1
    move $a1, $a2
    jal distance_to_origin  # $v0 = distance_to_origin(x, y)

    # Check the pointer field.
    lw $t0, 8($s0)
    bne $t0, $zero, insert__L1  # If the pointer is null, then this tree is empty. Properly initialize it.
        sb $zero, 3($s0)  # st.depth = 0
        li $t0, 3
        not $t0, $t0
        and $t0, $v0, $t0
        sw $t0, 4($s0)    # st.start = $v0 & !3
        # Initialize this node as a leaf
        li $a0, 64        # Enough for 8 points
        jal malloc
        sw $s1, 0($v0)    # Save the point
        sw $s2, 4($v0)
        sw $v0, 8($s0)    # Save the vector
        li $t0, 8
        sw $t0, 12($s0)
        li $t0, 1
        sw $t0, 16($s0)
        # Return
        move $sp, $fp     # Restore registers
        lw $fp, -32($sp)
        lw $ra, -28($sp)
        lw $s0, -24($sp)
        lw $s1, -20($sp)
        lw $s2, -16($sp)
        jr $ra
    insert__L1:
    lbu $s3, 3($s0)      # $s3 = depth
    lw $s4, 4($s0)       # $s4 = start
    move $s5, $v0        # $s5 = distance
    insert__L2:                     # loop:
        subu $t0, $s5, $s4          #     $t0 = distance - start
        li $t1, 4
        sllv $t1, $t1, $s3          #     $t1 = 4 << depth
        sltu $t0, $t0, $t1
        bne $t0, $zero, insert__L3  #     if 0 <= $t0 && $t0 < $t1: break
        # Create another node, move current root to there,
        # and replace root node with a non-leaf pointing at that node
        li $a0, 24
        jal malloc                  #     Allocate memory for a Node
        addi $s3, $s3, 1            #     Increment depth by 1, we need to write this back later
        li $t0, 4
        sllv $t0, $t0, $s3
        addi $t0, $t0, -1
        not $t0, $t0
        and $t0, $s4, $t0           #     $t0 = start & !((4 << depth) - 1)
        lw $t1, 8($s0)
        lw $t2, 12($s0)
        lw $t3, 16($s0)             #     Load old node from root
        sw $v0, 8($s0)
        bne $t0, $s4, insert__L4    #     if start == $t0:
            sw $t1, 0($v0)
            sw $t2, 4($v0)
            sw $t3, 8($v0)          #         store old node in the left half
            sw $zero, 12($v0)       #         Null-init the right half
            j insert__L2
        insert__L4:                 #     else:
            sw $t1, 12($v0)
            sw $t2, 16($v0)
            sw $t3, 20($v0)         #         store it in the right half instead
            sw $zero, 0($v0)        #         null-init the left half
            move $s4, $t0           #         start = $t0
            j insert__L2
    insert__L3:
    sb $s3, 3($s0)
    sw $s4, 4($s0)    # write depth and start back to memory

    # call node_insert
    addi $a0, $s0, 8
    move $a1, $s3
    move $a2, $s4
    move $a3, $s5
    sw $s1, 0($sp)
    sw $s2, 4($sp)
    jal node_insert

    move $sp, $fp     # Restore the registers
    lw $fp, -32($sp)
    lw $ra, -28($sp)
    lw $s0, -24($sp)
    lw $s1, -20($sp)
    lw $s2, -16($sp)
    lw $s3, -12($sp)
    lw $s4, -8($sp)
    lw $s5, -4($sp)
    jr $ra            # Return
.end insert

node_insert:
    # Arguments:
    #   $a0: &mut Node
    #   $a1: depth
    #   $a2: start
    #   $a3: distance
    #   0($fp) ~ 7($fp): point
    addi $sp, $sp, -24
    sw $fp, 16($sp)
    sw $ra, 20($sp)
    addi $fp, $sp, 24

    node_insert__self_tailcall:
    lw $t0, 0($a0)                       # load the ptr field
    bne $a1, $zero, node_insert__L1      # if depth == 0:
        bne $t0, $zero, node_insert__L2  #     if ptr == null:
            # Initialize this node as leaf
            sw $a0, -24($fp)             #         Save $a0
            li $a0, 64                   #         allocate 64 bytes, enough for 8 Point
            jal malloc
            lw $a0, -24($fp)             #         Load $a0 back
            sw $v0, 0($a0)               #         ptr = allocated block
            li $t0, 8
            sw $t0, 4($a0)               #         cap = 8
            li $t0, 1
            sw $t0, 8($a0)               #         len = 1, since we'll write one right now
            lw $t0, 0($fp)
            lw $t1, 4($fp)               #         Load the coordinates
            sw $t0, 0($v0)
            sw $t1, 4($v0)               #         And store them in the Points

            move $sp, $fp
            lw $fp, -8($sp)
            lw $ra, -4($sp)
            jr $ra                       #         Clean up and return
        node_insert__L2:                 #     else:
                                         #         Prepare for a tail-call; That is, load the arguments and clean up
                                         #         $a0 is already a pointer to Points
        lw $a1, 0($fp)
        lw $a2, 4($fp)

        move $sp, $fp
        lw $fp, -8($sp)
        lw $ra, -4($sp)
        j points_push                    #         tail-call away
    node_insert__L1:                     # else:
        addi $a1, $a1, -1                #     depth -= 1
        bne $t0, $zero, node_insert__L3  #     if ptr == null:
            sw $a0, -24($fp)
            sw $a1, -20($fp)
            sw $a2, -16($fp)
            sw $a3, -12($fp)             #         Save temporary registers
            li $a0, 24
            jal malloc                   #         Allocate memory for child node
            sw $zero, 0($v0)
            sw $zero, 12($v0)            #         null-initialize both child
            move $t0, $v0                #         ptr = allocated memory
            lw $a0, -24($fp)
            lw $a1, -20($fp)
            lw $a2, -16($fp)
            lw $a3, -12($fp)             #         Load the registers back
            sw $t0, 0($a0)               #         Update the ptr field
        node_insert__L3:
        li $t1, 4
        sllv $t1, $t1, $a1
        add $t1, $t1, $a2                #     $t1 = start + (4 << depth)
        slt $t2, $a3, $t1                #     $t2 = 1 if distance < $t1, else 0
        beq $t2, $zero, node_insert__L4  #     if $t2 == 1:
            move $a0, $t0
            j node_insert__self_tailcall #         Tail call to self, with node = node.0 
        node_insert__L4:                 #     else:
            addi $a0, $t0, 12
            move $a2, $t1
            j node_insert__self_tailcall #         Tail call to self, with node = node.1 and start = mid
.end node_insert

# Functions for querying the tree.
query:
    # Query the tree for Nearest Neighbor.
    #
    # Arguments:
    #   $a0: &Tree
    #   $a1: x coordinate of query point
    #   $a2: y coordinate of query point
    #
    # Return value:
    #   $v0: Pointer to nearest neighbor. Null if this tree has no points in it.
    addi $sp, $sp, -24  # Save registers 
    sw $fp, 16($sp)
    sw $ra, 20($sp)
    addi $fp, $sp, 24

    sw $a0, -24($fp)  # Save arguments for function call
    sw $a1, -16($fp)
    sw $a2, -12($fp)
    move $a0, $a1
    move $a1, $a2
    jal distance_to_origin  # Calculate the distance.
    lw $t0, -24($fp)        # Load $a0 back
    addi $a0, $t0, 8        # $a0 = &node
    lbu $a1, 3($t0)         # $a1 = depth
    lw $a2, 4($t0)          # $a2 = start
    move $a3, $v0           # $a3 = distance
    sw $zero, -24($fp)      # Current nearest neighbor = none
    # We already stored the coordinates in the appropriate slots!
    jal node_query

    move $sp, $fp # Restore the registers
    lw $fp, -8($sp)
    lw $ra, -4($sp)
    jr $ra
.end query

node_query:
    # Query this node for Nearest Neighbor.
    #
    # Arguments:
    #   $a0: &Node
    #   $a1: depth
    #   $a2: start
    #   $a3: distance
    #   0($fp) ~ 3($fp): Pointer to current nearest neighbor. Null if none.
    #   4($fp) ~ 7($fp): distance between current nearest neighbor and the query point. Only valid if the pointer is not null.
    #   8($fp) ~ 11($fp): x coordinate of query point
    #   12($fp) ~ 15($fp): y coordinate of query point
    #
    # Return values:
    #   $v0: Pointer to current nearest neighbor. Null if none.
    #   $v1: distance between... Well, you get the idea.
    addi $sp, $sp, -52
    sw $fp, 16($sp)
    sw $ra, 20($sp)
    sw $s0, 24($sp)
    sw $s1, 28($sp)
    sw $s2, 32($sp)
    sw $s3, 36($sp)
    sw $s4, 40($sp)
    sw $s5, 44($sp)
    sw $s6, 48($sp)
    addi $fp, $sp, 52

    node_query__self_tail_call:
    lw $s0, 0($a0)
    bne $s0, $zero, node_query__L1  # If the ptr of this node is null:
        lw $v0, 0($fp)
        lw $v1, 4($fp)              # Load the arguments to return them as-is

        move $sp, $fp
        lw $fp, -36($sp)
        lw $ra, -32($sp)
        lw $s0, -28($sp)
        lw $s1, -24($sp)
        lw $s2, -20($sp)
        lw $s3, -16($sp)
        lw $s4, -12($sp)
        lw $s5, -8($sp)
        lw $s6, -4($sp)
        jr $ra
    
    node_query__L1:
    bne $a1, $zero, node_query__L2    # if depth == 0:
        lw $s1, 8($a0)                #     $s1 = len
        li $s2, 0                     #     $s2 = 0
        lw $s3, 8($fp)
        lw $s4, 12($fp)               #     load the coordinates
        lw $s5, 0($fp)                #     load current nearest neighbor
        lw $s6, 4($fp)                #     load current least distance
        node_query__L3:
        beq $s1, $s2, node_query__L4  #     while $s1 != $s2:
            move $a0, $s3             #         // Iterate over points stored in this... points
            move $a1, $s4
            lw $a2, 0($s0)
            lw $a3, 4($s0)
            jal distance_between      #         $v0 = distance between query and this point
            beq $s5, $zero, node_query__L5
            slt $t0, $s6, $v0         #         if current nearest neighbor is none or this point is nearer
            bne $t0, $zero, node_query__L6
                node_query__L5:
                move $s6, $v0         #             least distance = the distance
                move $s5, $s0         #             nearest neighbor = this point
            node_query__L6:
            addi $s2, $s2, 1
            addi $s0, $s0, 8          #         point to next index
            j node_query__L3
        node_query__L4:
        move $v0, $s5
        move $v1, $s6                 #     Put the result in return registers
        move $sp, $fp                 #     Restore registers
        lw $fp, -36($sp)
        lw $ra, -32($sp)
        lw $s0, -28($sp)
        lw $s1, -24($sp)
        lw $s2, -20($sp)
        lw $s3, -16($sp)
        lw $s4, -12($sp)
        lw $s5, -8($sp)
        lw $s6, -4($sp)
        jr $ra                        #     return

    node_query__L2:                   # else:
        addi $s1, $a1, -1             #     $s1 = next_depth = depth - 1
        li $t0, 4
        sllv $t0, $t0, $s1
        add $s2, $t0, $a2             #     $s2 = mid = start + (4 << depth)
        move $s5, $a3                 #     preserve $a3 in $s5 for later
        slt $t0, $s5, $s2             #     if distance < mid:
        beq $t0, $zero, node_query__L7
            addi $s3, $s0, 12         #         $s3 = other = &child.1
            mult $a3, $s2
            mflo $a0
            jal ceilsqrt
            sll $v0, $v0, 1
            add $s4, $s5, $s2
            sub $s4, $s4, $v0         #         $s4 = minimum_in_other = mid + distance - 2ceilsqrt(mid * distance)
            move $a0, $s0             #         Load appropriate child onto the registers
            move $a1, $s1
            # move $a2, $a2
            # move $a3, $a3
            j node_query__L8
        node_query__L7:               #     else:
            move $s3, $s0             #         $s3 = other = &child.0
            addi $s6, $s2, -1         #         $s6 = mid - 1
            move $s2, $a2             #         $s2 = other_start = start (In the branch above, we don't change this so it's still mid)
            mult $a3, $s6
            mflo $a0
            jal ceilsqrt
            sll $v0, $v0, 1
            add $s4, $s5, $s6
            sub $s4, $s4, $v0         #         $s4 = minimum_in_other = mid-1 + distance - 2ceilsqrt((mid-1) * distance)
            addi $a0, $s0, 12         #         Load appropriate child onto the registers
            move $a1, $s1
            addi $a2, $s6, 1
            # move $a3, $a3
        node_query__L8:
        lw $t0, 0($fp)
        lw $t1, 4($fp)
        lw $t2, 8($fp)
        lw $t3, 12($fp)
        sw $t0, -52($fp)
        sw $t1, -48($fp)
        sw $t2, -44($fp)
        sw $t3, -40($fp)
        jal node_query                #         recursively call node_query on appropriate child
        beq $v0, $zero, node_query__L9  # Check the return value to determine whether we should call node_query on the other child.
        slt $t0, $s4, $v1               # If the result is null, or minimum-in-other is smaller than returned distance,
        beq $t0, $zero, node_query__L10 # then we call on the other child. else we skip and return.
            node_query__L9:
            sw $v0, 0($fp)
            sw $v1, 4($fp)
            move $a0, $s3
            move $a1, $s1
            move $a2, $s2
            move $a3, $s5
            j node_query__self_tail_call
        node_query__L10:
        move $sp, $fp                 #     Restore registers
        lw $fp, -36($sp)
        lw $ra, -32($sp)
        lw $s0, -28($sp)
        lw $s1, -24($sp)
        lw $s2, -20($sp)
        lw $s3, -16($sp)
        lw $s4, -12($sp)
        lw $s5, -8($sp)
        lw $s6, -4($sp)
        jr $ra                        #     return
.end node_query

# Points-family.
points_push:
    # Push a point to the end of this Points. Might reallocate.
    #
    # Arguments:
    #     $a0: &mut Points
    #     $a1: x coordinate of Point
    #     $a2: y coordinate of Point
    addi $sp, $sp, -24
    sw $fp, 16($sp)
    sw $ra, 20($sp)
    addi $fp, $sp, 24

    lw $v0, 0($a0)                 # $v0 = ptr
    lw $t0, 4($a0)                 # $t0 = cap
    lw $t1, 8($a0)                 # $t1 = len
    bne $t0, $t1, points_push__L1  # if cap == len:
        sw $a0, -24($fp)           #     Save the temporary registers, we want to use them post-function call too
        sw $t1, -20($fp)           #     We probably won't execute this branch often, so we only store inside the branch
        sw $a1, -16($fp)
        sw $a2, -12($fp)
        sll $a1, $t0, 1            #     new_cap = cap * 2
        sw $a1, 4($a0)             #     Write new_cap back
        sll $a1, $a1, 3            #     Actual bytes to allocate = new_cap * 8
        move $a0, $v0
        jal realloc                #     Reallocate memory
        lw $a0, -24($fp)           #     Load the temporary registers
        lw $t1, -20($fp)
        lw $a1, -16($fp)
        lw $a2, -12($fp)
        sw $v0, 0($a0)             #     Update the ptr field
    points_push__L1:
    sll $t0, $t1, 3
    add $t0, $v0, $t0              # addr = ptr + 8 * len, since size of Point == 8 bytes
    sw $a1, 0($t0)                 # Store
    sw $a2, 4($t0)
    addi $t1, $t1, 1               # len += 1
    sw $t1, 8($a0)                 # update the len field

    move $sp, $fp
    lw $fp, -8($sp)
    lw $ra, -4($sp)
    jr $ra
.end points_push

# The take-input family.
# I chose to parse input byte-by-byte, so these are functions that does just that.
# _stdin variants read from stdin, rest read from file.
next_byte:
    # Read and return a byte from FileWrapper.
    #
    # Arguments:
    #   $a0: &mut FileWrapper
    #
    # Return value:
    #   $v0: -2 on Error, -1 on EOF, 0~255 otherwise

    addi $sp, $sp, -8  # Preserve registers
    sw $fp, 0($sp)
    sw $s0, 4($sp)
    addi $fp, $sp, 8

    move $s0, $a0                     # $s0 = &fw
    lw $t0, 4($s0)                    # $t0 = fw.offset
    lw $t1, 8($s0)                    # $t1 = fw.cap
    bne $t0, $t1, next_byte__L1       # If $t0 == $t1:
        li $v0, 14                    #     $v0 = read_file(
        lw $a0, 0($s0)                #         fw.file,
        addi $a1, $s0, 12             #         &fw.buf,
        li $a2, 128                   #         128
        syscall                       #     )
        slti $t0, $v0, 1              #
        beq $t0, $zero, next_byte__L2 # 	if $v0 <= 0:
            addi $v0, $v0, -1         #         $v0 = $v0 - 1
            j next_byte__return       #         return $v0
        next_byte__L2:
        li $t0, 1
        sw $t0, 4($s0)                # 	fw.offset = 1
        sw $v0, 8($s0)                # 	fw.cap = $v0
        lbu $v0, 12($s0)              #     $v0 = fw.buf[0]
        j next_byte__return           #     return $v0
    next_byte__L1:
    add $t1, $t0, $s0
    lbu $v0, 12($t1)                  # $v0 = *(&fw + fw.offset + 12) (= fw.buf[fw.offset])
    addi $t0, $t0, 1
    sw $t0, 4($s0)

    next_byte__return:                # Clean up stuff, and return
    move $sp, $fp
    lw $fp, -8($sp)
    lw $s0, -4($sp)
    jr $ra
.end next_byte

skip_whitespace:
    # Read and return a byte from FileWrapper.
    # Skip whitespaces.
    #
    # Arguments:
    #   $a0: &mut FileWrapper
    #
    # Return value:
    #   $v0: -2 on Error, -1 on EOF, 0~255 minus whitespace, otherwise

    addi $sp, $sp, -12  # Preserve registers
    sw $fp, 0($sp)
    sw $ra, 4($sp)
    sw $s0, 8($sp)
    addi $fp, $sp, 12

    move $s0, $a0
    skip_whitespace__L1:
        move $a0, $s0
        jal next_byte                        # Call next_byte with current $a0
        slti $t0, $v0, 9
        bne $t0, $zero, skip_whitespace__L2  # Break if $v0 < 9
        slti $t0, $v0, 14
        bne $t0, $zero, skip_whitespace__L1  # Continue if $v0 < 14
        li $t0, 32
        beq $t0, $v0, skip_whitespace__L1    # Continue if $v0 == 32
        li $t0, 133
        beq $t0, $v0, skip_whitespace__L1    # Continue if $v0 == 133
        li $t0, 160
        beq $t0, $v0, skip_whitespace__L1    # Continue if $v0 == 160
    skip_whitespace__L2:
    move $sp, $fp     # Restore registers
    lw $fp, -12($sp)
    lw $ra, -8($sp)
    lw $s0, -4($sp)
    jr $ra
.end skip_whitespace

next_byte_stdin:
    # Read and return a single byte from standard input.
    # Since system call to read-from-stdin and read-from-file acts differently,
    # we cannot reuse next_byte.
    # 
    # Argument:
    #   $a0: &mut StdinWrapper
    #
    # Return value:
    #   $v0: byte that has been read

    addi $sp, $sp, -8    # Preserve registers
    sw $fp, 0($sp)
    sw $s0, 4($sp)
    addi $fp, $sp, 8

    lw $t0, 0($a0)       # Read current offset
    next_byte_stdin__L1:
        add $t1, $a0, $t0
        lbu $v0, 4($t1)  # Read a byte from (address of StdinWrapper + offset + 4) = address of the byte we want
        bne $zero, $v0, next_byte_stdin__L2  # if we've read a null byte, then
        move $s0, $a0    # Syscall doesn't preserve $a0
        li $v0, 8        # Read from stdin
        add $a0, $a0, 4  # Address of the buffer
        li $a1, 128      # The buffer is always 128-bytes long in this program!
        syscall
        # If the buffer isn't big enough, it will be read in the next syscall
        # and the last byte of buffer will be set to null, so just null-checking is fine
        
        # We don't know whether we've read anything; maybe we didn't!
        # Then the first byte in buffer should be null. we can just reuse the check above
        li $t0, 0
        move $a0, $s0
        j next_byte_stdin__L1
    next_byte_stdin__L2:
    # We read a non-null byte.
    addi $t0, $t0, 1    # Increment offset
    sw $t0, 0($a0)      # And write it back
    # Since we read into $v0, the byte is already there to be returned!
    
    move $sp, $fp   # Clean up
    lw $fp, -8($sp)
    lw $s0, -4($sp)

    jr $ra
.end next_byte_stdin

skip_whitespace_stdin:
    # Read and return a byte from StdWrapper.
    # Skip whitespaces. This is almost a complete copy-paste of file version!
    #
    # Arguments:
    #   $a0: &mut StdinWrapper
    #
    # Return value:
    #   $v0: Non-whitespace byte that has been read

    addi $sp, $sp, -12  # Preserve registers
    sw $fp, 0($sp)
    sw $ra, 4($sp)
    sw $s0, 8($sp)
    addi $fp, $sp, 12

    move $s0, $a0
    skip_whitespace_stdin__L1:
        move $a0, $s0
        jal next_byte_stdin                        # Call next_byte_stdin with current $a0
        slti $t0, $v0, 9
        bne $t0, $zero, skip_whitespace_stdin__L2  # Break if $v0 < 9
        slti $t0, $v0, 14
        bne $t0, $zero, skip_whitespace_stdin__L1  # Continue if $v0 < 14
        li $t0, 32
        beq $t0, $v0, skip_whitespace_stdin__L1    # Continue if $v0 == 32
        li $t0, 133
        beq $t0, $v0, skip_whitespace_stdin__L1    # Continue if $v0 == 133
        li $t0, 160
        beq $t0, $v0, skip_whitespace_stdin__L1    # Continue if $v0 == 160
    skip_whitespace_stdin__L2:
    move $sp, $fp     # Restore registers
    lw $fp, -12($sp)
    lw $ra, -8($sp)
    lw $s0, -4($sp)
    jr $ra
.end skip_whitespace_stdin

# The memory-allocation family.
# Here, we use super-naive versions of each. That means no memory re-use!
# We're not trying to write an actual Heap-allocator here, so probably fine for now.
malloc:
    # Allocate memory.
    # 
    # Argument:
    #   $a0: Amount of memory to allocate, in bytes.
    # 
    # Return values:
    #   $v0: Starting address of memory allocated.

    li $v0, 9  # System call code for memory allocation
               # $a0 already holds amount of memory to allocate in bytes.
    syscall    # So we syscall away

    jr $ra     # return
.end malloc

realloc:
    # Resize the memory block that has been previously allocated. This may move the block.
    # The contents of the block is preserved up to minimum of old and new size.
    #
    # Argument:
    #   $a0: Starting address of memory to be resized.
    #   $a1: new size for the memory block.
    #
    # Return values:
    #   $v0: Starting address of possibly-new memory block.

    addi $sp, $sp, -12  # Preserve regiters
    sw $fp, 8($sp)
    sw $s0, 4($sp)
    sw $s1, 0($sp)
    addi $fp, $sp, 12
    
    move $s0, $a0     # Preserve the arguments, since syscall *might* change them
    move $s1, $a1
    li $v0, 9         # System call for memory allocation.
    move $a0, $a1
    syscall
    move $t0, $v0

    realloc_L1:
    slti $t1, $s1, 8            # while amount of bytes to move >= 8:
    bne $t1, $zero, realloc_L2
        ld $t2, 0($s0)          #     load 8 bytes from original block, into $t2 and $t3
        sd $t2, 0($t0)          #     store that in new block
        addi $s1, $s1, -8       #     amount of bytes to move -= 8
        addi $s0, $s0, 8
        addi $t0, $t0, 8        #     Base-address of blocks += 8
        j realloc_L1
    realloc_L2:
    beq $s1, $zero, realloc_L3  # while amount of bytes to move != 0
        lb $t1, 0($s0)          #     Same as above, though 1 byte at a time
        sb $t1, 0($t0)
        addi $s1, $s1, -1
        addi $s0, $s0, 1
        addi $t0, $t0, 1
        j realloc_L2
    realloc_L3:
    # We never modified $v0 post-syscall, since we never explicitly changed it nor have we called anything
    # So we prepare to return
    move $sp, $fp    # Restore registers
    lw $s1, -12($sp)
    lw $s0, -8($sp)
    lw $fp, -4($sp)
    jr $ra           # And return
.end realloc

free:
    # Frees previously allocated memory.
    # 
    # Argument:
    #   $a0: Starting address of memory previously allocated.

    jr $ra
.end free