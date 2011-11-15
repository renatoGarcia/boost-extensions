function func(w)
    return "hello " .. w
end

root = {
    array = { 10, 11, 12, 13, 14, 15,},
    hi = func("world"),
    subtree = {
        one = 1,
        two = 2,
    },
}
