import random

board = list(x for x in range(1, 26))
empty = 24

moves = 0
total = 200
last_direction = None
while moves < total:
    direction = random.randrange(4)
    if (direction + 2) % 4 == last_direction:
        continue
    last_direction = direction
    if direction == 0:
        if empty < 5:
            continue
        tmp = board[empty - 5]
        board[empty - 5] = board[empty]
        board[empty] = tmp
        empty = empty - 5
    elif direction == 1:
        if empty % 5 == 0:
            continue
        tmp = board[empty - 1]
        board[empty - 1] = board[empty]
        board[empty] = tmp
        empty = empty - 1
    elif direction == 2:
        if empty >= 20:
            continue
        tmp = board[empty + 5]
        board[empty + 5] = board[empty]
        board[empty] = tmp
        empty = empty + 5
    else:
        if empty % 5 == 4:
            continue
        tmp = board[empty + 1]
        board[empty + 1] = board[empty]
        board[empty] = tmp
        empty = empty + 1
    moves += 1

print("1")

for x in range(5):
    print(' '.join(map(str, board[5*x:5*x+5])))
