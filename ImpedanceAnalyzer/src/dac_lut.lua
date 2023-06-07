local freq = { 20, 20, 20, 21, 22, 22, 23, 23, 24, 24, 25, 26, 26, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 36, 37,
    38, 39, 40, 41, 42, 43, 44, 45, 46, 48, 49, 50, 51, 53, 54, 55, 57, 58, 60, 61, 63, 65, 66, 68, 70, 71, 73, 75, 77,
    79, 81, 83, 85, 87, 90, 92, 94, 97, 100, 102, 104, 107, 110, 113, 116, 118, 122, 124, 128, 131, 134, 138, 142, 145,
    149, 153, 156, 161, 165, 169, 173, 178, 182, 187, 191, 196, 202, 206, 212, 218, 222, 229, 235, 240, 247, 254, 259,
    266, 272, 279, 287, 293, 302, 310, 316, 325, 334, 342, 351, 361, 369, 379, 387, 398, 409, 417, 429, 441, 450, 463,
    476, 486, 500, 514, 524, 539, 550, 566, 582, 594, 610, 628, 641, 659, 677, 691, 711, 731, 746, 767, 783, 805, 827,
    845, 869, 893, 912, 937, 963, 984, 1011, 1039, 1061, 1091, 1122, 1145, 1177, 1202, 1236, 1270, 1297, 1333, 1370,
    1399, 1438, 1479, 1510, 1552, 1595, 1629, 1675, 1710, 1758, 1807, 1845, 1896, 1949, 1990, 2046, 2103, 2147, 2208,
    2270, 2317, 2382, 2432, 2500, 2570, 2624, 2697, 2773, 2831, 2910, 2992, 3055, 3140, 3228, 3296, 3388, 3459, 3556,
    3656, 3732, 3837, 3944, 4027, 4140, 4256, 4345, 4467, 4592, 4688, 4819, 4920, 5058, 5200, 5309, 5457, 5610, 5728,
    5888, 6053, 6180, 6353, 6531, 6668, 6855, 6998, 7194, 7396, 7551, 7763, 7980, 8147, 8375, 8610, 8790, 9037, 9290,
    9500, 9750, 10023, 10250, 10520, 10740, 11041, 11350, 11588, 11913, 12247, 12503, 12853, 13213, 13500, 13868, 14250,
    14500, 14963, 15276, 15704, 16144, 16482, 16900, 17500, 17784, 18100, 18794, 19200, 20000 }

local function get_prescale(hz, lut_size, pre_div)
    return math.floor((170000000 / pre_div) / (lut_size * hz))
end

local function get_actual_hz(prescale, lut_size, pre_div)
    return (170000000 / pre_div) / (lut_size * prescale)
end

local list_lut = {}
local list_scale = {}
for i, v in pairs(freq) do
    local lut_size = 170 * 2
    local pre_div = 1
    ::retry::
    local presc = get_prescale(v, lut_size, pre_div)
    local act = get_actual_hz(presc, lut_size, pre_div)
    local diff = math.abs(v - act)
    if lut_size > 2048 or presc < 1 then
        lut_size = 128
        -- pre_div = pre_div + 1
        if lut_size > 2048 then
            print("freq", v, "failed", pre_div, presc, act, diff, lut_size)
            error("err")
        end
        goto retry
    end
    if diff > 1 then
        lut_size = lut_size + 1
        goto retry
    end
    print("+", v, "lut", lut_size, "scale", presc - 1)
    table.insert(list_lut, lut_size)
    table.insert(list_scale, presc - 1)
end

print("static const uint16_t list_lut[] = {" .. table.concat(list_lut, ",") .. "};")
print("static const uint16_t list_scale[] = {" .. table.concat(list_scale, ",") .. "};")
