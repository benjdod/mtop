def compress(value, start, end):

    value = value / float(start) * float(end)
    return round(int(value))

i = input('enter a number between 0 and 255 inclusive: ')
print (compress(i, 256, 6))
