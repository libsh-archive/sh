#!/usr/bin/python

import shtest, sys

test = shtest.StreamTest('tex', 1)
test.broken_backend("cc")

# all the tests use nearest-neighbour lookup right now

# test float textures
test.add_texture(shtest.ImageTexture("NoMIPFilter<Texture2D<Color3f> >", "test2d_tex", "tex.png")) 
test.add_call(shtest.Call(shtest.Call.call, 'test2d_tex', 1))
test.add_make_test((1.0, 0.0, 0.0), [(0.0, 0.0)])
test.add_make_test((0.0, 1.0, 0.0), [(1.0, 0.0)])
test.add_make_test((0.0, 0.0, 1.0), [(1.0, 1.0)])
test.add_make_test((0.5, 0.5, 0.5), [(0.0, 1.0)])

# test wrap repeat
test.add_texture(shtest.ImageTexture("NoMIPFilter<WrapRepeat<Texture2D<Color3f> > >", "repeat_tex", "tex.png")) 
test.clear_call()
test.add_call(shtest.Call(shtest.Call.call, 'repeat_tex', 1))
test.add_make_test((1.0, 0.0, 0.0), [(0.5/4.0, 0.5/4.0)])
test.add_make_test((1.0, 0.0, 0.0), [(4.5/4.0, -3.5/4.0)])

# test clamping
test.add_texture(shtest.GenTexture("NoMIPFilter<Texture2D<Color3fub> >",
    "FracUByte", "SH_FUBYTE", 3, (16, 16), "clamp_tex", "(i - 2 * j + 4 * elem) / 2"))
test.clear_call()
test.add_call(shtest.Call(shtest.Call.call, 'clamp_tex', 1))
test.add_make_test((0, 1, 1), [(0, 0)], ['i', 'i'])
test.add_make_test((0, 0, 0), [(1.0, 1.0)])

# test fractional textures
test.add_texture(shtest.GenTexture("NoMIPFilter<Texture2D<Color4fui> >",
    "FracUInt", "SH_FUINT", 4, (8, 8), "frac_uint2d_tex", "(i + 3 * j + 7 * elem) / 100.0"))
test.clear_call()
test.add_call(shtest.Call(shtest.Call.call, 'frac_uint2d_tex', 1))
test.add_make_test((0, .07, .14, .21), [(0, 0)]) # should really try out [] lookup
test.add_make_test((.07, .14, .21, .28), [(1, 0)]) # i = 7, j = 0 
test.add_make_test((.21, .28, .35, .42), [(0, 1)]) # i = 0, j = 7
test.add_make_test((.28, .35, .42, .49), [(1, 1)]) # i = j = 7
test.add_make_test((.19, .26, .33, .40), [(4/7.0, 5/7.0)]) # i = 4, j = 5 

# test signed fractional textures (these break since scale & bias doesn't work yet)
test.add_texture(shtest.GenTexture("NoMIPFilter<Texture2D<Color3fb> >",
    "FracByte", "SH_FBYTE", 3, (8, 8), "frac_byte2d_tex", "(-i + j + elem - 1) / 10.0"))
test.clear_call()
test.add_call(shtest.Call(shtest.Call.call, 'frac_byte2d_tex', 1))
#test.add_make_test((-.1, 0, .1), [(0, 0)]) # should really try out [] lookup
#test.add_make_test((-.8, -.7, -.6), [(1, 0)]) # i = 7, j = 0 
#test.add_make_test((.6, .7, .8), [(0, 1)]) # i = 0, j = 7
#test.add_make_test((-.1, 0, .1), [(1, 1)]) # i = j = 7
#test.add_make_test((.0, .1, .2), [(4/7.0, 5/7.0)]) # i = 4, j = 5 


# test half textures
test.add_texture(shtest.GenTexture("NoMIPFilter<Texture2D<Color3h> >",
    "Half", "SH_HALF", 3, (16, 16), "half2d_tex", "100 * (i - 8) + 10 * (j - 8) + elem"))
test.clear_call()
test.add_call(shtest.Call(shtest.Call.call, 'half2d_tex', 1))
test.add_make_test((-880, -879, -878), [(0, 0)], ['i', 'i'])
test.add_make_test((770, 771, 772), [(1.0, 1.0)])
test.add_make_test((10, 11, 12), [(8.5/16.0, 9.5/16.0)])
test.add_make_test((-730, -729, -728), [(-2, 2)]) # should clamp to 0,1 

# repeat, but with rect lookups
test.add_texture(shtest.GenTexture("NoMIPFilter<TextureRect<Color3h> >",
    "Half", "SH_HALF", 3, (16, 16), "half_rect_tex", "100 * (i - 8) + 10 * (j - 8) + elem"))
test.clear_call()
test.add_call(shtest.Call(shtest.Call.lookup, 'half_rect_tex', 1))
test.add_make_test((-880, -879, -878), [(0, 0)], ['i', 'i'])
test.add_make_test((770, 771, 772), [(15.5, 15.5)])
test.add_make_test((10, 11, 12), [(8.5, 9.5)])
test.add_make_test((-730, -729, -728), [(-20, 20)]) # should clamp to 0,1 
test.add_make_test((-730, -729, -728), [(-20, 20)]) # should clamp to 0,1 

test.add_texture(shtest.GenTexture("ArrayRect<Color3f>",
    "float", "SH_FLOAT", 3, (16, 16), "float_array", "i + 3 * j"))
test.clear_call()
test.add_call(shtest.Call(shtest.Call.lookup, 'float_array', 1))
test.add_make_test((32, 32, 32), [(8, 8)], ['i', 'i'])

test.output(sys.stdout)
