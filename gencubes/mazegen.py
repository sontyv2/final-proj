from random import shuffle, randrange, choice
 
def make_maze(w = 16, h = 8):
    vis = [[0] * w + [1] for _ in range(h)] + [[1] * (w + 1)]
    #print(vis)
    ver = [["100"] * w + ['1'] for _ in range(h)] + [[]]
    hor = [["111"] * w + ['1'] for _ in range(h + 1)]
 
    def walk(x, y):
        vis[y][x] = 1
 
        d = [(x - 1, y), (x, y + 1), (x + 1, y), (x, y - 1)]
        shuffle(d)
        for (xx, yy) in d:
            if vis[yy][xx]: continue
            if xx == x: hor[max(y, yy)][x] = "100"
            if yy == y: ver[y][max(x, xx)] = "000"
            walk(xx, yy)
 
    walk(randrange(w), randrange(h))
 
    hor[0][0] = '110'
    hor[-1][-2] = '110'

    s = ""
    for (a, b) in zip(hor, ver):
        s += ''.join(a + ['\n'] + b + ['\n'])

    s = add_glass_and_mirrors(s, w, h)
    return s

def add_glass_and_mirrors(s, w, h):
    row_count = 0
    col_count = 0
    power_indices = []
    for index in range(len(s)):
        if(row_count != 0 and row_count != (h * 2) and col_count != 0 and col_count != w * 3):
            r = randrange(1, 5)
            if s[index] == '1':
                if r == 2: 
                    s = s[:index] + '2' + s[index + 1:]
                elif r == 3: 
                    s = s[:index] + '3' + s[index + 1:]
            elif s[index] == '0':
                power_indices.append(index)

        if (s[index] == '\n'):
            row_count += 1
            col_count = 0
        else:
            col_count += 1

    for i in range(4):
        p_idx = choice(power_indices)
        s = s[:p_idx] + '4' + s[p_idx + 1:]

    return s

def write_to_file(text):
    file = open("maze.txt", "w")
    file.write(text)
    file.close()

def to_call(width, height):
    write_to_file(make_maze(width, height))

if __name__ == '__main__':
    to_call(16, 8)

